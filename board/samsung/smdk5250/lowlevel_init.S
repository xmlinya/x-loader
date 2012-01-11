/*
 * Lowlevel setup for SMDK5250 board based on S5PC520
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

#include <config.h>
#include <version.h>
#include <asm/arch/cpu.h>
#include "smdk5250_setup.h"

/*
 * Register usages:
 *
 * r5 has zero always
 * r7 has GPIO part1 base 0x11400000
 */
_TEXT_BASE:
	.word	CONFIG_SYS_TEXT_BASE

	.globl lowlevel_init
lowlevel_init:

	/* use iROM stack in bl2 */
	ldr	sp, =0x02050000

	push {lr}

	/* r5 has always zero */
	mov	r5, #0
	ldr	r7, =EXYNOS5_GPIO_PART1_BASE

	/* check reset status */
	ldr	r0, =(EXYNOS5_POWER_BASE + INFORM1_OFFSET)
	ldr	r1, [r0]

	/* AFTR wakeup reset */
	ldr	r2, =S5P_CHECK_DIDLE
	cmp	r1, r2
	beq	exit_wakeup

	/* LPA wakeup reset */
	ldr	r2, =S5P_CHECK_LPA
	cmp	r1, r2
	beq	exit_wakeup

	/* Sleep wakeup reset */
	ldr	r2, =S5P_CHECK_SLEEP
	cmp	r1, r2
	beq	wakeup_reset

	/*
	 * If U-boot is already running in ram, no need to relocate U-Boot.
	 * Memory controller must be configured before relocating U-Boot
	 * in ram.
	 */
	ldr	r0, =0x0ffffff		/* r0 <- Mask Bits*/
	bic	r1, pc, r0		/* pc <- current addr of code */
					/* r1 <- unmasked bits of pc */
	ldr	r2, _TEXT_BASE		/* r2 <- original base addr in ram */
	bic	r2, r2, r0		/* r2 <- unmasked bits of r2*/
	cmp	r1, r2			/* compare r1, r2 */
	beq	1f			/* r0 == r1 then skip sdram init */

	/* init system clock */
	bl system_clock_init

	/* Memory initialize */
	bl mem_ctrl_asm_init

1:
	/* for UART */
	bl uart_asm_init
	bl tzpc_init
	pop	{pc}

wakeup_reset:
	bl system_clock_init
	bl mem_ctrl_asm_init
	bl tzpc_init

exit_wakeup:
	/* Load return address and jump to kernel */
	ldr	r0, =(EXYNOS5_POWER_BASE + INFORM0_OFFSET)

	/* r1 = physical address of exynos5_cpu_resume function*/
	ldr	r1, [r0]

	/* Jump to kernel */
	mov	pc, r1
	nop
	nop

/*
 * system_clock_init: Initialize core clock and bus clock.
 * void system_clock_init(void)
 */
