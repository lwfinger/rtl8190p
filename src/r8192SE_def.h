/******************************************************************************
 * Copyright(c) 2008 - 2010 Realtek Corporation. All rights reserved.
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
#ifndef R8192SE_DEF_H
#define R8192SE_DEF_H

#include <linux/types.h>

#define HAL_RETRY_LIMIT_INFRA		48
#define HAL_RETRY_LIMIT_AP_ADHOC	7

#define	HAL_DM_DIG_DISABLE				BIT0
#define	HAL_DM_HIPWR_DISABLE				BIT1


#define TX_DESC_SIZE				32

#define RX_DESC_SIZE				24
#define RX_DRV_INFO_SIZE_UNIT	8

#if 0
#define BK_QUEUE					0
#define BE_QUEUE					1
#define VI_QUEUE					2
#define VO_QUEUE				3
#define BEACON_QUEUE			4
#define TXCMD_QUEUE				5
#define MGNT_QUEUE				6
#define HIGH_QUEUE				7
#define HCCA_QUEUE				8

#define LOW_QUEUE				BE_QUEUE
#define NORMAL_QUEUE			MGNT_QUEUE
#endif

#define RX_MPDU_QUEUE			0
#define RX_CMD_QUEUE			1
#define RX_MAX_QUEUE			2


typedef enum _rtl819x_loopback{
	RTL819X_NO_LOOPBACK = 0,
	RTL819X_MAC_LOOPBACK = 1,
	RTL819X_DMA_LOOPBACK = 2,
	RTL819X_CCK_LOOPBACK = 3,
}rtl819x_loopback_e;

#define RESET_DELAY_8185		20

#define RT_IBSS_INT_MASKS		0

#define DESC92S_RATE1M			0x00
#define DESC92S_RATE2M			0x01
#define DESC92S_RATE5_5M		0x02
#define DESC92S_RATE11M			0x03
#define DESC92S_RATE6M			0x04
#define DESC92S_RATE9M			0x05
#define DESC92S_RATE12M			0x06
#define DESC92S_RATE18M			0x07
#define DESC92S_RATE24M			0x08
#define DESC92S_RATE36M			0x09
#define DESC92S_RATE48M			0x0a
#define DESC92S_RATE54M			0x0b
#define DESC92S_RATEMCS0		0x0c
#define DESC92S_RATEMCS1		0x0d
#define DESC92S_RATEMCS2		0x0e
#define DESC92S_RATEMCS3		0x0f
#define DESC92S_RATEMCS4		0x10
#define DESC92S_RATEMCS5		0x11
#define DESC92S_RATEMCS6		0x12
#define DESC92S_RATEMCS7		0x13
#define DESC92S_RATEMCS8		0x14
#define DESC92S_RATEMCS9		0x15
#define DESC92S_RATEMCS10		0x16
#define DESC92S_RATEMCS11		0x17
#define DESC92S_RATEMCS12		0x18
#define DESC92S_RATEMCS13		0x19
#define DESC92S_RATEMCS14		0x1a
#define DESC92S_RATEMCS15		0x1b
#define DESC92S_RATEMCS15_SG	0x1c
#define DESC92S_RATEMCS32		0x20

#define SHORT_SLOT_TIME			9
#define NON_SHORT_SLOT_TIME	20


#define	MAX_LINES_HWCONFIG_TXT			1000
#define MAX_BYTES_LINE_HWCONFIG_TXT		256

#define SW_THREE_WIRE			0
#define HW_THREE_WIRE			2

#define BT_DEMO_BOARD			0
#define BT_QA_BOARD				1
#define BT_FPGA					2

#define	Rx_Smooth_Factor			20

#define QSLT_BK					0x2
#define QSLT_BE					0x0
#define QSLT_VI					0x5
#define QSLT_VO					0x7
#define QSLT_BEACON				0x10
#define QSLT_HIGH				0x11
#define QSLT_MGNT				0x12
#define QSLT_CMD				0x13

#define NUM_OF_FIRMWARE_QUEUE				10
#define NUM_OF_PAGES_IN_FW					0x100
#define NUM_OF_PAGE_IN_FW_QUEUE_BK		0x07
#define NUM_OF_PAGE_IN_FW_QUEUE_BE		0x07
#define NUM_OF_PAGE_IN_FW_QUEUE_VI		0x07
#define NUM_OF_PAGE_IN_FW_QUEUE_VO		0x07
#define NUM_OF_PAGE_IN_FW_QUEUE_HCCA		0x0
#define NUM_OF_PAGE_IN_FW_QUEUE_CMD		0x0
#define NUM_OF_PAGE_IN_FW_QUEUE_MGNT		0x02
#define NUM_OF_PAGE_IN_FW_QUEUE_HIGH		0x02
#define NUM_OF_PAGE_IN_FW_QUEUE_BCN		0x2
#define NUM_OF_PAGE_IN_FW_QUEUE_PUB		0xA1

#define NUM_OF_PAGE_IN_FW_QUEUE_BK_DTM	0x026
#define NUM_OF_PAGE_IN_FW_QUEUE_BE_DTM	0x048
#define NUM_OF_PAGE_IN_FW_QUEUE_VI_DTM	0x048
#define NUM_OF_PAGE_IN_FW_QUEUE_VO_DTM	0x026
#define NUM_OF_PAGE_IN_FW_QUEUE_PUB_DTM	0x00

#define HAL_PRIME_CHNL_OFFSET_DONT_CARE	0
#define HAL_PRIME_CHNL_OFFSET_LOWER		1
#define HAL_PRIME_CHNL_OFFSET_UPPER		2



#define		MAX_SILENT_RESET_RX_SLOT_NUM	10
typedef enum tag_Rf_OpType
{
    RF_OP_By_SW_3wire = 0,
    RF_OP_By_FW,
    RF_OP_MAX
}RF_OpType_E;


typedef	enum _POWER_SAVE_MODE
{
	POWER_SAVE_MODE_ACTIVE,
	POWER_SAVE_MODE_SAVE,
}POWER_SAVE_MODE;

typedef	enum _INTERFACE_SELECT_8190PCI{
	INTF_SEL1_MINICARD	= 0,
	INTF_SEL0_PCIE			= 1,
	INTF_SEL2_RSV			= 2,
	INTF_SEL3_RSV			= 3,
} INTERFACE_SELECT_8190PCI, *PINTERFACE_SELECT_8190PCI;


typedef struct _BB_REGISTER_DEFINITION{
	u32 rfintfs;
	u32 rfintfi;
	u32 rfintfo;
	u32 rfintfe;
	u32 rf3wireOffset;
	u32 rfLSSI_Select;
	u32 rfTxGainStage;
	u32 rfHSSIPara1;
	u32 rfHSSIPara2;
	u32 rfSwitchControl;
	u32 rfAGCControl1;
	u32 rfAGCControl2;
	u32 rfRxIQImbalance;
	u32 rfRxAFE;
	u32 rfTxIQImbalance;
	u32 rfTxAFE;
	u32 rfLSSIReadBack;
	u32 rfLSSIReadBackPi;
}BB_REGISTER_DEFINITION_T, *PBB_REGISTER_DEFINITION_T;




typedef struct  _rx_fwinfo_8192s{

	/*u32			gain_0:7;
	u32			trsw_0:1;
	u32			gain_1:7;
	u32			trsw_1:1;
	u32			gain_2:7;
	u32			trsw_2:1;
	u32			gain_3:7;
	u32			trsw_3:1;	*/
	u8			gain_trsw[4];

	/*u32			pwdb_all:8;
	u32			cfosho_0:8;
	u32			cfosho_1:8;
	u32			cfosho_2:8;*/
	u8			pwdb_all;
	u8			cfosho[4];

	/*u32			cfosho_3:8;
	u32			cfotail_0:8;
	u32			cfotail_1:8;
	u32			cfotail_2:8;*/
	u8			cfotail[4];

	/*u32			cfotail_3:8;
	u32			rxevm_0:8;
	u32			rxevm_1:8;
	u32			rxsnr_0:8;*/
	s8			rxevm[2];
	s8			rxsnr[4];

	/*u32			rxsnr_1:8;
	u32			rxsnr_2:8;
	u32			rxsnr_3:8;
	u32			pdsnr_0:8;*/
	u8			pdsnr[2];

	/*u32			pdsnr_1:8;
	u32			csi_current_0:8;
	u32			csi_current_1:8;
	u32			csi_target_0:8;*/
	u8			csi_current[2];
	u8			csi_target[2];

	/*u32			csi_target_1:8;
	u32			sigevm:8;
	u32			max_ex_pwr:8;
	u32			ex_intf_flag:1;
	u32			sgi_en:1;
	u32			rxsc:2;
	u32			reserve:4;*/
	u8			sigevm;
	u8			max_ex_pwr;
	u8			ex_intf_flag:1;
	u8			sgi_en:1;
	u8			rxsc:2;
	u8			reserve:4;

}rx_fwinfo, *prx_fwinfo;

