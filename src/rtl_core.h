/******************************************************************************
 * Copyright(c) 2008 - 2010 Realtek Corporation. All rights reserved.
 *
 * Based on the r8180 driver, which is:
 * Copyright 2004-2005 Andrea Merello <andreamrl@tiscali.it>, et al.
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
******************************************************************************/

#ifndef _RTL_CORE_H
#define _RTL_CORE_H

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
//#include <linux/config.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/netdevice.h>
#include <linux/pci.h>
//#include <linux/usb.h>
#include <linux/etherdevice.h>
#include <linux/delay.h>
#include <linux/rtnetlink.h>	//for rtnl_lock()
#include <linux/wireless.h>
#include <linux/timer.h>
#include <linux/proc_fs.h>	// Necessary because we use the proc fs
#include <linux/if_arp.h>
#include <linux/random.h>
#include <linux/version.h>
#include <asm/io.h>
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27))
#include <asm/semaphore.h>
#endif
#include "rtllib.h"

#ifdef ENABLE_DOT11D
#include "dot11d.h"
#endif

#ifdef RTL8192SE
#include "r8192S_phy.h"
#include "r8192S_phyreg.h"
#include "r8192S_firmware.h"
#include "r8192SE_hw.h"
#elif defined RTL8190P || defined RTL8192E
#include "r819xE_firmware.h"
#include "r8192E_hw.h"
#endif

#ifdef RTL8192SE
#include "rtl8192se.h"
#include "r8192S_led.h"
#include "r8192SE_def.h"
#elif defined RTL8190P || defined RTL8192E
#include "r8190P_def.h"
#include "rtl8192e.h"
#endif

#include "rtl_debug.h"
#include "rtl_eeprom.h"
#include "rtl_ps.h"
#include "rtl_cam.h"

#define DRV_COPYRIGHT  "Copyright(c) 2008 - 2010 Realtek Corporation"
#define DRV_AUTHOR  "<wlanfae@realtek.com>"
#define DRV_VERSION "V 1.1"

#ifdef RTL8190P
#define DRV_NAME "rtl819xP"
#elif defined RTL8192E
#define DRV_NAME "rtl819xE"
#elif defined RTL8192SE
#define DRV_NAME "rtl819xSE"
#elif defined RTL8192CE
#define DRV_NAME "rtl8192CE"
#endif

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,5,0))
#define RTL_PCI_DEVICE(vend, dev, cfg) \
	.vendor = (vend), .device = (dev), \
	.subvendor = PCI_ANY_ID, .subdevice =PCI_ANY_ID , \
	.driver_data = (kernel_ulong_t)&(cfg)
#else
#define RTL_PCI_DEVICE(vend, dev, cfg) \
	.vendor = (vend), .device = (dev), \
	.subvendor = PCI_ANY_ID, .subdevice =PCI_ANY_ID
#endif

//added for HW security, john.0629
#define MAX_KEY_LEN			61
#define KEY_BUF_SIZE			5

#define TOTAL_CAM_ENTRY		32
#define CAM_CONTENT_COUNT	8

#ifndef BIT
#define BIT(_i)				(1<<(_i))
#endif

