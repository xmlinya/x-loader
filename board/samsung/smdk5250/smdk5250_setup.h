/*
 * Machine Specific Values for SMDK5250 board based on S5PC520
 *
 * Copyright (C) 2011 Samsung Electronics
 *
 * See file CREDITS for list of people who contributed to this
 * project.
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
 */

#ifndef _SMDK5250_SETUP_H
#define _SMDK5250_SETUP_H

#include <config.h>
#include <version.h>
#include <asm/arch/cpu.h>

/* Offsets of clock registers (sources and dividers) */
#define CLK_SRC_CPU_OFFSET	0x00200
#define CLK_DIV_CPU0_OFFSET	0x00500
#define CLK_DIV_CPU1_OFFSET	0x00504

#define CLK_SRC_CORE0_OFFSET	0x04200
#define CLK_DIV_CORE0_OFFSET	0x04500

#define CLK_SRC_CORE1_OFFSET	0x04204
#define CLK_DIV_CORE1_OFFSET	0x04504

#define CLK_SRC_TOP0_OFFSET	0x10210
#define CLK_SRC_TOP1_OFFSET	0x10214
#define CLK_SRC_TOP2_OFFSET	0x10218
#define CLK_SRC_TOP3_OFFSET	0x1021C
#define CLK_DIV_TOP0_OFFSET	0x10510
#define CLK_DIV_TOP1_OFFSET	0x10514

#define CLK_SRC_FSYS_OFFSET	0x10244
#define CLK_DIV_FSYS0_OFFSET	0x10548
#define CLK_DIV_FSYS1_OFFSET	0x1054C
#define CLK_DIV_FSYS2_OFFSET	0x10550
#define CLK_DIV_FSYS3_OFFSET	0x10554

#define CLK_SRC_PERIC0_OFFSET	0x10250
#define CLK_DIV_PERIC0_OFFSET	0x10558
#define CLK_DIV_PERIC3_OFFSET	0x10564

#define CLK_SRC_LEX_OFFSET	0x14200
#define CLK_DIV_LEX_OFFSET	0x14500

#define CLK_DIV_R0X_OFFSET	0x18500
#define CLK_DIV_R1X_OFFSET	0x1C500


#define CLK_SRC_CDREX_OFFSET	0x20200
#define CLK_DIV_CDREX_OFFSET	0x20500
#define CLK_DIV_CDREX2_OFFSET	0x20504

#define CLK_DIV_ACP_OFFSET	0x08500
#define CLK_DIV_ISP0_OFFSET	0x0C300
#define CLK_DIV_ISP1_OFFSET	0x0C304
#define CLK_DIV_ISP2_OFFSET	0x0C308


#define APLL_LOCK_OFFSET	0x00000
#define MPLL_LOCK_OFFSET	0x04000
#define APLL_CON0_OFFSET	0x00100
#define APLL_CON1_OFFSET	0x00104
#define MPLL_CON0_OFFSET	0x04100
#define MPLL_CON1_OFFSET	0x04104

#define BPLL_LOCK_OFFSET	0x20010
#define BPLL_CON0_OFFSET	0x20110
#define BPLL_CON1_OFFSET	0x20114

#define CPLL_LOCK_OFFSET	0x10020
#define EPLL_LOCK_OFFSET	0x10030
#define VPLL_LOCK_OFFSET	0x10040

#define CPLL_CON0_OFFSET	0x10120
#define CPLL_CON1_OFFSET	0x10124

#define EPLL_CON0_OFFSET	0x10130
#define EPLL_CON1_OFFSET	0x10134
#define EPLL_CON2_OFFSET	0x10138

#define VPLL_CON0_OFFSET	0x10140
#define VPLL_CON1_OFFSET	0x10144
#define VPLL_CON2_OFFSET	0x10148


/* DMC: DRAM Controllor Register offsets */
#define DMC_CONCONTROL		0x00
#define DMC_MEMCONTROL		0x04
#define DMC_MEMCONFIG0		0x08
#define DMC_MEMCONFIG1		0x0C
#define DMC_DIRECTCMD		0x10
#define DMC_PRECHCONFIG		0x14
#define DMC_PHYCONTROL0		0x18
#define DMC_PWRDNCONFIG		0x28
#define DMC_TIMINGAREF		0x30
#define DMC_TIMINGROW		0x34
#define DMC_TIMINGDATA		0x38
#define DMC_TIMINGPOWER		0x3C
#define DMC_IVCONTROL		0xF0
#define DMC_RDLVL_CONFIG	0xF8
#define DMC_MEMBASECONFIG0	0x010C
#define DMC_MEMBASECONFIG1	0x0110
#define LPDDR3PHY_CTRL		0x20A10

