/*
 * (C) Copyright 2008-2011 STMicroelectronics.
 *
 * Sean McGoogan <Sean.McGoogan@st.com>
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

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 * (easy to change)
 */

#define CONFIG_SH4	1		/* This is an SH4 CPU		*/
#define CONFIG_CPU_SUBTYPE_SH4_3XX	/* it is an SH4-300		*/


/*-----------------------------------------------------------------------
 *	Switch settings to select between the SoC's main boot-modes:
 *		a) boot from 16-bit NOR flash
 *		b) boot from SPI serial flash
 *
 *	Jumper	NOR	SPI
 *	------	---	---
 *	SW1-4	 ON	off		MODE[5]
 *	SW1-3	off	 ON		MODE[4]
 *	SW1-2	 ON	off		MODE[3]
 *	SW1-1	off	 ON		MODE[2]
 *
 *	Note: There is no NAND flash on the MB903.
 */


/*-----------------------------------------------------------------------
 * Are we booting directly from a SPI Serial Flash device ?
 * If so, then define the "CFG_BOOT_FROM_SPI" macro,
 * otherwise (e.g. for NOR Flash booting), do not define it.
 *
 * Note: when in boot-from-SPI, the lower 64MiB of the NOR
 * flash is inaccessible, only the upper 64MiB is accessible.
 * This assumes that the EMI is configured as follows:
 *	sizeof(Bank #0) == sizeof(Bank #1) == 64MiB.
 */
#undef CFG_BOOT_FROM_SPI		/* define to build a SPI-bootable image */


/*-----------------------------------------------------------------------
 * Start addresses for the final memory configuration
 * Assume we run out of uncached memory for the moment
 */

#if defined(CFG_BOOT_FROM_SPI)			/* we are booting from SPI serial flash */
#define CFG_EMI_SPI_BASE	0xA0000000	/* EMI Bank #0: SPI Flash,  Physical 0x00000000 */
#define CFG_EMI_NOR_BASE	0xA4000000	/* CSB: NOR Flash,  Physical 0x04000000 */
#else
#define CFG_EMI_NOR_BASE	0xA0000000	/* CSA: NOR Flash,  Physical 0x00000000 */
#endif /* CFG_BOOT_FROM_SPI */

#ifdef CONFIG_SH_SE_MODE
#define CFG_FLASH_BASE		CFG_EMI_NOR_BASE/* NOR FLASH (uncached) via PMB */
#define CFG_SE_PHYSICAL_BASE	0x40000000	/* LMI Physical Address */
#define CFG_SDRAM_BASE		0x80000000      /* LMI    Cached addr via PMB */
#define CFG_SE_UNACHED_BASE	0x90000000	/* LMI UN-cached addr via PMB */
#define CFG_SE_SDRAM_WINDOW	(CFG_SDRAM_SIZE-1)
#else
#error This SoC is not supported in 29-bit mode, please enable SE-mode!
#endif

#define CFG_SDRAM_SIZE		0x10000000	/* 256 MiB of LMI SDRAM */

#define CFG_MONITOR_LEN		0x00040000	/* Reserve 256 KiB for Monitor */
#define CFG_MONITOR_BASE        CFG_FLASH_BASE
#define CFG_MALLOC_LEN		(1 << 20)	/* Reserve 1 MiB for malloc */
#define CFG_BOOTPARAMS_LEN	(128 << 10)	/* 128 KiB */
#define CFG_GBL_DATA_SIZE	1024		/* Global data structures */

#define CFG_MEMTEST_START	CFG_SDRAM_BASE
#define CFG_MEMTEST_END		(CFG_SDRAM_BASE + CFG_SDRAM_SIZE - (3 << 20))

#define CONFIG_BAUDRATE		115200
#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

#define XSTR(s) STR(s)
#define STR(s) #s

#define BOARD mb903

#if CFG_MONITOR_LEN == 0x00020000		/* 128 KiB */
#	define MONITOR_SECTORS	"1:0"		/* 1 sector */
#elif CFG_MONITOR_LEN == 0x00040000		/* 245 KiB */
#	define MONITOR_SECTORS	"1:0-1"		/* 2 sectors */
#else						/* unknown */
#	error "Unable to determine sectors for monitor"
#endif

#define CONFIG_EXTRA_ENV_SETTINGS \
		"board=" XSTR(BOARD) "\0" \
		"monitor_base=" XSTR(CFG_MONITOR_BASE) "\0" \
		"monitor_len=" XSTR(CFG_MONITOR_LEN) "\0" \
		"monitor_sec=" MONITOR_SECTORS "\0" \
		"load_addr=" XSTR(CFG_LOAD_ADDR) "\0" \
		"unprot=" \
		  "protect off $monitor_sec\0" \
		"update=" \
		  "erase $monitor_sec;" \
		  "cp.b $load_addr $monitor_base $monitor_len;" \
		  "protect on $monitor_sec\0"