#define DMESG(x,a...)
#define DMESGW(x,a...)
#define DMESGE(x,a...)
extern u32 rt_global_debug_component;
#define RT_TRACE(component, x, args...) \
do { if(rt_global_debug_component & component) \
	printk(KERN_DEBUG DRV_NAME ":" x "\n" , \
	       ##args);\
}while(0);

#ifdef RTL8192CE
#define RT_ASSERT(_Exp,Fmt)				\
		if(!(_Exp))						\
		{								\
			printk("Rtl819x: ");				\
			printk Fmt;					\
		}
// Define PHY-BB/RF/MAC check module bit		--> FPHY
#define		PHY_BBR				BIT0
#define		PHY_BBW				BIT1
#define		PHY_RFR				BIT2
#define		PHY_RFW				BIT3
#define		PHY_MACR				BIT4
#define		PHY_MACW				BIT5
#define		PHY_ALLR				BIT6
#define		PHY_ALLW				BIT7
#define		PHY_TXPWR				BIT8
#define		PHY_PWRDIFF			BIT9
// Define debug flag array for common debug print macro. */
/* Define different debug flag for dedicated service modules in debug flag array. */
// Each module has independt 32 bit debug flag you cnn define the flag as yout require.
typedef enum tag_DBGP_Flag_Type_Definition
{
	FQoS				= 0,
	FTX					= 1,
	FRX					= 2,
	FSEC				= 3,
	FMGNT				= 4,
	FMLME				= 5,
	FRESOURCE			= 6,
	FBEACON				= 7,
	FISR				= 8,
	FPHY				= 9,
	FMP					= 10,
	FEEPROM			= 11,
	FPWR				= 12,
	FDM					= 13,
	FDBGCtrl			= 14,
	FC2H				= 15,
	FBT					= 16,
	FINIT				= 17,
	DBGP_TYPE_MAX
}DBGP_FLAG_E;
// Define init check for module bit		--> FINIT
#define		INIT_EEPROM				BIT0
#define		INIT_TxPower				BIT1
#define		INIT_IQK					BIT2
#define		EFUSE_READ_ALL			BIT2
#define		EFUSE_PG					BIT1
#define		DM_Monitor			BIT2
#define RTPRINT(dbgtype, dbgflag, printstr)	\
		{									\
			{								\
				printk printstr;				\
			}								\
		}

#define RT_PRINT_DATA(_Comp, _TitleString, _HexData, _HexDataLen)			\
		do {\
			if((_Comp) & rt_global_debug_component )	\
			{									\
				int __i;								\
				u8*	ptr = (u8*)_HexData;				\
				printk("Rtl819x: ");						\
				printk(_TitleString);						\
				for( __i=0; __i<(int)_HexDataLen; __i++ )				\
				{								\
					printk("%02X%s", ptr[__i], (((__i + 1) % 4) == 0)?"  ":" ");	\
					if (((__i + 1) % 16) == 0)	printk("\n");			\
				}								\
				printk("\n");							\
			} \
		}while(0);
#endif

#define RTL819x_DEBUG
#ifdef RTL819x_DEBUG
#define assert(expr) \
        if (!(expr)) {                                  \
                printk( "Assertion failed! %s,%s,%s,line=%d\n", \
                #expr,__FILE__,__FUNCTION__,__LINE__);          \
        }
//wb added to debug out data buf
//if you want print DATA buffer related BA, please set rtllib_debug_level to DATA|BA
#define RT_DEBUG_DATA(level, data, datalen)      \
        do{ if ((rt_global_debug_component & (level)) == (level))   \
                {       \
                        int _i;                                  \
                        u8* _pdata = (u8*) data;                 \
                        printk(KERN_DEBUG DRV_NAME ": %s()\n", __FUNCTION__);   \
                        for(_i=0; _i<(int)(datalen); _i++)                 \
                        {                                               \
                                printk("%2x ", _pdata[_i]);               \
                                if ((_i+1)%16 == 0) printk("\n");        \
                        }                               \
                        printk("\n");                   \
                }                                       \
        } while (0)
#else
#define assert(expr) do {} while (0)
#define RT_DEBUG_DATA(level, data, datalen) do {} while(0)
#endif // RTL8169_DEBUG */

#define IS_UNDER_11N_AES_MODE(_rtllib)  ((_rtllib->pHTInfo->bCurrentHTSupport == true) &&\
					(_rtllib->pairwise_key_type == KEY_TYPE_CCMP))

#define HAL_MEMORY_MAPPED_IO_RANGE_8190PCI	0x1000     //8190 support 16 pages of IO registers
#define HAL_HW_PCI_REVISION_ID_8190PCI			0x00
#define HAL_MEMORY_MAPPED_IO_RANGE_8192PCIE	0x4000	//8192 support 16 pages of IO registers
#define HAL_HW_PCI_REVISION_ID_8192PCIE		0x01
#define HAL_MEMORY_MAPPED_IO_RANGE_8192SE	0x4000	//8192 support 16 pages of IO registers
#define HAL_HW_PCI_REVISION_ID_8192SE	0x10
#define HAL_HW_PCI_REVISION_ID_8192CE			0x1
#define HAL_MEMORY_MAPPED_IO_RANGE_8192CE	0x4000	//8192 support 16 pages of IO registers
#define HAL_HW_PCI_REVISION_ID_8192DE			0x0
#define HAL_MEMORY_MAPPED_IO_RANGE_8192DE	0x4000	//8192 support 16 pages of IO registers

#define HAL_HW_PCI_8180_DEVICE_ID			0x8180
#define HAL_HW_PCI_8185_DEVICE_ID			0x8185	//8185 or 8185b
#define HAL_HW_PCI_8188_DEVICE_ID			0x8188	//8185b
#define HAL_HW_PCI_8198_DEVICE_ID			0x8198	//8185b
#define HAL_HW_PCI_8190_DEVICE_ID			0x8190	//8190
#define HAL_HW_PCI_8192_DEVICE_ID			0x8192	//8192 PCI-E
#define HAL_HW_PCI_8192SE_DEVICE_ID				0x8192	//8192 SE
#define HAL_HW_PCI_8174_DEVICE_ID			0x8174	//8192 SE
#define HAL_HW_PCI_8173_DEVICE_ID			0x8173	//8191 SE Crab
#define HAL_HW_PCI_8172_DEVICE_ID			0x8172	//8191 SE RE
#define HAL_HW_PCI_8171_DEVICE_ID			0x8171	//8191 SE Unicron
#define HAL_HW_PCI_0045_DEVICE_ID				0x0045	//8190 PCI for Ceraga
#define HAL_HW_PCI_0046_DEVICE_ID				0x0046	//8190 Cardbus for Ceraga
#define HAL_HW_PCI_0044_DEVICE_ID				0x0044	//8192e PCIE for Ceraga
#define HAL_HW_PCI_0047_DEVICE_ID				0x0047	//8192e Express Card for Ceraga
#define HAL_HW_PCI_700F_DEVICE_ID				0x700F
#define HAL_HW_PCI_701F_DEVICE_ID				0x701F
#define HAL_HW_PCI_DLINK_DEVICE_ID				0x3304
#define HAL_HW_PCI_8192CET_DEVICE_ID			0x8191	//8192ce
#define HAL_HW_PCI_8192CE_DEVICE_ID				0x8178	//8192ce
#define HAL_HW_PCI_8191CE_DEVICE_ID				0x8177	//8192ce
#define HAL_HW_PCI_8188CE_DEVICE_ID				0x8176	//8192ce
#define HAL_HW_PCI_8192CU_DEVICE_ID				0x8191	//8192ce
#define HAL_HW_PCI_8192DE_DEVICE_ID				0x092D	//8192ce
#define HAL_HW_PCI_8192DU_DEVICE_ID				0x092D	//8192ce
//#endif

#define RTL819X_DEFAULT_RF_TYPE		RF_1T2R
#define RTLLIB_WATCH_DOG_TIME		2000

#define MAX_DEV_ADDR_SIZE		8  /* support till 64 bit bus width OS */
#define MAX_FIRMWARE_INFORMATION_SIZE   32 //2006/04/30 by Emily forRTL8190*/
#define MAX_802_11_HEADER_LENGTH	(40 + MAX_FIRMWARE_INFORMATION_SIZE)
#define ENCRYPTION_MAX_OVERHEAD		128
#define MAX_FRAGMENT_COUNT		8
#define MAX_TRANSMIT_BUFFER_SIZE	(1600+(MAX_802_11_HEADER_LENGTH+ENCRYPTION_MAX_OVERHEAD)*MAX_FRAGMENT_COUNT)

#define scrclng				4		// octets for crc32 (FCS, ICV)

#define DEFAULT_FRAG_THRESHOLD	2342U
#define MIN_FRAG_THRESHOLD	256U
#define DEFAULT_BEACONINTERVAL	0x64U

#define DEFAULT_SSID		""
#define DEFAULT_RETRY_RTS	7
#define DEFAULT_RETRY_DATA	7
#define PRISM_HDR_SIZE		64

#define	PHY_RSSI_SLID_WIN_MAX			100

#define RTL_IOCTL_WPA_SUPPLICANT		SIOCIWFIRSTPRIV+30

#define TxBBGainTableLength		        37
#define CCKTxBBGainTableLength	                23

#define CHANNEL_PLAN_LEN			10
#define sCrcLng			                4

#define NIC_SEND_HANG_THRESHOLD_NORMAL		4
#define NIC_SEND_HANG_THRESHOLD_POWERSAVE	8
#define MAX_TX_QUEUE				9 // BK, BE, VI, VO, HCCA, MANAGEMENT, COMMAND, HIGH, BEACON.

#define MAX_RX_COUNT                            64
#define MAX_TX_QUEUE_COUNT                      9

enum RTL819x_PHY_PARAM {
	RTL819X_PHY_MACPHY_REG		= 0,		//"rtl8192CE\\MACPHY_reg.txt"
	RTL819X_PHY_MACPHY_REG_PG	= 1,		//"rtl8192CE\\MACPHY_reg_PG.txt"
	RTL819X_PHY_MACREG			= 2,		//"rtl8192CE\\MACREG.txt"
	RTL819X_PHY_REG				= 3,		//"rtl8192CE\\PHY_REG.txt"
	RTL819X_PHY_REG_1T2R			= 4,		//"rtl8192CE\\PHY_REG_1T2R.txt"
	RTL819X_PHY_REG_to1T1R		= 5,		//"rtl8192CE\\phy_to1T1R_a.txt"
	RTL819X_PHY_REG_to1T2R		= 6,		//"rtl8192CE\\phy_to1T2R.txt"
	RTL819X_PHY_REG_to2T2R		= 7,		//"rtl8192CE\\phy_to2T2R.txt"
	RTL819X_PHY_REG_PG			= 8,		//"rtl8192CE\\PHY_REG_PG.txt"
	RTL819X_AGC_TAB				= 9,		//"rtl8192CE\\AGC_TAB.txt"
	RTL819X_PHY_RADIO_A			=10,		//"rtl8192CE\\radio_a.txt"
	RTL819X_PHY_RADIO_A_1T		=11,		//"rtl8192CE\\radio_a_1t.txt"
	RTL819X_PHY_RADIO_A_2T		=12,		//"rtl8192CE\\radio_a_2t.txt"
	RTL819X_PHY_RADIO_B			=13,		//"rtl8192CE\\radio_b.txt"
	RTL819X_PHY_RADIO_B_GM		=14,		//"rtl8192CE\\radio_b_gm.txt"
	RTL819X_PHY_RADIO_C			=15,		//"rtl8192CE\\radio_c.txt"
	RTL819X_PHY_RADIO_D			=16,		//"rtl8192CE\\radio_d.txt"
	RTL819X_EEPROM_MAP			=17,		//"rtl8192CE\\8192ce.map"
	RTL819X_EFUSE_MAP				=18,		//"rtl8192CE\\8192ce.map"
};

enum RTL_DEBUG {
	COMP_TRACE		= BIT0,	 // For function call tracing.
	COMP_DBG		= BIT1,	 // Only for temporary debug message.
	COMP_INIT		= BIT2,	 // during driver initialization / halt / reset.
	COMP_RECV		= BIT3,	 // Reveive part data path.
	COMP_SEND		= BIT4,	 // Send part path.
	COMP_CMD		= BIT5,	 // I/O Related. Added by Annie, 2006-03-02.
	COMP_POWER	        = BIT6,	 // 802.11 Power Save mode or System/Device Power state related.
	COMP_EPROM	        = BIT7,	 // 802.11 link related: join/start BSS, leave BSS.
	COMP_SWBW		= BIT8,	 // For bandwidth switch.
	COMP_SEC		= BIT9,	 // For Security.
	COMP_LPS		= BIT10, // For Turbo Mode related. By Annie, 2005-10-21.
	COMP_QOS		= BIT11, // For QoS.
	COMP_RATE		= BIT12, // For Rate Adaptive mechanism, 2006.07.02, by rcnjko.
	COMP_RXDESC	        = BIT13, // Show Rx desc information for SD3 debug. .
	COMP_PHY		= BIT14,
	COMP_DIG		= BIT15, // For DIG.
	COMP_TXAGC		= BIT16, // For Tx power
	COMP_HALDM	        = BIT17, // For HW Dynamic Mechanism
	COMP_POWER_TRACKING	= BIT18, //FOR 8190 TX POWER TRACKING
	COMP_CH		        = BIT19, // Event handling
	COMP_RF		        = BIT20, // For RF.
	COMP_FIRMWARE	        = BIT21, //for firmware downloading
	COMP_HT		        = BIT22, // For 802.11n HT related information. by Emily 2006-8-11
	COMP_RESET		= BIT23,
	COMP_CMDPKT	        = BIT24,
	COMP_SCAN		= BIT25,
	COMP_PS		        = BIT26,
	COMP_DOWN		= BIT27, // for rm driver module
	COMP_INTR		= BIT28, // for interrupt
	COMP_LED		= BIT29, // for led
	COMP_MLME		= BIT30,
	COMP_ERR		= BIT31  // for error out, always on
};

typedef enum{
	NIC_UNKNOWN     = 0,
	NIC_8192E       = 1,
	NIC_8190P       = 2,
	NIC_8192SE      = 4,
//#ifdef RTL8192CE
	NIC_8192CE		= 5,
	NIC_8192CU		= 6,
	NIC_8192DE		= 7,
	NIC_8192DU		= 8,
//#endif
	} nic_t;

typedef	enum _RT_EEPROM_TYPE{
	EEPROM_93C46,
	EEPROM_93C56,
	EEPROM_BOOT_EFUSE,
}RT_EEPROM_TYPE,*PRT_EEPROM_TYPE;

typedef enum _tag_TxCmd_Config_Index{
	TXCMD_TXRA_HISTORY_CTRL	        = 0xFF900000,
	TXCMD_RESET_TX_PKT_BUFF		= 0xFF900001,
	TXCMD_RESET_RX_PKT_BUFF		= 0xFF900002,
	TXCMD_SET_TX_DURATION		= 0xFF900003,
	TXCMD_SET_RX_RSSI		= 0xFF900004,
	TXCMD_SET_TX_PWR_TRACKING	= 0xFF900005,
	TXCMD_XXXX_CTRL,
}DCMD_TXCMD_OP;

typedef enum _RT_RF_TYPE_819xU{
        RF_TYPE_MIN = 0,
        RF_8225,
        RF_8256,
        RF_8258,
        RF_6052=4,		// 4 11b/g/n RF
        RF_PSEUDO_11N = 5,
}RT_RF_TYPE_819xU, *PRT_RF_TYPE_819xU;

// 2007/11/02 MH Define RF mode temporarily for test. */
typedef enum tag_Rf_Operatetion_State
{
    RF_STEP_INIT = 0,
    RF_STEP_NORMAL,
    RF_STEP_MAX
}RF_STEP_E;

typedef enum _RT_STATUS{
	RT_STATUS_SUCCESS,
	RT_STATUS_FAILURE,
	RT_STATUS_PENDING,
	RT_STATUS_RESOURCE
}RT_STATUS,*PRT_STATUS;

typedef enum _RT_CUSTOMER_ID
{
	RT_CID_DEFAULT          = 0,
	RT_CID_8187_ALPHA0      = 1,
	RT_CID_8187_SERCOMM_PS  = 2,
	RT_CID_8187_HW_LED      = 3,
	RT_CID_8187_NETGEAR     = 4,
	RT_CID_WHQL             = 5,
	RT_CID_819x_CAMEO       = 6,
	RT_CID_819x_RUNTOP      = 7,
	RT_CID_819x_Senao       = 8,
	RT_CID_TOSHIBA          = 9,	// Merge by Jacken, 2008/01/31.
	RT_CID_819x_Netcore     = 10,
	RT_CID_Nettronix        = 11,
	RT_CID_DLINK            = 12,
	RT_CID_PRONET           = 13,
	RT_CID_COREGA           = 14,
	RT_CID_819x_ALPHA       = 15,
	RT_CID_819x_Sitecom     = 16,
	RT_CID_CCX              = 17, // It's set under CCX logo test and isn't demanded for CCX functions, but for test behavior like retry limit and tx report. By Bruce, 2009-02-17.
	RT_CID_819x_Lenovo      = 18,
	RT_CID_819x_QMI         = 19,
	RT_CID_819x_Edimax_Belkin = 20,
	RT_CID_819x_Sercomm_Belkin = 21,
	RT_CID_819x_CAMEO1 = 22,
	RT_CID_819x_MSI = 23,
	RT_CID_819x_Acer = 24,
	RT_CID_819x_HP	=27,
}RT_CUSTOMER_ID, *PRT_CUSTOMER_ID;

typedef enum _RT_OP_MODE{
	RT_OP_MODE_AP,
	RT_OP_MODE_INFRASTRUCTURE,
	RT_OP_MODE_IBSS,
	RT_OP_MODE_NO_LINK,
}RT_OP_MODE, *PRT_OP_MODE;

typedef enum _RESET_TYPE {
	RESET_TYPE_NORESET = 0x00,
	RESET_TYPE_NORMAL = 0x01,
	RESET_TYPE_SILENT = 0x02
} RESET_TYPE;

//
// IC class identifier. Added by Roger, 2009.09.02.
//
typedef enum _IC_INFERIORITY_8192S{
	IC_INFERIORITY_A            = 0, // Class A
	IC_INFERIORITY_B            = 1, // Class B
}IC_INFERIORITY_8192S, *PIC_INFERIORITY_8192S;

typedef enum _PCI_BRIDGE_VENDOR {
	PCI_BRIDGE_VENDOR_INTEL = 0x01,//0b'0000,0001
	PCI_BRIDGE_VENDOR_ATI = 0x02,//0b'0000,0010
	PCI_BRIDGE_VENDOR_AMD = 0x04,//0b'0000,0100
	PCI_BRIDGE_VENDOR_SIS = 0x08,//0b'0000,1000
	PCI_BRIDGE_VENDOR_UNKNOWN = 0x40,//0b'0100,0000
	PCI_BRIDGE_VENDOR_MAX = 0x80
} PCI_BRIDGE_VENDOR;

typedef struct buffer
{
	struct buffer *next;
	u32 *buf;
	dma_addr_t dma;

} buffer;

typedef struct rtl_reg_debug{
        unsigned int  cmd;
        struct {
                unsigned char type;
                unsigned char addr;
                unsigned char page;
                unsigned char length;
        } head;
        unsigned char buf[0xff];
}rtl_reg_debug;

typedef struct _rt_9x_tx_rate_history {
	u32             cck[4];
	u32             ofdm[8];
	u32             ht_mcs[4][16];
}rt_tx_rahis_t, *prt_tx_rahis_t;

typedef	struct _RT_SMOOTH_DATA_4RF {
	char	elements[4][100];//array to store values
	u32	index;			//index to current array to store
	u32	TotalNum;		//num of valid elements
	u32	TotalVal[4];		//sum of valid elements
}RT_SMOOTH_DATA_4RF, *PRT_SMOOTH_DATA_4RF;

typedef struct Stats
{
	unsigned long txrdu;
	unsigned long rxrdu;
	//unsigned long rxnolast;
	//unsigned long rxnodata;
//	unsigned long rxreset;
//	unsigned long rxnopointer;
	unsigned long rxok;
	unsigned long rxframgment;
	unsigned long rxcmdpkt[4];		//08/05/08 amy rx cmd element txfeedback/bcn report/cfg set/query
	unsigned long rxurberr;
	unsigned long rxstaterr;
	unsigned long rxcrcerrmin;//crc error (0-500)
	unsigned long rxcrcerrmid;//crc error (500-1000)
	unsigned long rxcrcerrmax;//crc error (>1000)
	unsigned long received_rate_histogram[4][32];	//0: Total, 1:OK, 2:CRC, 3:ICV, 2007 07 03 cosa
	unsigned long received_preamble_GI[2][32];		//0: Long preamble/GI, 1:Short preamble/GI
	unsigned long	rx_AMPDUsize_histogram[5]; // level: (<4K), (4K~8K), (8K~16K), (16K~32K), (32K~64K)
	unsigned long rx_AMPDUnum_histogram[5]; // level: (<5), (5~10), (10~20), (20~40), (>40)
	unsigned long numpacket_matchbssid;	// debug use only.
	unsigned long numpacket_toself;		// debug use only.
	unsigned long num_process_phyinfo;		// debug use only.
	unsigned long numqry_phystatus;
	unsigned long numqry_phystatusCCK;
	unsigned long numqry_phystatusHT;
	unsigned long received_bwtype[5];              //0: 20M, 1: funn40M, 2: upper20M, 3: lower20M, 4: duplicate
	unsigned long txnperr;
	unsigned long txnpdrop;
	unsigned long txresumed;
//	unsigned long rxerr;
	unsigned long rxoverflow;
	unsigned long rxint;
	unsigned long txnpokint;
//	unsigned long txhpokint;
//	unsigned long txhperr;
	unsigned long ints;
	unsigned long shints;
	unsigned long txoverflow;
//	unsigned long rxdmafail;
//	unsigned long txbeacon;
//	unsigned long txbeaconerr;
	unsigned long txlpokint;
	unsigned long txlpdrop;
	unsigned long txlperr;
	unsigned long txbeokint;
	unsigned long txbedrop;
	unsigned long txbeerr;
	unsigned long txbkokint;
	unsigned long txbkdrop;
	unsigned long txbkerr;
	unsigned long txviokint;
	unsigned long txvidrop;
	unsigned long txvierr;
	unsigned long txvookint;
	unsigned long txvodrop;
	unsigned long txvoerr;
	unsigned long txbeaconokint;
	unsigned long txbeacondrop;
	unsigned long txbeaconerr;
	unsigned long txmanageokint;
	unsigned long txmanagedrop;
	unsigned long txmanageerr;
	unsigned long txcmdpktokint;
	unsigned long txdatapkt;
	unsigned long txfeedback;
	unsigned long txfeedbackok;
	unsigned long txoktotal;
	unsigned long txokbytestotal;
	unsigned long txokinperiod;
	unsigned long txmulticast;
	unsigned long txbytesmulticast;
	unsigned long txbroadcast;
	unsigned long txbytesbroadcast;
	unsigned long txunicast;
	unsigned long txbytesunicast;
	unsigned long rxbytesunicast;
	unsigned long txfeedbackfail;
	unsigned long txerrtotal;
	unsigned long txerrbytestotal;
	unsigned long txerrmulticast;
	unsigned long txerrbroadcast;
	unsigned long txerrunicast;
	unsigned long txretrycount;
	unsigned long txfeedbackretry;
	u8			last_packet_rate;
	unsigned long slide_signal_strength[100];
	unsigned long slide_evm[100];
	unsigned long	slide_rssi_total;	// For recording sliding window's RSSI value
	unsigned long slide_evm_total;	// For recording sliding window's EVM value
	long signal_strength; // Transformed, in dbm. Beautified signal strength for UI, not correct.
	long signal_quality;
	long last_signal_strength_inpercent;
	long	recv_signal_power;	// Correct smoothed ss in Dbm, only used in driver to report real power now.
	u8 rx_rssi_percentage[4];
	u8 rx_evm_percentage[2];
	long rxSNRdB[4];
	rt_tx_rahis_t txrate;
	u32 Slide_Beacon_pwdb[100];	//cosa add for beacon rssi
	u32 Slide_Beacon_Total;		//cosa add for beacon rssi
	RT_SMOOTH_DATA_4RF		cck_adc_pwdb;
	u32	CurrentShowTxate;
} Stats;

typedef struct	ChnlAccessSetting {
	u16 SIFS_Timer;
	u16 DIFS_Timer;
	u16 SlotTimeTimer;
	u16 EIFS_Timer;
	u16 CWminIndex;
	u16 CWmaxIndex;
}*PCHANNEL_ACCESS_SETTING,CHANNEL_ACCESS_SETTING;


typedef struct _txbbgain_struct
{
	long	txbb_iq_amplifygain;
	u32	txbbgain_value;
} txbbgain_struct, *ptxbbgain_struct;

typedef struct _ccktxbbgain_struct
{
	//The Value is from a22 to a29 one Byte one time is much Safer
	u8	ccktxbb_valuearray[8];
} ccktxbbgain_struct,*pccktxbbgain_struct;

typedef struct _init_gain
{
	u8				xaagccore1;
	u8				xbagccore1;
	u8				xcagccore1;
	u8				xdagccore1;
	u8				cca;

} init_gain, *pinit_gain;

typedef struct _tx_ring{
	u32 * desc;
	u8 nStuckCount;
	struct _tx_ring * next;
}__attribute__ ((packed)) tx_ring, * ptx_ring;

struct rtl8192_tx_ring {
    tx_desc *desc;
    dma_addr_t dma;
    unsigned int idx;
    unsigned int entries;
    struct sk_buff_head queue;
};



//definded by WB. Ready to fill handlers for different NIC types.
//add handle here when necessary.
struct rtl819x_ops{
	nic_t nic_type;
	//init private variables
//	void (* init_priv_variable)(struct net_device* dev);
	//get eeprom types
	void (* get_eeprom_size)(struct net_device* dev);
//	u8 (* read_eeprom_info)(struct net_device* dev);
//	u8 (* read_dev_info)(struct net_device* dev);
	bool (* initialize_adapter)(struct net_device* dev);
	void (*link_change)(struct net_device* dev);
	void (* tx_fill_descriptor)(struct net_device* dev, tx_desc * tx_desc, cb_desc * cb_desc, struct sk_buff *skb);
	void (* tx_fill_cmd_descriptor)(struct net_device* dev, tx_desc_cmd * entry, cb_desc * cb_desc, struct sk_buff *skb);
	bool (* rx_query_status_descriptor)(struct net_device* dev, struct rtllib_rx_stats*  stats, rx_desc *pdesc, struct sk_buff* skb);
	void (* stop_adapter)(struct net_device *dev, bool reset);
#if defined RTL8192SE || defined RTL8192CE
	void (* update_ratr_table)(struct net_device* dev,u8* pMcsRate,struct sta_info* pEntry);
#else
	void (* update_ratr_table)(struct net_device* dev);
#endif
};

typedef struct r8192_priv
{
	struct pci_dev *pdev;
#ifdef RTL8192SE
	struct pci_dev *bridge_pdev;
#endif
	//add for handles different nics' operations. WB
	struct rtl819x_ops* ops;
	bool bfirst_init;//added by amy 090414
	//added by amy 090414 for silent reset
	LOG_INTERRUPT_8190_T InterruptLog;
	bool bIgnoreSilentReset;
	u32	SilentResetRxSoltNum;
	u32	SilentResetRxSlotIndex;
#if defined RTL8192SE || defined RTL8192CE
	u32	SilentResetRxStuckEvent[MAX_SILENT_RESET_RX_SLOT_NUM];
#endif
	RT_CUSTOMER_ID CustomerID;
	bool	being_init_adapter;
	bool	sw_radio_on;
	//bool bDcut;
	u8	IC_Cut;
	int irq;
	short irq_enabled;
	struct rtllib_device *rtllib;
	u8 Rf_Mode;
	nic_t card_8192; // 1: rtl8192E, 2:rtl8190, 4:8192SE */
	u8 card_8192_version; // B,C,D or later cut */
	//	short phy_ver; /* meaningful for rtl8225 1:A 2:B 3:C */
	short enable_gpio0;
	enum card_type {PCI,MINIPCI,CARDBUS,USB}card_type;
	short hw_plcp_len;
	short plcp_preamble_mode;
	u8 ScanDelay;
	spinlock_t irq_lock;
	spinlock_t irq_th_lock;
	spinlock_t tx_lock;
	spinlock_t rf_ps_lock;
	spinlock_t rw_lock;
#ifdef RTL8192SE_CONFIG_ASPM_OR_D3
	spinlock_t D3_lock;
#endif
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,16))
	struct semaphore mutex;