#define DMC_PHY_CON0		0x00
#define DMC_PHY_CON1		0x04
#define DMC_PHY_CON10		0x28
#define DMC_PHY_CON12		0x30
#define DMC_PHY_CON14		0x38
#define DMC_PHY_CON16		0x40
#define DMC_PHY_CON2		0x08
#define DMC_PHY_CON22		0x5C
#define DMC_PHY_CON4		0x10
#define DMC_PHY_CON42		0xAC
#define DMC_PHY_CON6		0x18

/* Offset for inform registers */
#define INFORM0_OFFSET		0x800
#define INFORM1_OFFSET		0x804

/* GPIO Offsets for UART: GPIO Contol Register */
#define EXYNOS5_GPIO_A0_CON_OFFSET	0x0
#define EXYNOS5_GPIO_A1_CON_OFFSET	0x20

/* UART Register offsets */
#define ULCON_OFFSET		0x00
#define UCON_OFFSET		0x04
#define UFCON_OFFSET		0x08
#define UBRDIV_OFFSET		0x28
#define UFRACVAL_OFFSET		0x2C
#define UTXH_OFFSET		0x20

/* TZPC : Register Offsets */
#define TZPC0_BASE		0x10100000
#define TZPC1_BASE		0x10110000
#define TZPC2_BASE		0x10120000
#define TZPC3_BASE		0x10130000
#define TZPC4_BASE		0x10140000
#define TZPC5_BASE		0x10150000
#define TZPC6_BASE		0x10160000
#define TZPC7_BASE		0x10170000
#define TZPC8_BASE		0x10180000
#define TZPC9_BASE		0x10190000

#define TZPC_DECPROT0SET_OFFSET	0x804
#define TZPC_DECPROT1SET_OFFSET	0x810
#define TZPC_DECPROT2SET_OFFSET	0x81C
#define TZPC_DECPROT3SET_OFFSET	0x828

/* CLK_SRC_CPU */
/* 0 = MOUTAPLL, 1 = SCLKMPLL	*/
#define MUX_HPM_SEL		0
#define MUX_CPU_SEL		0
#define MUX_APLL_SEL		1
#define CLK_SRC_CPU_VAL		((MUX_HPM_SEL << 20) \
				| (MUX_CPU_SEL << 16) \
				| (MUX_APLL_SEL))

/* CLK_DIV_CPU0	*/
#define ARM2_RATIO		0x0
#define APLL_RATIO		0x1
#define PCLK_DBG_RATIO		0x1
#define ATB_RATIO		0x4
#define PERIPH_RATIO		0x7
#define ACP_RATIO		0x7
#define CPUD_RATIO		0x2
#define ARM_RATIO		0x0
#define CLK_DIV_CPU0_VAL	((ARM2_RATIO << 28) \
				| (APLL_RATIO << 24) \
				| (PCLK_DBG_RATIO << 20) \
				| (ATB_RATIO << 16) \
				| (PERIPH_RATIO << 12) \
				| (ACP_RATIO << 8) \
				| (CPUD_RATIO << 4) \
				| (ARM_RATIO))

/* CLK_DIV_CPU1	*/
#define HPM_RATIO		0x4
#define COPY_RATIO		0x0
#define CLK_DIV_CPU1_VAL	((HPM_RATIO << 4) \
				| (COPY_RATIO))

#define APLL_MDIV		0x7D
#define APLL_PDIV		0x3
#define APLL_SDIV		0x0

#define MPLL_MDIV		0x64
#define MPLL_PDIV		0x3
#define MPLL_SDIV		0x0

#define CPLL_MDIV		0x96
#define CPLL_PDIV		0x4
#define CPLL_SDIV		0x0

/* APLL_CON1	*/
#define APLL_CON1_VAL	(0x00203800)

/* MPLL_CON1	*/
#define MPLL_CON1_VAL	(0x00203800)

/* CPLL_CON1	*/
#define CPLL_CON1_VAL	(0x00203800)

#define EPLL_MDIV	0x60
#define EPLL_PDIV	0x3
#define EPLL_SDIV	0x3