/*--------------------------------------------------------------
 * Command line configuration.
 */

#include <config_cmd_default.h>

#define CONFIG_CMD_ASKENV
#define CONFIG_CMD_NFS
#define CONFIG_CMD_PING
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_MII

/*--------------------------------------------------------------
 * Serial console info
 */

/*
 * We can use one of two methods for the "serial" console.
 * We can either use the (normal hardware) internal ST ASC UART;
 * OR we can use STMicroelectronics' DTF (Data Transfer Format)
 * mechanism over a JTAG link to a remote GDB debugger.
 */
#if 1
#	define CONFIG_STM_ASC_SERIAL	/* use a ST ASC UART */
#else
#	define CONFIG_STM_DTF_SERIAL	/* use DTF over JTAG */
#endif

/* choose which ST ASC UART to use */
#define CFG_STM_ASC_BASE	ST40_ASC3_REGS_BASE	/* UART #3, on CN1 */

/*---------------------------------------------------------------
 * Ethernet driver config
 */

/*
 * There are 2 on-chip ST-GMACs.
 *
 *	GMAC #0 is connected to the MoCA Module.
 *
 *	GMAC #1 is connected to a on-board IC+ IP1001 PHY (U17)
 *
 *	Note: U-Boot does *not* support MoCA.
 */

/* are we using the internal ST GMAC device ? */
#define CONFIG_DRIVER_NET_STM_GMAC

/*
 * Select the appropriate base address for the GMAC.
 * Also, choose which PHY to use.
 */
#ifdef CONFIG_DRIVER_NET_STM_GMAC
#	define CFG_STM_STMAC0_BASE	0xfda88000ul	/* MII #0 (MoCA module) */
#	define CFG_STM_STMAC1_BASE	0xfe730000ul	/* MII #1 (on-board, IC+ IP1001) */
#	define CFG_STM_STMAC_BASE	CFG_STM_STMAC1_BASE
#	define CONFIG_STMAC_IP1001	/* IC+ IP1001 (U17) */
#endif	/* CONFIG_DRIVER_NET_STM_GMAC */

/*  If this board does not have eeprom for ethernet address so allow the user
 *  to set it in the environment
 */
#define CONFIG_ENV_OVERWRITE


/*---------------------------------------------------------------
 * USB driver config
 */

/* Choose if we want USB Mass-Storage Support */
#define CONFIG_SH_STB7100_USB

#ifdef CONFIG_SH_STB7100_USB
#	define CONFIG_CMD_USB
#	define CONFIG_CMD_FAT
#	define CONFIG_USB_OHCI_NEW
#	define CONFIG_USB_STORAGE
#	define CFG_USB_OHCI_CPU_INIT
#	define CFG_USB0_BASE			0xfe000000	/* #0 is front port */
#	define CFG_USB1_BASE			0xfe100000	/* #1 is upper-rear */
#	define CFG_USB2_BASE			0xfe200000	/* #2 is lower-rear */
#	define CFG_USB_BASE			CFG_USB0_BASE
#	define CFG_USB_OHCI_REGS_BASE		(CFG_USB_BASE+0xffc00)
#	define CFG_USB_OHCI_SLOT_NAME		"ohci"
#	define CFG_USB_OHCI_MAX_ROOT_PORTS	1
#	define LITTLEENDIAN
#endif	/* ifdef CONFIG_SH_STB7100_USB */

/*---------------------------------------------------------------
 * SATA driver config
 */

/* SATA works on cut 3.x of the STx7105 (just one port) */
/* Choose if we want to use a SATA HDD */
//#define CONFIG_SH_STM_SATA

#ifdef CONFIG_SH_STM_SATA
#	define CONFIG_CMD_IDE				/* enable "ide" command set */
#	define CFG_ATA_BASE_ADDR	0xfe209000	/* E-SATA panel connector */
#	define CFG_ATA_IDE0_OFFSET	0x800		/* Host Controller */
#	define CFG_ATA_REG_OFFSET	0x0
#	define CFG_ATA_DATA_OFFSET	0x0
#	define CFG_ATA_STRIDE		0x4
#	define CFG_IDE_MAXBUS		1
#	define CFG_IDE_MAXDEVICE	1
#endif	/* CONFIG_SH_STM_SATA */

#if defined(CONFIG_SH_STM_SATA) ||	\
    defined(CONFIG_SH_STB7100_USB)
#	define CFG_64BIT_LBA
#	define CONFIG_LBA48
#	define CONFIG_DOS_PARTITION
#	define CONFIG_CMD_EXT2
#endif

/*-----------------------------------------------------------------------
 * Miscellaneous configurable options
 */