#else
	struct mutex mutex;
#endif
	bool ps_force;
	spinlock_t rf_lock; //used to lock rf write operation added by wb
	spinlock_t ps_lock;

	u32 irq_mask[2];
	short chan;
	short sens;
	short max_sens;
	u32 rx_prevlen;
	//RX stuff*/
	rx_desc *rx_ring;
	dma_addr_t rx_ring_dma;
	unsigned int rx_idx;
	struct sk_buff *rx_buf[MAX_RX_COUNT];
	int rxringcount;
	u16 rxbuffersize;

	struct sk_buff *rx_skb;
	u32 *rxring;
	u32 *rxringtail;
	dma_addr_t rxringdma;
	struct buffer *rxbuffer;
	struct buffer *rxbufferhead;
	short rx_skb_complete;
	//TX stuff*/
	struct rtl8192_tx_ring tx_ring[MAX_TX_QUEUE_COUNT];
	int txringcount;
	int txbuffsize;
	int txfwbuffersize;
	//struct tx_pendingbuf txnp_pending;
	//struct tasklet_struct irq_tx_tasklet;
	struct tasklet_struct irq_rx_tasklet;
	struct tasklet_struct irq_tx_tasklet;
	struct tasklet_struct irq_prepare_beacon_tasklet;
	struct buffer *txmapbufs;
	struct buffer *txbkpbufs;
	struct buffer *txbepbufs;
	struct buffer *txvipbufs;
	struct buffer *txvopbufs;
	struct buffer *txcmdbufs;
	struct buffer *txmapbufstail;
	struct buffer *txbkpbufstail;
	struct buffer *txbepbufstail;
	struct buffer *txvipbufstail;
	struct buffer *txvopbufstail;
	struct buffer *txcmdbufstail;
	// adhoc/master mode stuff */
	ptx_ring txbeaconringtail;
	dma_addr_t txbeaconringdma;
	ptx_ring txbeaconring;
	int txbeaconcount;
	struct buffer *txbeaconbufs;
	struct buffer *txbeaconbufstail;
	ptx_ring txmapring;
	ptx_ring txbkpring;
	ptx_ring txbepring;
	ptx_ring txvipring;
	ptx_ring txvopring;
	ptx_ring txcmdring;
	ptx_ring txmapringtail;
	ptx_ring txbkpringtail;
	ptx_ring txbepringtail;
	ptx_ring txvipringtail;
	ptx_ring txvopringtail;
	ptx_ring txcmdringtail;
	ptx_ring txmapringhead;
	ptx_ring txbkpringhead;
	ptx_ring txbepringhead;
	ptx_ring txvipringhead;
	ptx_ring txvopringhead;
	ptx_ring txcmdringhead;
	dma_addr_t txmapringdma;
	dma_addr_t txbkpringdma;
	dma_addr_t txbepringdma;
	dma_addr_t txvipringdma;
	dma_addr_t txvopringdma;
	dma_addr_t txcmdringdma;
	short up;
	short up_first_time;
	short crcmon; //if 1 allow bad crc frame reception in monitor mode
	struct semaphore wx_sem;
	struct semaphore rf_sem;
	u8 rf_type; //0 means 1T2R, 1 means 2T4R
	RT_RF_TYPE_819xU rf_chip;
	char nick[IW_ESSID_MAX_SIZE + 1];

	//	u32 key0[4];
	short (*rf_set_sens)(struct net_device *dev,short sens);
	u8 (*rf_set_chan)(struct net_device *dev,u8 ch);
	void (*rf_close)(struct net_device *dev);
	void (*rf_init)(struct net_device *dev);
	//short rate;
	short promisc;
	/*stats*/
	struct Stats stats;
	struct iw_statistics wstats;
	struct proc_dir_entry *dir_dev;

	// modified by davad for Rx process */
	struct sk_buff_head rx_queue;
	struct sk_buff_head skb_queue;
	work_struct_rsl qos_activate;
	short  tx_urb_index;
	atomic_t tx_pending[0x10];//UART_PRIORITY+1

	struct urb *rxurb_task;

	//2 Tx Related variables
	u16	ShortRetryLimit;
	u16	LongRetryLimit;
	u32	TransmitConfig;
	u8	RegCWinMin;		// For turbo mode CW adaptive. Added by Annie, 2005-10-27.

	u32     LastRxDescTSFHigh;
	u32     LastRxDescTSFLow;


	//2 Rx Related variables
	u16	EarlyRxThreshold;
	u32	ReceiveConfig;
	u8	AcmControl;

	u8	RFProgType;

	u8 retry_data;
	u8 retry_rts;
	u16 rts;

	struct	ChnlAccessSetting  ChannelAccessSetting;

	work_struct_rsl reset_wq;