#define EPLL_CON1_VAL	0x00000000
#define EPLL_CON2_VAL	0x00000080

#define VPLL_MDIV	0x96
#define VPLL_PDIV	0x3
#define VPLL_SDIV	0x2

#define VPLL_CON1_VAL	0x00000000
#define VPLL_CON2_VAL	0x00000080

#define BPLL_MDIV	0x215
#define BPLL_PDIV	0xC
#define BPLL_SDIV	0x1

#define BPLL_CON1_VAL	0x00203800

/* Set PLL */
#define set_pll(mdiv, pdiv, sdiv)	(1<<31 | mdiv<<16 | pdiv<<8 | sdiv)

#define APLL_CON0_VAL	set_pll(APLL_MDIV, APLL_PDIV, APLL_SDIV)
#define MPLL_CON0_VAL	set_pll(MPLL_MDIV, MPLL_PDIV, MPLL_SDIV)
#define CPLL_CON0_VAL	set_pll(CPLL_MDIV, CPLL_PDIV, CPLL_SDIV)
#define EPLL_CON0_VAL	set_pll(EPLL_MDIV, EPLL_PDIV, EPLL_SDIV)
#define VPLL_CON0_VAL	set_pll(VPLL_MDIV, VPLL_PDIV, VPLL_SDIV)
#define BPLL_CON0_VAL	set_pll(BPLL_MDIV, BPLL_PDIV, BPLL_SDIV)

/* CLK_SRC_CORE0 */
#define CLK_SRC_CORE0_VAL	0x00060000

/* CLK_SRC_CORE1 */
#define CLK_SRC_CORE1_VAL	0x100

/* CLK_DIV_CORE0 */
#define CLK_DIV_CORE0_VAL	0x120000

/* CLK_DIV_CORE1 */
#define CLK_DIV_CORE1_VAL	0x07070700

/* CLK_SRC_CDREX */
#define CLK_SRC_CDREX_VAL	0x111

/* CLK_DIV_CDREX */
#define MCLK_CDREX2_RATIO	0x0
#define ACLK_EFCON_RATIO	0x1
#define MCLK_DPHY_RATIO		0x0
#define MCLK_CDREX_RATIO	0x0
#define ACLK_C2C_200_RATIO	0x1
#define C2C_CLK_400_RATIO	0x1
#define PCLK_CDREX_RATIO	0x3
#define ACLK_CDREX_RATIO	0x1
#define CLK_DIV_CDREX_VAL	((MCLK_DPHY_RATIO << 20) \
				| (MCLK_CDREX_RATIO << 16) \
				| (ACLK_C2C_200_RATIO << 12) \
				| (C2C_CLK_400_RATIO << 8) \
				| (PCLK_CDREX_RATIO << 4) \
				| (ACLK_CDREX_RATIO))

#define MCLK_EFPHY_RATIO	0x4
#define CLK_DIV_CDREX2_VAL	MCLK_EFPHY_RATIO

/* CLK_DIV_ACP */
#define CLK_DIV_ACP_VAL	0x12

/* CLK_SRC_TOP0	*/
#define MUX_ACLK_300_GSCL_SEL		0x1
#define MUX_ACLK_300_GSCL_MID_SEL	0x0
#define MUX_ACLK_400_SEL		0x0
#define MUX_ACLK_333_SEL		0x0
#define MUX_ACLK_300_DISP1_SEL		0x1
#define MUX_ACLK_300_DISP1_MID_SEL	0x0
#define MUX_ACLK_200_SEL		0x0
#define MUX_ACLK_166_SEL		0x0
#define CLK_SRC_TOP0_VAL	((MUX_ACLK_300_GSCL_SEL << 25) \
				| (MUX_ACLK_300_GSCL_MID_SEL << 24) \
				| (MUX_ACLK_400_SEL << 20) \
				| (MUX_ACLK_333_SEL << 16) \
				| (MUX_ACLK_300_DISP1_SEL << 15) \
				| (MUX_ACLK_300_DISP1_MID_SEL << 14)	\
				| (MUX_ACLK_200_SEL << 12) \
				| (MUX_ACLK_166_SEL << 8))

/* CLK_SRC_TOP1	*/
#define MUX_ACLK_400_ISP_SEL		0x0
#define MUX_ACLK_400_IOP_SEL		0x0
#define MUX_ACLK_MIPI_HSI_TXBASE_SEL	0x0
#define CLK_SRC_TOP1_VAL		((MUX_ACLK_400_ISP_SEL << 24) \
					|(MUX_ACLK_400_IOP_SEL << 20) \
					|(MUX_ACLK_MIPI_HSI_TXBASE_SEL << 16))

