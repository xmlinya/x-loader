/*
 * Freescale i.MX28 SPI driver
 *
 * Copyright (C) 2011 Marek Vasut <marek.vasut@gmail.com>
 * on behalf of DENX Software Engineering GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * NOTE: This driver only supports the SPI-controller chipselects,
 *       GPIO driven chipselects are not supported.
 */

#include <common.h>
#include <malloc.h>
#include <spi.h>
#include <asm/errno.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/dma.h>

#define	MXS_SPI_MAX_TIMEOUT	1000000
#define	MXS_SPI_PORT_OFFSET	0x2000
#define MXS_SSP_CHIPSELECT_MASK		0x00300000
#define MXS_SSP_CHIPSELECT_SHIFT	20

#define MXSSSP_SMALL_TRANSFER	512

/*
 * CONFIG_MXS_SPI_DMA_ENABLE: Experimental mixed PIO/DMA support for MXS SPI
 *                            host. Use with utmost caution!
 *
 *                            Enabling this is not yet recommended since this
 *                            still doesn't support transfers to/from unaligned
 *                            addresses. Therefore this driver will not work
 *                            for example with saving environment. This is
 *                            caused by DMA alignment constraints on MXS.
 */

struct mxs_spi_slave {
	struct spi_slave	slave;
	uint32_t		max_khz;
	uint32_t		mode;
	struct mxs_ssp_regs	*regs;
	struct mxs_dma_desc	*desc;
};

static inline struct mxs_spi_slave *to_mxs_slave(struct spi_slave *slave)
{
	return container_of(slave, struct mxs_spi_slave, slave);
}

void spi_init(void)
{
}

int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	/* MXS SPI: 4 ports and 3 chip selects maximum */
	if (bus > 3 || cs > 2)
		return 0;
	else
		return 1;
}

struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs,
				  unsigned int max_hz, unsigned int mode)
{
	struct mxs_spi_slave *mxs_slave;
	uint32_t addr;
	struct mxs_ssp_regs *ssp_regs;
	int reg;
	struct mxs_dma_desc *desc;

	if (!spi_cs_is_valid(bus, cs)) {
		printf("mxs_spi: invalid bus %d / chip select %d\n", bus, cs);
		return NULL;
	}

	mxs_slave = malloc(sizeof(struct mxs_spi_slave));
	if (!mxs_slave)
		return NULL;

	desc = mxs_dma_desc_alloc();
	if (!desc)
		goto err_desc;

	if (mxs_dma_init_channel(bus))
		goto err_init;

	addr = MXS_SSP0_BASE + (bus * MXS_SPI_PORT_OFFSET);

	mxs_slave->slave.bus = bus;
	mxs_slave->slave.cs = cs;
	mxs_slave->max_khz = max_hz / 1000;
	mxs_slave->mode = mode;
	mxs_slave->regs = (struct mxs_ssp_regs *)addr;
	mxs_slave->desc = desc;
	ssp_regs = mxs_slave->regs;

	reg = readl(&ssp_regs->hw_ssp_ctrl0);
	reg &= ~(MXS_SSP_CHIPSELECT_MASK);
	reg |= cs << MXS_SSP_CHIPSELECT_SHIFT;

	writel(reg, &ssp_regs->hw_ssp_ctrl0);
	return &mxs_slave->slave;

err_init:
	mxs_dma_desc_free(desc);
err_desc:
	free(mxs_slave);
	return NULL;
}

void spi_free_slave(struct spi_slave *slave)
{
	struct mxs_spi_slave *mxs_slave = to_mxs_slave(slave);
	mxs_dma_desc_free(mxs_slave->desc);
	free(mxs_slave);
}