#ifdef CONFIG_RTLWIFI_DEBUGFS
	// debugfs */
	rtl_fs_debug *debug;
#endif /* CONFIG_IWLWIFI_DEBUGFS */

	/**********************************************************/
	//for rtl819xPci
	// Data Rate Config. Added by Annie, 2006-04-13.
	u16	basic_rate;
	u8	short_preamble;
	u8	slot_time;
	u16 SifsTime;
	// WirelessMode*/
	u8 RegWirelessMode;
	//Firmware*/
	prt_firmware		pFirmware;
	rtl819x_loopback_e	LoopbackMode;
	firmware_source_e	firmware_source;
	u16 rf_pathmap;
	bool AutoloadFailFlag;
	//added for maintain info from eeprom
	short epromtype;
	u16 eeprom_vid;
	u16 eeprom_did;
	u16 eeprom_svid;
	u16 eeprom_smid;
	u8  eeprom_CustomerID;
	u16  eeprom_ChannelPlan;
	u8 eeprom_version;
	//efuse map
	u8					EfuseMap[2][HWSET_MAX_SIZE_92S];
	u16					EfuseUsedBytes;
	u8					EfuseUsedPercentage;

#ifdef EFUSE_REPG_WORKAROUND
	bool					efuse_RePGSec1Flag;
	u8					efuse_RePGData[8];