/* CLK_SRC_TOP2 */
#define MUX_BPLL_USER_SEL	0x1
#define MUX_MPLL_USER_SEL	0x1
#define MUX_VPLL_SEL		0x0
#define MUX_EPLL_SEL		0x0
#define MUX_CPLL_SEL		0x0
#define VPLLSRC_SEL		0x0
#define CLK_SRC_TOP2_VAL	((MUX_BPLL_USER_SEL << 24) \
				| (MUX_MPLL_USER_SEL << 20) \
				| (MUX_VPLL_SEL << 16) \
				| (MUX_EPLL_SEL << 12) \
				| (MUX_CPLL_SEL << 8) \
				| (VPLLSRC_SEL))
/* CLK_SRC_TOP3 */
#define MUX_ACLK_333_SUB_SEL		0x1
#define MUX_ACLK_400_SUB_SEL		0x1
#define MUX_ACLK_266_ISP_SUB_SEL	0x1
#define MUX_ACLK_266_GPS_SUB_SEL	0x1
#define MUX_ACLK_300_GSCL_SUB_SEL	0x1
#define MUX_ACLK_266_GSCL_SUB_SEL	0x1
#define MUX_ACLK_300_DISP1_SUB_SEL	0x1
#define MUX_ACLK_200_DISP1_SUB_SEL	0x1
#define CLK_SRC_TOP3_VAL		((MUX_ACLK_333_SUB_SEL << 24) \
					| (MUX_ACLK_400_SUB_SEL << 20) \
					| (MUX_ACLK_266_ISP_SUB_SEL << 16) \
					| (MUX_ACLK_266_GPS_SUB_SEL << 12) \
					| (MUX_ACLK_300_GSCL_SUB_SEL << 10) \
					| (MUX_ACLK_266_GSCL_SUB_SEL << 8) \
					| (MUX_ACLK_300_DISP1_SUB_SEL << 6) \
					| (MUX_ACLK_200_DISP1_SUB_SEL << 4))

/* CLK_DIV_TOP0	*/
#define ACLK_300_RATIO		0x0
#define ACLK_400_RATIO		0x3
#define ACLK_333_RATIO		0x2
#define ACLK_266_RATIO		0x2
#define ACLK_200_RATIO		0x3
#define ACLK_166_RATIO		0x5
#define ACLK_133_RATIO		0x1
#define ACLK_66_RATIO		0x5
#define CLK_DIV_TOP0_VAL	((ACLK_300_RATIO << 28) \
				| (ACLK_400_RATIO << 24) \
				| (ACLK_333_RATIO << 20) \
				| (ACLK_266_RATIO << 16) \
				| (ACLK_200_RATIO << 12) \
				| (ACLK_166_RATIO << 8) \
				| (ACLK_133_RATIO << 4) \
				| (ACLK_66_RATIO))

/* CLK_DIV_TOP1	*/
#define ACLK_MIPI_HSI_TX_BASE_RATIO	0x3
#define ACLK_66_PRE_RATIO	0x1
#define ACLK_400_ISP_RATIO	0x1
#define ACLK_400_IOP_RATIO	0x1
#define ACLK_300_GSCL_RATIO	0x0
#define ACLK_266_GPS_RATIO	0x7

#define CLK_DIV_TOP1_VAL	((ACLK_MIPI_HSI_TX_BASE_RATIO << 28) \
				| (ACLK_66_PRE_RATIO << 24) \
				| (ACLK_400_ISP_RATIO << 20) \
				| (ACLK_400_IOP_RATIO << 16) \
				| (ACLK_300_GSCL_RATIO << 12) \
				| (ACLK_266_GPS_RATIO << 8))

/* APLL_LOCK	*/
#define APLL_LOCK_VAL		(0x3E8)
/* MPLL_LOCK	*/
#define MPLL_LOCK_VAL		(0x2F1)
/* CPLL_LOCK	*/
#define CPLL_LOCK_VAL		(0x3E8)
/* EPLL_LOCK	*/
#define EPLL_LOCK_VAL		(0x2321)
/* VPLL_LOCK	*/
#define VPLL_LOCK_VAL		(0x2321)
/* BPLL_LOCK	*/
#define BPLL_LOCK_VAL		(0x3E8)