#define CFG_HUSH_PARSER		1
#define CONFIG_AUTO_COMPLETE	1
#define CFG_LONGHELP		1		/* undef to save memory		*/
#define CFG_PROMPT		"MB903> "	/* Monitor Command Prompt	*/
#define CFG_PROMPT_HUSH_PS2	"> "
#define CFG_CBSIZE		1024
#define CFG_PBSIZE (CFG_CBSIZE+sizeof(CFG_PROMPT)+16) /* Print Buffer Size	*/
#define CFG_MAXARGS		16		/* max number of command args	*/
#define CFG_HZ			1000		/* HZ for timer ticks	*/
#define CFG_LOAD_ADDR		CFG_SDRAM_BASE	/* default load address		*/
#define CONFIG_BOOTDELAY	10		/* default delay before executing bootcmd */
#define CONFIG_ZERO_BOOTDELAY_CHECK

#define CONFIG_CMDLINE_EDITING

/*-----------------------------------------------------------------------
 * FLASH organization
 */

/* Choose if we want FLASH Support (SPI &/or NOR devices)
 *
 * Note: by default CONFIG_CMD_FLASH is defined in config_cmd_default.h
 */
#undef CONFIG_CMD_FLASH		/* undefine it, define only if needed */
#define CONFIG_CMD_FLASH	/* define for NOR flash */
#define CONFIG_SPI_FLASH	/* define for SPI serial flash */

/*-----------------------------------------------------------------------
 * NOR FLASH organization
 */

/* S29GL01GP: 128MiB 1024 blocks, 128KiB block size
 * Note: in boot-from-SPI, only the upper 64MiB is accessible!
 */
#ifdef CONFIG_CMD_FLASH				/* NOR flash present ? */
#	define CFG_FLASH_CFI_DRIVER
#	define CFG_FLASH_CFI
#	define CONFIG_FLASH_PROTECT_SINGLE_CELL
#	define CFG_FLASH_PROTECTION	1	/* use hardware flash protection	*/
#	define CFG_MAX_FLASH_BANKS	1	/* max number of memory banks		*/
#	define CFG_MAX_FLASH_SECT	1024	/* max number of sectors on one chip	*/
#	define CFG_FLASH_EMPTY_INFO		/* test if each sector is empty		*/
#	define MTDPARTS_NOR						\
	"physmap-flash:"	/* First NOR flash device */		\
		"256k(U-Boot)"		/* first partition */		\
		",128k(Environment)"					\
		",4M(Kernel)"						\
		",-(RestOfNor0)"	/* last partition */
#	define MTDIDS_NOR						\
	"nor0=physmap-flash"	/* First NOR flash device */
#else
#	undef CONFIG_CMD_IMLS			/* NOR-flash specific */
#	define CFG_NO_FLASH			/* NOR-flash specific */
#endif	/* CONFIG_CMD_FLASH */


/*-----------------------------------------------------------------------
 * SPI SERIAL FLASH organization
 */

/*
 *	Name		Manuf	Device
 *	-----		-----	------
 *	U48		Numonyx	M25P128
 */
#if defined(CONFIG_SPI_FLASH)			/* SPI serial flash present ? */
#	define CONFIG_SPI_FLASH_ST		/* ST M25Pxx */
#	define CONFIG_SPI			/* enable the SPI driver */
#	define CONFIG_CMD_EEPROM		/* enable the "eeprom" command set */
#	define CFG_I2C_FRAM			/* to minimize performance degradation */
#	undef  CFG_EEPROM_PAGE_WRITE_DELAY_MS	/* to minimize performance degradation */
	/* On cut 2.0, we *should* be able to use the FSM SPI Controller */
#if 1						/* Do we use S/W "bit-banging" PIO ? */
#	define CONFIG_SOFT_SPI			/* Use S/W "bit-banging" PIO (not SSC nor FSM) */
#	define CONFIG_CMD_SPI			/* add SPI serial bus command support */
#	define SPI_SCL(val)	do { stx7108_spi_scl((val)); } while (0)
#	define SPI_SDA(val)	do { stx7108_spi_sda((val)); } while (0)
#	define SPI_DELAY	do { udelay(1); } while (0)	/* QQQ: only 500 kHz ??? */
#	define SPI_READ		stx7108_spi_read()
#else						/* else, use the H/W FSM SPI Controller */
#	define CONFIG_STM_FSM_SPI		/* Use the H/W FSM Controller for SPI */
#	define CFG_STM_SPI_FSM_BASE	0xfe902000	/* FSM SPI Controller Base */
#	define CFG_STM_SPI_CLOCKDIV	4	/* set SPI_CLOCKDIV = 4 */
#	undef CONFIG_CMD_SPI			/* SPI serial bus command support - NOT with FSM! */
#endif	/* SOFT -v- FSM */

#endif	/* CONFIG_SPI_FLASH */