#endif
	u8 EEPROMRegulatory;
	u8 EEPROMPwrGroup[2][3];
	// [For 90/92U/ USB single chip, read EEPROM to decied TxRx] 2009.03.31, by Bohn
	u8 EEPROMOptional;
	u8 ThermalValue;
	long EntryMinUndecoratedSmoothedPWDB;
	long EntryMaxUndecoratedSmoothedPWDB;
	//Add by Jacken Tx Power Control for Near/Far Range 2008/03/06
	u8 DynamicTxHighPowerLvl;  // Tx high power level
	u8 LastDTPLvl;
	u32 CurrentRATR0;
	FALSE_ALARM_STATISTICS FalseAlmCnt;

	// Determine if using RA Mask, by hpfan 2009.06.10
//	bool bUseRAMask;
	bool bInformFWDriverControlDM;
	// For power group
	u8 PwrGroupHT20[2][14];
	u8 PwrGroupHT40[2][14];
	// FW Cmd IO related
	u16 FwCmdIOMap;
	u32 FwCmdIOParam;
	//bool pwrdown;
	bool bSupportASPM; // If it supports ASPM, Offset[560h] = 0x40, otherwise Offset[560h] = 0x00. By Bruce, 2009-05-12.
	//bool bIgnoreSilentReset;
	u32 RfRegChnlVal[2];
	u8 DMFlag; // Indicate if each dynamic mechanism's status.
	u8 DM_Type;
	u8 ShowRateMode;
	u8 RATRTableBitmap;