/* CLK_SRC_PERIC0 */
/* SRC_CLOCK = SCLK_MPLL */
#define PWM_SEL			0
#define UART4_SEL		6
#define UART3_SEL		6
#define UART2_SEL		6
#define UART1_SEL		6
#define UART0_SEL		6
#define CLK_SRC_PERIC0_VAL	((PWM_SEL << 24) \
				| (UART4_SEL << 16) \
				| (UART3_SEL << 12) \
				| (UART2_SEL << 8) \
				| (UART1_SEL << 4) \
				| (UART0_SEL << 0))

#define CLK_SRC_FSYS_VAL	0x66666
#define CLK_DIV_FSYS0_VAL	0x0BB00000
#define CLK_DIV_FSYS1_VAL	0x000f000f
#define CLK_DIV_FSYS2_VAL	0x020f020f
#define CLK_DIV_FSYS3_VAL	0x000f

/* CLK_DIV_PERIC0	*/
#define UART5_RATIO		8
#define UART4_RATIO		8
#define UART3_RATIO		8
#define UART2_RATIO		8
#define UART1_RATIO		8
#define UART0_RATIO		8
#define CLK_DIV_PERIC0_VAL	((UART4_RATIO << 16) \
				| (UART3_RATIO << 12) \
				| (UART2_RATIO << 8) \
				| (UART1_RATIO << 4) \
				| (UART0_RATIO << 0))

/* CLK_DIV_PERIC3	*/
#define PWM_RATIO		8
#define CLK_DIV_PERIC3_VAL	(PWM_RATIO << 0)

/* CLK_SRC_LEX */
#define CLK_SRC_LEX_VAL		0x0

/* CLK_DIV_LEX */
#define CLK_DIV_LEX_VAL		0x10

/* CLK_DIV_R0X */
#define CLK_DIV_R0X_VAL		0x10

/* CLK_DIV_L0X */
#define CLK_DIV_R1X_VAL		0x10

/* SCLK_SRC_ISP */
#define SCLK_SRC_ISP_VAL	0x600
/* CLK_DIV_ISP0 */
#define CLK_DIV_ISP0_VAL	0x31

/* CLK_DIV_ISP1 */
#define CLK_DIV_ISP1_VAL	0x0

/* CLK_DIV_ISP2 */
#define CLK_DIV_ISP2_VAL	0x1

#define MPLL_DEC	(MPLL_MDIV * MPLL_MDIV / (MPLL_PDIV * 2^(MPLL_SDIV-1)))

#define DISABLE			0
#define ENABLE			1

/*
 * UART GPIO_A0/GPIO_A1 Control Register Value
 * 0x2: UART Function
 */
#define EXYNOS5_GPIO_A0_CON_VAL	0x22222222
#define EXYNOS5_GPIO_A1_CON_VAL	0x222222

/* ULCON: UART Line Control Value 8N1 */
#define WORD_LEN_5_BIT		0x00
#define WORD_LEN_6_BIT		0x01
#define WORD_LEN_7_BIT		0x02
#define WORD_LEN_8_BIT		0x03

#define STOP_BIT_1		0x00
#define STOP_BIT_2		0x01

#define NO_PARITY			0x00
#define ODD_PARITY			0x4
#define EVEN_PARITY			0x5
#define FORCED_PARITY_CHECK_AS_1	0x6
#define FORCED_PARITY_CHECK_AS_0	0x7

#define INFRAMODE_NORMAL		0x00
#define INFRAMODE_INFRARED		0x01

#define ULCON_VAL		((INFRAMODE_NORMAL << 6) \
				| (NO_PARITY << 3) \
				| (STOP_BIT_1 << 2) \
				| (WORD_LEN_8_BIT << 0))

/*
 * UCON: UART Control Value
 * Tx_interrupt Type: Level
 * Rx_interrupt Type: Level
 * Rx Timeout Enabled: Yes
 * Rx-Error Atatus_Int Enable: Yes
 * Loop_Back: No
 * Break Signal: No
 * Transmit mode : Interrupt request/polling
 * Receive mode : Interrupt request/polling
 */
#define TX_PULSE_INTERRUPT	0
#define TX_LEVEL_INTERRUPT	1
#define RX_PULSE_INTERRUPT	0
#define RX_LEVEL_INTERRUPT	1