int spi_claim_bus(struct spi_slave *slave)
{
	struct mxs_spi_slave *mxs_slave = to_mxs_slave(slave);
	struct mxs_ssp_regs *ssp_regs = mxs_slave->regs;
	uint32_t reg = 0;

	mx28_reset_block(&ssp_regs->hw_ssp_ctrl0_reg);

	writel(SSP_CTRL0_BUS_WIDTH_ONE_BIT, &ssp_regs->hw_ssp_ctrl0);

	reg = SSP_CTRL1_SSP_MODE_SPI | SSP_CTRL1_WORD_LENGTH_EIGHT_BITS;
	reg |= (mxs_slave->mode & SPI_CPOL) ? SSP_CTRL1_POLARITY : 0;
	reg |= (mxs_slave->mode & SPI_CPHA) ? SSP_CTRL1_PHASE : 0;
	writel(reg, &ssp_regs->hw_ssp_ctrl1);

	writel(0, &ssp_regs->hw_ssp_cmd0);

	mx28_set_ssp_busclock(slave->bus, mxs_slave->max_khz);

	return 0;
}

void spi_release_bus(struct spi_slave *slave)
{
}

static void mxs_spi_start_xfer(struct mxs_ssp_regs *ssp_regs)
{
	writel(SSP_CTRL0_LOCK_CS, &ssp_regs->hw_ssp_ctrl0_set);
	writel(SSP_CTRL0_IGNORE_CRC, &ssp_regs->hw_ssp_ctrl0_clr);
}

static void mxs_spi_end_xfer(struct mxs_ssp_regs *ssp_regs)
{
	writel(SSP_CTRL0_LOCK_CS, &ssp_regs->hw_ssp_ctrl0_clr);
	writel(SSP_CTRL0_IGNORE_CRC, &ssp_regs->hw_ssp_ctrl0_set);
}

static int mxs_spi_xfer_pio(struct mxs_spi_slave *slave,
			char *data, int length, int write, unsigned long flags)
{
	struct mxs_ssp_regs *ssp_regs = slave->regs;

	if (flags & SPI_XFER_BEGIN)
		mxs_spi_start_xfer(ssp_regs);

	while (length--) {
		/* We transfer 1 byte */
		writel(1, &ssp_regs->hw_ssp_xfer_size);

		if ((flags & SPI_XFER_END) && !length)
			mxs_spi_end_xfer(ssp_regs);

		if (write)
			writel(SSP_CTRL0_READ, &ssp_regs->hw_ssp_ctrl0_clr);
		else
			writel(SSP_CTRL0_READ, &ssp_regs->hw_ssp_ctrl0_set);

		writel(SSP_CTRL0_RUN, &ssp_regs->hw_ssp_ctrl0_set);

		if (mx28_wait_mask_set(&ssp_regs->hw_ssp_ctrl0_reg,
			SSP_CTRL0_RUN, MXS_SPI_MAX_TIMEOUT)) {
			printf("MXS SPI: Timeout waiting for start\n");
			return -ETIMEDOUT;
		}

		if (write)
			writel(*data++, &ssp_regs->hw_ssp_data);

		writel(SSP_CTRL0_DATA_XFER, &ssp_regs->hw_ssp_ctrl0_set);

		if (!write) {
			if (mx28_wait_mask_clr(&ssp_regs->hw_ssp_status_reg,
				SSP_STATUS_FIFO_EMPTY, MXS_SPI_MAX_TIMEOUT)) {
				printf("MXS SPI: Timeout waiting for data\n");
				return -ETIMEDOUT;
			}

			*data = readl(&ssp_regs->hw_ssp_data);
			data++;
		}

		if (mx28_wait_mask_clr(&ssp_regs->hw_ssp_ctrl0_reg,
			SSP_CTRL0_RUN, MXS_SPI_MAX_TIMEOUT)) {
			printf("MXS SPI: Timeout waiting for finish\n");
			return -ETIMEDOUT;
		}
	}

	return 0;
}