#if (EEPROM_OLD_FORMAT_SUPPORT == 1)
	u8					EEPROMTxPowerLevelCCK[14];		// CCK 2.4G channel 1~14
	u8					EEPROMTxPowerLevelOFDM24G[14];	// OFDM 2.4G channel 1~14
	u8					EEPROMTxPowerLevelOFDM5G[24];	// OFDM 5G
#else
	// F92S new definition
	//RF-A&B CCK/OFDM Tx Power Level at three channel are [1-3] [4-9] [10-14]
	u8					RfCckChnlAreaTxPwr[2][3];
	u8					RfOfdmChnlAreaTxPwr1T[2][3];
	u8					RfOfdmChnlAreaTxPwr2T[2][3];
#endif
	// For EEPROM TX Power Index like 8190 series
	u8					EEPROMRfACCKChnl1TxPwLevel[3];	//RF-A CCK Tx Power Level at channel 7
	u8					EEPROMRfAOfdmChnlTxPwLevel[3];//RF-A CCK Tx Power Level at [0],[1],[2] = channel 1,7,13
	u8					EEPROMRfCCCKChnl1TxPwLevel[3];	//RF-C CCK Tx Power Level at channel 7
	u8					EEPROMRfCOfdmChnlTxPwLevel[3];//RF-C CCK Tx Power Level at [0],[1],[2] = channel 1,7,13

	u16 EEPROMTxPowerDiff;
	u16 EEPROMAntPwDiff;		// Antenna gain offset from B/C/D to A
	u8 EEPROMThermalMeter;
	u8 EEPROMPwDiff;
	u8 EEPROMCrystalCap;
	u8 EEPROMBluetoothCoexist;
	u8 EEPROMBoardType;
	u8 EEPROM_Def_Ver;
	u8 EEPROMHT2T_TxPwr[6];			// For channel 1, 7 and 13 on path A/B.
	u8 EEPROMTSSI_A;
	u8 EEPROMTSSI_B;
	// The following definition is for eeprom 93c56
	//	u8 EEPROMRfACCKChnl1TxPwLevel[3];	//RF-A CCK Tx Power Level at channel 7
	//	u8 EEPROMRfAOfdmChnlTxPwLevel[3];//RF-A CCK Tx Power Level at [0],[1],[2] = channel 1,7,13
	//	u8 EEPROMRfCCCKChnl1TxPwLevel[3];	//RF-C CCK Tx Power Level at channel 7
	//	u8 EEPROMRfCOfdmChnlTxPwLevel[3];//RF-C CCK Tx Power Level at [0],[1],[2] = channel 1,7,13
	u8 EEPROMTxPowerLevelCCK_V1[3];
	//	u8 EEPROMTxPowerLevelOFDM24G[14]; // OFDM 2.4G channel 1~14
	//	u8 EEPROMTxPowerLevelOFDM5G[24];	// OFDM 5G
	u8 EEPROMLegacyHTTxPowerDiff;	// Legacy to HT rate power diff
	bool bTXPowerDataReadFromEEPORM;
	//channel plan*/
	u16 RegChannelPlan; // Channel Plan specifed by user, 15: following setting of EEPROM, 0-14: default channel plan index specified by user.
	u16 ChannelPlan;
	bool bChnlPlanFromHW;
	//PS related*/
	bool RegRfOff;
	bool isRFOff;//added by amy for IPS 090331
	bool bInPowerSaveMode;//added by amy for IPS 090331
	// Rf off action for power save
	u8	bHwRfOffAction;	//0:No action, 1:By GPIO, 2:By Disable
	//PHY related*/
	BB_REGISTER_DEFINITION_T	PHYRegDef[4];	//Radio A/B/C/D
	// Read/write are allow for following hardware information variables
	u8					pwrGroupCnt;
#if defined RTL8192SE  || defined RTL8192CE
	u32	MCSTxPowerLevelOriginalOffset[4][7];
#else
	u32	MCSTxPowerLevelOriginalOffset[6];
#endif
	u32	CCKTxPowerLevelOriginalOffset;
	u8	TxPowerLevelCCK[14];			// CCK channel 1~14
	u8	TxPowerLevelCCK_A[14];			// RF-A, CCK channel 1~14
	u8	TxPowerLevelCCK_C[14];
	u8	TxPowerLevelOFDM24G[14];		// OFDM 2.4G channel 1~14
	u8	TxPowerLevelOFDM5G[14];			// OFDM 5G
	u8	TxPowerLevelOFDM24G_A[14];	// RF-A, OFDM 2.4G channel 1~14
	u8	TxPowerLevelOFDM24G_C[14];	// RF-C, OFDM 2.4G channel 1~14
	u8	LegacyHTTxPowerDiff;			// Legacy to HT rate power diff
	u8	TxPowerDiff;
	s8	RF_C_TxPwDiff;					// Antenna gain offset, rf-c to rf-a
	s8	RF_B_TxPwDiff;
	u8	RfTxPwrLevelCck[2][14];
	u8	RfTxPwrLevelOfdm1T[2][14];
	u8	RfTxPwrLevelOfdm2T[2][14];
	u8	AntennaTxPwDiff[3];				// Antenna gain offset, index 0 for B, 1 for C, and 2 for D
	// 2009/01/20 MH Add for new EEPROM format.
	u8	TxPwrHt20Diff[2][14];				// HT 20<->40 Pwr diff
	u8	TxPwrLegacyHtDiff[2][14];		// For HT<->legacy pwr diff
#if 0//cosa, remove band-edge related stuff by SD3's request
	u8	TxPwrbandEdgeHt40[2][2];		// Band edge for HY 40MHZlow/up channel
	u8	TxPwrbandEdgeHt20[2][2];		// Band edge for HY 40MHZ low/up channel
	u8	TxPwrbandEdgeLegacyOfdm[2][2];	// Band edge for legacy ofdm low/up channel