typedef struct _LOG_INTERRUPT_8190
{
	u32	nIMR_COMDOK;
	u32	nIMR_MGNTDOK;
	u32	nIMR_HIGH;
	u32	nIMR_VODOK;
	u32	nIMR_VIDOK;
	u32	nIMR_BEDOK;
	u32	nIMR_BKDOK;
	u32	nIMR_ROK;
	u32	nIMR_RCOK;
	u32	nIMR_TBDOK;
	u32	nIMR_BDOK;
	u32	nIMR_RXFOVW;
} LOG_INTERRUPT_8190_T, *PLOG_INTERRUPT_8190_T;

typedef struct _phy_cck_rx_status_report_819xpci
{
	u8	adc_pwdb_X[4];
	u8	sq_rpt;
	u8	cck_agc_rpt;
}phy_sts_cck_819xpci_t, phy_sts_cck_8192s_t;

#define		PHY_RSSI_SLID_WIN_MAX				100
#define		PHY_LINKQUALITY_SLID_WIN_MAX		20
#define		PHY_Beacon_RSSI_SLID_WIN_MAX		10

typedef struct _tx_desc_8192se{

	u32		PktSize:16;
	u32		Offset:8;
	u32		Type:2;
	u32		LastSeg:1;
	u32		FirstSeg:1;
	u32		LINIP:1;
	u32		AMSDU:1;
	u32		GF:1;
	u32		OWN:1;

	u32		MacID:5;
	u32		MoreData:1;
	u32		MoreFrag:1;
	u32		PIFS:1;
	u32		QueueSel:5;
	u32		AckPolicy:2;
	u32		NoACM:1;
	u32		NonQos:1;
	u32		KeyID:2;
	u32		OUI:1;
	u32		PktType:1;
	u32		EnDescID:1;
	u32		SecType:2;
	u32		HTC:1;
	u32		WDS:1;
	u32		PktOffset:5;
	u32		HWPC:1;

	u32		DataRetryLmt:6;
	u32		RetryLmtEn:1;
	u32		TSFL:5;
	u32		RTSRC:6;
	u32		DATARC:6;
#ifdef _RTL8192_EXT_PATCH_
	u32		Rsvd_MacID:5;
#else
	u32		Rsvd1:5;
#endif
	u32		AggEn:1;
	u32		BK:1;
	u32		OwnMAC:1;

	u32		NextHeadPage:8;
	u32		TailPage:8;
	u32		Seq:12;
	u32		Frag:4;

	u32		RTSRate:6;
	u32		DisRTSFB:1;
	u32		RTSRateFBLmt:4;
	u32		CTS2Self:1;
	u32		RTSEn:1;
	u32		RaBRSRID:3;
	u32		TXHT:1;
	u32		TxShort:1;
	u32		TxBw:1;
	u32		TXSC:2;
	u32		STBC:2;
	u32		RD:1;
	u32		RTSHT:1;
	u32		RTSShort:1;
	u32		RTSBW:1;
	u32		RTSSC:2;
	u32		RTSSTBC:2;
	u32		UserRate:1;

	u32		PktID:9;
	u32		TxRate:6;
	u32		DISFB:1;
	u32		DataRateFBLmt:5;
	u32		TxAGC:11;

	u32		IPChkSum:16;
	u32		TCPChkSum:16;

	u32		TxBufferSize:16;
	u32		IPHdrOffset:8;
	u32		Rsvd3:7;
	u32		TCPEn:1;

	u32		TxBuffAddr;

	u32		NextDescAddress;

	u32		Reserve_Pass_92S_PCIE_MM_Limit[6];

} tx_desc, *ptx_desc;