system_clock_init:
	push	{lr}
	ldr	r0, =EXYNOS5_CLOCK_BASE

	/*
	 * MUX_APLL_SEL[0]: FINPLL = 0
	 * MUX_CPU_SEL[6]: MOUTAPLL = 0
	 * MUX_HPM_SEL[20]: MOUTAPLL = 0
	 */
	ldr	r1, =0x0
	ldr	r2, =CLK_SRC_CPU_OFFSET
	str	r1, [r0, r2]

	/* MUX_MPLL_SEL[8]: FINPLL = 0 */
	ldr	r1, =0x0
	ldr	r2, =CLK_SRC_CORE1_OFFSET
	str	r1, [r0, r2]

	/*
	 * VPLLSRC_SEL[0]: FINPLL = 0
	 * MUX_{CPLL[8]}|{EPLL[12]}|{VPLL[16]}_SEL: FINPLL = 0
	 */
	ldr	r1, =0x0
	ldr	r2, =CLK_SRC_TOP2_OFFSET
	str	r1, [r0, r2]

	/* MUX_BPLL_SEL[0]: FINPLL = 0*/
	ldr	r1, =0x0
	ldr	r2, =CLK_SRC_CDREX_OFFSET
	str	r1, [r0, r2]

	/* MUX_ACLK_* Clock Selection */
	ldr	r1, =CLK_SRC_TOP0_VAL
	ldr	r2, =CLK_SRC_TOP0_OFFSET
	str	r1, [r0, r2]

	/* MUX_ACLK_* Clock Selection */
	ldr	r1, =CLK_SRC_TOP1_VAL
	ldr	r2, =CLK_SRC_TOP1_OFFSET
	str	r1, [r0, r2]


	/* MUX_ACLK_* Clock Selection */
	ldr	r1, =CLK_SRC_TOP3_VAL
	ldr	r2, =CLK_SRC_TOP3_OFFSET
	str	r1, [r0, r2]

	/* MUX_PWI_SEL[19:16]: SCLKMPLL = 6 */
	ldr	r1, =CLK_SRC_CORE0_VAL
	ldr	r2, =CLK_SRC_CORE0_OFFSET
	str	r1, [r0, r2]

	/* MUX_ATCLK_LEX[0]: ACLK_200 = 0*/
	ldr	r1, =CLK_SRC_LEX_VAL
	ldr	r2, =CLK_SRC_LEX_OFFSET
	str	r1, [r0, r2]


	/* UART [0-5]: SCLKMPLL = 6 */
	ldr	r1, =CLK_SRC_PERIC0_VAL
	ldr	r2, =CLK_SRC_PERIC0_OFFSET
	str	r1, [r0, r2]

	/* Set Clock Ratios */
	ldr	r1, =CLK_DIV_CPU0_VAL
	ldr	r2, =CLK_DIV_CPU0_OFFSET
	str	r1, [r0, r2]

	/* Set COPY and HPM Ratio */
	ldr	r1, =CLK_DIV_CPU1_VAL
	ldr	r2, =CLK_DIV_CPU1_OFFSET
	str	r1, [r0, r2]

	/* CORED_RATIO, COREP_RATIO */
	ldr	r1, =CLK_DIV_CORE0_VAL
	ldr	r2, =CLK_DIV_CORE0_OFFSET
	str	r1, [r0, r2]

	/* PWI_RATIO[11:8], DVSEM_RATIO[22:16], DPM_RATIO[24:20] */
	ldr	r1, =CLK_DIV_CORE1_VAL
	ldr	r2, =CLK_DIV_CORE1_OFFSET
	str	r1, [r0, r2]

	/* ACLK_*_RATIO */
	ldr	r1, =CLK_DIV_TOP0_VAL
	ldr	r2, =CLK_DIV_TOP0_OFFSET
	str	r1, [r0, r2]

	/* ACLK_*_RATIO */
	ldr	r1, =CLK_DIV_TOP1_VAL
	ldr	r2, =CLK_DIV_TOP1_OFFSET
	str	r1, [r0, r2]

	/* CDREX Ratio */
	ldr	r1, =CLK_DIV_CDREX_VAL
	ldr	r2, =CLK_DIV_CDREX_OFFSET
	str	r1, [r0, r2]

	/* MCLK_EFPHY_RATIO[3:0] */
	ldr	r1, =CLK_DIV_CDREX2_VAL
	ldr	r2, =CLK_DIV_CDREX2_OFFSET
	str	r1, [r0, r2]

	/* {PCLK[4:6]|ATCLK[10:8]}_RATIO */
	ldr	r1, =CLK_DIV_LEX_VAL
	ldr	r2, =CLK_DIV_LEX_OFFSET
	str	r1, [r0, r2]

	/* PCLK_R0X_RATIO[3:0] */
	ldr	r1, =CLK_DIV_R0X_VAL
	ldr	r2, =CLK_DIV_R0X_OFFSET
	str	r1, [r0, r2]

	/* PCLK_R1X_RATIO[3:0] */
	ldr	r1, =CLK_DIV_R1X_VAL
	ldr	r2, =CLK_DIV_R1X_OFFSET
	str	r1, [r0, r2]

	/* SATA[24]: SCLKMPLL=0, MMC[0-4]: SCLKMPLL = 6 */
	ldr	r1, =CLK_SRC_FSYS_VAL
	ldr	r2, =CLK_SRC_FSYS_OFFSET
	str	r1, [r0, r2]

	/* UART[0-4] */
	ldr	r1, =CLK_DIV_PERIC0_VAL
	ldr	r2, =CLK_DIV_PERIC0_OFFSET
	str	r1, [r0, r2]

	/* PWM_RATIO[3:0] */
	ldr	r1, =CLK_DIV_PERIC3_VAL
	ldr	r2, =CLK_DIV_PERIC3_OFFSET
	str	r1, [r0, r2]

	/* SATA_RATIO, USB_DRD_RATIO */
	ldr	r1, =CLK_DIV_FSYS0_VAL
	ldr	r2, =CLK_DIV_FSYS0_OFFSET
	str	r1, [r0, r2]

	/* MMC[0-1] */
	ldr	r1, =CLK_DIV_FSYS1_VAL
	ldr	r2, =CLK_DIV_FSYS1_OFFSET
	str	r1, [r0, r2]

	/* MMC[2-3] */
	ldr	r1, =CLK_DIV_FSYS2_VAL
	ldr	r2, =CLK_DIV_FSYS2_OFFSET
	str	r1, [r0, r2]

	/* MMC[4] */
	ldr	r1, =CLK_DIV_FSYS3_VAL
	ldr	r2, =CLK_DIV_FSYS3_OFFSET
	str	r1, [r0, r2]

	/* ACLK|PLCK_ACP_RATIO */
	ldr	r1, =CLK_DIV_ACP_VAL
	ldr	r2, =CLK_DIV_ACP_OFFSET
	str	r1, [r0, r2]

	/* ISPDIV0_RATIO, ISPDIV1_RATIO */
	ldr	r1, =CLK_DIV_ISP0_VAL
	ldr	r2, =CLK_DIV_ISP0_OFFSET
	str	r1, [r0, r2]

	/* MCUISPDIV0_RATIO, MCUISPDIV1_RATIO */
	ldr	r1, =CLK_DIV_ISP1_VAL
	ldr	r2, =CLK_DIV_ISP1_OFFSET
	str	r1, [r0, r2]

	/* MPWMDIV_RATIO */
	ldr	r1, =CLK_DIV_ISP2_VAL
	ldr	r2, =CLK_DIV_ISP2_OFFSET
	str	r1, [r0, r2]

	/* PLL locktime */
	ldr	r1, =APLL_LOCK_VAL
	ldr	r2, =APLL_LOCK_OFFSET
	str	r1, [r0, r2]

	ldr	r1, =MPLL_LOCK_VAL
	ldr	r2, =MPLL_LOCK_OFFSET
	str	r1, [r0, r2]

	ldr	r1, =BPLL_LOCK_VAL
	ldr	r2, =BPLL_LOCK_OFFSET
	str	r1, [r0, r2]

	ldr	r1, =CPLL_LOCK_VAL
	ldr	r2, =CPLL_LOCK_OFFSET
	str	r1, [r0, r2]

	ldr	r1, =EPLL_LOCK_VAL
	ldr	r2, =EPLL_LOCK_OFFSET
	str	r1, [r0, r2]

	ldr	r1, =VPLL_LOCK_VAL
	ldr	r2, =VPLL_LOCK_OFFSET
	str	r1, [r0, r2]

	mov	r1, #0x10000
