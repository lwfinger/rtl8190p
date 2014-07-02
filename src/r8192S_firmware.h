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
#ifndef __INC_FIRMWARE_H
#define __INC_FIRMWARE_H


#define		RTL8190_MAX_FIRMWARE_CODE_SIZE	64000
#define		RTL8190_CPU_START_OFFSET			0x80
#define	MAX_FIRMWARE_CODE_SIZE	0xFF00

#define		H2C_TX_CMD_HDR_LEN				8

#define		RTL8192S_FW_PKT_FRAG_SIZE		0x4000

#ifdef RTL8192SE
#define GET_COMMAND_PACKET_FRAG_THRESHOLD(v)	4*(v/4) - 8
#else
#define GET_COMMAND_PACKET_FRAG_THRESHOLD(v)	(4*(v/4) - 8 - USB_HWDESC_HEADER_LEN)
#endif

typedef enum _DESC_PACKET_TYPE{
	DESC_PACKET_TYPE_INIT = 0,
	DESC_PACKET_TYPE_NORMAL = 1,
}DESC_PACKET_TYPE;


#ifdef RTL8192S
typedef enum _firmware_init_step{
	FW_INIT_STEP0_IMEM = 0,
	FW_INIT_STEP1_MAIN = 1,
	FW_INIT_STEP2_DATA = 2,
}firmware_init_step_e;
#else
typedef enum _firmware_init_step{
	FW_INIT_STEP0_BOOT = 0,
	FW_INIT_STEP1_MAIN = 1,
	FW_INIT_STEP2_DATA = 2,
}firmware_init_step_e;
#endif

typedef enum _firmware_source{
	FW_SOURCE_IMG_FILE = 0,
	FW_SOURCE_HEADER_FILE = 1,
}firmware_source_e, *pfirmware_source_e;

typedef enum _opt_rst_type{
	OPT_SYSTEM_RESET = 0,
	OPT_FIRMWARE_RESET = 1,
}opt_rst_type_e;

typedef enum _FIRMWARE_STATUS{
	FW_STATUS_0_INIT = 0,
	FW_STATUS_1_MOVE_BOOT_CODE = 1,
	FW_STATUS_2_MOVE_MAIN_CODE = 2,
	FW_STATUS_3_TURNON_CPU = 3,
	FW_STATUS_4_MOVE_DATA_CODE = 4,
	FW_STATUS_5_READY = 5,
}FIRMWARE_STATUS;

typedef  struct _RT_8192S_FIRMWARE_PRIV {

	u8		signature_0;
	u8		signature_1;
	u8		hci_sel;
	u8		chip_version;
	u8		customer_ID_0;
	u8		customer_ID_1;
	u8		rf_config;
	u8		usb_ep_num;

	u8		regulatory_class_0;
	u8		regulatory_class_1;
	u8		regulatory_class_2;
	u8		regulatory_class_3;
	u8		rfintfs;
	u8		def_nettype;
	u8		rsvd010;
	u8		rsvd011;


	u8		lbk_mode;
	u8		mp_mode;
	u8		rsvd020;
	u8		rsvd021;
	u8		rsvd022;
	u8		rsvd023;
	u8		rsvd024;
	u8		rsvd025;

	u8		qos_en;
	u8		bw_40MHz_en;
	u8		AMSDU2AMPDU_en;
	u8		AMPDU_en;
	u8		rate_control_offload;
	u8		aggregation_offload;
	u8		rsvd030;
	u8		rsvd031;


	unsigned char		beacon_offload;
	unsigned char		MLME_offload;
	unsigned char		hwpc_offload;
	unsigned char		tcp_checksum_offload;
	unsigned char		tcp_offload;
	unsigned char		ps_control_offload;
	unsigned char		WWLAN_offload;
	unsigned char		rsvd040;

	u8		tcp_tx_frame_len_L;
	u8		tcp_tx_frame_len_H;
	u8		tcp_rx_frame_len_L;
	u8		tcp_rx_frame_len_H;
	u8		rsvd050;
	u8		rsvd051;
	u8		rsvd052;
	u8		rsvd053;
}RT_8192S_FIRMWARE_PRIV, *PRT_8192S_FIRMWARE_PRIV;