typedef struct _tx_desc_cmd_8192se{
	u32		PktSize:16;
	u32		Offset:8;
	u32		Rsvd0:2;
	u32		FirstSeg:1;
	u32		LastSeg:1;
	u32		LINIP:1;
	u32		Rsvd1:2;
	u32		OWN:1;

	u32		Rsvd2;
	u32		Rsvd3;
	u32		Rsvd4;
	u32		Rsvd5;
	u32		Rsvd6;
	u32		Rsvd7;

	u32		TxBufferSize:16;
	u32		Rsvd8:16;

	u32		TxBufferAddr;

	u32		NextTxDescAddress;

	u32		Reserve_Pass_92S_PCIE_MM_Limit[6];

}tx_desc_cmd, *ptx_desc_cmd;


typedef struct _tx_status_desc_8192se{

	u32		PktSize:16;
	u32		Offset:8;
	u32		Type:2;
	u32		LastSeg:1;
	u32		FirstSeg:1;
	u32		LINIP:1;
	u32		AMSDU:1;
	u32		GF:1;
	u32		OWN:1;

	u32		MacID:5;
	u32		MoreData:1;
	u32		MoreFrag:1;
	u32		PIFS:1;
	u32		QueueSel:5;
	u32		AckPolicy:2;
	u32		NoACM:1;
	u32		NonQos:1;
	u32		KeyID:2;
	u32		OUI:1;
	u32		PktType:1;
	u32		EnDescID:1;
	u32		SecType:2;
	u32		HTC:1;
	u32		WDS:1;
	u32		PktOffset:5;
	u32		HWPC:1;

	u32		DataRetryLmt:6;
	u32		RetryLmtEn:1;
	u32		TSFL:5;
	u32		RTSRC:6;
	u32		DATARC:6;
	u32		Rsvd1:5;
	u32		AggEn:1;
	u32		BK:1;
	u32		OwnMAC:1;

	u32		NextHeadPage:8;
	u32		TailPage:8;
	u32		Seq:12;
	u32		Frag:4;

	u32		RTSRate:6;
	u32		DisRTSFB:1;
	u32		RTSRateFBLmt:4;
	u32		CTS2Self:1;
	u32		RTSEn:1;
	u32		RaBRSRID:3;
	u32		TXHT:1;
	u32		TxShort:1;
	u32		TxBw:1;
	u32		TXSC:2;
	u32		STBC:2;
	u32		RD:1;
	u32		RTSHT:1;
	u32		RTSShort:1;
	u32		RTSBW:1;
	u32		RTSSC:2;
	u32		RTSSTBC:2;
	u32		UserRate:1;

	u32		PktID:9;
	u32		TxRate:6;
	u32		DISFB:1;
	u32		DataRateFBLmt:5;
	u32		TxAGC:11;

	u32		IPChkSum:16;
	u32		TCPChkSum:16;

	u32		TxBufferSize:16;
	u32		IPHdrOffset:8;
	u32		Rsvd2:7;
	u32		TCPEn:1;

	u32		TxBufferAddr;

	u32		NextDescAddress;

	u32		Reserve_Pass_92S_PCIE_MM_Limit[6];

}tx_status_desc, *ptx_status_desc;