2:	subs	r1, r1, #1
	bne	2b


	/* Set APLL */
	ldr	r1, =APLL_CON1_VAL
	ldr	r2, =APLL_CON1_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =APLL_CON0_VAL
	ldr	r2, =APLL_CON0_OFFSET
	str	r1, [r0, r2]

	mov	r1, #0x30000
3:	subs	r1, r1, #1
	bne	3b

	/* Set MPLL */
	ldr	r1, =MPLL_CON1_VAL
	ldr	r2, =MPLL_CON1_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =MPLL_CON0_VAL
	ldr	r2, =MPLL_CON0_OFFSET
	str	r1, [r0, r2]

	/* wait ?us */
	mov	r1, #0x30000
4:	subs	r1, r1, #1
	bne	4b

	/* Set BPLL */
	ldr	r1, =BPLL_CON1_VAL
	ldr	r2, =BPLL_CON1_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =BPLL_CON0_VAL
	ldr	r2, =BPLL_CON0_OFFSET
	str	r1, [r0, r2]

	/* wait ?us */
	mov	r1, #0x30000
5:	subs	r1, r1, #1
	bne	5b

	/* Set CPLL */
	ldr	r1, =CPLL_CON1_VAL
	ldr	r2, =CPLL_CON1_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =CPLL_CON0_VAL
	ldr	r2, =CPLL_CON0_OFFSET
	str	r1, [r0, r2]

	/* wait ?us */
	mov	r1, #0x30000
6:	subs	r1, r1, #1
	bne	6b

	/* Set EPLL */
	ldr	r1, =EPLL_CON2_VAL
	ldr	r2, =EPLL_CON2_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =EPLL_CON1_VAL
	ldr	r2, =EPLL_CON1_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =EPLL_CON0_VAL
	ldr	r2, =EPLL_CON0_OFFSET
	str	r1, [r0, r2]

	/* wait ?us */
	mov	r1, #0x30000
7:	subs	r1, r1, #1
	bne	7b

	/* Set VPLL */
	ldr	r1, =VPLL_CON2_VAL
	ldr	r2, =VPLL_CON2_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =VPLL_CON1_VAL
	ldr	r2, =VPLL_CON1_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =VPLL_CON0_VAL
	ldr	r2, =VPLL_CON0_OFFSET
	str	r1, [r0, r2]

	/* wait ?us */
	mov	r1, #0x30000
