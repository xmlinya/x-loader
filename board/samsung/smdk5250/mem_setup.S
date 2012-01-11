/*
 * Memory setup for SMDK5250 board based on EXYNOS5
 *
 * (C) Copyright 2011 Samsung Electronics Co. Ltd
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <config.h>
#include "smdk5250_setup.h"
/* APLL : 1GHz */
/* MCLK_CDREX: MCLK_CDREX_533*/
/* LPDDR support: LPDDR2 */

/* (Memory Interleaving Size = 1 << IV_SIZE) */
#define CONFIG_IV_SIZE 0x07
#define RD_LVL		1

	.globl mem_ctrl_asm_init
mem_ctrl_asm_init:
	push	{lr}

	/* CLK_DIV_DMC0 on iROM DMC=50MHz for Init DMC */
	ldr	r0, =EXYNOS5_CLOCK_BASE

	/* Reset PHY Controllor */
	ldr	r1, =0x0
	ldr	r2, =LPDDR3PHY_CTRL
	str	r1, [r0, r2]

	bl	delay

	/*set Read Latance and Burst Length for PHY0 and PHY1 */
	ldr	r0, =EXYNOS5_PHY0_CTRL_BASE
	ldr	r1, =0x408
	str	r1, [r0, #DMC_PHY_CON42]
	ldr	r0, =EXYNOS5_PHY1_CTRL_BASE
	ldr	r1, =0x408
	str	r1, [r0, #DMC_PHY_CON42]

	/*
	 * ZQ Calibration:
	 * Select Driver Strength,
	 * long calibration for manual calibration
	 */
	ldr	r0, =EXYNOS5_PHY0_CTRL_BASE
	ldr	r1, =0x0DA40304
	str	r1, [r0, #DMC_PHY_CON16]
	ldr	r0, =EXYNOS5_PHY1_CTRL_BASE
	ldr	r1, =0x0DA40304
	str	r1, [r0, #DMC_PHY_CON16]

	/* Enable termination */
	ldr	r0, =EXYNOS5_PHY0_CTRL_BASE
	ldr	r1, =0x0DAC0304
	str	r1, [r0, #DMC_PHY_CON16]
	ldr	r0, =EXYNOS5_PHY1_CTRL_BASE
	ldr	r1, =0x0DAC0304
	str	r1, [r0, #DMC_PHY_CON16]

	/* Start Manual Calibration */
	ldr	r0, =EXYNOS5_PHY0_CTRL_BASE
	ldr	r1, =0x0DAC0306
	str	r1, [r0, #DMC_PHY_CON16]
	ldr	r0, =EXYNOS5_PHY1_CTRL_BASE
	ldr	r1, =0x0DAC0306
	str	r1, [r0, #DMC_PHY_CON16]

	bl	delay

	/* Enable termination */
	ldr	r0, =EXYNOS5_PHY0_CTRL_BASE
	ldr	r1, =0x0DAC0304
	str	r1, [r0, #DMC_PHY_CON16]
	ldr	r0, =EXYNOS5_PHY1_CTRL_BASE
	ldr	r1, =0x0DAC0304
	str	r1, [r0, #DMC_PHY_CON16]

	/* DDR Mode: LPDDR2 */
	ldr	r0, =EXYNOS5_PHY0_CTRL_BASE
	ldr	r1, =0x17021240
	str	r1, [r0, #DMC_PHY_CON0]
	ldr	r0, =EXYNOS5_PHY1_CTRL_BASE
	ldr	r1, =0x17021240
	str	r1, [r0, #DMC_PHY_CON0]

	/* DQS, DQ: Signal, for LPDDR2: Always Set */
	ldr	r0, =EXYNOS5_PHY0_CTRL_BASE
	ldr	r1, =0x00000F0F
	str	r1, [r0, #DMC_PHY_CON14]
	ldr	r0, =EXYNOS5_PHY1_CTRL_BASE
	ldr	r1, =0x00000F0F
	str	r1, [r0, #DMC_PHY_CON14]

	/* RD_FETCH: 1 */
	ldr	r0, =EXYNOS5_DMC_CTRL_BASE
	ldr	r1, =0x1FFF1000
	str	r1, [r0, #DMC_CONCONTROL]
	bl	delay

	ldr	r0, =EXYNOS5_DMC_CTRL_BASE
	ldr	r1, =0x0FFF1000
	str	r1, [r0, #DMC_CONCONTROL]

	bl	delay
	/*
	 * Update DLL Information:
	 * Force DLL Resyncronization
	 */
	ldr	r0, =EXYNOS5_DMC_CTRL_BASE
	ldr	r1, =0x00000008
	str	r1, [r0, #DMC_PHYCONTROL0]

	/* Reset Force DLL Resyncronization */
	ldr	r0, =EXYNOS5_DMC_CTRL_BASE
	ldr	r1, =0x00000000
	str	r1, [r0, #DMC_PHYCONTROL0]

	/*
	 * Dynamic Clock: Always Running
	 * Memory Burst length: 4
	 * Number of chips: 2
	 * Memory Bus width: 32 bit
	 * Memory Type: LPDDR2-S4
	 * Additional Latancy for PLL: 1 Cycle
	 */
	ldr	r1, =0x00212500
	str	r1, [r0, #DMC_MEMCONTROL]

	/*
	 * Memory Configuration Chip 0
	 * Address Mapping: Interleaved
	 * Number of Column address Bits: 10 bits
	 * Number of Rows Address Bits: 14
	 * Number of Banks: 8
	 */
	ldr	r1, =0x00001323
	str	r1, [r0, #DMC_MEMCONFIG0]

	/*
	 * Memory Configuration Chip 1
	 * Address Mapping: Interleaved
	 * Number of Column address Bits: 10 bits
	 * Number of Rows Address Bits: 14
	 * Number of Banks: 8
	 */
	ldr	r1, =0x00001323
	str	r1, [r0, #DMC_MEMCONFIG1]

	/*
	 * Chip0: AXI
	 * AXI Base Address: 0x40000000
	 * AXI Base Address Mask: 0x780
	 */
	ldr	r1, =0x00400780
	str	r1, [r0, #DMC_MEMBASECONFIG0]

	/*
	 * Chip1: AXI
	 * AXI Base Address: 0x80000000
	 * AXI Base Address Mask: 0x780
	 */
	ldr	r1, =0x00800780
	str	r1, [r0, #DMC_MEMBASECONFIG1]

	/* Precharge Configuration */
	ldr	r1, =0xFF000000
	str	r1, [r0, #DMC_PRECHCONFIG]

	/* Power Down mode Configuration */
	ldr	r1, =0xFFFF00FF
	str	r1, [r0, #DMC_PWRDNCONFIG]

	/* Periodic Refrese Interval */
	ldr	r1, =0x0000005D
	str	r1, [r0, #DMC_TIMINGAREF]

	/* MCLK_CDREX_533 */
	/*
	 * TimingRow, TimingData, TimingPower Setting:
	 * Values as per Memory AC Parameters
	 */
	ldr	r1, =0x2336544C
	str	r1, [r0, #DMC_TIMINGROW]

	ldr	r1, =0x24202408
	str	r1, [r0, #DMC_TIMINGDATA]

	ldr	r1, =0x38260235
	str	r1, [r0, #DMC_TIMINGPOWER]

	/* Memory Channel Inteleaving Size: 128 Bytes */
	ldr	r1, =CONFIG_IV_SIZE
	str	r1, [r0, #DMC_IVCONTROL]

	/* Set Offsets to read DQS */
	ldr	r0, =EXYNOS5_PHY0_CTRL_BASE
	ldr	r1, =0x7F7F7F7F
	str	r1, [r0, #DMC_PHY_CON4]
	ldr	r0, =EXYNOS5_PHY1_CTRL_BASE
	ldr	r1, =0x7F7F7F7F
	str	r1, [r0, #DMC_PHY_CON4]

	/* Set Offsets to read DQ */
	ldr	r0, =EXYNOS5_PHY0_CTRL_BASE
	ldr	r1, =0x7F7F7F7F
	str	r1, [r0, #DMC_PHY_CON6]
	ldr	r0, =EXYNOS5_PHY1_CTRL_BASE
	ldr	r1, =0x7F7F7F7F
	str	r1, [r0, #DMC_PHY_CON6]

	/* Debug Offset */
	ldr	r0, =EXYNOS5_PHY0_CTRL_BASE
	ldr	r1, =0x0000007F
	str	r1, [r0, #DMC_PHY_CON10]
	ldr	r0, =EXYNOS5_PHY1_CTRL_BASE
	ldr	r1, =0x0000007F
	str	r1, [r0, #DMC_PHY_CON10]

	/* Start DLL Locking */
	ldr	r0, =EXYNOS5_PHY0_CTRL_BASE
	ldr	r1, =0x10107F50
	str	r1, [r0, #DMC_PHY_CON12]
	ldr	r0, =EXYNOS5_PHY1_CTRL_BASE
	ldr	r1, =0x10107F50
	str	r1, [r0, #DMC_PHY_CON12]

	bl	delay

	/*
	 * Update DLL Information:
	 * Force DLL Resyncronization
	 */
	ldr	r0, =EXYNOS5_DMC_CTRL_BASE
	ldr	r1, =0x00000008
	str	r1, [r0, #DMC_PHYCONTROL0]

	/* Reset DLL Resyncronization */
	ldr	r0, =EXYNOS5_DMC_CTRL_BASE
	ldr	r1, =0x00000000
	str	r1, [r0, #DMC_PHYCONTROL0]

	/*
	 * NOP CMD: Channel 0, Chip 0
	 * Exit from active/precharge power down or deep power down
	 */
	ldr	r0, =EXYNOS5_DMC_CTRL_BASE
	ldr	r1, =0x07000000
	str	r1, [r0, #DMC_DIRECTCMD]

	bl	delay

	/* EMRS2, EMRS3, EMRS, MRS Commands Using Direct Command */
	ldr	r1, =0x00071C00
	str	r1, [r0, #DMC_DIRECTCMD]

	bl	delay

	ldr	r1, =0x00010BFC
	str	r1, [r0, #DMC_DIRECTCMD]

	bl	delay

	/* MCLK_CDREX_533 */
	ldr	r1, =0x00000708
	str	r1, [r0, #DMC_DIRECTCMD]

	bl	delay

	ldr	r1, =0x00000818
	str	r1, [r0, #DMC_DIRECTCMD]

	bl	delay

	/*
	 * NOP CMD: Channel 0, Chip 1
	 * Exit from active/precharge power down or deep power down
	 */
	ldr	r0, =EXYNOS5_DMC_CTRL_BASE
	ldr	r1, =0x07100000
	str	r1, [r0, #DMC_DIRECTCMD]

	bl	delay

	/* EMRS2, EMRS3, EMRS, MRS Commands Using Direct Command */
	ldr	r1, =0x00171C00
	str	r1, [r0, #DMC_DIRECTCMD]

	bl	delay

	ldr	r1, =0x00110BFC
	str	r1, [r0, #DMC_DIRECTCMD]

	bl	delay

	/* MCLK_CDREX_533 */
	ldr	r1, =0x00100708
	str	r1, [r0, #DMC_DIRECTCMD]

	bl	delay

	ldr	r1, =0x00100818
	str	r1, [r0, #DMC_DIRECTCMD]

	bl	delay

	/*
	 * NOP CMD: Channel 1, Chip 0
	 * Exit from active/precharge power down or deep power down
	 */
	ldr	r0, =EXYNOS5_DMC_CTRL_BASE
	ldr	r1, =0x17000000
	str	r1, [r0, #DMC_DIRECTCMD]

	bl	delay

	/* EMRS2, EMRS3, EMRS, MRS Commands Using Direct Command */
	ldr	r1, =0x10071C00
	str	r1, [r0, #DMC_DIRECTCMD]

	bl	delay

	ldr	r1, =0x10010BFC
	str	r1, [r0, #DMC_DIRECTCMD]

	bl	delay

	/* MCLK_CDREX_533 */
	ldr	r1, =0x10000708
	str	r1, [r0, #DMC_DIRECTCMD]

	bl	delay

	ldr	r1, =0x10000818
	str	r1, [r0, #DMC_DIRECTCMD]

	bl	delay

	/*
	 * NOP CMD: Channel 1, Chip 1
	 * Exit from active/precharge power down or deep power down
	 */
	ldr	r0, =EXYNOS5_DMC_CTRL_BASE
	ldr	r1, =0x17100000
	str	r1, [r0, #DMC_DIRECTCMD]

	bl	delay

	/* EMRS2, EMRS3, EMRS, MRS Commands Using Direct Command */
	ldr	r1, =0x10171C00
	str	r1, [r0, #DMC_DIRECTCMD]

	bl	delay

	ldr	r1, =0x10110BFC
	str	r1, [r0, #DMC_DIRECTCMD]

	bl	delay

	/* MCLK_CDREX_533 */
	ldr	r1, =0x10100708
	str	r1, [r0, #DMC_DIRECTCMD]

	bl	delay

	ldr	r1, =0x10100818
	str	r1, [r0, #DMC_DIRECTCMD]

	bl	delay

	/* Reset DQS Offsets */
	ldr	r0, =EXYNOS5_PHY0_CTRL_BASE
	ldr	r1, =0x08080808
	str	r1, [r0, #DMC_PHY_CON4]
	ldr	r0, =EXYNOS5_PHY1_CTRL_BASE
	ldr	r1, =0x08080808
	str	r1, [r0, #DMC_PHY_CON4]

	/* Reset DQ Offsets */
	ldr	r0, =EXYNOS5_PHY0_CTRL_BASE
	ldr	r1, =0x08080808
	str	r1, [r0, #DMC_PHY_CON6]
	ldr	r0, =EXYNOS5_PHY1_CTRL_BASE
	ldr	r1, =0x08080808
	str	r1, [r0, #DMC_PHY_CON6]

	/* Reset debug Offsets */
	ldr	r0, =EXYNOS5_PHY0_CTRL_BASE
	ldr	r1, =0x00000008
	str	r1, [r0, #DMC_PHY_CON10]
	ldr	r0, =EXYNOS5_PHY1_CTRL_BASE
	ldr	r1, =0x00000008
	str	r1, [r0, #DMC_PHY_CON10]

	/* Set DLL Locking */
	ldr	r0, =EXYNOS5_PHY0_CTRL_BASE
	ldr	r1, =0x10107F30
	str	r1, [r0, #DMC_PHY_CON12]
	ldr	r0, =EXYNOS5_PHY1_CTRL_BASE
	ldr	r1, =0x10107F30
	str	r1, [r0, #DMC_PHY_CON12]

	bl	delay

	/* Start DLL Locking */
	ldr	r0, =EXYNOS5_PHY0_CTRL_BASE
	ldr	r1, =0x10107F70
	str	r1, [r0, #DMC_PHY_CON12]
	ldr	r0, =EXYNOS5_PHY1_CTRL_BASE
	ldr	r1, =0x10107F70
	str	r1, [r0, #DMC_PHY_CON12]

	bl	delay

	/*
	 * Update DLL Information:
	 * Force DLL Resyncronization
	 */
	ldr	r0, =EXYNOS5_DMC_CTRL_BASE
	ldr	r1, =0x00000008
	str	r1, [r0, #DMC_PHYCONTROL0]

	/* Reset DLL Resyncronization */
	ldr	r0, =EXYNOS5_DMC_CTRL_BASE
	ldr	r1, =0x00000000
	str	r1, [r0, #DMC_PHYCONTROL0]

	bl	delay

	/*
	 * Update DLL Information:
	 * Force DLL Resyncronization
	 */
	ldr	r0, =EXYNOS5_DMC_CTRL_BASE
	ldr	r1, =0x00000008
	str	r1, [r0, #DMC_PHYCONTROL0]

	/* Reset DLL Resyncronization */
	ldr	r0, =EXYNOS5_DMC_CTRL_BASE
	ldr	r1, =0x00000000
	str	r1, [r0, #DMC_PHYCONTROL0]

	bl	delay
#if defined(RD_LVL)
	/* DLL On */
	ldr	r0, =EXYNOS5_PHY0_CTRL_BASE
	ldr	r1, =0x10102D50
	str	r1, [r0, #DMC_PHY_CON12]
	ldr	r0, =EXYNOS5_PHY1_CTRL_BASE
	ldr	r1, =0x10102D50
	str	r1, [r0, #DMC_PHY_CON12]

	/*
	 * Set ctrl_gateadj, ctrl_readadj
	 * ctrl_gateduradj, rdlvl_pass_adj
	 * rdlvl_rddataPadj
	 */
	ldr	r0, =EXYNOS5_PHY0_CTRL_BASE
	ldr	r1, =0x09210001
	str	r1, [r0, #DMC_PHY_CON1]
	ldr	r0, =EXYNOS5_PHY1_CTRL_BASE
	ldr	r1, =0x09210001
	str	r1, [r0, #DMC_PHY_CON1]

	/* LPDDR2 Address */
	ldr	r0, =EXYNOS5_PHY0_CTRL_BASE
	ldr	r1, =0x00000208
	str	r1, [r0, #DMC_PHY_CON22]
	ldr	r0, =EXYNOS5_PHY1_CTRL_BASE
	ldr	r1, =0x00000208
	str	r1, [r0, #DMC_PHY_CON22]

	/* Enable Byte Read Leleling */
	ldr	r0, =EXYNOS5_PHY0_CTRL_BASE
	ldr	r1, =0x17023240
	str	r1, [r0, #DMC_PHY_CON0]
	ldr	r0, =EXYNOS5_PHY1_CTRL_BASE
	ldr	r1, =0x17023240
	str	r1, [r0, #DMC_PHY_CON0]

	/* rdlvl_en: Use levelling offset instead ctrl_shiftc */
	ldr	r0, =EXYNOS5_PHY0_CTRL_BASE
	ldr	r1, =0x02010004
	str	r1, [r0, #DMC_PHY_CON2]
	ldr	r0, =EXYNOS5_PHY1_CTRL_BASE
	ldr	r1, =0x02010004
	str	r1, [r0, #DMC_PHY_CON2]

	bl	delay

	/* Enable Data Eye Trainig */
	ldr	r0, =EXYNOS5_DMC_CTRL_BASE
	ldr	r1, =0x00000002
	str	r1, [r0, #DMC_RDLVL_CONFIG]

	bl	delay

	/* Disable Data Eye Trainig */
	ldr	r0, =EXYNOS5_DMC_CTRL_BASE
	ldr	r1, =0x00000000
	str	r1, [r0, #DMC_RDLVL_CONFIG]

	/* RdDeSkew_clear: Clear */
	ldr	r0, =EXYNOS5_PHY0_CTRL_BASE
	ldr	r1, =0x02012004
	str	r1, [r0, #DMC_PHY_CON2]
	ldr	r0, =EXYNOS5_PHY1_CTRL_BASE
	ldr	r1, =0x02012004
	str	r1, [r0, #DMC_PHY_CON2]

	/* Start DLL Locking */
	ldr	r0, =EXYNOS5_PHY0_CTRL_BASE
	ldr	r1, =0x10107F70
	str	r1, [r0, #DMC_PHY_CON12]
	ldr	r0, =EXYNOS5_PHY1_CTRL_BASE
	ldr	r1, =0x10107F70
	str	r1, [r0, #DMC_PHY_CON12]

	/* Force DLL Resyncronization */
	ldr	r0, =EXYNOS5_DMC_CTRL_BASE
	ldr	r1, =0x00000008
	str	r1, [r0, #DMC_PHYCONTROL0]

	/* Reset DLL Resyncronization */
	ldr	r0, =EXYNOS5_DMC_CTRL_BASE
	ldr	r1, =0x00000000
	str	r1, [r0, #DMC_PHYCONTROL0]

	bl	delay

	/* ctrl_atgate: ctrl_gate_p*, ctrl_read_p* generated by PHY*/
	ldr	r0, =EXYNOS5_PHY0_CTRL_BASE
	ldr	r1, =0x17023200
	str	r1, [r0, #DMC_PHY_CON0]
	ldr	r0, =EXYNOS5_PHY1_CTRL_BASE
	ldr	r1, =0x17023200
	str	r1, [r0, #DMC_PHY_CON0]

	/* Channel:0, Chip:0, PALL (all banks precharge) CMD */
	ldr	r0, =EXYNOS5_DMC_CTRL_BASE
	ldr	r1, =0x01000000
	str	r1, [r0, #DMC_DIRECTCMD]

	bl	delay

	/* Channel:0, Chip:1, PALL (all banks precharge) CMD */
	ldr	r0, =EXYNOS5_DMC_CTRL_BASE
	ldr	r1, =0x01100000
	str	r1, [r0, #DMC_DIRECTCMD]

	bl	delay

	/* Channel:1, Chip:0, PALL (all banks precharge) CMD */
	ldr	r0, =EXYNOS5_DMC_CTRL_BASE
	ldr	r1, =0x11000000
	str	r1, [r0, #DMC_DIRECTCMD]

	bl	delay

	/* Channel:1, Chip:1, PALL (all banks precharge) CMD */
	ldr	r0, =EXYNOS5_DMC_CTRL_BASE
	ldr	r1, =0x11100000
	str	r1, [r0, #DMC_DIRECTCMD]

	bl	delay
#endif
	/*
	 * Dynamic Clock: Stops During Idle Period
	 * Dynamic Power Down: Enable
	 * Dynamic Self refresh: Enable
	 * Memory Burst length: 4
	 * Number of chips: 2
	 * Memory Bus width: 32 bit
	 * Memory Type: LPDDR2-S4
	 * Additional Latancy for PLL: 1 Cycle
	 */
	ldr	r0, =EXYNOS5_DMC_CTRL_BASE
	ldr	r1, =0x00212523
	str	r1, [r0, #DMC_MEMCONTROL]

	/* Start Auto refresh */
	ldr	r0, =EXYNOS5_DMC_CTRL_BASE
	ldr	r1, =0x0FFF10E0
	str	r1, [r0, #DMC_CONCONTROL]

	pop	{lr}
	mov	pc, lr

delay:
	mov	r2, #0x10000
delayloop:
	subs	r2, r2, #1
	bne	delayloop
	mov	pc, lr