#endif
	u8	TxPwrSafetyFlag;				// Band edge enable flag
	// Antenna gain offset, rf-c to rf-a
	u8	HT2T_TxPwr_A[14];				// For channel 1~14 on path A.
	u8	HT2T_TxPwr_B[14];				// For channel 1~14 on path B.
	//the current Tx power level
	u8	CurrentCckTxPwrIdx;
	u8	CurrentOfdm24GTxPwrIdx;

	u8	CrystalCap;						// CrystalCap.
	u8	ThermalMeter[2];	// ThermalMeter, index 0 for RFIC0, and 1 for RFIC1
	u8      BluetoothCoexist;
	IC_INFERIORITY_8192S IC_Class;

	//05/27/2008 cck power enlarge
	u8	CckPwEnl;
	u16	TSSI_13dBm;
	u32	Pwr_Track;
	u8				CCKPresentAttentuation_20Mdefault;
	u8				CCKPresentAttentuation_40Mdefault;
	char				CCKPresentAttentuation_difference;
	char				CCKPresentAttentuation;
	// Use to calculate PWBD.
	u8	bCckHighPower;
	long	undecorated_smoothed_pwdb;
	long	undecorated_smoothed_cck_adc_pwdb[4];
	//for set channel
	u8	SwChnlInProgress;
	u8	SwChnlStage;
	u8	SwChnlStep;
	u8	SetBWModeInProgress;
	HT_CHANNEL_WIDTH		CurrentChannelBW;

	// 8190 40MHz mode
	//
	u8	nCur40MhzPrimeSC;	// Control channel sub-carrier
	// Joseph test for shorten RF configuration time.
	// We save RF reg0 in this variable to reduce RF reading.
	//
	u32					RfReg0Value[4];
	u8					NumTotalRFPath;
	bool				brfpath_rxenable[4];
	//+by amy 080507
	struct timer_list watch_dog_timer;

	//+by amy 080515 for dynamic mechenism
	//Add by amy Tx Power Control for Near/Far Range 2008/05/15
	bool	bdynamic_txpower;  //bDynamicTxPower
	bool	bDynamicTxHighPower;  // Tx high power state
	bool	bDynamicTxLowPower;  // Tx low power state
	bool	bLastDTPFlag_High;
	bool	bLastDTPFlag_Low;

	bool	bstore_last_dtpflag;
	bool	bstart_txctrl_bydtp;   //Define to discriminate on High power State or on sitesuvey to change Tx gain index
	//Add by amy for Rate Adaptive
	rate_adaptive rate_adaptive;
	//Add by amy for TX power tracking
	//2008/05/15  Mars OPEN/CLOSE TX POWER TRACKING
	txbbgain_struct txbbgain_table[TxBBGainTableLength];
	u8			   txpower_count;//For 6 sec do tracking again
	bool			   btxpower_trackingInit;
	u8			   OFDM_index;
	u8			   CCK_index;
	u8			   Record_CCK_20Mindex;
	u8			   Record_CCK_40Mindex;
	//2007/09/10 Mars Add CCK TX Power Tracking
	ccktxbbgain_struct	cck_txbbgain_table[CCKTxBBGainTableLength];
	ccktxbbgain_struct	cck_txbbgain_ch14_table[CCKTxBBGainTableLength];
	u8 rfa_txpowertrackingindex;
	u8 rfa_txpowertrackingindex_real;
	u8 rfa_txpowertracking_default;
	u8 rfc_txpowertrackingindex;
	u8 rfc_txpowertrackingindex_real;
	u8 rfc_txpowertracking_default;
	bool btxpower_tracking;
	bool bcck_in_ch14;

	//For Backup Initial Gain
	init_gain initgain_backup;
	u8		DefaultInitialGain[4];
	// For EDCA Turbo mode, Added by amy 080515.
	bool		bis_any_nonbepkts;
	bool		bcurrent_turbo_EDCA;

	bool SetFwCmdInProgress; //is set FW CMD in Progress? 92S only
	u8 CurrentFwCmdIO;

	// L1 and L2 high power threshold.
	u8	MidHighPwrTHR_L1;
	u8	MidHighPwrTHR_L2;


	bool		bis_cur_rdlstate;
	struct timer_list fsync_timer;
	bool bfsync_processing;	// 500ms Fsync timer is active or not
	u32	rate_record;
	u32	rateCountDiffRecord;
	u32	ContiuneDiffCount;
	bool bswitch_fsync;

	u8	framesync;
	u32	framesyncC34;
	u8	framesyncMonitor;
	//Added by amy 080516  for RX related
	u16	nrxAMPDU_size;
	u8	nrxAMPDU_aggr_num;

	//Last RxDesc TSF value*/
	u32 last_rxdesc_tsf_high;
	u32 last_rxdesc_tsf_low;

	//by amy for gpio
	bool bHwRadioOff;
	bool pwrdown;
	bool blinked_ingpio;
	// ASPM related variable  */
	bool aspm_clkreq_enable;
	u32 pci_bridge_vendor;
	// pci-e bridge */
	u8 RegHostPciASPMSetting;
	int PciBridgeASPMRegOffset;
	u8 PciBridgeLinkCtrlReg;
	// pci-e device */
	u8 RegDevicePciASPMSetting;
	int ASPMRegOffset;
	int ClkReqOffset;
	u8 LinkCtrlReg;

	//by amy for ps
	bool RFChangeInProgress; // RF Chnage in progress, by Bruce, 2007-10-30
	bool SetRFPowerStateInProgress;
	bool bdisable_nic;//added by amy for IPS 090408
	RT_OP_MODE OpMode;
	//by amy for reset_count
	u32 reset_count;
	bool bpbc_pressed;
	//by amy for debug
	u32 txpower_checkcnt;
	u32 txpower_tracking_callback_cnt;
	u8 thermal_read_val[40];
	u8 thermal_readback_index;
	u32 ccktxpower_adjustcnt_not_ch14;
	u32 ccktxpower_adjustcnt_ch14;
	u8 tx_fwinfo_force_subcarriermode;
	u8 tx_fwinfo_force_subcarrierval;

	//by amy for silent reset
	RESET_TYPE	ResetProgress;
	bool		bForcedSilentReset;
	bool		bDisableNormalResetCheck;
	u16		TxCounter;
	u16		RxCounter;
	int		IrpPendingCount;
	bool		bResetInProgress;
	bool		force_reset;
	bool		force_lps;
	u8		InitialGainOperateType;
	//by amy for LED control 090313
	//
	// LED control.
	//
#if defined RTL8192SE || defined RTL8192CE
	LED_STRATEGY_8190	LedStrategy;
	LED_8190			SwLed0;
	LED_8190			SwLed1;
#endif
	//define work item by amy 080526
	delayed_work_struct_rsl update_beacon_wq;
	delayed_work_struct_rsl watch_dog_wq;
	delayed_work_struct_rsl txpower_tracking_wq;
	delayed_work_struct_rsl rfpath_check_wq;
	delayed_work_struct_rsl gpio_change_rf_wq;
	delayed_work_struct_rsl initialgain_operate_wq;
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,5,0)
	struct workqueue_struct *priv_wq;
#else
	u32 *priv_wq;
#endif
	//#ifdef POLLING_METHOD_FOR_RADIO
	struct timer_list gpio_polling_timer;
	u8     polling_timer_on;
	//#endif
	bool bDriverIsGoingToUnload;

	//These are for MP
	bool	chan_forced;
	bool bSingleCarrier;
	bool RegBoard;
	bool	bCckContTx; // true if we are in CCK Continuous Tx test.
	bool	bOfdmContTx; // true if we are in OFDM Continuous Tx test.
	bool	bStartContTx; // true if we have start Continuous Tx test.
	// PA model.
	u8	RegPaModel; // See "PA Model" defined above, 090627, by rcnjko.
	u8	btMpCckTxPower; // Register value kept for Single Carrier Tx test.
	u8	btMpOfdmTxPower; // Register value kept for Single Carrier Tx test.

	// Variable needed in each implementation of CurrMptAct.
	u32	MptActType;	// Type of action performed in CurrMptAct.
	// The Offset of IO operation is depend of MptActType.
	u32	MptIoOffset;
	// The Value of IO operation is depend of MptActType.
	u32	MptIoValue;
	// The RfPath of IO operation is depend of MptActType.
	u32	MptRfPath;

	u32	MptBandWidth;			// bandwidth to switch.
	u32	MptRateIndex;			// rate index.
	u8	MptChannelToSw;			// Channel to switch.
	u32  MptRCR;
	//These are for MP