typedef struct _rx_desc_8192se{
	u32		Length:14;
	u32		CRC32:1;
	u32		ICVError:1;
	u32		DrvInfoSize:4;
	u32		Security:3;
	u32		Qos:1;
	u32		Shift:2;
	u32		PHYStatus:1;
	u32		SWDec:1;
	u32		LastSeg:1;
	u32		FirstSeg:1;
	u32		EOR:1;
	u32		OWN:1;

	u32		MACID:5;
	u32		TID:4;
	u32		HwRsvd:5;
	u32		PAGGR:1;
	u32		FAGGR:1;
	u32		A1_FIT:4;
	u32		A2_FIT:4;
	u32		PAM:1;
	u32		PWR:1;
	u32		MoreData:1;
	u32		MoreFrag:1;
	u32		Type:2;
	u32		MC:1;
	u32		BC:1;

	u32		Seq:12;
	u32		Frag:4;
	u32		NextPktLen:14;
	u32		NextIND:1;
	u32		Rsvd:1;

	u32		RxMCS:6;
	u32		RxHT:1;
	u32		AMSDU:1;
	u32		SPLCP:1;
	u32		BandWidth:1;
	u32		HTC:1;
	u32		TCPChkRpt:1;
	u32		IPChkRpt:1;
	u32		TCPChkValID:1;
	u32		HwPCErr:1;
	u32		HwPCInd:1;
	u32		IV0:16;

	u32		IV1;

	u32		TSFL;

	u32		BufferAddress;

	u32		NextRxDescAddress;

#if 0
	u32		BA_SSN:12;
	u32		BA_VLD:1;
	u32		RSVD:19;
#endif

} rx_desc, *prx_desc;