#define RX_TIME_OUT		ENABLE
#define RX_ERROR_STATE_INT_ENB	ENABLE
#define LOOP_BACK		DISABLE
#define BREAK_SIGNAL		DISABLE

#define TX_MODE_DISABLED	0X00
#define TX_MODE_IRQ_OR_POLL	0X01
#define TX_MODE_DMA		0X02

#define RX_MODE_DISABLED	0X00
#define RX_MODE_IRQ_OR_POLL	0X01
#define RX_MODE_DMA		0X02

#define UCON_VAL		((TX_LEVEL_INTERRUPT << 9) \
				| (RX_LEVEL_INTERRUPT << 8) \
				| (RX_TIME_OUT << 7) \
				| (RX_ERROR_STATE_INT_ENB << 6) \
				| (LOOP_BACK << 5) \
				| (BREAK_SIGNAL << 4) \
				| (TX_MODE_IRQ_OR_POLL << 2) \
				| (RX_MODE_IRQ_OR_POLL << 0))

/*
 * UFCON: UART FIFO Control Value
 * Tx FIFO Trigger LEVEL: 2 Bytes (001)
 * Rx FIFO Trigger LEVEL: 2 Bytes (001)
 * Tx Fifo Reset: No
 * Rx Fifo Reset: No
 * FIFO Enable: Yes
 */
#define TX_FIFO_TRIGGER_LEVEL_0_BYTES	0x00
#define TX_FIFO_TRIGGER_LEVEL_2_BYTES	0x1
#define TX_FIFO_TRIGGER_LEVEL_4_BYTES	0x2
#define TX_FIFO_TRIGGER_LEVEL_6_BYTES	0x3
#define TX_FIFO_TRIGGER_LEVEL_8_BYTES	0x4
#define TX_FIFO_TRIGGER_LEVEL_10_BYTES	0x5
#define TX_FIFO_TRIGGER_LEVEL_12_BYTES	0x6
#define TX_FIFO_TRIGGER_LEVEL_14_BYTES	0x7

#define RX_FIFO_TRIGGER_LEVEL_2_BYTES	0x0
#define RX_FIFO_TRIGGER_LEVEL_4_BYTES	0x1
#define RX_FIFO_TRIGGER_LEVEL_6_BYTES	0x2
#define RX_FIFO_TRIGGER_LEVEL_8_BYTES	0x3
#define RX_FIFO_TRIGGER_LEVEL_10_BYTES	0x4
#define RX_FIFO_TRIGGER_LEVEL_12_BYTES	0x5
#define RX_FIFO_TRIGGER_LEVEL_14_BYTES	0x6
#define RX_FIFO_TRIGGER_LEVEL_16_BYTES	0x7

#define TX_FIFO_TRIGGER_LEVEL		TX_FIFO_TRIGGER_LEVEL_2_BYTES
#define RX_FIFO_TRIGGER_LEVEL		RX_FIFO_TRIGGER_LEVEL_4_BYTES
#define TX_FIFO_RESET			DISABLE
#define RX_FIFO_RESET			DISABLE
#define FIFO_ENABLE			ENABLE
#define UFCON_VAL			((TX_FIFO_TRIGGER_LEVEL << 8) \
					| (RX_FIFO_TRIGGER_LEVEL << 4) \
					| (TX_FIFO_RESET << 2) \
					| (RX_FIFO_RESET << 1) \
					| (FIFO_ENABLE << 0))
/*
 * Baud Rate Division Value
 * 115200 BAUD:
 * UBRDIV_VAL = SCLK_UART/((115200 * 16) - 1)
 * UBRDIV_VAL = (800 MHz)/((115200 * 16) - 1)
 */
#define UBRDIV_VAL		0x2F

/*
 * Fractional Part of Baud Rate Divisor:
 * 115200 BAUD:
 * UBRFRACVAL = ((((SCLK_UART*10/(115200*16) -10))%10)*16/10)
 * UBRFRACVAL = ((((800MHz*10/(115200*16) -10))%10)*16/10)
 */
#define UFRACVAL_VAL		0x3

/*
 * TZPC Register Value :
 * R0SIZE: 0x0 : Size of secured ram
 */
#define R0SIZE			0x0

/*
 * TZPC Decode Protection Register Value :
 * DECPROTXSET: 0xFF : Set Decode region to non-secure
 */
#define DECPROTXSET		0xFF
#endif