/*-----------------------------------------------------------------------
 * Address, size, & location of U-boot's Environment Sector
 */

#define CFG_ENV_SIZE			0x4000	/* 16 KiB of environment data */

#if 1 && defined(CONFIG_CMD_FLASH)		/* NOR flash present ? */
#	define CFG_ENV_IS_IN_FLASH		/* environment in NOR flash */
#	define CFG_ENV_OFFSET	CFG_MONITOR_LEN	/* immediately after u-boot.bin */
#	define CFG_ENV_SECT_SIZE	0x20000	/* 128 KiB Sector size */
#elif 1 && defined(CONFIG_SPI_FLASH)		/* SPI serial flash present ? */
#	define CFG_ENV_IS_IN_EEPROM		/* ENV is stored in SPI Serial Flash */
#	define CFG_ENV_OFFSET	CFG_MONITOR_LEN	/* immediately after u-boot.bin */
#else
#	define CFG_ENV_IS_NOWHERE		/* ENV is stored in volatile RAM */
#	undef CONFIG_CMD_ENV			/* no need for "saveenv" */
#endif	/* CONFIG_CMD_FLASH */

/*----------------------------------------------------------------------
 * JFFS2 + MTD Partition support
 */

#if 1 && (defined(CONFIG_CMD_FLASH))
#	define CONFIG_CMD_JFFS2			/* enable JFFS2 support */
#endif

#if defined(CONFIG_CMD_JFFS2)
#	define CONFIG_JFFS2_CMDLINE		/* mtdparts command line support */
#	if defined(CONFIG_CMD_FLASH)		/* Only NOR flash devices */
#		define MTDPARTS_DEFAULT	"mtdparts=" MTDPARTS_NOR
#		define MTDIDS_DEFAULT	MTDIDS_NOR
#	endif	/* defined(CONFIG_CMD_FLASH) */
#endif	/* CONFIG_CMD_JFFS2 */


/*----------------------------------------------------------------------
 * I2C configuration
 */

#define CONFIG_CMD_I2C				/* do we want I2C support ? */

#if defined(CONFIG_CMD_I2C)
#	define CONFIG_I2C_BUS		6	/* Use I2C Bus associated with SSC #6 */
#	define CONFIG_I2C_CMD_TREE		/* use a "i2c" root command */
#	define CFG_I2C_SLAVE		0x7F	/* I2C slave address - Bogus: master-only in U-Boot */
#	define CONFIG_SOFT_I2C			/* I2C with S/W bit-banging	*/
#	undef  CONFIG_HARD_I2C			/* I2C withOUT hardware support	*/
#	define I2C_ACTIVE			/* open-drain, nothing to do */
#	define I2C_TRISTATE			/* open-drain, nothing to do */
#	define I2C_SCL(val)		do { stx7108_i2c_scl((val)); } while (0)
#	define I2C_SDA(val)		do { stx7108_i2c_sda((val)); } while (0)
#	define I2C_READ			stx7108_i2c_read()

	/*
	 * The "BOGOS" for NDELAY() may be calibrated using the
	 * following code fragment, and measuring (using an oscilloscope)
	 * the frequency of the I2C SCL pin, and adjusting
	 * NDELAY_BOGOS, until the SCL is approximately 100 kHz.
	 * (100kHz has a period of 5us + 5us).
	 *
	 *	printf("just toggling I2C SCL (100kHz frequency) ...\n");
	 *	while (1)
	 *	{
	 *		I2C_SCL(1); NDELAY(5000);
	 *		I2C_SCL(0); NDELAY(5000);
	 *	}
	 */
#	define NDELAY_BOGOS		20	/* Empirical measurement for 1ns on MB837A */
#	define NDELAY(ns)						\
		do {							\
			const unsigned n_bogo = NDELAY_BOGOS;		\
			const unsigned n_ticks = 			\
				((ns)<n_bogo) ? 1u : (ns)/n_bogo;	\
			volatile unsigned n_count;			\
			for(n_count=0; n_count<n_ticks; n_count++)	\
				;	/* do nothing */		\
		} while(0)

	/*
	 * Note there are 4 * I2C_DELAY per I2C clock cycle
	 * So, 400 kHz requires an I2C delay of 625 ns.
	 * However, this calculation only works if the S/W
	 * overhead in I2C bit-banging is negligible - which it is not!
	 * So, in practice, either I2C_DELAY or CFG_I2C_SPEED will be lower.
	 * The higher the clock frequency, the greater the difference.
	 * Empirical measurement/adjustment is recommended.
	 */
#	define CFG_I2C_SPEED	400000				/* I2C speed (Hz) */
#	define I2C_DELAY	do { NDELAY(625); } while (0)	/* 625 ns */
#endif	/* CONFIG_CMD_I2C */

#endif	/* __CONFIG_H */