8:	subs	r1, r1, #1
	bne	8b

	/* After Initiallising th PLL select the sources accordingly */
	/* MUX_APLL_SEL[0]: MOUTAPLLFOUT = 1 */
	ldr	r1, =CLK_SRC_CPU_VAL
	ldr	r2, =CLK_SRC_CPU_OFFSET
	str	r1, [r0, r2]

	/* MUX_MPLL_SEL[8]: MOUTMPLLFOUT = 1 */
	ldr	r1, =CLK_SRC_CORE1_VAL
	ldr	r2, =CLK_SRC_CORE1_OFFSET
	str	r1, [r0, r2]

	/* MUX_BPLL_SEL[0]: FOUTBPLL = 1*/
	ldr	r1, =CLK_SRC_CDREX_VAL
	ldr	r2, =CLK_SRC_CDREX_OFFSET
	str	r1, [r0, r2]

	/*
	 * VPLLSRC_SEL[0]: FINPLL = 0
	 * MUX_{CPLL[8]}|{EPLL[12]}|{VPLL[16]}_SEL: MOUT{CPLL|EPLL|VPLL} = 1
	 * MUX_{MPLL[20]}|{BPLL[24]}_USER_SEL: FOUT{MPLL|BPLL} = 1
	 */
	ldr	r1, =CLK_SRC_TOP2_VAL
	ldr	r2, =CLK_SRC_TOP2_OFFSET
	str	r1, [r0, r2]

	pop	{pc}


/*
 * uart_asm_init: Initialize UART in asm mode, 115200bps fixed.
 * void uart_asm_init(void)
 */
	.globl uart_asm_init
uart_asm_init:

	/* setup UART0-UART3 GPIOs (part1) */
	mov	r0, r7
	ldr	r1, =EXYNOS5_GPIO_A0_CON_VAL
	str	r1, [r0, #EXYNOS5_GPIO_A0_CON_OFFSET]
	ldr	r1, =EXYNOS5_GPIO_A1_CON_VAL
	str	r1, [r0, #EXYNOS5_GPIO_A1_CON_OFFSET]

	ldr r0, =EXYNOS5_UART_BASE
	add r0, r0, #EXYNOS5_DEFAULT_UART_OFFSET

	ldr	r1, =ULCON_VAL
	str	r1, [r0, #ULCON_OFFSET]
	ldr	r1, =UCON_VAL
	str	r1, [r0, #UCON_OFFSET]
	ldr	r1, =UFCON_VAL
	str	r1, [r0, #UFCON_OFFSET]
	ldr	r1, =UBRDIV_VAL
	str	r1, [r0, #UBRDIV_OFFSET]
	ldr	r1, =UFRACVAL_VAL
	str	r1, [r0, #UFRACVAL_OFFSET]
	ldr	r1, =0x4f
	str	r1, [r0, #UTXH_OFFSET]		@'O'
	mov	pc, lr
	nop
	nop
	nop

/* Setting TZPC[TrustZone Protection Controller] */
tzpc_init:

	ldr	r0, =TZPC0_BASE
	mov	r1, #R0SIZE
	str	r1, [r0]
	mov	r1, #DECPROTXSET
	str	r1, [r0, #TZPC_DECPROT0SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT1SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT2SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT3SET_OFFSET]

	ldr	r0, =TZPC1_BASE
	str	r1, [r0, #TZPC_DECPROT0SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT1SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT2SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT3SET_OFFSET]

	ldr	r0, =TZPC2_BASE
	str	r1, [r0, #TZPC_DECPROT0SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT1SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT2SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT3SET_OFFSET]

	ldr	r0, =TZPC3_BASE
	str	r1, [r0, #TZPC_DECPROT0SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT1SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT2SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT3SET_OFFSET]

	ldr	r0, =TZPC4_BASE
	str	r1, [r0, #TZPC_DECPROT0SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT1SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT2SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT3SET_OFFSET]

	ldr	r0, =TZPC5_BASE
	str	r1, [r0, #TZPC_DECPROT0SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT1SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT2SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT3SET_OFFSET]

	ldr	r0, =TZPC6_BASE
	str	r1, [r0, #TZPC_DECPROT0SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT1SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT2SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT3SET_OFFSET]

	ldr	r0, =TZPC7_BASE
	str	r1, [r0, #TZPC_DECPROT0SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT1SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT2SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT3SET_OFFSET]

	ldr	r0, =TZPC8_BASE
	str	r1, [r0, #TZPC_DECPROT0SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT1SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT2SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT3SET_OFFSET]

	ldr	r0, =TZPC9_BASE
	str	r1, [r0, #TZPC_DECPROT0SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT1SET_OFFSET]

	mov	pc, lr