typedef struct _rx_desc_status_8192se{
	u32		Length:14;
	u32		CRC32:1;
	u32		ICVError:1;
	u32		DrvInfoSize:4;
	u32		Security:3;
	u32		Qos:1;
	u32		Shift:2;
	u32		PHYStatus:1;
	u32		SWDec:1;
	u32		LastSeg:1;
	u32		FirstSeg:1;
	u32		EOR:1;
	u32		OWN:1;

	u32		MACID:5;
	u32		TID:4;
	u32		HwRsvd:5;
	u32		PAGGR:1;
	u32		FAGGR:1;
	u32		A1_FIT:4;
	u32		A2_FIT:4;
	u32		PAM:1;
	u32		PWR:1;
	u32		MoreData:1;
	u32		MoreFrag:1;
	u32		Type:2;
	u32		MC:1;
	u32		BC:1;

	u32		Seq:12;
	u32		Frag:4;
	u32		NextPktLen:14;
	u32		NextIND:1;
	u32		Rsvd:1;

	u32		RxMCS:6;
	u32		RxHT:1;
	u32		AMSDU:1;
	u32		SPLCP:1;
	u32		BW:1;
	u32		HTC:1;
	u32		TCPChkRpt:1;
	u32		IPChkRpt:1;
	u32		TCPChkValID:1;
	u32		HwPCErr:1;
	u32		HwPCInd:1;
	u32		IV0:16;

	u32		IV1;

	u32		TSFL;


	u32		BufferAddress;

	u32		NextRxDescAddress;

#if 0
	u32		BA_SSN:12;
	u32		BA_VLD:1;
	u32		RSVD:19;
#endif
}rx_desc_status, *prx_desc_status;

typedef enum _HAL_FW_C2H_CMD_ID
{
	HAL_FW_C2H_CMD_Read_MACREG = 0,
	HAL_FW_C2H_CMD_Read_BBREG = 1,
	HAL_FW_C2H_CMD_Read_RFREG = 2,
	HAL_FW_C2H_CMD_Read_EEPROM = 3,
	HAL_FW_C2H_CMD_Read_EFUSE = 4,
	HAL_FW_C2H_CMD_Read_CAM = 5,
	HAL_FW_C2H_CMD_Get_BasicRate = 6,
	HAL_FW_C2H_CMD_Get_DataRate = 7,
	HAL_FW_C2H_CMD_Survey = 8 ,
	HAL_FW_C2H_CMD_SurveyDone = 9,
	HAL_FW_C2H_CMD_JoinBss = 10,
	HAL_FW_C2H_CMD_AddSTA = 11,
	HAL_FW_C2H_CMD_DelSTA = 12,
	HAL_FW_C2H_CMD_AtimDone = 13,
	HAL_FW_C2H_CMD_TX_Report = 14,
	HAL_FW_C2H_CMD_CCX_Report = 15,
	HAL_FW_C2H_CMD_DTM_Report = 16,
	HAL_FW_C2H_CMD_TX_Rate_Statistics = 17,
	HAL_FW_C2H_CMD_C2HLBK = 18,
	HAL_FW_C2H_CMD_C2HDBG = 19,
	HAL_FW_C2H_CMD_C2HFEEDBACK = 20,
	HAL_FW_C2H_CMD_MAX
}HAL_FW_C2H_CMD_ID;

#define	HAL_FW_C2H_CMD_C2HFEEDBACK_CCX_PER_PKT_RPT			0x04
#define	HAL_FW_C2H_CMD_C2HFEEDBACK_DTM_TX_STATISTICS_RPT	0x05
#endif