#ifdef RTL8192CE
	u8	EEPROMTSSI[2];
	u8	EEPROMPwrLimitHT20[3];
	u8	EEPROMPwrLimitHT40[3];
	u8	EEPROMChnlAreaTxPwrCCK[2][3];
	u8	EEPROMChnlAreaTxPwrHT40_1S[2][3];
	u8	EEPROMChnlAreaTxPwrHT40_2SDiff[2][3];
	u8	TxPwrLevelCck[2][14];
	u8	TxPwrLevelHT40_1S[2][14];		// For HT 40MHZ pwr
	u8	TxPwrLevelHT40_2S[2][14];		// For HT 40MHZ pwr
#endif
	// dm control initial gain flag
	bool	bDMInitialGainEnable;
        bool    MutualAuthenticationFail;
	// Frame bursting setting.
	bool	bDisableFrameBursting;
	bool	bLedOpenDrain; // Support Open-drain arrangement for controlling the LED. Added by Roger, 2009.10.16.
}r8192_priv;


extern const struct ethtool_ops rtl819x_ethtool_ops;

bool init_firmware(struct net_device *dev);
void rtl819xE_tx_cmd(struct net_device *dev, struct sk_buff *skb);
short rtl8192_tx(struct net_device *dev, struct sk_buff* skb);
u8 read_nic_byte(struct net_device *dev, int x);
u8 read_nic_byte_E(struct net_device *dev, int x);
u32 read_nic_dword(struct net_device *dev, int x);
u16 read_nic_word(struct net_device *dev, int x) ;
void write_nic_byte(struct net_device *dev, int x,u8 y);
void write_nic_byte_E(struct net_device *dev, int x,u8 y);
void write_nic_word(struct net_device *dev, int x,u16 y);
void write_nic_dword(struct net_device *dev, int x,u32 y);
void force_pci_posting(struct net_device *dev);

void rtl8192_rx_enable(struct net_device *);
void rtl8192_tx_enable(struct net_device *);

void rtl8192_disassociate(struct net_device *dev);
//void fix_rx_fifo(struct net_device *dev);
void rtl8185_set_rf_pins_enable(struct net_device *dev,u32 a);

int rtl8192_hard_start_xmit(struct sk_buff *skb,struct net_device *dev);
void rtl8192_hard_data_xmit(struct sk_buff *skb, struct net_device *dev, int rate);
void rtl8192_data_hard_stop(struct net_device *dev);
void rtl8192_data_hard_resume(struct net_device *dev);
void rtl8192_restart(void *data);
void rtl819x_watchdog_wqcallback(void *data);
void rtl8192_hw_sleep_wq (void *data);
void watch_dog_timer_callback(unsigned long data);
void rtl8192_irq_rx_tasklet(struct r8192_priv *priv);
void rtl8192_irq_tx_tasklet(struct r8192_priv *priv);
//void rtl8192_set_anaparam(struct net_device *dev,u32 a);
//void rtl8185_set_anaparam2(struct net_device *dev,u32 a);
void rtl8192_update_msr(struct net_device *dev);
int rtl8192_down(struct net_device *dev,bool shutdownrf);
int rtl8192_up(struct net_device *dev);
void rtl8192_commit(struct net_device *dev);
void rtl8192_set_chan(struct net_device *dev,short ch);

//short check_nic_enough_desc(struct net_device *dev, priority_t priority);
void rtl8192_start_beacon(struct net_device *dev);

//extern	void	dm_cck_txpower_adjust(struct net_device *dev,bool  binch14);
extern void firmware_init_param(struct net_device *dev);
extern bool cmpk_message_handle_tx(struct net_device *dev, u8* codevirtualaddress, u32 packettype, u32 buffer_len);
void rtl8192_hw_wakeup_wq(void *data);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
void rtl8192_interrupt(int irq, void *netdev, struct pt_regs *regs);
#else
irqreturn_t rtl8192_interrupt(int irq, void *netdev, struct pt_regs *regs);
#endif
#else
irqreturn_t rtl8192_interrupt(int irq, void *netdev);
#endif

short rtl8192_pci_initdescring(struct net_device *dev);

void rtl8192_cancel_deferred_work(struct r8192_priv* priv);

int _rtl8192_up(struct net_device *dev);

short rtl8192_is_tx_queue_empty(struct net_device *dev);

void check_rfctrl_gpio_timer(unsigned long data);
u8 HalSetSysClk8192SE(struct net_device *dev, u8 Data);
void gen_RefreshLedState(struct net_device *dev);
#define IS_HARDWARE_TYPE_819xP(_priv) ((((struct r8192_priv*)rtllib_priv(dev))->card_8192==NIC_8190P)||\
					(((struct r8192_priv*)rtllib_priv(dev))->card_8192==NIC_8192E))
#define IS_HARDWARE_TYPE_8192SE(_priv)	(((struct r8192_priv*)rtllib_priv(dev))->card_8192==NIC_8192SE)
#define IS_HARDWARE_TYPE_8192CE(_priv)	(((struct r8192_priv*)rtllib_priv(dev))->card_8192==NIC_8192CE)
#define IS_HARDWARE_TYPE_8192CU(_priv)	(((struct r8192_priv*)rtllib_priv(dev))->card_8192==NIC_8192CU)
#define IS_HARDWARE_TYPE_8192DE(_priv)	(((struct r8192_priv*)rtllib_priv(dev))->card_8192==NIC_8192DE)
#define IS_HARDWARE_TYPE_8192DU(_priv)	(((struct r8192_priv*)rtllib_priv(dev))->card_8192==NIC_8192DU)
extern void dm_InitRateAdaptiveMask(struct net_device * dev);

void tx_timeout(struct net_device *dev);
void rtl8192_pci_resetdescring(struct net_device *dev);
void rtl8192_SetWirelessMode(struct net_device* dev, u8 wireless_mode);
void rtl8192_irq_enable(struct net_device *dev);
void rtl8192_config_rate(struct net_device* dev, u16* rate_config);
void rtl8192_update_cap(struct net_device* dev, u16 cap);
u8 QueryIsShort(u8 TxHT, u8 TxRate, cb_desc *tcb_desc);
u8 MapHwQueueToFirmwareQueue(u8 QueueID, u8 priority);
void UpdateReceivedRateHistogramStatistics8190(struct net_device *dev, struct rtllib_rx_stats *pstats);
void UpdateRxPktTimeStamp8190 (struct net_device *dev, struct rtllib_rx_stats *stats);
void TranslateRxSignalStuff819xpci(struct net_device *dev, struct sk_buff *skb,
		struct rtllib_rx_stats * pstats, prx_desc pdesc, prx_fwinfo pdrvinfo);
void rtl8192_query_rxphystatus(
	struct r8192_priv * priv,
	struct rtllib_rx_stats * pstats,
	prx_desc  pdesc,
	prx_fwinfo   pdrvinfo,
	struct rtllib_rx_stats * precord_stats,
	bool bpacket_match_bssid,
	bool bpacket_toself,
	bool bPacketBeacon,
	bool bToSelfBA
	);
void rtl8192_process_phyinfo(struct r8192_priv * priv, u8* buffer,struct rtllib_rx_stats * pprevious_stats, struct rtllib_rx_stats * pcurrent_stats);
void rtl8192_irq_disable(struct net_device *dev);


bool NicIFEnableNIC(struct net_device* dev);
bool NicIFDisableNIC(struct net_device* dev);

bool
MgntActSet_RF_State(
	struct net_device* dev,
	RT_RF_POWER_STATE	StateToSet,
	RT_RF_CHANGE_SOURCE ChangeSource
	);
#endif