typedef struct _RT_8192S_FIRMWARE_HDR {

	u16		Signature;
	u16		Version;
	u32		DMEMSize;


	u32		IMG_IMEM_SIZE;
	u32		IMG_SRAM_SIZE;

	u32		FW_PRIV_SIZE;
	u32		Rsvd0;

	u32		Rsvd1;
	u32		Rsvd2;

	RT_8192S_FIRMWARE_PRIV	FWPriv;

}RT_8192S_FIRMWARE_HDR, *PRT_8192S_FIRMWARE_HDR;

#define	RT_8192S_FIRMWARE_HDR_SIZE	80
#define   RT_8192S_FIRMWARE_HDR_EXCLUDE_PRI_SIZE	32

typedef enum _FIRMWARE_8192S_STATUS{
	FW_STATUS_INIT = 0,
	FW_STATUS_LOAD_IMEM = 1,
	FW_STATUS_LOAD_EMEM = 2,
	FW_STATUS_LOAD_DMEM = 3,
	FW_STATUS_READY = 4,
}FIRMWARE_8192S_STATUS;

typedef struct _rt_firmware{
	firmware_source_e	eFWSource;
	PRT_8192S_FIRMWARE_HDR	pFwHeader;
	FIRMWARE_8192S_STATUS	FWStatus;
	u16		FirmwareVersion;
	u8		FwIMEM[RTL8190_MAX_FIRMWARE_CODE_SIZE];
	u8		FwEMEM[RTL8190_MAX_FIRMWARE_CODE_SIZE];
	u32		FwIMEMLen;
	u32		FwEMEMLen;
	u8		szFwTmpBuffer[164000];
	u32		szFwTmpBufferLen;
	u16		CmdPacketFragThresold;
}rt_firmware, *prt_firmware;

#define		FW_DIG_ENABLE_CTL			BIT0
#define		FW_HIGH_PWR_ENABLE_CTL	BIT1
#define		FW_SS_CTL					BIT2
#define		FW_RA_INIT_CTL				BIT3
#define		FW_RA_BG_CTL				BIT4
#define		FW_RA_N_CTL				BIT5
#define		FW_PWR_TRK_CTL			BIT6
#define		FW_IQK_CTL					BIT7
#define		FW_FA_CTL					BIT8
#define		FW_DRIVER_CTRL_DM_CTL		BIT9
#define		FW_DISABLE_ALL_DM			0

#define		FW_PWR_TRK_PARAM_CLR		0x0000ffff
#define		FW_RA_PARAM_CLR			0xffff0000

#define FW_CMD_IO_CLR(priv, _Bit)		\
	udelay(1000);	\
	priv->FwCmdIOMap &= (~_Bit);

#define FW_CMD_IO_UPDATE(priv, _val)		\
	priv->FwCmdIOMap = _val;

#define FW_CMD_IO_SET(priv, _val)	\
	write_nic_word(priv->rtllib->dev, LBUS_MON_ADDR, (u16)_val);	\
	FW_CMD_IO_UPDATE(priv, _val);\

#define FW_CMD_PARA_SET(priv, _val)		\
	write_nic_dword(priv->rtllib->dev, LBUS_ADDR_MASK, _val);	\
	priv->FwCmdIOParam = _val;

#define FW_CMD_IO_QUERY(priv)	(u16)(priv->FwCmdIOMap)
#define FW_CMD_IO_PARA_QUERY(priv)	((u32)(priv->FwCmdIOParam))

bool FirmwareEnableCPU(struct net_device *dev);
bool FirmwareCheckReady(struct net_device *dev, u8 LoadFWStatus);

bool FirmwareDownload92S(struct net_device *dev);

#endif