static int mxs_spi_xfer_dma(struct mxs_spi_slave *slave,
			char *data, int length, int write, unsigned long flags)
{
	struct mxs_dma_desc *desc = slave->desc;
	struct mxs_ssp_regs *ssp_regs = slave->regs;
	uint32_t ctrl0 = SSP_CTRL0_DATA_XFER;
	uint32_t cache_data_count;
	int dmach;

	memset(desc, 0, sizeof(struct mxs_dma_desc));
	desc->address = (dma_addr_t)desc;

	if (flags & SPI_XFER_BEGIN)
		ctrl0 |= SSP_CTRL0_LOCK_CS;
	if (flags & SPI_XFER_END)
		ctrl0 |= SSP_CTRL0_IGNORE_CRC;
	if (!write)
		ctrl0 |= SSP_CTRL0_READ;

	writel(length, &ssp_regs->hw_ssp_xfer_size);

	if (length % ARCH_DMA_MINALIGN)
		cache_data_count = roundup(length, ARCH_DMA_MINALIGN);
	else
		cache_data_count = length;

	if (!write) {
		slave->desc->cmd.data = MXS_DMA_DESC_COMMAND_DMA_WRITE;
		slave->desc->cmd.address = (dma_addr_t)data;
	} else {
		slave->desc->cmd.data = MXS_DMA_DESC_COMMAND_DMA_READ;
		slave->desc->cmd.address = (dma_addr_t)data;

		/* Flush data to DRAM so DMA can pick them up */
		flush_dcache_range((uint32_t)data,
			(uint32_t)(data + cache_data_count));
	}

	slave->desc->cmd.data |= MXS_DMA_DESC_IRQ | MXS_DMA_DESC_DEC_SEM |
				(length << MXS_DMA_DESC_BYTES_OFFSET) |
				(1 << MXS_DMA_DESC_PIO_WORDS_OFFSET) |
				MXS_DMA_DESC_WAIT4END;

	slave->desc->cmd.pio_words[0] = ctrl0;

	dmach = MXS_DMA_CHANNEL_AHB_APBH_SSP0 + slave->slave.bus;
	mxs_dma_desc_append(dmach, slave->desc);
	if (mxs_dma_go(dmach))
		return -EINVAL;

	/* The data arrived into DRAM, invalidate cache over them */
	if (!write) {
		invalidate_dcache_range((uint32_t)data,
			(uint32_t)(data + cache_data_count));
	}

	return 0;
}

int spi_xfer(struct spi_slave *slave, unsigned int bitlen,
		const void *dout, void *din, unsigned long flags)
{
	struct mxs_spi_slave *mxs_slave = to_mxs_slave(slave);
	struct mxs_ssp_regs *ssp_regs = mxs_slave->regs;
	int len = bitlen / 8;
	char dummy;
	int write = 0;
	char *data = NULL;

#ifdef CONFIG_MXS_SPI_DMA_ENABLE
	int dma = 1;
#else
	int dma = 0;
#endif

	if (bitlen == 0) {
		if (flags & SPI_XFER_END) {
			din = (void *)&dummy;
			len = 1;
		} else
			return 0;
	}

	/* Half-duplex only */
	if (din && dout)
		return -EINVAL;
	/* No data */
	if (!din && !dout)
		return 0;

	if (dout) {
		data = (char *)dout;
		write = 1;
	} else if (din) {
		data = (char *)din;
		write = 0;
	}

	/*
	 * Check for alignment, if the buffer is aligned, do DMA transfer,
	 * PIO otherwise. This is a temporary workaround until proper bounce
	 * buffer is in place.
	 */
	if (dma) {
		if (((uint32_t)data) & (ARCH_DMA_MINALIGN - 1))
			dma = 0;
		if (((uint32_t)len) & (ARCH_DMA_MINALIGN - 1))
			dma = 0;
	}

	if (!dma || (len < MXSSSP_SMALL_TRANSFER)) {
		writel(SSP_CTRL1_DMA_ENABLE, &ssp_regs->hw_ssp_ctrl1_clr);
		return mxs_spi_xfer_pio(mxs_slave, data, len, write, flags);
	} else {
		writel(SSP_CTRL1_DMA_ENABLE, &ssp_regs->hw_ssp_ctrl1_set);
		return mxs_spi_xfer_dma(mxs_slave, data, len, write, flags);
	}
}
