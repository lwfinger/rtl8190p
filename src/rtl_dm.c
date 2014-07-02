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
#ifdef RTL8192SE
#include "rtl_core.h"
#include "rtl_dm.h"
#include "r8192S_phy.h"
#include "r8192S_phyreg.h"
#else
#include "rtl_core.h"
#include "rtl_dm.h"
#include "r8192E_hw.h"
#include "r819xE_phy.h"
#include "r819xE_phyreg.h"
#include "r8190_rtl8256.h"
#endif
#ifdef _RTL8192_EXT_PATCH_
#include "../../mshclass/msh_class.h"
#endif

/*---------------------------Define Local Constant---------------------------*/
//
// Indicate different AP vendor for IOT issue.
//
#ifdef  RTL8190P
static u32 edca_setting_DL[HT_IOT_PEER_MAX] =
{ 0x5e4322,	// UNKNOWN
   0x5e4322,	//REALTEK_90
   0x5ea44f,	//REALTEK_92SE TMP
   0x5e4322,	//BROADCOM
   0x604322,	//RALINK
   0xa44f,		//ATHEROS
   0x5e4322,	//CISCO
   0x5e4322	//MARVELL
 };

static u32 edca_setting_DL_GMode[HT_IOT_PEER_MAX] =
{ 0x5e4322,	// UNKNOWN
   0x5e4322,	//REALTEK_90
   0x5e4322,	//REALTEK_92SE TMP
   0x5e4322,	//BROADCOM
   0x604322,	//RALINK
   0xa44f,		//ATHEROS
   0x5e4322,	//CISCO
   0x5e4322	//MARVELL
};

static u32 edca_setting_UL[HT_IOT_PEER_MAX] =
{ 0x5e4322,	// UNKNOWN
   0xa44f,	//REALTEK_90
   0x5ea44f,	//REALTEK_92SE TMP
   0x5e4322,	 //BROADCOM
   0x604322,	//RALINK
   0x5e4322,	//ATHEROS
   0x5e4322,	//CISCO
   0x5e4322	//MARVELL
};

#elif defined RTL8192E
static u32 edca_setting_DL[HT_IOT_PEER_MAX] =
{ 0x5e4322,	// UNKNOWN
   0x5e4322,	//REALTEK_90
   0x5ea44f,	//REALTEK_92SE TMP
   0x5e4322,	//BROADCOM
   0x604322,	//RALINK
   0xa44f,		//ATHEROS
   0x5e4322,	//CISCO
   0x5e4332	//MARVELL
 };

static u32 edca_setting_DL_GMode[HT_IOT_PEER_MAX] =
{ 0x5e4322,	// UNKNOWN
   0x5e4322,	//REALTEK_90
   0x5e4322,	//REALTEK_92SE TMP
   0x5e4322,	//BROADCOM
   0x604322,	//RALINK
   0xa44f,		//ATHEROS
   0x5e4322,	//CISCO
   0x5e4322	//MARVELL
};

static u32 edca_setting_UL[HT_IOT_PEER_MAX] =
{ 0x5e4322,	// UNKNOWN
   0xa44f,		//REALTEK_90
   0x5ea44f,	//REALTEK_92SE TMP
   0x5e4322,	//BROADCOM
   0x604322,	//RALINK
   0x5e4322,	//ATHEROS
   0x5e4322,	//CISCO
   0x5e4332	//MARVELL
};

#elif defined(RTL8192SE)
static u32 edca_setting_DL[HT_IOT_PEER_MAX] =
{ 0xa44f,		// UNKNOWN
#ifdef _RTL8192_EXT_PATCH_
   0xa44f,		//REALTEK_90
#else
   0x5ea44f,	//REALTEK_90
#endif
   0x5ea44f,	//REALTEK_92SE
   0xa630,		//BROADCOM
   0xa44f,//0x5e4322,	//RALINK
#ifdef _RTL8192_EXT_PATCH_
   0x4322,//0xa42b,//		//ATHEROS
#else
   0xa630,		//0x4322,		//0xa42b,//		//ATHEROS
#endif
   0xa630,		//CISCO
   0xa42b,//0xa44f,		//MARVELL
   0x5e4322,	//92U_AP
   0x5e4322	//SELF_AP
 };

static u32 edca_setting_DL_GMode[HT_IOT_PEER_MAX] =

{ 0x4322,		// UNKNOWN
   0xa44f,		//REALTEK_90
#ifdef _RTL8192_EXT_PATCH_
   0x4322,		//REALTEK_92SE TMP
#else
   0x5ea44f,	//REALTEK_92SE TMP
#endif
   0xa42b,		//BROADCOM
   0x5e4322,	//RALINK
   0x4322,		//ATHEROS
   0xa430,		//CISCO
   0x5ea44f,	//MARVELL
   0x5e4322,	//92U_AP
   0x5e4322	//SELF_AP
};

static u32 edca_setting_UL[HT_IOT_PEER_MAX] =
{ 0x5e4322,	// UNKNOWN
#ifdef _RTL8192_EXT_PATCH_
   0x5ea44f,	//REALTEK_90
#else
   0xa44f,		//REALTEK_90
#endif
   0x5ea44f,	//REALTEK_92SE
   0x5ea322,	//BROADCOM
   0x5ea422,//0x5e4322,		//RALINK
   0x5ea322,	//ATHEROS
   0x3ea44f,	//CISCO
   0x5ea44f,	//MARVELL
   0x5e4322,	//92U_AP
   0x5e4322	//SELF_AP
 };
#endif

#define RTK_UL_EDCA 0xa44f
#define RTK_DL_EDCA 0x5e4322
/*---------------------------Define Local Constant---------------------------*/


/*------------------------Define global variable-----------------------------*/
// Debug variable ?
dig_t	dm_digtable;
// Store current shoftware write register content for MAC PHY.
u8		dm_shadow[16][256] = {{0}};
// For Dynamic Rx Path Selection by Signal Strength
DRxPathSel	DM_RxPathSelTable;
/*------------------------Define global variable-----------------------------*/


/*------------------------Define local variable------------------------------*/
/*------------------------Define local variable------------------------------*/


/*--------------------Define export function prototype-----------------------*/
extern	void	init_hal_dm(struct net_device *dev);
extern	void deinit_hal_dm(struct net_device *dev);

extern void hal_dm_watchdog(struct net_device *dev);


extern	void	init_rate_adaptive(struct net_device *dev);
extern	void	dm_txpower_trackingcallback(void *data);

//extern	void	dm_cck_txpower_adjust(struct net_device *dev,bool  binch14);
extern	void	dm_restore_dynamic_mechanism_state(struct net_device *dev);
extern	void	dm_backup_dynamic_mechanism_state(struct net_device *dev);
extern	void	dm_change_dynamic_initgain_thresh(struct net_device *dev,
								u32		dm_type,
								u32		dm_value);
extern	void	DM_ChangeFsyncSetting(struct net_device *dev,
												s32		DM_Type,
												s32		DM_Value);
extern	void dm_force_tx_fw_info(struct net_device *dev,
										u32		force_type,
										u32		force_value);
extern	void	dm_init_edca_turbo(struct net_device *dev);
extern	void	dm_rf_operation_test_callback(unsigned long data);
extern	void	dm_rf_pathcheck_workitemcallback(void *data);
extern	void dm_fsync_timer_callback(unsigned long data);
#if 0
extern	bool	dm_check_lbus_status(struct net_device *dev);
#endif
extern	void dm_check_fsync(struct net_device *dev);
extern	void	dm_shadow_init(struct net_device *dev);
extern	void dm_initialize_txpower_tracking(struct net_device *dev);

#if (defined RTL8192E || defined RTL8192SE)
extern  void    dm_gpio_change_rf_callback(void *data);
#endif

#ifdef RTL8192SE
extern	void DM_TXPowerTracking92SDirectCall(struct net_device *dev);
static	void dm_CtrlInitGainByTwoPort(struct net_device *dev);
static	void dm_CtrlInitGainBeforeConnectByRssiAndFalseAlarm(struct net_device *dev);
static	void	dm_initial_gain_STABeforeConnect(struct net_device *dev);
//static	void	dm_CheckStatistics(struct net_device *dev);
//static	void dm_PrintCVCurveBBSwing(struct net_device *dev);

// For Lenovo SMBios check, if Lenovo NIC and Lenovo Platform matched (find key)
// then Turn on RF and work ok, if not matched, turn off RF forever.
//void			dm_CheckMutualAuthentication(struct net_device *dev);
void	dm_InitRateAdaptiveMask(struct net_device *dev);
static	void dm_RefreshRateAdaptiveMask(struct net_device *dev);
static	void Adhoc_dm_CheckRateAdaptive(struct net_device * dev);
void		Adhoc_InitRateAdaptive(struct net_device *dev,struct sta_info  *pEntry);
#endif

/*--------------------Define export function prototype-----------------------*/


/*---------------------Define local function prototype-----------------------*/
// DM --> Rate Adaptive
static	void	dm_check_rate_adaptive(struct net_device *dev);

// DM --> Bandwidth switch
static	void	dm_init_bandwidth_autoswitch(struct net_device *dev);
static	void	dm_bandwidth_autoswitch(	struct net_device *dev);

// DM --> TX power control
//static	void	dm_initialize_txpower_tracking(struct net_device *dev);

static	void	dm_check_txpower_tracking(struct net_device *dev);



//static	void	dm_txpower_reset_recovery(struct net_device *dev);


// DM --> BB init gain restore
#if defined(RTL8192E)||defined(RTL8190P)
static	void	dm_bb_initialgain_restore(struct net_device *dev);


// DM --> BB init gain backup
static	void	dm_bb_initialgain_backup(struct net_device *dev);
#endif

// DM --> Dynamic Init Gain by RSSI
static	void	dm_dig_init(struct net_device *dev);
static	void	dm_ctrl_initgain_byrssi(struct net_device *dev);
static	void	dm_ctrl_initgain_byrssi_highpwr(struct net_device *dev);
static	void	dm_ctrl_initgain_byrssi_by_driverrssi(	struct net_device *dev);
static	void	dm_ctrl_initgain_byrssi_by_fwfalse_alarm(struct net_device *dev);
static	void	dm_initial_gain(struct net_device *dev);
static	void	dm_pd_th(struct net_device *dev);
static	void	dm_cs_ratio(struct net_device *dev);

static	void dm_init_ctstoself(struct net_device *dev);
static	void dm_Init_WA_Broadcom_IOT(struct net_device *dev);
#ifdef RTL8192SE
static	void dm_WA_Broadcom_IOT(struct net_device *dev);
#endif


// DM --> EDCA turboe mode control
static	void	dm_check_edca_turbo(struct net_device *dev);

// DM --> HW RF control
#if 0
static	void	dm_check_rfctrl_gpio(struct net_device *dev);
#endif

#ifndef RTL8190P
//static	void	dm_gpio_change_rf(struct net_device *dev);
#endif
// DM --> Check PBC
static	void dm_check_pbc_gpio(struct net_device *dev);


// DM --> Check current RX RF path state
static	void	dm_check_rx_path_selection(struct net_device *dev);
static	void dm_init_rxpath_selection(struct net_device *dev);
static	void dm_rxpath_sel_byrssi(struct net_device *dev);


// DM --> Fsync for broadcom ap
static void dm_init_fsync(struct net_device *dev);
static void dm_deInit_fsync(struct net_device *dev);

//Added by vivi, 20080522
static	void	dm_check_txrateandretrycount(struct net_device *dev);

static  void	dm_check_ac_dc_power(struct net_device *dev);
/*---------------------Define local function prototype-----------------------*/

//---------------------Define of Tx Power Control For Near/Far Range --------
//Add by Jacken 2008/02/18
static	void	dm_init_dynamic_txpower(struct net_device *dev);
static	void	dm_dynamic_txpower(struct net_device *dev);


// DM --> For rate adaptive and DIG, we must send RSSI to firmware
static	void dm_send_rssi_tofw(struct net_device *dev);
static	void	dm_ctstoself(struct net_device *dev);
#ifdef _RTL8192_EXT_PATCH_
static	void dm_refresh_rate_adaptive_mask(struct net_device *dev);
#endif
/*---------------------------Define function prototype------------------------*/
//================================================================================
//	HW Dynamic mechanism interface.
//================================================================================

//
//	Description:
//		Prepare SW resource for HW dynamic mechanism.
//
//	Assumption:
//		This function is only invoked at driver intialization once.
//
//
extern	void
init_hal_dm(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
#ifdef _RTL8192_EXT_PATCH_
	priv->DM_Type = DM_Type_ByDriver;
#endif

	// Undecorated Smoothed Signal Strength, it can utilized to dynamic mechanism.
	priv->undecorated_smoothed_pwdb = -1;

	//Initial TX Power Control for near/far range , add by amy 2008/05/15, porting from windows code.
	dm_init_dynamic_txpower(dev);

#ifdef RTL8192SE
	if (IS_HARDWARE_TYPE_8192SE(dev))
		dm_InitRateAdaptiveMask(dev);
	else
#endif
		init_rate_adaptive(dev);

	//dm_initialize_txpower_tracking(dev);
	dm_dig_init(dev);
	dm_init_edca_turbo(dev);
	dm_init_bandwidth_autoswitch(dev);
	dm_init_fsync(dev);
	dm_init_rxpath_selection(dev);
	dm_init_ctstoself(dev);
        if (IS_HARDWARE_TYPE_8192SE(dev))
	dm_Init_WA_Broadcom_IOT(dev);

#if (defined RTL8192E || defined RTL8192SE)
	INIT_DELAYED_WORK_RSL(&priv->gpio_change_rf_wq, (void *)dm_gpio_change_rf_callback,dev);
#endif

}	// InitHalDm

extern void deinit_hal_dm(struct net_device *dev)
{

	dm_deInit_fsync(dev);

}


#ifdef USB_RX_AGGREGATION_SUPPORT
void dm_CheckRxAggregation(struct net_device *dev) {
	struct r8192_priv *priv = rtllib_priv((struct net_device *)dev);
	PRT_HIGH_THROUGHPUT	pHTInfo = priv->rtllib->pHTInfo;
	static unsigned long	lastTxOkCnt = 0;
	static unsigned long	lastRxOkCnt = 0;
	unsigned long		curTxOkCnt = 0;
	unsigned long		curRxOkCnt = 0;

	curTxOkCnt = priv->stats.txbytesunicast - lastTxOkCnt;
	curRxOkCnt = priv->stats.rxbytesunicast - lastRxOkCnt;

	if((curTxOkCnt + curRxOkCnt) < 15000000) {
		return;
	}

	if(curTxOkCnt > 4*curRxOkCnt) {
		if (priv->bCurrentRxAggrEnable) {
			write_nic_dword(dev, 0x1a8, 0);
			priv->bCurrentRxAggrEnable = false;
		}
	}else{
		if (!priv->bCurrentRxAggrEnable && !pHTInfo->bCurrentRT2RTAggregation) {
			u32 ulValue;
			ulValue = (pHTInfo->UsbRxFwAggrEn<<24) | (pHTInfo->UsbRxFwAggrPageNum<<16) |
				(pHTInfo->UsbRxFwAggrPacketNum<<8) | (pHTInfo->UsbRxFwAggrTimeout);
			//
			// If usb rx firmware aggregation is enabled,
			// when anyone of three threshold conditions above is reached,
			// firmware will send aggregated packet to driver.
			//
			write_nic_dword(dev, 0x1a8, ulValue);
			priv->bCurrentRxAggrEnable = true;
		}
	}

	lastTxOkCnt = priv->stats.txbytesunicast;
	lastRxOkCnt = priv->stats.rxbytesunicast;
}	// dm_CheckEdcaTurbo
#endif



extern  void    hal_dm_watchdog(struct net_device *dev)
{
#ifdef _RTL8192_EXT_PATCH_
	struct r8192_priv *priv = rtllib_priv(dev);
	if(priv->being_init_adapter)
		return;
#endif
	// call script file to enable/disable legacy power save */
	dm_check_ac_dc_power(dev);

	// Add by amy 2008/05/15 ,porting from windows code.
	dm_check_pbc_gpio(dev);
	dm_check_txrateandretrycount(dev);
	dm_check_edca_turbo(dev);
#ifdef _RTL8192_EXT_PATCH_
	dm_refresh_rate_adaptive_mask(dev);
#endif
        if (IS_HARDWARE_TYPE_8192SE(dev)){
#ifdef RTL8192SE
	    dm_WA_Broadcom_IOT(dev);
            return;
            dm_check_txpower_tracking(dev);
	    dm_ctrl_initgain_byrssi(dev);	// moved by hpfan 2008-03-04
		dm_dynamic_txpower(dev);
		dm_RefreshRateAdaptiveMask(dev);
		dm_check_fsync(dev);
		if(priv->rtllib->iw_mode == IW_MODE_ADHOC)
			Adhoc_dm_CheckRateAdaptive(dev);
		else
			dm_check_rate_adaptive(dev);
		// ====================================================
		// If any dynamic mechanism is ready, put it above this return;
		// ====================================================
#endif
        }
	dm_check_rate_adaptive(dev);
	dm_dynamic_txpower(dev);
	dm_check_txpower_tracking(dev);

	dm_ctrl_initgain_byrssi(dev);
	dm_bandwidth_autoswitch(dev);

	//lzm 090323 move into check_rfctrl_gpio_timer for probe/disconnect
	//dm_check_rfctrl_gpio(dev);
	dm_check_rx_path_selection(dev);
	dm_check_fsync(dev);

	// Add by amy 2008-05-15 porting from windows code.
	dm_send_rssi_tofw(dev);
	dm_ctstoself(dev);

#ifdef USB_RX_AGGREGATION_SUPPORT
	dm_CheckRxAggregation(dev);
#endif
}	//HalDmWatchDog

// call the script file to enable
void dm_check_ac_dc_power(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	static char *ac_dc_check_script_path = "/etc/acpi/wireless-rtl-ac-dc-power.sh";
	char *argv[] = {ac_dc_check_script_path,DRV_NAME,NULL};
	static char *envp[] = {"HOME=/",
			"TERM=linux",
			"PATH=/usr/bin:/bin",
			 NULL};

	if(priv->ResetProgress == RESET_TYPE_SILENT)
	{
		RT_TRACE((COMP_INIT | COMP_POWER | COMP_RF), "GPIOChangeRFWorkItemCallBack(): Silent Reseting!!!!!!!\n");
		return;
	}

	if(priv->rtllib->state != RTLLIB_LINKED) {
		return;
	}
	call_usermodehelper(ac_dc_check_script_path,argv,envp,1);

	return;
};


//
// Decide Rate Adaptive Set according to distance (signal strength)
//	01/11/2008	MHC		Modify input arguments and RATR table level.
//	01/16/2008	MHC		RF_Type is assigned in ReadAdapterInfo(). We must call
//						the function after making sure RF_Type.
//
extern void init_rate_adaptive(struct net_device * dev)
{

	struct r8192_priv *priv = rtllib_priv(dev);
	prate_adaptive			pra = (prate_adaptive)&priv->rate_adaptive;

	pra->ratr_state = DM_RATR_STA_MAX;
	pra->high2low_rssi_thresh_for_ra = RateAdaptiveTH_High;
	pra->low2high_rssi_thresh_for_ra20M = RateAdaptiveTH_Low_20M+5;
	pra->low2high_rssi_thresh_for_ra40M = RateAdaptiveTH_Low_40M+5;

	pra->high_rssi_thresh_for_ra = RateAdaptiveTH_High+5;
	pra->low_rssi_thresh_for_ra20M = RateAdaptiveTH_Low_20M;
	pra->low_rssi_thresh_for_ra40M = RateAdaptiveTH_Low_40M;

	if(priv->CustomerID == RT_CID_819x_Netcore)
		pra->ping_rssi_enable = 1;
	else
		pra->ping_rssi_enable = 0;
	pra->ping_rssi_thresh_for_ra = 15;


	if (priv->rf_type == RF_2T4R)
	{
		// 07/10/08 MH Modify for RA smooth scheme.
		// 2008/01/11 MH Modify 2T RATR table for different RSSI. 080515
		// porting by amy from windows code.
		pra->upper_rssi_threshold_ratr		=	0x8f0f0000;
		pra->middle_rssi_threshold_ratr		=	0x8f0ff000;
		pra->low_rssi_threshold_ratr		=	0x8f0ff001;
		pra->low_rssi_threshold_ratr_40M	=	0x8f0ff005;
		pra->low_rssi_threshold_ratr_20M	=	0x8f0ff001;
		pra->ping_rssi_ratr	=	0x0000000d;//cosa add for test
	}
	else if (priv->rf_type == RF_1T2R)
	{
		pra->upper_rssi_threshold_ratr		=	0x000f0000;
		pra->middle_rssi_threshold_ratr		=	0x000ff000;
		pra->low_rssi_threshold_ratr		=	0x000ff001;
		pra->low_rssi_threshold_ratr_40M	=	0x000ff005;
		pra->low_rssi_threshold_ratr_20M	=	0x000ff001;
		pra->ping_rssi_ratr	=	0x0000000d;//cosa add for test
	}

}	// InitRateAdaptive


//-----------------------------------------------------------------------------
// Function:	dm_check_rate_adaptive()
//
// Overview:
//
// Input:		NONE
//
// Output:		NONE
//
// Return:		NONE
//
// Revised History:
//	When		Who		Remark
//	05/26/08	amy	Create version 0 proting from windows code.
//
//---------------------------------------------------------------------------*/
static void dm_check_rate_adaptive(struct net_device * dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	PRT_HIGH_THROUGHPUT	pHTInfo = priv->rtllib->pHTInfo;
	prate_adaptive			pra = (prate_adaptive)&priv->rate_adaptive;
	u32						currentRATR, targetRATR = 0;
	u32						LowRSSIThreshForRA = 0, HighRSSIThreshForRA = 0;
	bool						bshort_gi_enabled = false;
	static u8					ping_rssi_state=0;


#ifdef _RTL8192_EXT_PATCH_
	if((!priv->up)&& (!priv->mesh_up))
#else
	if(!priv->up)
#endif
	{
		RT_TRACE(COMP_RATE, "<---- dm_check_rate_adaptive(): driver is going to unload\n");
		return;
	}

	if(pra->rate_adaptive_disabled)//this variable is set by ioctl.
		return;

	// TODO: Only 11n mode is implemented currently,
	if( !(priv->rtllib->mode == WIRELESS_MODE_N_24G ||
		 priv->rtllib->mode == WIRELESS_MODE_N_5G))
		 return;

	if( priv->rtllib->state == RTLLIB_LINKED )
	{
	//	RT_TRACE(COMP_RATE, "dm_CheckRateAdaptive(): \t");

		//
		// Check whether Short GI is enabled
		//
		bshort_gi_enabled = (pHTInfo->bCurTxBW40MHz && pHTInfo->bCurShortGI40MHz) ||
			(!pHTInfo->bCurTxBW40MHz && pHTInfo->bCurShortGI20MHz);


		pra->upper_rssi_threshold_ratr =
				(pra->upper_rssi_threshold_ratr & (~BIT31)) | ((bshort_gi_enabled)? BIT31:0) ;

		pra->middle_rssi_threshold_ratr =
				(pra->middle_rssi_threshold_ratr & (~BIT31)) | ((bshort_gi_enabled)? BIT31:0) ;

		if (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20)
		{
			pra->low_rssi_threshold_ratr =
				(pra->low_rssi_threshold_ratr_40M & (~BIT31)) | ((bshort_gi_enabled)? BIT31:0) ;
		}
		else
		{
			pra->low_rssi_threshold_ratr =
			(pra->low_rssi_threshold_ratr_20M & (~BIT31)) | ((bshort_gi_enabled)? BIT31:0) ;
		}
		//cosa add for test
		pra->ping_rssi_ratr =
				(pra->ping_rssi_ratr & (~BIT31)) | ((bshort_gi_enabled)? BIT31:0) ;

		// 2007/10/08 MH We support RA smooth scheme now. When it is the first
		// time to link with AP. We will not change upper/lower threshold. If
		// STA stay in high or low level, we must change two different threshold
		// to prevent jumping frequently.
		if (pra->ratr_state == DM_RATR_STA_HIGH)
		{
			HighRSSIThreshForRA	= pra->high2low_rssi_thresh_for_ra;
			LowRSSIThreshForRA	= (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20)?
					(pra->low_rssi_thresh_for_ra40M):(pra->low_rssi_thresh_for_ra20M);
		}
		else if (pra->ratr_state == DM_RATR_STA_LOW)
		{
			HighRSSIThreshForRA	= pra->high_rssi_thresh_for_ra;
			LowRSSIThreshForRA	= (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20)?
					(pra->low2high_rssi_thresh_for_ra40M):(pra->low2high_rssi_thresh_for_ra20M);
		}
		else
		{
			HighRSSIThreshForRA	= pra->high_rssi_thresh_for_ra;
			LowRSSIThreshForRA	= (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20)?
					(pra->low_rssi_thresh_for_ra40M):(pra->low_rssi_thresh_for_ra20M);
		}

		//DbgPrint("[DM] THresh H/L=%d/%d\n\r", RATR.HighRSSIThreshForRA, RATR.LowRSSIThreshForRA);
		if(priv->undecorated_smoothed_pwdb >= (long)HighRSSIThreshForRA)
		{
			//DbgPrint("[DM] RSSI=%d STA=HIGH\n\r", pHalData->UndecoratedSmoothedPWDB);
			pra->ratr_state = DM_RATR_STA_HIGH;
			targetRATR = pra->upper_rssi_threshold_ratr;
		}else if(priv->undecorated_smoothed_pwdb >= (long)LowRSSIThreshForRA)
		{
			//DbgPrint("[DM] RSSI=%d STA=Middle\n\r", pHalData->UndecoratedSmoothedPWDB);
			pra->ratr_state = DM_RATR_STA_MIDDLE;
			targetRATR = pra->middle_rssi_threshold_ratr;
		}else
		{
			//DbgPrint("[DM] RSSI=%d STA=LOW\n\r", pHalData->UndecoratedSmoothedPWDB);
			pra->ratr_state = DM_RATR_STA_LOW;
			targetRATR = pra->low_rssi_threshold_ratr;
		}

			//cosa add for test
		if(pra->ping_rssi_enable)
		{
			//pHalData->UndecoratedSmoothedPWDB = 19;
			if(priv->undecorated_smoothed_pwdb < (long)(pra->ping_rssi_thresh_for_ra+5))
			{
				if( (priv->undecorated_smoothed_pwdb < (long)pra->ping_rssi_thresh_for_ra) ||
					ping_rssi_state )
				{
					//DbgPrint("TestRSSI = %d, set RATR to 0x%x \n", pHalData->UndecoratedSmoothedPWDB, pRA->TestRSSIRATR);
					pra->ratr_state = DM_RATR_STA_LOW;
					targetRATR = pra->ping_rssi_ratr;
					ping_rssi_state = 1;
				}
				//else
				//	DbgPrint("TestRSSI is between the range. \n");
			}
			else
			{
				//DbgPrint("TestRSSI Recover to 0x%x \n", targetRATR);
				ping_rssi_state = 0;
			}
		}

		// 2008.04.01
#if 1
		// For RTL819X, if pairwisekey = wep/tkip, we support only MCS0~7.
		if(priv->rtllib->GetHalfNmodeSupportByAPsHandler(dev))
			targetRATR &=  0xf00fffff;
#endif

		//
		// Check whether updating of RATR0 is required
		//
		currentRATR = read_nic_dword(dev, RATR0);
		if( targetRATR !=  currentRATR )
		{
			u32 ratr_value;
			ratr_value = targetRATR;
			RT_TRACE(COMP_RATE,"currentRATR = %x, targetRATR = %x\n", currentRATR, targetRATR);
			if(priv->rf_type == RF_1T2R)
			{
				ratr_value &= ~(RATE_ALL_OFDM_2SS);
			}
			write_nic_dword(dev, RATR0, ratr_value);
			write_nic_byte(dev, UFWP, 1);

			pra->last_ratr = targetRATR;
		}

	}
	else
	{
		pra->ratr_state = DM_RATR_STA_MAX;
	}

}	// dm_CheckRateAdaptive

#ifdef RTL8192SE
void dm_InitRateAdaptiveMask(struct net_device * dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	prate_adaptive	pRA =  (prate_adaptive)&priv->rate_adaptive;

	pRA->ratr_state = DM_RATR_STA_MAX;
	pRA->PreRATRState = DM_RATR_STA_MAX;

	if (priv->DM_Type == DM_Type_ByDriver && priv->pFirmware->FirmwareVersion >= 60)
		priv->rtllib->bUseRAMask = true;
	else
		priv->rtllib->bUseRAMask = false;

	priv->bInformFWDriverControlDM = false;

}
#endif

static void dm_init_bandwidth_autoswitch(struct net_device * dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);

	priv->rtllib->bandwidth_auto_switch.threshold_20Mhzto40Mhz = BW_AUTO_SWITCH_LOW_HIGH;
	priv->rtllib->bandwidth_auto_switch.threshold_40Mhzto20Mhz = BW_AUTO_SWITCH_HIGH_LOW;
	priv->rtllib->bandwidth_auto_switch.bforced_tx20Mhz = false;
	priv->rtllib->bandwidth_auto_switch.bautoswitch_enable = false;

}	// dm_init_bandwidth_autoswitch


static void dm_bandwidth_autoswitch(struct net_device * dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);

	if(priv->CurrentChannelBW == HT_CHANNEL_WIDTH_20 ||!priv->rtllib->bandwidth_auto_switch.bautoswitch_enable){
		return;
	}else{
		if(priv->rtllib->bandwidth_auto_switch.bforced_tx20Mhz == false){//If send packets in 40 Mhz in 20/40
			if(priv->undecorated_smoothed_pwdb <= priv->rtllib->bandwidth_auto_switch.threshold_40Mhzto20Mhz)
				priv->rtllib->bandwidth_auto_switch.bforced_tx20Mhz = true;
		}else{//in force send packets in 20 Mhz in 20/40
			if(priv->undecorated_smoothed_pwdb >= priv->rtllib->bandwidth_auto_switch.threshold_20Mhzto40Mhz)
				priv->rtllib->bandwidth_auto_switch.bforced_tx20Mhz = false;

		}
	}
}	// dm_BandwidthAutoSwitch

//OFDM default at 0db, index=6.
#ifdef Rtl8192SE
//OFDM default at 0db, index=6.
u32	OFDMSwingTable[OFDM_Table_Length] = {
	0x7f8001fe,	// 0, +6.0dB
	0x788001e2,	// 1, +5.5dB
	0x71c001c7,	// 2, +5.0dB
	0x6b8001ae,	// 3, +4.5dB
	0x65400195,	// 4, +4.0dB
	0x5fc0017f,	// 5, +3.5dB
	0x5a400169,	// 6, +3.0dB
	0x55400155,	// 7, +2.5dB
	0x50800142,	// 8, +2.0dB
	0x4c000130,	// 9, +1.5dB
	0x47c0011f,	// 10, +1.0dB
	0x43c0010f,	// 11, +0.5dB
	0x40000100,	// 12, +0dB
	0x3c8000f2,	// 13, -0.5dB
	0x390000e4,	// 14, -1.0dB
	0x35c000d7,	// 15, -1.5dB
	0x32c000cb,	// 16, -2.0dB
	0x300000c0,	// 17, -2.5dB
	0x2d4000b5,	// 18, -3.0dB
	0x2ac000ab,	// 19, -3.5dB
	0x288000a2,	// 20, -4.0dB
	0x26000098,	// 21, -4.5dB
	0x24000090,	// 22, -5.0dB
	0x22000088,	// 23, -5.5dB
	0x20000080,	// 24, -6.0dB
	0x1e400079,	// 25, -6.5dB
	0x1c800072,	// 26, -7.0dB
	0x1b00006c,	// 27. -7.5dB
	0x19800066,	// 28, -8.0dB
	0x18000060,	// 29, -8.5dB
	0x16c0005b,	// 30, -9.0dB
	0x15800056,	// 31, -9.5dB
	0x14400051,	// 32, -10.0dB
	0x1300004c,	// 33, -10.5dB
	0x12000048,	// 34, -11.0dB
	0x11000044,	// 35, -11.5dB
	0x10000040,	// 36, -12.0dB
};

u8	CCKSwingTable_Ch1_Ch13[CCK_Table_length][8] = {
	{0x36, 0x35, 0x2e, 0x25, 0x1c, 0x12, 0x09, 0x04},	// 0, +0dB
	{0x33, 0x32, 0x2b, 0x23, 0x1a, 0x11, 0x08, 0x04},	// 1, -0.5dB
	{0x30, 0x2f, 0x29, 0x21, 0x19, 0x10, 0x08, 0x03},	// 2, -1.0dB
	{0x2d, 0x2d, 0x27, 0x1f, 0x18, 0x0f, 0x08, 0x03},	// 3, -1.5dB
	{0x2b, 0x2a, 0x25, 0x1e, 0x16, 0x0e, 0x07, 0x03},	// 4, -2.0dB
	{0x28, 0x28, 0x22, 0x1c, 0x15, 0x0d, 0x07, 0x03},	// 5, -2.5dB
	{0x26, 0x25, 0x21, 0x1b, 0x14, 0x0d, 0x06, 0x03},	// 6, -3.0dB
	{0x24, 0x23, 0x1f, 0x19, 0x13, 0x0c, 0x06, 0x03},	// 7, -3.5dB
	{0x22, 0x21, 0x1d, 0x18, 0x11, 0x0b, 0x06, 0x02},	// 8, -4.0dB
	{0x20, 0x20, 0x1b, 0x16, 0x11, 0x08, 0x05, 0x02},	// 9, -4.5dB
	{0x1f, 0x1e, 0x1a, 0x15, 0x10, 0x0a, 0x05, 0x02},	// 10, -5.0dB
	{0x1d, 0x1c, 0x18, 0x14, 0x0f, 0x0a, 0x05, 0x02},	// 11, -5.5dB
	{0x1b, 0x1a, 0x17, 0x13, 0x0e, 0x09, 0x04, 0x02},	// 12, -6.0dB <== default
	{0x1a, 0x19, 0x16, 0x12, 0x0d, 0x09, 0x04, 0x02},	// 13, -6.5dB
	{0x18, 0x17, 0x15, 0x11, 0x0c, 0x08, 0x04, 0x02},	// 14, -7.0dB
	{0x17, 0x16, 0x13, 0x10, 0x0c, 0x08, 0x04, 0x02},	// 15, -7.5dB
	{0x16, 0x15, 0x12, 0x0f, 0x0b, 0x07, 0x04, 0x01},	// 16, -8.0dB
	{0x14, 0x14, 0x11, 0x0e, 0x0b, 0x07, 0x03, 0x02},	// 17, -8.5dB
	{0x13, 0x13, 0x10, 0x0d, 0x0a, 0x06, 0x03, 0x01},	// 18, -9.0dB
	{0x12, 0x12, 0x0f, 0x0c, 0x09, 0x06, 0x03, 0x01},	// 19, -9.5dB
	{0x11, 0x11, 0x0f, 0x0c, 0x09, 0x06, 0x03, 0x01},	// 20, -10.0dB
	{0x10, 0x10, 0x0e, 0x0b, 0x08, 0x05, 0x03, 0x01},	// 21, -10.5dB
	{0x0f, 0x0f, 0x0d, 0x0b, 0x08, 0x05, 0x03, 0x01},	// 22, -11.0dB
	{0x0e, 0x0e, 0x0c, 0x0a, 0x08, 0x05, 0x02, 0x01},	// 23, -11.5dB
	{0x0d, 0x0d, 0x0c, 0x0a, 0x07, 0x05, 0x02, 0x01},	// 24, -12.0dB
	{0x0d, 0x0c, 0x0b, 0x09, 0x07, 0x04, 0x02, 0x01},	// 25, -12.5dB
	{0x0c, 0x0c, 0x0a, 0x09, 0x06, 0x04, 0x02, 0x01},	// 26, -13.0dB
	{0x0b, 0x0b, 0x0a, 0x08, 0x06, 0x04, 0x02, 0x01},	// 27, -13.5dB
	{0x0b, 0x0a, 0x09, 0x08, 0x06, 0x04, 0x02, 0x01},	// 28, -14.0dB
	{0x0a, 0x0a, 0x09, 0x07, 0x05, 0x03, 0x02, 0x01},	// 29, -14.5dB
	{0x0a, 0x09, 0x08, 0x07, 0x05, 0x03, 0x02, 0x01},	// 30, -15.0dB
	{0x09, 0x09, 0x08, 0x06, 0x05, 0x03, 0x01, 0x01},	// 31, -15.5dB
	{0x09, 0x08, 0x07, 0x06, 0x04, 0x03, 0x01, 0x01}	// 32, -16.0dB
};


u8	CCKSwingTable_Ch14[CCK_Table_length][8] = {
	{0x36, 0x35, 0x2e, 0x1b, 0x00, 0x00, 0x00, 0x00},	// 0, +0dB
	{0x33, 0x32, 0x2b, 0x19, 0x00, 0x00, 0x00, 0x00},	// 1, -0.5dB
	{0x30, 0x2f, 0x29, 0x18, 0x00, 0x00, 0x00, 0x00},	// 2, -1.0dB
	{0x2d, 0x2d, 0x17, 0x17, 0x00, 0x00, 0x00, 0x00},	// 3, -1.5dB
	{0x2b, 0x2a, 0x25, 0x15, 0x00, 0x00, 0x00, 0x00},	// 4, -2.0dB
	{0x28, 0x28, 0x24, 0x14, 0x00, 0x00, 0x00, 0x00},	// 5, -2.5dB
	{0x26, 0x25, 0x21, 0x13, 0x00, 0x00, 0x00, 0x00},	// 6, -3.0dB
	{0x24, 0x23, 0x1f, 0x12, 0x00, 0x00, 0x00, 0x00},	// 7, -3.5dB
	{0x22, 0x21, 0x1d, 0x11, 0x00, 0x00, 0x00, 0x00},	// 8, -4.0dB
	{0x20, 0x20, 0x1b, 0x10, 0x00, 0x00, 0x00, 0x00},	// 9, -4.5dB
	{0x1f, 0x1e, 0x1a, 0x0f, 0x00, 0x00, 0x00, 0x00},	// 10, -5.0dB
	{0x1d, 0x1c, 0x18, 0x0e, 0x00, 0x00, 0x00, 0x00},	// 11, -5.5dB
	{0x1b, 0x1a, 0x17, 0x0e, 0x00, 0x00, 0x00, 0x00},	// 12, -6.0dB  <== default
	{0x1a, 0x19, 0x16, 0x0d, 0x00, 0x00, 0x00, 0x00},	// 13, -6.5dB
	{0x18, 0x17, 0x15, 0x0c, 0x00, 0x00, 0x00, 0x00},	// 14, -7.0dB
	{0x17, 0x16, 0x13, 0x0b, 0x00, 0x00, 0x00, 0x00},	// 15, -7.5dB
	{0x16, 0x15, 0x12, 0x0b, 0x00, 0x00, 0x00, 0x00},	// 16, -8.0dB
	{0x14, 0x14, 0x11, 0x0a, 0x00, 0x00, 0x00, 0x00},	// 17, -8.5dB
	{0x13, 0x13, 0x10, 0x0a, 0x00, 0x00, 0x00, 0x00},	// 18, -9.0dB
	{0x12, 0x12, 0x0f, 0x09, 0x00, 0x00, 0x00, 0x00},	// 19, -9.5dB
	{0x11, 0x11, 0x0f, 0x09, 0x00, 0x00, 0x00, 0x00},	// 20, -10.0dB
	{0x10, 0x10, 0x0e, 0x08, 0x00, 0x00, 0x00, 0x00},	// 21, -10.5dB
	{0x0f, 0x0f, 0x0d, 0x08, 0x00, 0x00, 0x00, 0x00},	// 22, -11.0dB
	{0x0e, 0x0e, 0x0c, 0x07, 0x00, 0x00, 0x00, 0x00},	// 23, -11.5dB
	{0x0d, 0x0d, 0x0c, 0x07, 0x00, 0x00, 0x00, 0x00},	// 24, -12.0dB
	{0x0d, 0x0c, 0x0b, 0x06, 0x00, 0x00, 0x00, 0x00},	// 25, -12.5dB
	{0x0c, 0x0c, 0x0a, 0x06, 0x00, 0x00, 0x00, 0x00},	// 26, -13.0dB
	{0x0b, 0x0b, 0x0a, 0x06, 0x00, 0x00, 0x00, 0x00},	// 27, -13.5dB
	{0x0b, 0x0a, 0x09, 0x05, 0x00, 0x00, 0x00, 0x00},	// 28, -14.0dB
	{0x0a, 0x0a, 0x09, 0x05, 0x00, 0x00, 0x00, 0x00},	// 29, -14.5dB
	{0x0a, 0x09, 0x08, 0x05, 0x00, 0x00, 0x00, 0x00},	// 30, -15.0dB
	{0x09, 0x09, 0x08, 0x05, 0x00, 0x00, 0x00, 0x00},	// 31, -15.5dB
	{0x09, 0x08, 0x07, 0x04, 0x00, 0x00, 0x00, 0x00}	// 32, -16.0dB
};
#elif defined RTL8192E
static u32 OFDMSwingTable[OFDM_Table_Length] = {
	0x7f8001fe,	// 0, +6db
	0x71c001c7,	// 1, +5db
	0x65400195,	// 2, +4db
	0x5a400169,	// 3, +3db
	0x50800142,	// 4, +2db
	0x47c0011f,	// 5, +1db
	0x40000100,	// 6, +0db ===> default, upper for higher temprature, lower for low temprature
	0x390000e4,	// 7, -1db
	0x32c000cb,	// 8, -2db
	0x2d4000b5,	// 9, -3db
	0x288000a2,	// 10, -4db
	0x24000090,	// 11, -5db
	0x20000080,	// 12, -6db
	0x1c800072,	// 13, -7db
	0x19800066,	// 14, -8db
	0x26c0005b,	// 15, -9db
	0x24400051,	// 16, -10db
	0x12000048,	// 17, -11db
	0x10000040	// 18, -12db
};
static u8	CCKSwingTable_Ch1_Ch13[CCK_Table_length][8] = {
	{0x36, 0x35, 0x2e, 0x25, 0x1c, 0x12, 0x09, 0x04},	// 0, +0db ===> CCK40M default
	{0x30, 0x2f, 0x29, 0x21, 0x19, 0x10, 0x08, 0x03},	// 1, -1db
	{0x2b, 0x2a, 0x25, 0x1e, 0x16, 0x0e, 0x07, 0x03},	// 2, -2db
	{0x26, 0x25, 0x21, 0x1b, 0x14, 0x0d, 0x06, 0x03},	// 3, -3db
	{0x22, 0x21, 0x1d, 0x18, 0x11, 0x0b, 0x06, 0x02},	// 4, -4db
	{0x1f, 0x1e, 0x1a, 0x15, 0x10, 0x0a, 0x05, 0x02},	// 5, -5db
	{0x1b, 0x1a, 0x17, 0x13, 0x0e, 0x09, 0x04, 0x02},	// 6, -6db ===> CCK20M default
	{0x18, 0x17, 0x15, 0x11, 0x0c, 0x08, 0x04, 0x02},	// 7, -7db
	{0x16, 0x15, 0x12, 0x0f, 0x0b, 0x07, 0x04, 0x01},	// 8, -8db
	{0x13, 0x13, 0x10, 0x0d, 0x0a, 0x06, 0x03, 0x01},	// 9, -9db
	{0x11, 0x11, 0x0f, 0x0c, 0x09, 0x06, 0x03, 0x01},	// 10, -10db
	{0x0f, 0x0f, 0x0d, 0x0b, 0x08, 0x05, 0x03, 0x01}	// 11, -11db
};

static u8	CCKSwingTable_Ch14[CCK_Table_length][8] = {
	{0x36, 0x35, 0x2e, 0x1b, 0x00, 0x00, 0x00, 0x00},	// 0, +0db  ===> CCK40M default
	{0x30, 0x2f, 0x29, 0x18, 0x00, 0x00, 0x00, 0x00},	// 1, -1db
	{0x2b, 0x2a, 0x25, 0x15, 0x00, 0x00, 0x00, 0x00},	// 2, -2db
	{0x26, 0x25, 0x21, 0x13, 0x00, 0x00, 0x00, 0x00},	// 3, -3db
	{0x22, 0x21, 0x1d, 0x11, 0x00, 0x00, 0x00, 0x00},	// 4, -4db
	{0x1f, 0x1e, 0x1a, 0x0f, 0x00, 0x00, 0x00, 0x00},	// 5, -5db
	{0x1b, 0x1a, 0x17, 0x0e, 0x00, 0x00, 0x00, 0x00},	// 6, -6db  ===> CCK20M default
	{0x18, 0x17, 0x15, 0x0c, 0x00, 0x00, 0x00, 0x00},	// 7, -7db
	{0x16, 0x15, 0x12, 0x0b, 0x00, 0x00, 0x00, 0x00},	// 8, -8db
	{0x13, 0x13, 0x10, 0x0a, 0x00, 0x00, 0x00, 0x00},	// 9, -9db
	{0x11, 0x11, 0x0f, 0x09, 0x00, 0x00, 0x00, 0x00},	// 10, -10db
	{0x0f, 0x0f, 0x0d, 0x08, 0x00, 0x00, 0x00, 0x00}	// 11, -11db
};
#endif
#define		Pw_Track_Flag				0x11d
#define		Tssi_Mea_Value				0x13c
#define		Tssi_Report_Value1			0x134
#define		Tssi_Report_Value2			0x13e
#define		FW_Busy_Flag				0x13f

#ifndef RTL8192SE
static void dm_TXPowerTrackingCallback_TSSI(struct net_device * dev)
	{
	struct r8192_priv *priv = rtllib_priv(dev);
	bool						bHighpowerstate, viviflag = false;
	DCMD_TXCMD_T			tx_cmd;
	u8					powerlevelOFDM24G;
	int					i =0, j = 0, k = 0;
	u8						RF_Type, tmp_report[5]={0, 0, 0, 0, 0};
	u32						Value;
	u8						Pwr_Flag;
	u16					Avg_TSSI_Meas, TSSI_13dBm, Avg_TSSI_Meas_from_driver=0;
#ifdef RTL8192U
	RT_STATUS				rtStatus = RT_STATUS_SUCCESS;
#endif
//	bool rtStatus = true;
	u32						delta=0;
	RT_TRACE(COMP_POWER_TRACKING,"%s()\n",__FUNCTION__);
//	write_nic_byte(dev, 0x1ba, 0);
	write_nic_byte(dev, Pw_Track_Flag, 0);
	write_nic_byte(dev, FW_Busy_Flag, 0);
	priv->rtllib->bdynamic_txpower_enable = false;
	bHighpowerstate = priv->bDynamicTxHighPower;

	powerlevelOFDM24G = (u8)(priv->Pwr_Track>>24);
	RF_Type = priv->rf_type;
	Value = (RF_Type<<8) | powerlevelOFDM24G;

	RT_TRACE(COMP_POWER_TRACKING, "powerlevelOFDM24G = %x\n", powerlevelOFDM24G);

	// 2009/03/31 MH Enable tx power track flag to suspend scan process to execute RF 3-wire.
	//priv->bTxPwrTrack = true;

#ifdef RTL8190P//FIXME for 92E
	// Tx command only send once to prevent fimware stop!!
	for(j = 0; j<1; j++)
#else
	for(j = 0; j<=30; j++)
#endif
{	//fill tx_cmd

	tx_cmd.Op		= TXCMD_SET_TX_PWR_TRACKING;
	tx_cmd.Length	= 4;
	tx_cmd.Value		= Value;
#ifdef RTL8192U
	rtStatus = SendTxCommandPacket(dev, &tx_cmd, 12);
	if (rtStatus == RT_STATUS_FAILURE)
	{
		RT_TRACE(COMP_POWER_TRACKING, "Set configuration with tx cmd queue fail!\n");
	}
#else
	cmpk_message_handle_tx(dev, (u8*)&tx_cmd, DESC_PACKET_TYPE_INIT, sizeof(DCMD_TXCMD_T));
#endif
	mdelay(1);
	//DbgPrint("hi, vivi, strange\n");
	for(i = 0;i <= 30; i++)
	{
		Pwr_Flag = read_nic_byte(dev, Pw_Track_Flag);

		if (Pwr_Flag == 0)
		{
			mdelay(1);

			if(priv->bResetInProgress)
			{
				RT_TRACE(COMP_POWER_TRACKING, "we are in slient reset progress, so return\n");
				write_nic_byte(dev, Pw_Track_Flag, 0);
				write_nic_byte(dev, FW_Busy_Flag, 0);
				//priv->bTxPwrTrack = false;
				return;
			}
#ifdef RTL8192E
			if((priv->rtllib->eRFPowerState != eRfOn))
			{
				RT_TRACE(COMP_POWER_TRACKING, "we are in power save, so return\n");
				write_nic_byte(dev, Pw_Track_Flag, 0);
				write_nic_byte(dev, FW_Busy_Flag, 0);
				//priv->bTxPwrTrack = false;
				return;
			}

#endif
			continue;
		}

		Avg_TSSI_Meas = read_nic_word(dev, Tssi_Mea_Value);

		if(Avg_TSSI_Meas == 0)
		{
			write_nic_byte(dev, Pw_Track_Flag, 0);
			write_nic_byte(dev, FW_Busy_Flag, 0);
			//priv->bTxPwrTrack = false;
			return;
		}

		for(k = 0;k < 5; k++)
		{
			if(k !=4)
				tmp_report[k] = read_nic_byte(dev, Tssi_Report_Value1+k);
			else
				tmp_report[k] = read_nic_byte(dev, Tssi_Report_Value2);

			RT_TRACE(COMP_POWER_TRACKING, "TSSI_report_value = %d\n", tmp_report[k]);

		        //check if the report value is right
		        {
			       if(tmp_report[k] <= 20)
			       {
				      viviflag =true;
				      break;
			       }
		        }
		}

		if(viviflag ==true)
		{
			write_nic_byte(dev, Pw_Track_Flag, 0);
			viviflag = false;
			RT_TRACE(COMP_POWER_TRACKING, "we filted this data\n");
			for(k = 0;k < 5; k++)
				tmp_report[k] = 0;
			break;
		}

		for(k = 0;k < 5; k++)
		{
			Avg_TSSI_Meas_from_driver += tmp_report[k];
		}

		Avg_TSSI_Meas_from_driver = Avg_TSSI_Meas_from_driver*100/5;
		RT_TRACE(COMP_POWER_TRACKING, "Avg_TSSI_Meas_from_driver = %d\n", Avg_TSSI_Meas_from_driver);
		TSSI_13dBm = priv->TSSI_13dBm;
		RT_TRACE(COMP_POWER_TRACKING, "TSSI_13dBm = %d\n", TSSI_13dBm);

		//if(abs(Avg_TSSI_Meas_from_driver - TSSI_13dBm) <= E_FOR_TX_POWER_TRACK)
		// For MacOS-compatible
		if(Avg_TSSI_Meas_from_driver > TSSI_13dBm)
			delta = Avg_TSSI_Meas_from_driver - TSSI_13dBm;
		else
			delta = TSSI_13dBm - Avg_TSSI_Meas_from_driver;

		if(delta <= E_FOR_TX_POWER_TRACK)
		{
			priv->rtllib->bdynamic_txpower_enable = true;
			write_nic_byte(dev, Pw_Track_Flag, 0);
			write_nic_byte(dev, FW_Busy_Flag, 0);
			RT_TRACE(COMP_POWER_TRACKING, "tx power track is done\n");
			RT_TRACE(COMP_POWER_TRACKING, "priv->rfa_txpowertrackingindex = %d\n", priv->rfa_txpowertrackingindex);
			RT_TRACE(COMP_POWER_TRACKING, "priv->rfa_txpowertrackingindex_real = %d\n", priv->rfa_txpowertrackingindex_real);
#ifdef RTL8190P
			RT_TRACE(COMP_POWER_TRACKING, "priv->rfc_txpowertrackingindex = %d\n", priv->rfc_txpowertrackingindex);
			RT_TRACE(COMP_POWER_TRACKING, "priv->rfc_txpowertrackingindex_real = %d\n", priv->rfc_txpowertrackingindex_real);
#endif
			RT_TRACE(COMP_POWER_TRACKING, "priv->CCKPresentAttentuation_difference = %d\n", priv->CCKPresentAttentuation_difference);
			RT_TRACE(COMP_POWER_TRACKING, "priv->CCKPresentAttentuation = %d\n", priv->CCKPresentAttentuation);
			//priv->bTxPwrTrack = false;
			return;
		}
		else
		{
			if(Avg_TSSI_Meas_from_driver < TSSI_13dBm - E_FOR_TX_POWER_TRACK)
			{
				if (RF_Type == RF_2T4R)
				{

						if((priv->rfa_txpowertrackingindex > 0) &&(priv->rfc_txpowertrackingindex > 0))
				{
					priv->rfa_txpowertrackingindex--;
					if(priv->rfa_txpowertrackingindex_real > 4)
					{
						priv->rfa_txpowertrackingindex_real--;
						rtl8192_setBBreg(dev, rOFDM0_XATxIQImbalance, bMaskDWord, priv->txbbgain_table[priv->rfa_txpowertrackingindex_real].txbbgain_value);
					}

					priv->rfc_txpowertrackingindex--;
					if(priv->rfc_txpowertrackingindex_real > 4)
					{
						priv->rfc_txpowertrackingindex_real--;
						rtl8192_setBBreg(dev, rOFDM0_XCTxIQImbalance, bMaskDWord, priv->txbbgain_table[priv->rfc_txpowertrackingindex_real].txbbgain_value);
					}
						}
						else
						{
								rtl8192_setBBreg(dev, rOFDM0_XATxIQImbalance, bMaskDWord, priv->txbbgain_table[4].txbbgain_value);
								rtl8192_setBBreg(dev, rOFDM0_XCTxIQImbalance, bMaskDWord, priv->txbbgain_table[4].txbbgain_value);
				}
			}
			else
			{
						//switch(Adapter->HardwareType)
						{
#ifdef RTL8190P
							//case HARDWARE_TYPE_RTL8190P:
								{
						if(priv->rfc_txpowertrackingindex > 0)
						{
							priv->rfc_txpowertrackingindex--;
							if(priv->rfc_txpowertrackingindex_real > 4)
							{
								priv->rfc_txpowertrackingindex_real--;
								rtl8192_setBBreg(dev, rOFDM0_XCTxIQImbalance, bMaskDWord, priv->txbbgain_table[priv->rfc_txpowertrackingindex_real].txbbgain_value);
							}
						}
						else
							rtl8192_setBBreg(dev, rOFDM0_XCTxIQImbalance, bMaskDWord, priv->txbbgain_table[4].txbbgain_value);
				}
								//break;
#endif
#ifdef RTL8192E
							//case HARDWARE_TYPE_RTL8192E:
							//case HARDWARE_TYPE_RTL819xU:
								{
									if(priv->rfa_txpowertrackingindex > 0)
									{
										priv->rfa_txpowertrackingindex--;
										if(priv->rfa_txpowertrackingindex_real > 4)
										{
											priv->rfa_txpowertrackingindex_real--;
											rtl8192_setBBreg(dev, rOFDM0_XATxIQImbalance, bMaskDWord, priv->txbbgain_table[priv->rfa_txpowertrackingindex_real].txbbgain_value);
										}
									}
									else
											rtl8192_setBBreg(dev, rOFDM0_XATxIQImbalance, bMaskDWord, priv->txbbgain_table[4].txbbgain_value);
								}
								//break;
#endif
							//default:
							//	RT_TRACE(COMP_DBG,DBG_LOUD,("Undefine Hardware Type \n"));
							//	break;
						}

				}
			}
			else
			{
				if (RF_Type == RF_2T4R)
				{
					if((priv->rfa_txpowertrackingindex < TxBBGainTableLength - 1) &&(priv->rfc_txpowertrackingindex < TxBBGainTableLength - 1))
				{
					priv->rfa_txpowertrackingindex++;
					priv->rfa_txpowertrackingindex_real++;
					rtl8192_setBBreg(dev, rOFDM0_XATxIQImbalance, bMaskDWord, priv->txbbgain_table[priv->rfa_txpowertrackingindex_real].txbbgain_value);
					priv->rfc_txpowertrackingindex++;
					priv->rfc_txpowertrackingindex_real++;
					rtl8192_setBBreg(dev, rOFDM0_XCTxIQImbalance, bMaskDWord, priv->txbbgain_table[priv->rfc_txpowertrackingindex_real].txbbgain_value);
				}
					else
					{
						rtl8192_setBBreg(dev, rOFDM0_XATxIQImbalance, bMaskDWord, priv->txbbgain_table[TxBBGainTableLength - 1].txbbgain_value);
						rtl8192_setBBreg(dev, rOFDM0_XCTxIQImbalance, bMaskDWord, priv->txbbgain_table[TxBBGainTableLength - 1].txbbgain_value);
			}
				}
				else
				{
					//switch(Adapter->HardwareType)
					{
#ifdef RTL8190P
						//case HARDWARE_TYPE_RTL8190P:
							{
					if(priv->rfc_txpowertrackingindex < (TxBBGainTableLength - 1))
					{
							priv->rfc_txpowertrackingindex++;
							priv->rfc_txpowertrackingindex_real++;
							rtl8192_setBBreg(dev, rOFDM0_XCTxIQImbalance, bMaskDWord, priv->txbbgain_table[priv->rfc_txpowertrackingindex_real].txbbgain_value);
					}
					else
							rtl8192_setBBreg(dev, rOFDM0_XCTxIQImbalance, bMaskDWord, priv->txbbgain_table[TxBBGainTableLength - 1].txbbgain_value);
				}
							//break;
#endif
#ifdef RTL8192E
						//case HARDWARE_TYPE_RTL8192E:
						//case HARDWARE_TYPE_RTL819xU:
							{
								if(priv->rfa_txpowertrackingindex < (TxBBGainTableLength - 1))
								{
									priv->rfa_txpowertrackingindex++;
									priv->rfa_txpowertrackingindex_real++;
									rtl8192_setBBreg(dev, rOFDM0_XATxIQImbalance, bMaskDWord, priv->txbbgain_table[priv->rfa_txpowertrackingindex_real].txbbgain_value);
			}
								else
									rtl8192_setBBreg(dev, rOFDM0_XATxIQImbalance, bMaskDWord, priv->txbbgain_table[TxBBGainTableLength - 1].txbbgain_value);
							}
							//break;
#endif
						//default:
						//	RT_TRACE(COMP_DBG,DBG_LOUD,("Undefine Hardware Type \n"));
						//	break;
					}
				}
			}
			if (RF_Type == RF_2T4R){
			priv->CCKPresentAttentuation_difference
				= priv->rfa_txpowertrackingindex - priv->rfa_txpowertracking_default;
			}else{
				//switch(Adapter->HardwareType)
				{
#ifdef RTL8190P
					//case HARDWARE_TYPE_RTL8190P:
				priv->CCKPresentAttentuation_difference
					= priv->rfc_txpowertrackingindex - priv->rfc_txpowertracking_default;
					//	break;
#endif
#ifdef RTL8192E
					//case HARDWARE_TYPE_RTL8192E:
					//case HARDWARE_TYPE_RTL819xU:
						priv->CCKPresentAttentuation_difference
							= priv->rfa_txpowertrackingindex_real - priv->rfa_txpowertracking_default;
					//	break;
#endif
					//default:
					//	RT_TRACE(COMP_DBG,DBG_LOUD,("Undefine Hardware Type \n"));
					//	break;
				}
			}

			if(priv->CurrentChannelBW == HT_CHANNEL_WIDTH_20)
				priv->CCKPresentAttentuation
				= priv->CCKPresentAttentuation_20Mdefault + priv->CCKPresentAttentuation_difference;
			else
				priv->CCKPresentAttentuation
				= priv->CCKPresentAttentuation_40Mdefault + priv->CCKPresentAttentuation_difference;

			if(priv->CCKPresentAttentuation > (CCKTxBBGainTableLength-1))
					priv->CCKPresentAttentuation = CCKTxBBGainTableLength-1;
			if(priv->CCKPresentAttentuation < 0)
					priv->CCKPresentAttentuation = 0;

			if(priv->CCKPresentAttentuation > -1&&priv->CCKPresentAttentuation < CCKTxBBGainTableLength)//(1)
			{
				if(priv->rtllib->current_network.channel == 14 && !priv->bcck_in_ch14)
				{
					priv->bcck_in_ch14 = true;
					dm_cck_txpower_adjust(dev,priv->bcck_in_ch14);
				}
				else if(priv->rtllib->current_network.channel != 14 && priv->bcck_in_ch14)
				{
					priv->bcck_in_ch14 = false;
					dm_cck_txpower_adjust(dev,priv->bcck_in_ch14);
				}
				else
					dm_cck_txpower_adjust(dev,priv->bcck_in_ch14);
			}
		RT_TRACE(COMP_POWER_TRACKING, "priv->rfa_txpowertrackingindex = %d\n", priv->rfa_txpowertrackingindex);
		RT_TRACE(COMP_POWER_TRACKING, "priv->rfa_txpowertrackingindex_real = %d\n", priv->rfa_txpowertrackingindex_real);
#ifdef RTL8190P
		RT_TRACE(COMP_POWER_TRACKING, "priv->rfc_txpowertrackingindex = %d\n", priv->rfc_txpowertrackingindex);
		RT_TRACE(COMP_POWER_TRACKING, "priv->rfc_txpowertrackingindex_real = %d\n", priv->rfc_txpowertrackingindex_real);
#endif
		RT_TRACE(COMP_POWER_TRACKING, "priv->CCKPresentAttentuation_difference = %d\n", priv->CCKPresentAttentuation_difference);
		RT_TRACE(COMP_POWER_TRACKING, "priv->CCKPresentAttentuation = %d\n", priv->CCKPresentAttentuation);

		if (priv->CCKPresentAttentuation_difference <= -12||priv->CCKPresentAttentuation_difference >= 24)
		{
			priv->rtllib->bdynamic_txpower_enable = true;
			write_nic_byte(dev, Pw_Track_Flag, 0);
			write_nic_byte(dev, FW_Busy_Flag, 0);
			RT_TRACE(COMP_POWER_TRACKING, "tx power track--->limited\n");
			//priv->bTxPwrTrack = false;
			return;
		}


	}
		write_nic_byte(dev, Pw_Track_Flag, 0);
		Avg_TSSI_Meas_from_driver = 0;
		for(k = 0;k < 5; k++)
			tmp_report[k] = 0;
		break;
	}
	write_nic_byte(dev, FW_Busy_Flag, 0);
}
		priv->rtllib->bdynamic_txpower_enable = true;
		write_nic_byte(dev, Pw_Track_Flag, 0);
}
#endif

#ifdef RTL8192E
static void dm_TXPowerTrackingCallback_ThermalMeter(struct net_device * dev)
{
#define ThermalMeterVal	9
	struct r8192_priv *priv = rtllib_priv(dev);
	u32 tmpRegA, TempCCk;
	u8 tmpOFDMindex, tmpCCKindex, tmpCCK20Mindex, tmpCCK40Mindex, tmpval;
	int i =0, CCKSwingNeedUpdate=0;

	if(!priv->btxpower_trackingInit)
	{
		//Query OFDM default setting
		tmpRegA= rtl8192_QueryBBReg(dev, rOFDM0_XATxIQImbalance, bMaskDWord);
		for(i=0; i<OFDM_Table_Length; i++)	//find the index
		{
			if(tmpRegA == OFDMSwingTable[i])
			{
				priv->OFDM_index= (u8)i;
				RT_TRACE(COMP_POWER_TRACKING, "Initial reg0x%x = 0x%x, OFDM_index=0x%x\n",
					rOFDM0_XATxIQImbalance, tmpRegA, priv->OFDM_index);
			}
		}

		//Query CCK default setting From 0xa22
		TempCCk = rtl8192_QueryBBReg(dev, rCCK0_TxFilter1, bMaskByte2);
		for(i=0 ; i<CCK_Table_length ; i++)
		{
			if(TempCCk == (u32)CCKSwingTable_Ch1_Ch13[i][0])
			{
				priv->CCK_index =(u8) i;
				RT_TRACE(COMP_POWER_TRACKING, "Initial reg0x%x = 0x%x, CCK_index=0x%x\n",
					rCCK0_TxFilter1, TempCCk, priv->CCK_index);
		break;
	}
}
		priv->btxpower_trackingInit = true;
		//pHalData->TXPowercount = 0;
		return;
	}

	//==========================
	// this is only for test, should be masked
#if 0
{
	//UINT32	eRFPath;
	//UINT32	start_rf, end_rf;
	UINT32	curr_addr;
	//UINT32	reg_addr;
	//UINT32	reg_addr_end;
	UINT32	reg_value;
	//start_rf		= RF90_PATH_A;
	//end_rf			= RF90_PATH_B;//RF90_PATH_MAX;
	//reg_addr		= 0x0;
	//reg_addr_end	= 0x2F;

		for (curr_addr = 0; curr_addr < 0x2d; curr_addr++)
		{
			reg_value = PHY_QueryRFReg(	dev, (RF90_RADIO_PATH_E)RF90_PATH_A,
										curr_addr, bMaskDWord);
		}

	pHalData->TXPowercount = 0;
	return;
}
#endif
	//==========================

	// read and filter out unreasonable value
	tmpRegA = rtl8192_phy_QueryRFReg(dev, RF90_PATH_A, 0x12, 0x078);	// 0x12: RF Reg[10:7]
	RT_TRACE(COMP_POWER_TRACKING, "Readback ThermalMeterA = %d \n", tmpRegA);
	if(tmpRegA < 3 || tmpRegA > 13)
		return;
	if(tmpRegA >= 12)	// if over 12, TP will be bad when high temprature
		tmpRegA = 12;
	RT_TRACE(COMP_POWER_TRACKING, "Valid ThermalMeterA = %d \n", tmpRegA);
	priv->ThermalMeter[0] = ThermalMeterVal;	//We use fixed value by Bryant's suggestion
	priv->ThermalMeter[1] = ThermalMeterVal;	//We use fixed value by Bryant's suggestion

	//Get current RF-A temprature index
	if(priv->ThermalMeter[0] >= (u8)tmpRegA)	//lower temprature
	{
		tmpOFDMindex = tmpCCK20Mindex = 6+(priv->ThermalMeter[0]-(u8)tmpRegA);
		tmpCCK40Mindex = tmpCCK20Mindex - 6;
		if(tmpOFDMindex >= OFDM_Table_Length)
			tmpOFDMindex = OFDM_Table_Length-1;
		if(tmpCCK20Mindex >= CCK_Table_length)
			tmpCCK20Mindex = CCK_Table_length-1;
		if(tmpCCK40Mindex >= CCK_Table_length)
			tmpCCK40Mindex = CCK_Table_length-1;
	}
	else
	{
		tmpval = ((u8)tmpRegA - priv->ThermalMeter[0]);
		if(tmpval >= 6)								// higher temprature
			tmpOFDMindex = tmpCCK20Mindex = 0;		// max to +6dB
		else
			tmpOFDMindex = tmpCCK20Mindex = 6 - tmpval;
		tmpCCK40Mindex = 0;
	}
	//DbgPrint("%ddb, tmpOFDMindex = %d, tmpCCK20Mindex = %d, tmpCCK40Mindex = %d",
		//((u1Byte)tmpRegA - pHalData->ThermalMeter[0]),
		//tmpOFDMindex, tmpCCK20Mindex, tmpCCK40Mindex);
	if(priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20)	//40M
		tmpCCKindex = tmpCCK40Mindex;
	else
		tmpCCKindex = tmpCCK20Mindex;

	//record for bandwidth swith
	priv->Record_CCK_20Mindex = tmpCCK20Mindex;
	priv->Record_CCK_40Mindex = tmpCCK40Mindex;
	RT_TRACE(COMP_POWER_TRACKING, "Record_CCK_20Mindex / Record_CCK_40Mindex = %d / %d.\n",
		priv->Record_CCK_20Mindex, priv->Record_CCK_40Mindex);

	if(priv->rtllib->current_network.channel == 14 && !priv->bcck_in_ch14)
	{
		priv->bcck_in_ch14 = true;
		CCKSwingNeedUpdate = 1;
	}
	else if(priv->rtllib->current_network.channel != 14 && priv->bcck_in_ch14)
	{
		priv->bcck_in_ch14 = false;
		CCKSwingNeedUpdate = 1;
	}

	if(priv->CCK_index != tmpCCKindex)
{
		priv->CCK_index = tmpCCKindex;
		CCKSwingNeedUpdate = 1;
	}

	if(CCKSwingNeedUpdate)
	{
		//DbgPrint("Update CCK Swing, CCK_index = %d\n", pHalData->CCK_index);
		dm_cck_txpower_adjust(dev, priv->bcck_in_ch14);
	}
	if(priv->OFDM_index != tmpOFDMindex)
	{
		priv->OFDM_index = tmpOFDMindex;
		rtl8192_setBBreg(dev, rOFDM0_XATxIQImbalance, bMaskDWord, OFDMSwingTable[priv->OFDM_index]);
		RT_TRACE(COMP_POWER_TRACKING, "Update OFDMSwing[%d] = 0x%x\n",
			priv->OFDM_index, OFDMSwingTable[priv->OFDM_index]);
	}
	priv->txpower_count = 0;
}
#elif defined (RTL8192SE)
static void dm_TXPowerTrackingCallback_ThermalMeter(struct net_device * dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
        u8	ThermalValue=0;
	u32	FwCmdVal=0;

	//priv->TXPowerTrackingCallbackCnt++;	//cosa add for debug
	priv->btxpower_trackingInit = true;


	ThermalValue = (u8)rtl8192_phy_QueryRFReg(dev, RF90_PATH_A, RF_T_METER, 0x1f);	// 0x24: RF Reg[4:0]
	RT_TRACE(COMP_POWER_TRACKING, "Readback Thermal Meter = 0x%x\n", ThermalValue);
	printk("%s()Readback Thermal Meter = 0x%x\n", __func__,ThermalValue);
	if(ThermalValue)
	{
		priv->ThermalValue = ThermalValue;
		if(priv->pFirmware->FirmwareVersion >= 0x35)
		{
			priv->rtllib->SetFwCmdHandler(dev, FW_CMD_TXPWR_TRACK_THERMAL);
		}
		else
		{
		FwCmdVal = (FW_TXPWR_TRACK_THERMAL|
		(priv->ThermalMeter[0]<<8)|(ThermalValue<<16));
		RT_TRACE(COMP_POWER_TRACKING, "Write to FW Thermal Val = 0x%x\n", FwCmdVal);
		write_nic_dword(dev, WFM5, FwCmdVal);
				ChkFwCmdIoDone(dev);
		}
	}

	priv->txpower_count = 0;
}
#endif

void	dm_txpower_trackingcallback(void *data)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20))
	struct r8192_priv *priv = container_of_dwork_rsl(data,struct r8192_priv,txpower_tracking_wq);
	struct net_device *dev = priv->rtllib->dev;
#else
	struct net_device *dev = (struct net_device *)data;
	struct r8192_priv *priv = rtllib_priv(dev);
#endif

#ifdef RTL8190P
	dm_TXPowerTrackingCallback_TSSI(dev);
#elif defined(RTL8192E)
	//if(priv->bDcut == true)
	if(priv->IC_Cut >= IC_VersionCut_D)
		dm_TXPowerTrackingCallback_TSSI(dev);
	else
		dm_TXPowerTrackingCallback_ThermalMeter(dev);
#elif defined(RTL8192SE)
	dm_TXPowerTrackingCallback_ThermalMeter(dev);
#endif
}

#ifndef RTL8192SE
static void dm_InitializeTXPowerTracking_TSSI(struct net_device *dev)
{

	struct r8192_priv *priv = rtllib_priv(dev);

	//Initial the Tx BB index and mapping value
	priv->txbbgain_table[0].txbb_iq_amplifygain =			12;
	priv->txbbgain_table[0].txbbgain_value=0x7f8001fe;
	priv->txbbgain_table[1].txbb_iq_amplifygain =			11;
	priv->txbbgain_table[1].txbbgain_value=0x788001e2;
	priv->txbbgain_table[2].txbb_iq_amplifygain =			10;
	priv->txbbgain_table[2].txbbgain_value=0x71c001c7;
	priv->txbbgain_table[3].txbb_iq_amplifygain =			9;
	priv->txbbgain_table[3].txbbgain_value=0x6b8001ae;
	priv->txbbgain_table[4].txbb_iq_amplifygain =		       8;
	priv->txbbgain_table[4].txbbgain_value=0x65400195;
	priv->txbbgain_table[5].txbb_iq_amplifygain =		       7;
	priv->txbbgain_table[5].txbbgain_value=0x5fc0017f;
	priv->txbbgain_table[6].txbb_iq_amplifygain =		       6;
	priv->txbbgain_table[6].txbbgain_value=0x5a400169;
	priv->txbbgain_table[7].txbb_iq_amplifygain =		       5;
	priv->txbbgain_table[7].txbbgain_value=0x55400155;
	priv->txbbgain_table[8].txbb_iq_amplifygain =		       4;
	priv->txbbgain_table[8].txbbgain_value=0x50800142;
	priv->txbbgain_table[9].txbb_iq_amplifygain =		       3;
	priv->txbbgain_table[9].txbbgain_value=0x4c000130;
	priv->txbbgain_table[10].txbb_iq_amplifygain =		       2;
	priv->txbbgain_table[10].txbbgain_value=0x47c0011f;
	priv->txbbgain_table[11].txbb_iq_amplifygain =		       1;
	priv->txbbgain_table[11].txbbgain_value=0x43c0010f;
	priv->txbbgain_table[12].txbb_iq_amplifygain =		       0;
	priv->txbbgain_table[12].txbbgain_value=0x40000100;
	priv->txbbgain_table[13].txbb_iq_amplifygain =		       -1;
	priv->txbbgain_table[13].txbbgain_value=0x3c8000f2;
	priv->txbbgain_table[14].txbb_iq_amplifygain =		     -2;
	priv->txbbgain_table[14].txbbgain_value=0x390000e4;
	priv->txbbgain_table[15].txbb_iq_amplifygain =		     -3;
	priv->txbbgain_table[15].txbbgain_value=0x35c000d7;
	priv->txbbgain_table[16].txbb_iq_amplifygain =		     -4;
	priv->txbbgain_table[16].txbbgain_value=0x32c000cb;
	priv->txbbgain_table[17].txbb_iq_amplifygain =		     -5;
	priv->txbbgain_table[17].txbbgain_value=0x300000c0;
	priv->txbbgain_table[18].txbb_iq_amplifygain =			    -6;
	priv->txbbgain_table[18].txbbgain_value=0x2d4000b5;
	priv->txbbgain_table[19].txbb_iq_amplifygain =		     -7;
	priv->txbbgain_table[19].txbbgain_value=0x2ac000ab;
	priv->txbbgain_table[20].txbb_iq_amplifygain =		     -8;
	priv->txbbgain_table[20].txbbgain_value=0x288000a2;
	priv->txbbgain_table[21].txbb_iq_amplifygain =		     -9;
	priv->txbbgain_table[21].txbbgain_value=0x26000098;
	priv->txbbgain_table[22].txbb_iq_amplifygain =		     -10;
	priv->txbbgain_table[22].txbbgain_value=0x24000090;
	priv->txbbgain_table[23].txbb_iq_amplifygain =		     -11;
	priv->txbbgain_table[23].txbbgain_value=0x22000088;
	priv->txbbgain_table[24].txbb_iq_amplifygain =		     -12;
	priv->txbbgain_table[24].txbbgain_value=0x20000080;
	priv->txbbgain_table[25].txbb_iq_amplifygain =		     -13;
	priv->txbbgain_table[25].txbbgain_value=0x1a00006c;
	priv->txbbgain_table[26].txbb_iq_amplifygain =		     -14;
	priv->txbbgain_table[26].txbbgain_value=0x1c800072;
	priv->txbbgain_table[27].txbb_iq_amplifygain =		     -15;
	priv->txbbgain_table[27].txbbgain_value=0x18000060;
	priv->txbbgain_table[28].txbb_iq_amplifygain =		     -16;
	priv->txbbgain_table[28].txbbgain_value=0x19800066;
	priv->txbbgain_table[29].txbb_iq_amplifygain =		     -17;
	priv->txbbgain_table[29].txbbgain_value=0x15800056;
	priv->txbbgain_table[30].txbb_iq_amplifygain =		     -18;
	priv->txbbgain_table[30].txbbgain_value=0x26c0005b;
	priv->txbbgain_table[31].txbb_iq_amplifygain =		     -19;
	priv->txbbgain_table[31].txbbgain_value=0x14400051;
	priv->txbbgain_table[32].txbb_iq_amplifygain =		     -20;
	priv->txbbgain_table[32].txbbgain_value=0x24400051;
	priv->txbbgain_table[33].txbb_iq_amplifygain =		     -21;
	priv->txbbgain_table[33].txbbgain_value=0x1300004c;
	priv->txbbgain_table[34].txbb_iq_amplifygain =		     -22;
	priv->txbbgain_table[34].txbbgain_value=0x12000048;
	priv->txbbgain_table[35].txbb_iq_amplifygain =		     -23;
	priv->txbbgain_table[35].txbbgain_value=0x11000044;
	priv->txbbgain_table[36].txbb_iq_amplifygain =		     -24;
	priv->txbbgain_table[36].txbbgain_value=0x10000040;

	//ccktxbb_valuearray[0] is 0xA22 [1] is 0xA24 ...[7] is 0xA29
	//This Table is for CH1~CH13
	priv->cck_txbbgain_table[0].ccktxbb_valuearray[0] = 0x36;
	priv->cck_txbbgain_table[0].ccktxbb_valuearray[1] = 0x35;
	priv->cck_txbbgain_table[0].ccktxbb_valuearray[2] = 0x2e;
	priv->cck_txbbgain_table[0].ccktxbb_valuearray[3] = 0x25;
	priv->cck_txbbgain_table[0].ccktxbb_valuearray[4] = 0x1c;
	priv->cck_txbbgain_table[0].ccktxbb_valuearray[5] = 0x12;
	priv->cck_txbbgain_table[0].ccktxbb_valuearray[6] = 0x09;
	priv->cck_txbbgain_table[0].ccktxbb_valuearray[7] = 0x04;

	priv->cck_txbbgain_table[1].ccktxbb_valuearray[0] = 0x33;
	priv->cck_txbbgain_table[1].ccktxbb_valuearray[1] = 0x32;
	priv->cck_txbbgain_table[1].ccktxbb_valuearray[2] = 0x2b;
	priv->cck_txbbgain_table[1].ccktxbb_valuearray[3] = 0x23;
	priv->cck_txbbgain_table[1].ccktxbb_valuearray[4] = 0x1a;
	priv->cck_txbbgain_table[1].ccktxbb_valuearray[5] = 0x11;
	priv->cck_txbbgain_table[1].ccktxbb_valuearray[6] = 0x08;
	priv->cck_txbbgain_table[1].ccktxbb_valuearray[7] = 0x04;

	priv->cck_txbbgain_table[2].ccktxbb_valuearray[0] = 0x30;
	priv->cck_txbbgain_table[2].ccktxbb_valuearray[1] = 0x2f;
	priv->cck_txbbgain_table[2].ccktxbb_valuearray[2] = 0x29;
	priv->cck_txbbgain_table[2].ccktxbb_valuearray[3] = 0x21;
	priv->cck_txbbgain_table[2].ccktxbb_valuearray[4] = 0x19;
	priv->cck_txbbgain_table[2].ccktxbb_valuearray[5] = 0x10;
	priv->cck_txbbgain_table[2].ccktxbb_valuearray[6] = 0x08;
	priv->cck_txbbgain_table[2].ccktxbb_valuearray[7] = 0x03;

	priv->cck_txbbgain_table[3].ccktxbb_valuearray[0] = 0x2d;
	priv->cck_txbbgain_table[3].ccktxbb_valuearray[1] = 0x2d;
	priv->cck_txbbgain_table[3].ccktxbb_valuearray[2] = 0x27;
	priv->cck_txbbgain_table[3].ccktxbb_valuearray[3] = 0x1f;
	priv->cck_txbbgain_table[3].ccktxbb_valuearray[4] = 0x18;
	priv->cck_txbbgain_table[3].ccktxbb_valuearray[5] = 0x0f;
	priv->cck_txbbgain_table[3].ccktxbb_valuearray[6] = 0x08;
	priv->cck_txbbgain_table[3].ccktxbb_valuearray[7] = 0x03;

	priv->cck_txbbgain_table[4].ccktxbb_valuearray[0] = 0x2b;
	priv->cck_txbbgain_table[4].ccktxbb_valuearray[1] = 0x2a;
	priv->cck_txbbgain_table[4].ccktxbb_valuearray[2] = 0x25;
	priv->cck_txbbgain_table[4].ccktxbb_valuearray[3] = 0x1e;
	priv->cck_txbbgain_table[4].ccktxbb_valuearray[4] = 0x16;
	priv->cck_txbbgain_table[4].ccktxbb_valuearray[5] = 0x0e;
	priv->cck_txbbgain_table[4].ccktxbb_valuearray[6] = 0x07;
	priv->cck_txbbgain_table[4].ccktxbb_valuearray[7] = 0x03;

	priv->cck_txbbgain_table[5].ccktxbb_valuearray[0] = 0x28;
	priv->cck_txbbgain_table[5].ccktxbb_valuearray[1] = 0x28;
	priv->cck_txbbgain_table[5].ccktxbb_valuearray[2] = 0x22;
	priv->cck_txbbgain_table[5].ccktxbb_valuearray[3] = 0x1c;
	priv->cck_txbbgain_table[5].ccktxbb_valuearray[4] = 0x15;
	priv->cck_txbbgain_table[5].ccktxbb_valuearray[5] = 0x0d;
	priv->cck_txbbgain_table[5].ccktxbb_valuearray[6] = 0x07;
	priv->cck_txbbgain_table[5].ccktxbb_valuearray[7] = 0x03;

	priv->cck_txbbgain_table[6].ccktxbb_valuearray[0] = 0x26;
	priv->cck_txbbgain_table[6].ccktxbb_valuearray[1] = 0x25;
	priv->cck_txbbgain_table[6].ccktxbb_valuearray[2] = 0x21;
	priv->cck_txbbgain_table[6].ccktxbb_valuearray[3] = 0x1b;
	priv->cck_txbbgain_table[6].ccktxbb_valuearray[4] = 0x14;
	priv->cck_txbbgain_table[6].ccktxbb_valuearray[5] = 0x0d;
	priv->cck_txbbgain_table[6].ccktxbb_valuearray[6] = 0x06;
	priv->cck_txbbgain_table[6].ccktxbb_valuearray[7] = 0x03;

	priv->cck_txbbgain_table[7].ccktxbb_valuearray[0] = 0x24;
	priv->cck_txbbgain_table[7].ccktxbb_valuearray[1] = 0x23;
	priv->cck_txbbgain_table[7].ccktxbb_valuearray[2] = 0x1f;
	priv->cck_txbbgain_table[7].ccktxbb_valuearray[3] = 0x19;
	priv->cck_txbbgain_table[7].ccktxbb_valuearray[4] = 0x13;
	priv->cck_txbbgain_table[7].ccktxbb_valuearray[5] = 0x0c;
	priv->cck_txbbgain_table[7].ccktxbb_valuearray[6] = 0x06;
	priv->cck_txbbgain_table[7].ccktxbb_valuearray[7] = 0x03;

	priv->cck_txbbgain_table[8].ccktxbb_valuearray[0] = 0x22;
	priv->cck_txbbgain_table[8].ccktxbb_valuearray[1] = 0x21;
	priv->cck_txbbgain_table[8].ccktxbb_valuearray[2] = 0x1d;
	priv->cck_txbbgain_table[8].ccktxbb_valuearray[3] = 0x18;
	priv->cck_txbbgain_table[8].ccktxbb_valuearray[4] = 0x11;
	priv->cck_txbbgain_table[8].ccktxbb_valuearray[5] = 0x0b;
	priv->cck_txbbgain_table[8].ccktxbb_valuearray[6] = 0x06;
	priv->cck_txbbgain_table[8].ccktxbb_valuearray[7] = 0x02;

	priv->cck_txbbgain_table[9].ccktxbb_valuearray[0] = 0x20;
	priv->cck_txbbgain_table[9].ccktxbb_valuearray[1] = 0x20;
	priv->cck_txbbgain_table[9].ccktxbb_valuearray[2] = 0x1b;
	priv->cck_txbbgain_table[9].ccktxbb_valuearray[3] = 0x16;
	priv->cck_txbbgain_table[9].ccktxbb_valuearray[4] = 0x11;
	priv->cck_txbbgain_table[9].ccktxbb_valuearray[5] = 0x08;
	priv->cck_txbbgain_table[9].ccktxbb_valuearray[6] = 0x05;
	priv->cck_txbbgain_table[9].ccktxbb_valuearray[7] = 0x02;

	priv->cck_txbbgain_table[10].ccktxbb_valuearray[0] = 0x1f;
	priv->cck_txbbgain_table[10].ccktxbb_valuearray[1] = 0x1e;
	priv->cck_txbbgain_table[10].ccktxbb_valuearray[2] = 0x1a;
	priv->cck_txbbgain_table[10].ccktxbb_valuearray[3] = 0x15;
	priv->cck_txbbgain_table[10].ccktxbb_valuearray[4] = 0x10;
	priv->cck_txbbgain_table[10].ccktxbb_valuearray[5] = 0x0a;
	priv->cck_txbbgain_table[10].ccktxbb_valuearray[6] = 0x05;
	priv->cck_txbbgain_table[10].ccktxbb_valuearray[7] = 0x02;

	priv->cck_txbbgain_table[11].ccktxbb_valuearray[0] = 0x1d;
	priv->cck_txbbgain_table[11].ccktxbb_valuearray[1] = 0x1c;
	priv->cck_txbbgain_table[11].ccktxbb_valuearray[2] = 0x18;
	priv->cck_txbbgain_table[11].ccktxbb_valuearray[3] = 0x14;
	priv->cck_txbbgain_table[11].ccktxbb_valuearray[4] = 0x0f;
	priv->cck_txbbgain_table[11].ccktxbb_valuearray[5] = 0x0a;
	priv->cck_txbbgain_table[11].ccktxbb_valuearray[6] = 0x05;
	priv->cck_txbbgain_table[11].ccktxbb_valuearray[7] = 0x02;

	priv->cck_txbbgain_table[12].ccktxbb_valuearray[0] = 0x1b;
	priv->cck_txbbgain_table[12].ccktxbb_valuearray[1] = 0x1a;
	priv->cck_txbbgain_table[12].ccktxbb_valuearray[2] = 0x17;
	priv->cck_txbbgain_table[12].ccktxbb_valuearray[3] = 0x13;
	priv->cck_txbbgain_table[12].ccktxbb_valuearray[4] = 0x0e;
	priv->cck_txbbgain_table[12].ccktxbb_valuearray[5] = 0x09;
	priv->cck_txbbgain_table[12].ccktxbb_valuearray[6] = 0x04;
	priv->cck_txbbgain_table[12].ccktxbb_valuearray[7] = 0x02;

	priv->cck_txbbgain_table[13].ccktxbb_valuearray[0] = 0x1a;
	priv->cck_txbbgain_table[13].ccktxbb_valuearray[1] = 0x19;
	priv->cck_txbbgain_table[13].ccktxbb_valuearray[2] = 0x16;
	priv->cck_txbbgain_table[13].ccktxbb_valuearray[3] = 0x12;
	priv->cck_txbbgain_table[13].ccktxbb_valuearray[4] = 0x0d;
	priv->cck_txbbgain_table[13].ccktxbb_valuearray[5] = 0x09;
	priv->cck_txbbgain_table[13].ccktxbb_valuearray[6] = 0x04;
	priv->cck_txbbgain_table[13].ccktxbb_valuearray[7] = 0x02;

	priv->cck_txbbgain_table[14].ccktxbb_valuearray[0] = 0x18;
	priv->cck_txbbgain_table[14].ccktxbb_valuearray[1] = 0x17;
	priv->cck_txbbgain_table[14].ccktxbb_valuearray[2] = 0x15;
	priv->cck_txbbgain_table[14].ccktxbb_valuearray[3] = 0x11;
	priv->cck_txbbgain_table[14].ccktxbb_valuearray[4] = 0x0c;
	priv->cck_txbbgain_table[14].ccktxbb_valuearray[5] = 0x08;
	priv->cck_txbbgain_table[14].ccktxbb_valuearray[6] = 0x04;
	priv->cck_txbbgain_table[14].ccktxbb_valuearray[7] = 0x02;

	priv->cck_txbbgain_table[15].ccktxbb_valuearray[0] = 0x17;
	priv->cck_txbbgain_table[15].ccktxbb_valuearray[1] = 0x16;
	priv->cck_txbbgain_table[15].ccktxbb_valuearray[2] = 0x13;
	priv->cck_txbbgain_table[15].ccktxbb_valuearray[3] = 0x10;
	priv->cck_txbbgain_table[15].ccktxbb_valuearray[4] = 0x0c;
	priv->cck_txbbgain_table[15].ccktxbb_valuearray[5] = 0x08;
	priv->cck_txbbgain_table[15].ccktxbb_valuearray[6] = 0x04;
	priv->cck_txbbgain_table[15].ccktxbb_valuearray[7] = 0x02;

	priv->cck_txbbgain_table[16].ccktxbb_valuearray[0] = 0x16;
	priv->cck_txbbgain_table[16].ccktxbb_valuearray[1] = 0x15;
	priv->cck_txbbgain_table[16].ccktxbb_valuearray[2] = 0x12;
	priv->cck_txbbgain_table[16].ccktxbb_valuearray[3] = 0x0f;
	priv->cck_txbbgain_table[16].ccktxbb_valuearray[4] = 0x0b;
	priv->cck_txbbgain_table[16].ccktxbb_valuearray[5] = 0x07;
	priv->cck_txbbgain_table[16].ccktxbb_valuearray[6] = 0x04;
	priv->cck_txbbgain_table[16].ccktxbb_valuearray[7] = 0x01;

	priv->cck_txbbgain_table[17].ccktxbb_valuearray[0] = 0x14;
	priv->cck_txbbgain_table[17].ccktxbb_valuearray[1] = 0x14;
	priv->cck_txbbgain_table[17].ccktxbb_valuearray[2] = 0x11;
	priv->cck_txbbgain_table[17].ccktxbb_valuearray[3] = 0x0e;
	priv->cck_txbbgain_table[17].ccktxbb_valuearray[4] = 0x0b;
	priv->cck_txbbgain_table[17].ccktxbb_valuearray[5] = 0x07;
	priv->cck_txbbgain_table[17].ccktxbb_valuearray[6] = 0x03;
	priv->cck_txbbgain_table[17].ccktxbb_valuearray[7] = 0x02;

	priv->cck_txbbgain_table[18].ccktxbb_valuearray[0] = 0x13;
	priv->cck_txbbgain_table[18].ccktxbb_valuearray[1] = 0x13;
	priv->cck_txbbgain_table[18].ccktxbb_valuearray[2] = 0x10;
	priv->cck_txbbgain_table[18].ccktxbb_valuearray[3] = 0x0d;
	priv->cck_txbbgain_table[18].ccktxbb_valuearray[4] = 0x0a;
	priv->cck_txbbgain_table[18].ccktxbb_valuearray[5] = 0x06;
	priv->cck_txbbgain_table[18].ccktxbb_valuearray[6] = 0x03;
	priv->cck_txbbgain_table[18].ccktxbb_valuearray[7] = 0x01;

	priv->cck_txbbgain_table[19].ccktxbb_valuearray[0] = 0x12;
	priv->cck_txbbgain_table[19].ccktxbb_valuearray[1] = 0x12;
	priv->cck_txbbgain_table[19].ccktxbb_valuearray[2] = 0x0f;
	priv->cck_txbbgain_table[19].ccktxbb_valuearray[3] = 0x0c;
	priv->cck_txbbgain_table[19].ccktxbb_valuearray[4] = 0x09;
	priv->cck_txbbgain_table[19].ccktxbb_valuearray[5] = 0x06;
	priv->cck_txbbgain_table[19].ccktxbb_valuearray[6] = 0x03;
	priv->cck_txbbgain_table[19].ccktxbb_valuearray[7] = 0x01;

	priv->cck_txbbgain_table[20].ccktxbb_valuearray[0] = 0x11;
	priv->cck_txbbgain_table[20].ccktxbb_valuearray[1] = 0x11;
	priv->cck_txbbgain_table[20].ccktxbb_valuearray[2] = 0x0f;
	priv->cck_txbbgain_table[20].ccktxbb_valuearray[3] = 0x0c;
	priv->cck_txbbgain_table[20].ccktxbb_valuearray[4] = 0x09;
	priv->cck_txbbgain_table[20].ccktxbb_valuearray[5] = 0x06;
	priv->cck_txbbgain_table[20].ccktxbb_valuearray[6] = 0x03;
	priv->cck_txbbgain_table[20].ccktxbb_valuearray[7] = 0x01;

	priv->cck_txbbgain_table[21].ccktxbb_valuearray[0] = 0x10;
	priv->cck_txbbgain_table[21].ccktxbb_valuearray[1] = 0x10;
	priv->cck_txbbgain_table[21].ccktxbb_valuearray[2] = 0x0e;
	priv->cck_txbbgain_table[21].ccktxbb_valuearray[3] = 0x0b;
	priv->cck_txbbgain_table[21].ccktxbb_valuearray[4] = 0x08;
	priv->cck_txbbgain_table[21].ccktxbb_valuearray[5] = 0x05;
	priv->cck_txbbgain_table[21].ccktxbb_valuearray[6] = 0x03;
	priv->cck_txbbgain_table[21].ccktxbb_valuearray[7] = 0x01;

	priv->cck_txbbgain_table[22].ccktxbb_valuearray[0] = 0x0f;
	priv->cck_txbbgain_table[22].ccktxbb_valuearray[1] = 0x0f;
	priv->cck_txbbgain_table[22].ccktxbb_valuearray[2] = 0x0d;
	priv->cck_txbbgain_table[22].ccktxbb_valuearray[3] = 0x0b;
	priv->cck_txbbgain_table[22].ccktxbb_valuearray[4] = 0x08;
	priv->cck_txbbgain_table[22].ccktxbb_valuearray[5] = 0x05;
	priv->cck_txbbgain_table[22].ccktxbb_valuearray[6] = 0x03;
	priv->cck_txbbgain_table[22].ccktxbb_valuearray[7] = 0x01;

	//ccktxbb_valuearray[0] is 0xA22 [1] is 0xA24 ...[7] is 0xA29
	//This Table is for CH14
	priv->cck_txbbgain_ch14_table[0].ccktxbb_valuearray[0] = 0x36;
	priv->cck_txbbgain_ch14_table[0].ccktxbb_valuearray[1] = 0x35;
	priv->cck_txbbgain_ch14_table[0].ccktxbb_valuearray[2] = 0x2e;
	priv->cck_txbbgain_ch14_table[0].ccktxbb_valuearray[3] = 0x1b;
	priv->cck_txbbgain_ch14_table[0].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[0].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[0].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[0].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[1].ccktxbb_valuearray[0] = 0x33;
	priv->cck_txbbgain_ch14_table[1].ccktxbb_valuearray[1] = 0x32;
	priv->cck_txbbgain_ch14_table[1].ccktxbb_valuearray[2] = 0x2b;
	priv->cck_txbbgain_ch14_table[1].ccktxbb_valuearray[3] = 0x19;
	priv->cck_txbbgain_ch14_table[1].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[1].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[1].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[1].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[2].ccktxbb_valuearray[0] = 0x30;
	priv->cck_txbbgain_ch14_table[2].ccktxbb_valuearray[1] = 0x2f;
	priv->cck_txbbgain_ch14_table[2].ccktxbb_valuearray[2] = 0x29;
	priv->cck_txbbgain_ch14_table[2].ccktxbb_valuearray[3] = 0x18;
	priv->cck_txbbgain_ch14_table[2].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[2].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[2].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[2].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[3].ccktxbb_valuearray[0] = 0x2d;
	priv->cck_txbbgain_ch14_table[3].ccktxbb_valuearray[1] = 0x2d;
	priv->cck_txbbgain_ch14_table[3].ccktxbb_valuearray[2] = 0x27;
	priv->cck_txbbgain_ch14_table[3].ccktxbb_valuearray[3] = 0x17;
	priv->cck_txbbgain_ch14_table[3].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[3].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[3].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[3].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[4].ccktxbb_valuearray[0] = 0x2b;
	priv->cck_txbbgain_ch14_table[4].ccktxbb_valuearray[1] = 0x2a;
	priv->cck_txbbgain_ch14_table[4].ccktxbb_valuearray[2] = 0x25;
	priv->cck_txbbgain_ch14_table[4].ccktxbb_valuearray[3] = 0x15;
	priv->cck_txbbgain_ch14_table[4].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[4].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[4].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[4].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[5].ccktxbb_valuearray[0] = 0x28;
	priv->cck_txbbgain_ch14_table[5].ccktxbb_valuearray[1] = 0x28;
	priv->cck_txbbgain_ch14_table[5].ccktxbb_valuearray[2] = 0x22;
	priv->cck_txbbgain_ch14_table[5].ccktxbb_valuearray[3] = 0x14;
	priv->cck_txbbgain_ch14_table[5].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[5].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[5].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[5].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[6].ccktxbb_valuearray[0] = 0x26;
	priv->cck_txbbgain_ch14_table[6].ccktxbb_valuearray[1] = 0x25;
	priv->cck_txbbgain_ch14_table[6].ccktxbb_valuearray[2] = 0x21;
	priv->cck_txbbgain_ch14_table[6].ccktxbb_valuearray[3] = 0x13;
	priv->cck_txbbgain_ch14_table[6].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[6].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[6].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[6].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[7].ccktxbb_valuearray[0] = 0x24;
	priv->cck_txbbgain_ch14_table[7].ccktxbb_valuearray[1] = 0x23;
	priv->cck_txbbgain_ch14_table[7].ccktxbb_valuearray[2] = 0x1f;
	priv->cck_txbbgain_ch14_table[7].ccktxbb_valuearray[3] = 0x12;
	priv->cck_txbbgain_ch14_table[7].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[7].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[7].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[7].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[8].ccktxbb_valuearray[0] = 0x22;
	priv->cck_txbbgain_ch14_table[8].ccktxbb_valuearray[1] = 0x21;
	priv->cck_txbbgain_ch14_table[8].ccktxbb_valuearray[2] = 0x1d;
	priv->cck_txbbgain_ch14_table[8].ccktxbb_valuearray[3] = 0x11;
	priv->cck_txbbgain_ch14_table[8].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[8].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[8].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[8].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[9].ccktxbb_valuearray[0] = 0x20;
	priv->cck_txbbgain_ch14_table[9].ccktxbb_valuearray[1] = 0x20;
	priv->cck_txbbgain_ch14_table[9].ccktxbb_valuearray[2] = 0x1b;
	priv->cck_txbbgain_ch14_table[9].ccktxbb_valuearray[3] = 0x10;
	priv->cck_txbbgain_ch14_table[9].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[9].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[9].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[9].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[10].ccktxbb_valuearray[0] = 0x1f;
	priv->cck_txbbgain_ch14_table[10].ccktxbb_valuearray[1] = 0x1e;
	priv->cck_txbbgain_ch14_table[10].ccktxbb_valuearray[2] = 0x1a;
	priv->cck_txbbgain_ch14_table[10].ccktxbb_valuearray[3] = 0x0f;
	priv->cck_txbbgain_ch14_table[10].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[10].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[10].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[10].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[11].ccktxbb_valuearray[0] = 0x1d;
	priv->cck_txbbgain_ch14_table[11].ccktxbb_valuearray[1] = 0x1c;
	priv->cck_txbbgain_ch14_table[11].ccktxbb_valuearray[2] = 0x18;
	priv->cck_txbbgain_ch14_table[11].ccktxbb_valuearray[3] = 0x0e;
	priv->cck_txbbgain_ch14_table[11].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[11].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[11].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[11].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[12].ccktxbb_valuearray[0] = 0x1b;
	priv->cck_txbbgain_ch14_table[12].ccktxbb_valuearray[1] = 0x1a;
	priv->cck_txbbgain_ch14_table[12].ccktxbb_valuearray[2] = 0x17;
	priv->cck_txbbgain_ch14_table[12].ccktxbb_valuearray[3] = 0x0e;
	priv->cck_txbbgain_ch14_table[12].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[12].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[12].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[12].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[13].ccktxbb_valuearray[0] = 0x1a;
	priv->cck_txbbgain_ch14_table[13].ccktxbb_valuearray[1] = 0x19;
	priv->cck_txbbgain_ch14_table[13].ccktxbb_valuearray[2] = 0x16;
	priv->cck_txbbgain_ch14_table[13].ccktxbb_valuearray[3] = 0x0d;
	priv->cck_txbbgain_ch14_table[13].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[13].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[13].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[13].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[14].ccktxbb_valuearray[0] = 0x18;
	priv->cck_txbbgain_ch14_table[14].ccktxbb_valuearray[1] = 0x17;
	priv->cck_txbbgain_ch14_table[14].ccktxbb_valuearray[2] = 0x15;
	priv->cck_txbbgain_ch14_table[14].ccktxbb_valuearray[3] = 0x0c;
	priv->cck_txbbgain_ch14_table[14].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[14].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[14].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[14].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[15].ccktxbb_valuearray[0] = 0x17;
	priv->cck_txbbgain_ch14_table[15].ccktxbb_valuearray[1] = 0x16;
	priv->cck_txbbgain_ch14_table[15].ccktxbb_valuearray[2] = 0x13;
	priv->cck_txbbgain_ch14_table[15].ccktxbb_valuearray[3] = 0x0b;
	priv->cck_txbbgain_ch14_table[15].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[15].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[15].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[15].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[16].ccktxbb_valuearray[0] = 0x16;
	priv->cck_txbbgain_ch14_table[16].ccktxbb_valuearray[1] = 0x15;
	priv->cck_txbbgain_ch14_table[16].ccktxbb_valuearray[2] = 0x12;
	priv->cck_txbbgain_ch14_table[16].ccktxbb_valuearray[3] = 0x0b;
	priv->cck_txbbgain_ch14_table[16].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[16].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[16].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[16].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[17].ccktxbb_valuearray[0] = 0x14;
	priv->cck_txbbgain_ch14_table[17].ccktxbb_valuearray[1] = 0x14;
	priv->cck_txbbgain_ch14_table[17].ccktxbb_valuearray[2] = 0x11;
	priv->cck_txbbgain_ch14_table[17].ccktxbb_valuearray[3] = 0x0a;
	priv->cck_txbbgain_ch14_table[17].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[17].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[17].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[17].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[18].ccktxbb_valuearray[0] = 0x13;
	priv->cck_txbbgain_ch14_table[18].ccktxbb_valuearray[1] = 0x13;
	priv->cck_txbbgain_ch14_table[18].ccktxbb_valuearray[2] = 0x10;
	priv->cck_txbbgain_ch14_table[18].ccktxbb_valuearray[3] = 0x0a;
	priv->cck_txbbgain_ch14_table[18].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[18].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[18].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[18].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[19].ccktxbb_valuearray[0] = 0x12;
	priv->cck_txbbgain_ch14_table[19].ccktxbb_valuearray[1] = 0x12;
	priv->cck_txbbgain_ch14_table[19].ccktxbb_valuearray[2] = 0x0f;
	priv->cck_txbbgain_ch14_table[19].ccktxbb_valuearray[3] = 0x09;
	priv->cck_txbbgain_ch14_table[19].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[19].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[19].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[19].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[20].ccktxbb_valuearray[0] = 0x11;
	priv->cck_txbbgain_ch14_table[20].ccktxbb_valuearray[1] = 0x11;
	priv->cck_txbbgain_ch14_table[20].ccktxbb_valuearray[2] = 0x0f;
	priv->cck_txbbgain_ch14_table[20].ccktxbb_valuearray[3] = 0x09;
	priv->cck_txbbgain_ch14_table[20].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[20].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[20].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[20].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[21].ccktxbb_valuearray[0] = 0x10;
	priv->cck_txbbgain_ch14_table[21].ccktxbb_valuearray[1] = 0x10;
	priv->cck_txbbgain_ch14_table[21].ccktxbb_valuearray[2] = 0x0e;
	priv->cck_txbbgain_ch14_table[21].ccktxbb_valuearray[3] = 0x08;
	priv->cck_txbbgain_ch14_table[21].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[21].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[21].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[21].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[22].ccktxbb_valuearray[0] = 0x0f;
	priv->cck_txbbgain_ch14_table[22].ccktxbb_valuearray[1] = 0x0f;
	priv->cck_txbbgain_ch14_table[22].ccktxbb_valuearray[2] = 0x0d;
	priv->cck_txbbgain_ch14_table[22].ccktxbb_valuearray[3] = 0x08;
	priv->cck_txbbgain_ch14_table[22].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[22].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[22].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[22].ccktxbb_valuearray[7] = 0x00;

	priv->btxpower_tracking = true;
	priv->txpower_count       = 0;
	priv->btxpower_trackingInit = false;

}
#endif
#ifndef RTL8190P
static void dm_InitializeTXPowerTracking_ThermalMeter(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);

	// Tx Power tracking by Theremal Meter require Firmware R/W 3-wire. This mechanism
	// can be enabled only when Firmware R/W 3-wire is enabled. Otherwise, frequent r/w
	// 3-wire by driver cause RF goes into wrong state.
#ifdef RTL8192SE
		priv->btxpower_tracking = false;
		priv->txpower_count       = 0;
		priv->btxpower_trackingInit = false;
#else

	if(priv->rtllib->FwRWRF)
		priv->btxpower_tracking = true;
	else
		priv->btxpower_tracking = false;
	priv->txpower_count       = 0;
	priv->btxpower_trackingInit = false;
#endif
	RT_TRACE(COMP_POWER_TRACKING, "pMgntInfo->bTXPowerTracking = %d\n", priv->btxpower_tracking);
}
#endif

void dm_initialize_txpower_tracking(struct net_device *dev)
{
#ifdef RTL8192E
	struct r8192_priv *priv = rtllib_priv(dev);
#endif
#ifdef RTL8190P
	dm_InitializeTXPowerTracking_TSSI(dev);
#elif defined RTL8192E
	//if(priv->bDcut == true)
	if(priv->IC_Cut >= IC_VersionCut_D)
		dm_InitializeTXPowerTracking_TSSI(dev);
	else
		dm_InitializeTXPowerTracking_ThermalMeter(dev);
#elif defined RTL8192SE
	dm_InitializeTXPowerTracking_ThermalMeter(dev);
#endif
}	// dm_InitializeTXPowerTracking

#if (defined RTL8192E || defined RTL8190P)
static void dm_CheckTXPowerTracking_TSSI(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	static u32 tx_power_track_counter = 0;
	RT_TRACE(COMP_POWER_TRACKING,"%s()\n",__FUNCTION__);
	if(read_nic_byte(dev, 0x11e) ==1)
		return;
	if(!priv->btxpower_tracking)
		return;
	tx_power_track_counter++;


	 if(tx_power_track_counter >= 180)	// every 180*2=360 seconds execute txpwrtrack!!
		{
		queue_delayed_work_rsl(priv->priv_wq,&priv->txpower_tracking_wq,0);
		tx_power_track_counter =0;
		}

}
#endif
#ifndef RTL8190P
static void dm_CheckTXPowerTracking_ThermalMeter(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	static u8	TM_Trigger=0;
	u8		TxPowerCheckCnt = 0;

	if(IS_HARDWARE_TYPE_8192SE(dev))
		TxPowerCheckCnt = 5;	//10 sec
	else
		TxPowerCheckCnt = 2;	// 4 sec
        if(!priv->btxpower_tracking){
            return;
        } else {
            if(priv->txpower_count  <= TxPowerCheckCnt) {
			priv->txpower_count++;
			return;
		}
	}

	if(!TM_Trigger)
	{
		//Attention!! You have to wirte all 12bits data to RF, or it may cause RF to crash
		//actually write reg0x02 bit1=0, then bit1=1.
		//DbgPrint("Trigger ThermalMeter, write RF reg0x2 = 0x4d to 0x4f\n");
#ifdef RTL8192SE
		if(IS_HARDWARE_TYPE_8192SE(dev))
		{
                    rtl8192_phy_SetRFReg(dev, RF90_PATH_A, RF_T_METER, bRFRegOffsetMask, 0x60);
                    RT_TRACE(COMP_POWER_TRACKING, "Trigger 92S Thermal Meter!!\n");
                }
                else
#endif
                {
		rtl8192_phy_SetRFReg(dev, RF90_PATH_A, 0x02, bMask12Bits, 0x4d);
		rtl8192_phy_SetRFReg(dev, RF90_PATH_A, 0x02, bMask12Bits, 0x4f);
		rtl8192_phy_SetRFReg(dev, RF90_PATH_A, 0x02, bMask12Bits, 0x4d);
		rtl8192_phy_SetRFReg(dev, RF90_PATH_A, 0x02, bMask12Bits, 0x4f);
                }
		TM_Trigger = 1;
		return;
	} else {
            printk("===============>Schedule TxPowerTrackingWorkItem\n");
#ifdef RTL8192SE
		DM_TXPowerTracking92SDirectCall(dev); //Using direct call is instead, added by Roger, 2009.06.18.
#else

		queue_delayed_work_rsl(priv->priv_wq,&priv->txpower_tracking_wq,0);
#endif
		TM_Trigger = 0;
		}

	}
#endif

//
//	Description:
//		- Dispatch TxPower Tracking direct call ONLY for 92s.
//		- We shall NOT schedule Workitem within PASSIVE LEVEL, which will cause system resource
//		   leakage under some platform.
//
//	Assumption:
//		PASSIVE_LEVEL when this routine is called.
//
//	Added by Roger, 2009.06.18.
//
#ifdef RTL8192SE
void DM_TXPowerTracking92SDirectCall(struct net_device *dev)
{
	dm_TXPowerTrackingCallback_ThermalMeter(dev);
	}
#endif

static void dm_check_txpower_tracking(struct net_device *dev)
{
#ifdef RTL8192E
	struct r8192_priv *priv = rtllib_priv(dev);
	//static u32 tx_power_track_counter = 0;
#endif
#ifdef  RTL8190P
	dm_CheckTXPowerTracking_TSSI(dev);
#elif defined RTL8192E
	//if(priv->bDcut == true)
	if(priv->IC_Cut >= IC_VersionCut_D)
		dm_CheckTXPowerTracking_TSSI(dev);
	else
		dm_CheckTXPowerTracking_ThermalMeter(dev);
#elif defined RTL8192SE
	dm_CheckTXPowerTracking_ThermalMeter(dev);
#endif

}	// dm_CheckTXPowerTracking

#ifndef RTL8192SE
static void dm_CCKTxPowerAdjust_TSSI(struct net_device *dev, bool  bInCH14)
{
	u32 TempVal;
	struct r8192_priv *priv = rtllib_priv(dev);
	//Write 0xa22 0xa23
	TempVal = 0;
	if(!bInCH14){
		//Write 0xa22 0xa23
		TempVal =	(u32)(priv->cck_txbbgain_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[0] +
					(priv->cck_txbbgain_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[1]<<8)) ;

		rtl8192_setBBreg(dev, rCCK0_TxFilter1,bMaskHWord, TempVal);
		//Write 0xa24 ~ 0xa27
		TempVal = 0;
		TempVal =	(u32)(priv->cck_txbbgain_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[2] +
					(priv->cck_txbbgain_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[3]<<8) +
					(priv->cck_txbbgain_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[4]<<16 )+
					(priv->cck_txbbgain_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[5]<<24));
		rtl8192_setBBreg(dev, rCCK0_TxFilter2,bMaskDWord, TempVal);
		//Write 0xa28  0xa29
		TempVal = 0;
		TempVal =	(u32)(priv->cck_txbbgain_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[6] +
					(priv->cck_txbbgain_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[7]<<8)) ;

		rtl8192_setBBreg(dev, rCCK0_DebugPort,bMaskLWord, TempVal);
	}
	else
	{
		TempVal =	(u32)(priv->cck_txbbgain_ch14_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[0] +
					(priv->cck_txbbgain_ch14_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[1]<<8)) ;

		rtl8192_setBBreg(dev, rCCK0_TxFilter1,bMaskHWord, TempVal);
		//Write 0xa24 ~ 0xa27
		TempVal = 0;
		TempVal =	(u32)(priv->cck_txbbgain_ch14_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[2] +
					(priv->cck_txbbgain_ch14_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[3]<<8) +
					(priv->cck_txbbgain_ch14_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[4]<<16 )+
					(priv->cck_txbbgain_ch14_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[5]<<24));
		rtl8192_setBBreg(dev, rCCK0_TxFilter2,bMaskDWord, TempVal);
		//Write 0xa28  0xa29
		TempVal = 0;
		TempVal =	(u32)(priv->cck_txbbgain_ch14_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[6] +
					(priv->cck_txbbgain_ch14_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[7]<<8)) ;

		rtl8192_setBBreg(dev, rCCK0_DebugPort,bMaskLWord, TempVal);
	}


}
#endif
#ifdef RTL8192E
static void dm_CCKTxPowerAdjust_ThermalMeter(struct net_device *dev,	bool  bInCH14)
{
	u32 TempVal;
	struct r8192_priv *priv = rtllib_priv(dev);

	TempVal = 0;
	if(!bInCH14)
	{
		//Write 0xa22 0xa23
		TempVal =	CCKSwingTable_Ch1_Ch13[priv->CCK_index][0] +
					(CCKSwingTable_Ch1_Ch13[priv->CCK_index][1]<<8) ;
		rtl8192_setBBreg(dev, rCCK0_TxFilter1, bMaskHWord, TempVal);
		RT_TRACE(COMP_POWER_TRACKING, "CCK not chnl 14, reg 0x%x = 0x%x\n",
			rCCK0_TxFilter1, TempVal);
		//Write 0xa24 ~ 0xa27
		TempVal = 0;
		TempVal =	CCKSwingTable_Ch1_Ch13[priv->CCK_index][2] +
					(CCKSwingTable_Ch1_Ch13[priv->CCK_index][3]<<8) +
					(CCKSwingTable_Ch1_Ch13[priv->CCK_index][4]<<16 )+
					(CCKSwingTable_Ch1_Ch13[priv->CCK_index][5]<<24);
		rtl8192_setBBreg(dev, rCCK0_TxFilter2, bMaskDWord, TempVal);
		RT_TRACE(COMP_POWER_TRACKING, "CCK not chnl 14, reg 0x%x = 0x%x\n",
			rCCK0_TxFilter2, TempVal);
		//Write 0xa28  0xa29
		TempVal = 0;
		TempVal =	CCKSwingTable_Ch1_Ch13[priv->CCK_index][6] +
					(CCKSwingTable_Ch1_Ch13[priv->CCK_index][7]<<8) ;

		rtl8192_setBBreg(dev, rCCK0_DebugPort, bMaskLWord, TempVal);
		RT_TRACE(COMP_POWER_TRACKING, "CCK not chnl 14, reg 0x%x = 0x%x\n",
			rCCK0_DebugPort, TempVal);
	}
	else
	{
//		priv->CCKTxPowerAdjustCntNotCh14++;	//cosa add for debug.
		//Write 0xa22 0xa23
		TempVal =	CCKSwingTable_Ch14[priv->CCK_index][0] +
					(CCKSwingTable_Ch14[priv->CCK_index][1]<<8) ;

		rtl8192_setBBreg(dev, rCCK0_TxFilter1, bMaskHWord, TempVal);
		RT_TRACE(COMP_POWER_TRACKING, "CCK chnl 14, reg 0x%x = 0x%x\n",
			rCCK0_TxFilter1, TempVal);
		//Write 0xa24 ~ 0xa27
		TempVal = 0;
		TempVal =	CCKSwingTable_Ch14[priv->CCK_index][2] +
					(CCKSwingTable_Ch14[priv->CCK_index][3]<<8) +
					(CCKSwingTable_Ch14[priv->CCK_index][4]<<16 )+
					(CCKSwingTable_Ch14[priv->CCK_index][5]<<24);
		rtl8192_setBBreg(dev, rCCK0_TxFilter2, bMaskDWord, TempVal);
		RT_TRACE(COMP_POWER_TRACKING, "CCK chnl 14, reg 0x%x = 0x%x\n",
			rCCK0_TxFilter2, TempVal);
		//Write 0xa28  0xa29
		TempVal = 0;
		TempVal =	CCKSwingTable_Ch14[priv->CCK_index][6] +
					(CCKSwingTable_Ch14[priv->CCK_index][7]<<8) ;

		rtl8192_setBBreg(dev, rCCK0_DebugPort, bMaskLWord, TempVal);
		RT_TRACE(COMP_POWER_TRACKING,"CCK chnl 14, reg 0x%x = 0x%x\n",
			rCCK0_DebugPort, TempVal);
	}
	}
#endif

#ifndef RTL8192SE
extern void dm_cck_txpower_adjust(
	struct net_device *dev,
	bool  binch14
)
{	// dm_CCKTxPowerAdjust
#ifndef RTL8190P
	struct r8192_priv *priv = rtllib_priv(dev);
#endif
#ifdef RTL8190P
	dm_CCKTxPowerAdjust_TSSI(dev, binch14);
#else
	//if(priv->bDcut == true)
	if(priv->IC_Cut >= IC_VersionCut_D)
		dm_CCKTxPowerAdjust_TSSI(dev, binch14);
	else
		dm_CCKTxPowerAdjust_ThermalMeter(dev, binch14);
#endif
}
#endif


#if defined(RTL8192E)||defined(RTL8190P)
static void dm_txpower_reset_recovery(
	struct net_device *dev
)
{
	struct r8192_priv *priv = rtllib_priv(dev);

	RT_TRACE(COMP_POWER_TRACKING, "Start Reset Recovery ==>\n");
	rtl8192_setBBreg(dev, rOFDM0_XATxIQImbalance, bMaskDWord, priv->txbbgain_table[priv->rfa_txpowertrackingindex].txbbgain_value);
	RT_TRACE(COMP_POWER_TRACKING, "Reset Recovery: Fill in 0xc80 is %08x\n",priv->txbbgain_table[priv->rfa_txpowertrackingindex].txbbgain_value);
	RT_TRACE(COMP_POWER_TRACKING, "Reset Recovery: Fill in RFA_txPowerTrackingIndex is %x\n",priv->rfa_txpowertrackingindex);
	RT_TRACE(COMP_POWER_TRACKING, "Reset Recovery : RF A I/Q Amplify Gain is %ld\n",priv->txbbgain_table[priv->rfa_txpowertrackingindex].txbb_iq_amplifygain);
	RT_TRACE(COMP_POWER_TRACKING, "Reset Recovery: CCK Attenuation is %d dB\n",priv->CCKPresentAttentuation);
	dm_cck_txpower_adjust(dev,priv->bcck_in_ch14);

	rtl8192_setBBreg(dev, rOFDM0_XCTxIQImbalance, bMaskDWord, priv->txbbgain_table[priv->rfc_txpowertrackingindex].txbbgain_value);
	RT_TRACE(COMP_POWER_TRACKING, "Reset Recovery: Fill in 0xc90 is %08x\n",priv->txbbgain_table[priv->rfc_txpowertrackingindex].txbbgain_value);
	RT_TRACE(COMP_POWER_TRACKING, "Reset Recovery: Fill in RFC_txPowerTrackingIndex is %x\n",priv->rfc_txpowertrackingindex);
	RT_TRACE(COMP_POWER_TRACKING, "Reset Recovery : RF C I/Q Amplify Gain is %ld\n",priv->txbbgain_table[priv->rfc_txpowertrackingindex].txbb_iq_amplifygain);

}	// dm_TXPowerResetRecovery

extern void dm_restore_dynamic_mechanism_state(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	u32	reg_ratr = priv->rate_adaptive.last_ratr;

#ifdef _RTL8192_EXT_PATCH_
	if((!priv->up)&& (!priv->mesh_up))
#else
	if(!priv->up)
#endif
	{
		RT_TRACE(COMP_RATE, "<---- dm_restore_dynamic_mechanism_state(): driver is going to unload\n");
		return;
	}

	//
	// Restore previous state for rate adaptive
	//
	if(priv->rate_adaptive.rate_adaptive_disabled)
		return;
	// TODO: Only 11n mode is implemented currently,
	if( !(priv->rtllib->mode==WIRELESS_MODE_N_24G ||
		 priv->rtllib->mode==WIRELESS_MODE_N_5G))
		 return;
	{
			// 2007/11/15 MH Copy from 8190PCI.
			u32 ratr_value;
			ratr_value = reg_ratr;
			if(priv->rf_type == RF_1T2R)	// 1T2R, Spatial Stream 2 should be disabled
			{
				ratr_value &=~ (RATE_ALL_OFDM_2SS);
				//DbgPrint("HW_VAR_TATR_0 from 0x%x ==> 0x%x\n", ((pu4Byte)(val))[0], ratr_value);
			}
			//DbgPrint("set HW_VAR_TATR_0 = 0x%x\n", ratr_value);
			//cosa PlatformEFIOWrite4Byte(dev, RATR0, ((pu4Byte)(val))[0]);
			write_nic_dword(dev, RATR0, ratr_value);
			write_nic_byte(dev, UFWP, 1);
#if 0		// Disable old code.
			u1Byte index;
			u4Byte input_value;
			index = (u1Byte)((((pu4Byte)(val))[0]) >> 28);
			input_value = (((pu4Byte)(val))[0]) & 0x0fffffff;
			// TODO: Correct it. Emily 2007.01.11
			PlatformEFIOWrite4Byte(dev, RATR0+index*4, input_value);
#endif
	}
	//Resore TX Power Tracking Index
	if(priv->btxpower_trackingInit && priv->btxpower_tracking){
		dm_txpower_reset_recovery(dev);
	}

	//
	//Restore BB Initial Gain
	//
	dm_bb_initialgain_restore(dev);

}	// DM_RestoreDynamicMechanismState

static void dm_bb_initialgain_restore(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	u32 bit_mask = 0x7f; //Bit0~ Bit6

	if(dm_digtable.dig_algorithm == DIG_ALGO_BY_RSSI)
		return;

	//Disable Initial Gain
	//PHY_SetBBReg(dev, UFWP, bMaskLWord, 0x800);
	rtl8192_setBBreg(dev, UFWP, bMaskByte1, 0x8);	// Only clear byte 1 and rewrite.
	rtl8192_setBBreg(dev, rOFDM0_XAAGCCore1, bit_mask, (u32)priv->initgain_backup.xaagccore1);
	rtl8192_setBBreg(dev, rOFDM0_XBAGCCore1, bit_mask, (u32)priv->initgain_backup.xbagccore1);
	rtl8192_setBBreg(dev, rOFDM0_XCAGCCore1, bit_mask, (u32)priv->initgain_backup.xcagccore1);
	rtl8192_setBBreg(dev, rOFDM0_XDAGCCore1, bit_mask, (u32)priv->initgain_backup.xdagccore1);
	bit_mask  = bMaskByte2;
	rtl8192_setBBreg(dev, rCCK0_CCA, bit_mask, (u32)priv->initgain_backup.cca);

	RT_TRACE(COMP_DIG, "dm_BBInitialGainRestore 0xc50 is %x\n",priv->initgain_backup.xaagccore1);
	RT_TRACE(COMP_DIG, "dm_BBInitialGainRestore 0xc58 is %x\n",priv->initgain_backup.xbagccore1);
	RT_TRACE(COMP_DIG, "dm_BBInitialGainRestore 0xc60 is %x\n",priv->initgain_backup.xcagccore1);
	RT_TRACE(COMP_DIG, "dm_BBInitialGainRestore 0xc68 is %x\n",priv->initgain_backup.xdagccore1);
	RT_TRACE(COMP_DIG, "dm_BBInitialGainRestore 0xa0a is %x\n",priv->initgain_backup.cca);
	//Enable Initial Gain
	//PHY_SetBBReg(dev, UFWP, bMaskLWord, 0x100);
	rtl8192_setBBreg(dev, UFWP, bMaskByte1, 0x1);	// Only clear byte 1 and rewrite.

}	// dm_BBInitialGainRestore


extern void dm_backup_dynamic_mechanism_state(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);

	// Fsync to avoid reset
	priv->bswitch_fsync  = false;
	priv->bfsync_processing = false;
	//Backup BB InitialGain
	dm_bb_initialgain_backup(dev);

}	// DM_BackupDynamicMechanismState


static void dm_bb_initialgain_backup(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	u32 bit_mask = bMaskByte0; //Bit0~ Bit6

	if(dm_digtable.dig_algorithm == DIG_ALGO_BY_RSSI)
		return;

	//PHY_SetBBReg(dev, UFWP, bMaskLWord, 0x800);
	rtl8192_setBBreg(dev, UFWP, bMaskByte1, 0x8);	// Only clear byte 1 and rewrite.
	priv->initgain_backup.xaagccore1 = (u8)rtl8192_QueryBBReg(dev, rOFDM0_XAAGCCore1, bit_mask);
	priv->initgain_backup.xbagccore1 = (u8)rtl8192_QueryBBReg(dev, rOFDM0_XBAGCCore1, bit_mask);
	priv->initgain_backup.xcagccore1 = (u8)rtl8192_QueryBBReg(dev, rOFDM0_XCAGCCore1, bit_mask);
	priv->initgain_backup.xdagccore1 = (u8)rtl8192_QueryBBReg(dev, rOFDM0_XDAGCCore1, bit_mask);
	bit_mask  = bMaskByte2;
	priv->initgain_backup.cca = (u8)rtl8192_QueryBBReg(dev, rCCK0_CCA, bit_mask);

	RT_TRACE(COMP_DIG, "BBInitialGainBackup 0xc50 is %x\n",priv->initgain_backup.xaagccore1);
	RT_TRACE(COMP_DIG, "BBInitialGainBackup 0xc58 is %x\n",priv->initgain_backup.xbagccore1);
	RT_TRACE(COMP_DIG, "BBInitialGainBackup 0xc60 is %x\n",priv->initgain_backup.xcagccore1);
	RT_TRACE(COMP_DIG, "BBInitialGainBackup 0xc68 is %x\n",priv->initgain_backup.xdagccore1);
	RT_TRACE(COMP_DIG, "BBInitialGainBackup 0xa0a is %x\n",priv->initgain_backup.cca);

}   // dm_BBInitialGainBakcup

#endif
//-----------------------------------------------------------------------------
// Function:	dm_change_dynamic_initgain_thresh()
//
// Overview:
//
// Input:		NONE
//
// Output:		NONE
//
// Return:		NONE
//
// Revised History:
//	When		Who		Remark
//	05/29/2008	amy		Create Version 0 porting from windows code.
//
//---------------------------------------------------------------------------*/
extern void dm_change_dynamic_initgain_thresh(struct net_device *dev,
								u32		dm_type,
								u32		dm_value)
{
#ifdef RTL8192SE
	struct r8192_priv *priv = rtllib_priv(dev);
	if(dm_type == DIG_TYPE_THRESH_HIGHPWR_HIGH)
		priv->MidHighPwrTHR_L2 = (u8)dm_value;
	else if(dm_type == DIG_TYPE_THRESH_HIGHPWR_LOW)
		priv->MidHighPwrTHR_L1 = (u8)dm_value;
	return;
#endif
	if (dm_type == DIG_TYPE_THRESH_HIGH)
	{
		dm_digtable.rssi_high_thresh = dm_value;
	}
	else if (dm_type == DIG_TYPE_THRESH_LOW)
	{
		dm_digtable.rssi_low_thresh = dm_value;
	}
	else if (dm_type == DIG_TYPE_THRESH_HIGHPWR_HIGH)
	{
		dm_digtable.rssi_high_power_highthresh = dm_value;
	}
	else if (dm_type == DIG_TYPE_THRESH_HIGHPWR_HIGH)
	{
		dm_digtable.rssi_high_power_highthresh = dm_value;
	}
	else if (dm_type == DIG_TYPE_ENABLE)
	{
		dm_digtable.dig_state		= DM_STA_DIG_MAX;
		dm_digtable.dig_enable_flag	= true;
	}
	else if (dm_type == DIG_TYPE_DISABLE)
	{
		dm_digtable.dig_state		= DM_STA_DIG_MAX;
		dm_digtable.dig_enable_flag	= false;
	}
	else if (dm_type == DIG_TYPE_DBG_MODE)
	{
		if(dm_value >= DM_DBG_MAX)
			dm_value = DM_DBG_OFF;
		dm_digtable.dbg_mode		= (u8)dm_value;
	}
	else if (dm_type == DIG_TYPE_RSSI)
	{
		if(dm_value > 100)
			dm_value = 30;
		dm_digtable.rssi_val			= (long)dm_value;
	}
	else if (dm_type == DIG_TYPE_ALGORITHM)
	{
		if (dm_value >= DIG_ALGO_MAX)
			dm_value = DIG_ALGO_BY_FALSE_ALARM;
		if(dm_digtable.dig_algorithm != (u8)dm_value)
			dm_digtable.dig_algorithm_switch = 1;
		dm_digtable.dig_algorithm	= (u8)dm_value;
	}
	else if (dm_type == DIG_TYPE_BACKOFF)
	{
		if(dm_value > 30)
			dm_value = 30;
		dm_digtable.backoff_val		= (u8)dm_value;
	}
	else if(dm_type == DIG_TYPE_RX_GAIN_MIN)
	{
		if(dm_value == 0)
			dm_value = 0x1;
		dm_digtable.rx_gain_range_min = (u8)dm_value;
	}
	else if(dm_type == DIG_TYPE_RX_GAIN_MAX)
	{
		if(dm_value > 0x50)
			dm_value = 0x50;
		dm_digtable.rx_gain_range_max = (u8)dm_value;
	}
}	// DM_ChangeDynamicInitGainThresh */
extern	void
dm_change_fsync_setting(
	struct net_device *dev,
	s32		DM_Type,
	s32		DM_Value)
{
	struct r8192_priv *priv = rtllib_priv(dev);

	if (DM_Type == 0)	// monitor 0xc38 register
	{
		if(DM_Value > 1)
			DM_Value = 1;
		priv->framesyncMonitor = (u8)DM_Value;
		//DbgPrint("pHalData->framesyncMonitor = %d", pHalData->framesyncMonitor);
	}
}

extern void
dm_change_rxpath_selection_setting(
	struct net_device *dev,
	s32		DM_Type,
	s32		DM_Value)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	prate_adaptive	pRA = (prate_adaptive)&(priv->rate_adaptive);


	if(DM_Type == 0)
	{
		if(DM_Value > 1)
			DM_Value = 1;
		DM_RxPathSelTable.Enable = (u8)DM_Value;
	}
	else if(DM_Type == 1)
	{
		if(DM_Value > 1)
			DM_Value = 1;
		DM_RxPathSelTable.DbgMode = (u8)DM_Value;
	}
	else if(DM_Type == 2)
	{
		if(DM_Value > 40)
			DM_Value = 40;
		DM_RxPathSelTable.SS_TH_low = (u8)DM_Value;
	}
	else if(DM_Type == 3)
	{
		if(DM_Value > 25)
			DM_Value = 25;
		DM_RxPathSelTable.diff_TH = (u8)DM_Value;
	}
	else if(DM_Type == 4)
	{
		if(DM_Value >= CCK_Rx_Version_MAX)
			DM_Value = CCK_Rx_Version_1;
		DM_RxPathSelTable.cck_method= (u8)DM_Value;
	}
	else if(DM_Type == 10)
	{
		if(DM_Value > 100)
			DM_Value = 50;
		DM_RxPathSelTable.rf_rssi[0] = (u8)DM_Value;
	}
	else if(DM_Type == 11)
	{
		if(DM_Value > 100)
			DM_Value = 50;
		DM_RxPathSelTable.rf_rssi[1] = (u8)DM_Value;
	}
	else if(DM_Type == 12)
	{
		if(DM_Value > 100)
			DM_Value = 50;
		DM_RxPathSelTable.rf_rssi[2] = (u8)DM_Value;
	}
	else if(DM_Type == 13)
	{
		if(DM_Value > 100)
			DM_Value = 50;
		DM_RxPathSelTable.rf_rssi[3] = (u8)DM_Value;
	}
	else if(DM_Type == 20)
	{
		if(DM_Value > 1)
			DM_Value = 1;
		pRA->ping_rssi_enable = (u8)DM_Value;
	}
	else if(DM_Type == 21)
	{
		if(DM_Value > 30)
			DM_Value = 30;
		pRA->ping_rssi_thresh_for_ra = DM_Value;
	}
}

#if 0
extern void dm_force_tx_fw_info(struct net_device *dev,
										u32		force_type,
										u32		force_value)
{
	struct r8192_priv *priv = rtllib_priv(dev);

	if (force_type == 0)	// don't force TxSC
	{
		//DbgPrint("Set Force SubCarrier Off\n");
		priv->tx_fwinfo_force_subcarriermode = 0;
	}
	else if(force_type == 1) //force
	{
		//DbgPrint("Set Force SubCarrier On\n");
		priv->tx_fwinfo_force_subcarriermode = 1;
		if(force_value > 3)
			force_value = 3;
		priv->tx_fwinfo_force_subcarrierval = (u8)force_value;
	}
}
#endif

//-----------------------------------------------------------------------------
// Function:	dm_dig_init()
//
// Overview:	Set DIG scheme init value.
//
// Input:		NONE
//
// Output:		NONE
//
// Return:		NONE
//
// Revised History:
//	When		Who		Remark
//	05/15/2008	amy		Create Version 0 porting from windows code.
//
//---------------------------------------------------------------------------*/
static void dm_dig_init(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	// 2007/10/05 MH Disable DIG scheme now. Not tested. */
	dm_digtable.dig_enable_flag	= true;
	dm_digtable.Backoff_Enable_Flag = true;

#ifdef RTL8192SE
	if((priv->DM_Type  == DM_Type_ByDriver) && (priv->pFirmware->FirmwareVersion >= 0x3c))
		dm_digtable.dig_algorithm = DIG_ALGO_BY_TOW_PORT;
	else
		dm_digtable.dig_algorithm = DIG_ALGO_BEFORE_CONNECT_BY_RSSI_AND_ALARM;
#else
	dm_digtable.dig_algorithm = DIG_ALGO_BY_RSSI;
#endif

	dm_digtable.Dig_TwoPort_Algorithm = DIG_TWO_PORT_ALGO_RSSI;
	dm_digtable.Dig_Ext_Port_Stage = DIG_EXT_PORT_STAGE_MAX;
	dm_digtable.dbg_mode = DM_DBG_OFF;	//off=by real rssi value, on=by DM_DigTable.Rssi_val for new dig
	dm_digtable.dig_algorithm_switch = 0;

	// 2007/10/04 MH Define init gain threshol. */
	dm_digtable.dig_state		= DM_STA_DIG_MAX;
	dm_digtable.dig_highpwr_state	= DM_STA_DIG_MAX;
	dm_digtable.CurSTAConnectState = dm_digtable.PreSTAConnectState = DIG_STA_DISCONNECT;
	dm_digtable.CurAPConnectState = dm_digtable.PreAPConnectState = DIG_AP_DISCONNECT;
	dm_digtable.initialgain_lowerbound_state = false;

	dm_digtable.rssi_low_thresh	= DM_DIG_THRESH_LOW;
	dm_digtable.rssi_high_thresh	= DM_DIG_THRESH_HIGH;

	dm_digtable.FALowThresh	= DM_FALSEALARM_THRESH_LOW;
	dm_digtable.FAHighThresh	= DM_FALSEALARM_THRESH_HIGH;

	dm_digtable.rssi_high_power_lowthresh = DM_DIG_HIGH_PWR_THRESH_LOW;
	dm_digtable.rssi_high_power_highthresh = DM_DIG_HIGH_PWR_THRESH_HIGH;

	dm_digtable.rssi_val = 50;	//for new dig debug rssi value
	dm_digtable.backoff_val = DM_DIG_BACKOFF;
	dm_digtable.rx_gain_range_max = DM_DIG_MAX;
	if(priv->CustomerID == RT_CID_819x_Netcore)
		dm_digtable.rx_gain_range_min = DM_DIG_MIN_Netcore;
	else
		dm_digtable.rx_gain_range_min = DM_DIG_MIN;

	dm_digtable.BackoffVal_range_max = DM_DIG_BACKOFF_MAX;
	dm_digtable.BackoffVal_range_min = DM_DIG_BACKOFF_MIN;
}	// dm_dig_init */

void dm_FalseAlarmCounterStatistics(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	u32 ret_value;
	PFALSE_ALARM_STATISTICS FalseAlmCnt = &(priv->FalseAlmCnt);

	ret_value = rtl8192_QueryBBReg(dev, rOFDM_PHYCounter1, bMaskDWord);
        FalseAlmCnt->Cnt_Parity_Fail = ((ret_value&0xffff0000)>>16);

        ret_value = rtl8192_QueryBBReg(dev, rOFDM_PHYCounter2, bMaskDWord);
	FalseAlmCnt->Cnt_Rate_Illegal = (ret_value&0xffff);
	FalseAlmCnt->Cnt_Crc8_fail = ((ret_value&0xffff0000)>>16);
	ret_value = rtl8192_QueryBBReg(dev, rOFDM_PHYCounter3, bMaskDWord);
	FalseAlmCnt->Cnt_Mcs_fail = (ret_value&0xffff);

	FalseAlmCnt->Cnt_Ofdm_fail = FalseAlmCnt->Cnt_Parity_Fail + FalseAlmCnt->Cnt_Rate_Illegal +
							  FalseAlmCnt->Cnt_Crc8_fail + FalseAlmCnt->Cnt_Mcs_fail;

	ret_value = rtl8192_QueryBBReg(dev, 0xc64, bMaskDWord);	// read CCK false alarm
	FalseAlmCnt->Cnt_Cck_fail = (ret_value&0xffff);
	FalseAlmCnt->Cnt_all = (FalseAlmCnt->Cnt_Parity_Fail +
						FalseAlmCnt->Cnt_Rate_Illegal +
						FalseAlmCnt->Cnt_Crc8_fail +
						FalseAlmCnt->Cnt_Mcs_fail +
						FalseAlmCnt->Cnt_Cck_fail);

	RT_TRACE(COMP_DIG, "Cnt_Ofdm_fail = %d, Cnt_Cck_fail = %d, Cnt_all = %d\n",
				FalseAlmCnt->Cnt_Ofdm_fail, FalseAlmCnt->Cnt_Cck_fail , FalseAlmCnt->Cnt_all);
}

#ifdef RTL8192SE
static void dm_CtrlInitGainAPByFalseAlarm(struct net_device *dev)
{
	static u8		binitialized = false;

	//if(	(!ACTING_AS_AP(pTmpAdapter))
	//	|| (DM_DigTable.CurSTAConnectState != DIG_STA_DISCONNECT))
	{
		binitialized = false;
		dm_digtable.Dig_Ext_Port_Stage = DIG_EXT_PORT_STAGE_MAX;
		return;
	}
}
#endif

//-----------------------------------------------------------------------------
// Function:	dm_ctrl_initgain_byrssi()
//
// Overview:	Driver must monitor RSSI and notify firmware to change initial
//				gain according to different threshold. BB team provide the
//				suggested solution.
//
// Input:			struct net_device *dev
//
// Output:		NONE
//
// Return:		NONE
//
// Revised History:
//	When		Who		Remark
//	05/27/2008	amy		Create Version 0 porting from windows code.
//---------------------------------------------------------------------------*/
static void dm_ctrl_initgain_byrssi(struct net_device *dev)
{

	if (dm_digtable.dig_enable_flag == false)
		return;

	if(dm_digtable.dig_algorithm == DIG_ALGO_BY_FALSE_ALARM)
		dm_ctrl_initgain_byrssi_by_fwfalse_alarm(dev);
	else if(dm_digtable.dig_algorithm == DIG_ALGO_BY_RSSI)
		dm_ctrl_initgain_byrssi_by_driverrssi(dev);
#ifdef RTL8192SE
	else if(dm_digtable.dig_algorithm == DIG_ALGO_BEFORE_CONNECT_BY_RSSI_AND_ALARM)
		dm_CtrlInitGainBeforeConnectByRssiAndFalseAlarm(dev);
	else if(dm_digtable.dig_algorithm == DIG_ALGO_BY_TOW_PORT)
		dm_CtrlInitGainByTwoPort(dev);
#endif
	else
		return;
}

#ifdef RTL8192SE
static void dm_CtrlInitGainByTwoPort(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);

	if(priv->rtllib->be_scan_inprogress == true)
		return;

	// Decide the current status and if modify initial gain or not
	//if(pMgntInfo->bJoinInProgress)
	if((priv->rtllib->state > RTLLIB_NOLINK) && (priv->rtllib->state < RTLLIB_LINKED))
		dm_digtable.CurSTAConnectState = DIG_STA_BEFORE_CONNECT;
	//else if(pMgntInfo->mAssoc || pMgntInfo->mIbss)
	else if((priv->rtllib->state == RTLLIB_LINKED) ||(priv->rtllib->state == RTLLIB_LINKED_SCANNING))
		dm_digtable.CurSTAConnectState = DIG_STA_CONNECT;
	else
		dm_digtable.CurSTAConnectState = DIG_STA_DISCONNECT;

	dm_digtable.rssi_val = priv->undecorated_smoothed_pwdb;

	// Change dig mode to rssi
	if(dm_digtable.CurSTAConnectState != DIG_STA_DISCONNECT)
	{
		if(dm_digtable.Dig_TwoPort_Algorithm == DIG_TWO_PORT_ALGO_FALSE_ALARM)
		{
			dm_digtable.Dig_TwoPort_Algorithm = DIG_TWO_PORT_ALGO_RSSI;
			priv->rtllib->SetFwCmdHandler(dev, FW_CMD_DIG_MODE_SS);
		}
	}

	dm_FalseAlarmCounterStatistics(dev);
	dm_initial_gain_STABeforeConnect(dev);
	dm_CtrlInitGainAPByFalseAlarm(dev);

	dm_digtable.PreSTAConnectState = dm_digtable.CurSTAConnectState;
}
#endif

/*-----------------------------------------------------------------------------
 * Function:	dm_CtrlInitGainBeforeConnectByRssiAndFalseAlarm()
 *
 * Overview:	Driver monitor RSSI and False Alarm to change initial gain.
			Only change initial gain during link in progress.
 *
 * Input:		IN	PADAPTER	pAdapter
 *
 * Output:		NONE
 *
 * Return:		NONE
 *
 * Revised History:
 *	When		Who		Remark
 *	03/04/2009	hpfan	Create Version 0.
 *
 *---------------------------------------------------------------------------*/

#ifdef RTL8192SE
static void dm_CtrlInitGainBeforeConnectByRssiAndFalseAlarm(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);

	if(priv->rtllib->be_scan_inprogress == true)
		return;

	// Decide the current status and if modify initial gain or not
	if((priv->rtllib->state > RTLLIB_NOLINK) && (priv->rtllib->state < RTLLIB_LINKED))
		dm_digtable.CurSTAConnectState = DIG_STA_BEFORE_CONNECT;
	else if((priv->rtllib->state == RTLLIB_LINKED) ||(priv->rtllib->state == RTLLIB_LINKED_SCANNING))
		dm_digtable.CurSTAConnectState = DIG_STA_CONNECT;
	else
		dm_digtable.CurSTAConnectState = DIG_STA_DISCONNECT;

	if(dm_digtable.dbg_mode == DM_DBG_OFF)
		dm_digtable.rssi_val = priv->undecorated_smoothed_pwdb;

	dm_FalseAlarmCounterStatistics(dev);
	dm_initial_gain_STABeforeConnect(dev);
	dm_digtable.PreSTAConnectState = dm_digtable.CurSTAConnectState;

}
#endif
static void dm_ctrl_initgain_byrssi_by_driverrssi(
	struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	u8 i;
	static u8	fw_dig=0;

	if (dm_digtable.dig_enable_flag == false)
		return;

	//DbgPrint("Dig by Sw Rssi \n");
	if(dm_digtable.dig_algorithm_switch)	// if swithed algorithm, we have to disable FW Dig.
		fw_dig = 0;
	if(fw_dig <= 3)	// execute several times to make sure the FW Dig is disabled
	{// FW DIG Off
		for(i=0; i<3; i++)
			rtl8192_setBBreg(dev, UFWP, bMaskByte1, 0x8);	// Only clear byte 1 and rewrite.
		fw_dig++;
		dm_digtable.dig_state = DM_STA_DIG_OFF;	//fw dig off.
	}

	if(priv->rtllib->state == RTLLIB_LINKED)
		dm_digtable.CurSTAConnectState = DIG_STA_CONNECT;
	else
		dm_digtable.CurSTAConnectState = DIG_STA_DISCONNECT;

	//DbgPrint("DM_DigTable.PreConnectState = %d, DM_DigTable.CurConnectState = %d \n",
		//DM_DigTable.PreConnectState, DM_DigTable.CurConnectState);

	if(dm_digtable.dbg_mode == DM_DBG_OFF)
		dm_digtable.rssi_val = priv->undecorated_smoothed_pwdb;
	//DbgPrint("DM_DigTable.Rssi_val = %d \n", DM_DigTable.Rssi_val);
	dm_initial_gain(dev);
	dm_pd_th(dev);
	dm_cs_ratio(dev);
	if(dm_digtable.dig_algorithm_switch)
		dm_digtable.dig_algorithm_switch = 0;
	dm_digtable.PreSTAConnectState = dm_digtable.CurSTAConnectState;

}	// dm_CtrlInitGainByRssi */

static void dm_ctrl_initgain_byrssi_by_fwfalse_alarm(
	struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	static u32 reset_cnt = 0;
	u8 i;

	if (dm_digtable.dig_enable_flag == false)
		return;

	if(dm_digtable.dig_algorithm_switch)
	{
		dm_digtable.dig_state = DM_STA_DIG_MAX;
		// Fw DIG On.
		for(i=0; i<3; i++)
			rtl8192_setBBreg(dev, UFWP, bMaskByte1, 0x1);	// Only clear byte 1 and rewrite.
		dm_digtable.dig_algorithm_switch = 0;
	}

	if (priv->rtllib->state != RTLLIB_LINKED)
		return;

	// For smooth, we can not change DIG state.
	if ((priv->undecorated_smoothed_pwdb > dm_digtable.rssi_low_thresh) &&
		(priv->undecorated_smoothed_pwdb < dm_digtable.rssi_high_thresh))
	{
		return;
	}
	//DbgPrint("Dig by Fw False Alarm\n");
	//if (DM_DigTable.Dig_State == DM_STA_DIG_OFF)
	//DbgPrint("DIG Check\n\r RSSI=%d LOW=%d HIGH=%d STATE=%d",
	//pHalData->UndecoratedSmoothedPWDB, DM_DigTable.RssiLowThresh,
	//DM_DigTable.RssiHighThresh, DM_DigTable.Dig_State);*/
	// 1. When RSSI decrease, We have to judge if it is smaller than a treshold
	//	  and then execute below step. */
	if ((priv->undecorated_smoothed_pwdb <= dm_digtable.rssi_low_thresh))
	{
		// 2008/02/05 MH When we execute silent reset, the DIG PHY parameters
		//  will be reset to init value. We must prevent the condition. */
		if (dm_digtable.dig_state == DM_STA_DIG_OFF &&
			(priv->reset_count == reset_cnt))
		{
			return;
		}
		else
		{
			reset_cnt = priv->reset_count;
		}

		// If DIG is off, DIG high power state must reset.
		dm_digtable.dig_highpwr_state = DM_STA_DIG_MAX;
		dm_digtable.dig_state = DM_STA_DIG_OFF;

		// 1.1 DIG Off.
		rtl8192_setBBreg(dev, UFWP, bMaskByte1, 0x8);	// Only clear byte 1 and rewrite.

		// 1.2 Set initial gain.
		write_nic_byte(dev, rOFDM0_XAAGCCore1, 0x17);
		write_nic_byte(dev, rOFDM0_XBAGCCore1, 0x17);
		write_nic_byte(dev, rOFDM0_XCAGCCore1, 0x17);
		write_nic_byte(dev, rOFDM0_XDAGCCore1, 0x17);

		// 1.3 Lower PD_TH for OFDM.
		if (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20)
		{
			// 2008/01/11 MH 40MHZ 90/92 register are not the same. */
			// 2008/02/05 MH SD3-Jerry 92U/92E PD_TH are the same.
			#ifdef RTL8190P
			write_nic_byte(dev, rOFDM0_RxDetector1, 0x40);
			#else
				write_nic_byte(dev, (rOFDM0_XATxAFE+3), 0x00);
				#endif
			/*else if (priv->card_8192 == HARDWARE_TYPE_RTL8190P)
				write_nic_byte(pAdapter, rOFDM0_RxDetector1, 0x40);
			*/
			//else if (pAdapter->HardwareType == HARDWARE_TYPE_RTL8192E)


			//else
				//write_nic_byte(pAdapter, rOFDM0_RxDetector1, 0x40);
		}
		else
			write_nic_byte(dev, rOFDM0_RxDetector1, 0x42);

		// 1.4 Lower CS ratio for CCK.
		write_nic_byte(dev, 0xa0a, 0x08);

		// 1.5 Higher EDCCA.
		//PlatformEFIOWrite4Byte(pAdapter, rOFDM0_ECCAThreshold, 0x325);
		return;

	}

	// 2. When RSSI increase, We have to judge if it is larger than a treshold
	//	  and then execute below step.  */
	if ((priv->undecorated_smoothed_pwdb >= dm_digtable.rssi_high_thresh) )
	{
		u8 reset_flag = 0;

		if (dm_digtable.dig_state == DM_STA_DIG_ON &&
			(priv->reset_count == reset_cnt))
		{
			dm_ctrl_initgain_byrssi_highpwr(dev);
			return;
		}
		else
		{
			if (priv->reset_count != reset_cnt)
				reset_flag = 1;

			reset_cnt = priv->reset_count;
		}

		dm_digtable.dig_state = DM_STA_DIG_ON;
		//DbgPrint("DIG ON\n\r");

		// 2.1 Set initial gain.
		// 2008/02/26 MH SD3-Jerry suggest to prevent dirty environment.
		if (reset_flag == 1)
		{
			write_nic_byte(dev, rOFDM0_XAAGCCore1, 0x2c);
			write_nic_byte(dev, rOFDM0_XBAGCCore1, 0x2c);
			write_nic_byte(dev, rOFDM0_XCAGCCore1, 0x2c);
			write_nic_byte(dev, rOFDM0_XDAGCCore1, 0x2c);
		}
		else
		{
		write_nic_byte(dev, rOFDM0_XAAGCCore1, 0x20);
		write_nic_byte(dev, rOFDM0_XBAGCCore1, 0x20);
		write_nic_byte(dev, rOFDM0_XCAGCCore1, 0x20);
		write_nic_byte(dev, rOFDM0_XDAGCCore1, 0x20);
		}

		// 2.2 Higher PD_TH for OFDM.
		if (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20)
		{
			// 2008/01/11 MH 40MHZ 90/92 register are not the same. */
			// 2008/02/05 MH SD3-Jerry 92U/92E PD_TH are the same.
			#ifdef RTL8190P
			write_nic_byte(dev, rOFDM0_RxDetector1, 0x42);
			#else
				write_nic_byte(dev, (rOFDM0_XATxAFE+3), 0x20);
				#endif
			/*
			else if (priv->card_8192 == HARDWARE_TYPE_RTL8190P)
				write_nic_byte(dev, rOFDM0_RxDetector1, 0x42);
			*/
			//else if (pAdapter->HardwareType == HARDWARE_TYPE_RTL8192E)

			//else
				//write_nic_byte(pAdapter, rOFDM0_RxDetector1, 0x42);
		}
		else
			write_nic_byte(dev, rOFDM0_RxDetector1, 0x44);

		// 2.3 Higher CS ratio for CCK.
		write_nic_byte(dev, 0xa0a, 0xcd);

		// 2.4 Lower EDCCA.
		// 2008/01/11 MH 90/92 series are the same. */
		//PlatformEFIOWrite4Byte(pAdapter, rOFDM0_ECCAThreshold, 0x346);

		// 2.5 DIG On.
		rtl8192_setBBreg(dev, UFWP, bMaskByte1, 0x1);	// Only clear byte 1 and rewrite.

	}

	dm_ctrl_initgain_byrssi_highpwr(dev);

}	// dm_CtrlInitGainByRssi */


//-----------------------------------------------------------------------------
// Function:	dm_ctrl_initgain_byrssi_highpwr()
//
// Overview:
//
// Input:		NONE
//
// Output:		NONE
//
// Return:		NONE
//
// Revised History:
//	When		Who		Remark
//	05/28/2008	amy		Create Version 0 porting from windows code.
//
//---------------------------------------------------------------------------*/
static void dm_ctrl_initgain_byrssi_highpwr(
	struct net_device * dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	static u32 reset_cnt_highpwr = 0;

	// For smooth, we can not change high power DIG state in the range.
	if ((priv->undecorated_smoothed_pwdb > dm_digtable.rssi_high_power_lowthresh) &&
		(priv->undecorated_smoothed_pwdb < dm_digtable.rssi_high_power_highthresh))
	{
		return;
	}

	// 3. When RSSI >75% or <70%, it is a high power issue. We have to judge if
	//	  it is larger than a treshold and then execute below step.  */
	// 2008/02/05 MH SD3-Jerry Modify PD_TH for high power issue.
	if (priv->undecorated_smoothed_pwdb >= dm_digtable.rssi_high_power_highthresh)
	{
		if (dm_digtable.dig_highpwr_state == DM_STA_DIG_ON &&
			(priv->reset_count == reset_cnt_highpwr))
			return;
		else
			dm_digtable.dig_highpwr_state = DM_STA_DIG_ON;

		// 3.1 Higher PD_TH for OFDM for high power state.
		if (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20)
		{
			#ifdef RTL8190P
			write_nic_byte(dev, rOFDM0_RxDetector1, 0x41);
			#else
				write_nic_byte(dev, (rOFDM0_XATxAFE+3), 0x10);
				#endif

			/*else if (priv->card_8192 == HARDWARE_TYPE_RTL8190P)
				write_nic_byte(dev, rOFDM0_RxDetector1, 0x41);
			*/

		}
		else
			write_nic_byte(dev, rOFDM0_RxDetector1, 0x43);
	}
	else
	{
		if (dm_digtable.dig_highpwr_state == DM_STA_DIG_OFF&&
			(priv->reset_count == reset_cnt_highpwr))
			return;
		else
			dm_digtable.dig_highpwr_state = DM_STA_DIG_OFF;

		if (priv->undecorated_smoothed_pwdb < dm_digtable.rssi_high_power_lowthresh &&
			 priv->undecorated_smoothed_pwdb >= dm_digtable.rssi_high_thresh)
		{
			// 3.2 Recover PD_TH for OFDM for normal power region.
			if (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20)
			{
				#ifdef RTL8190P
				write_nic_byte(dev, rOFDM0_RxDetector1, 0x42);
				#else
					write_nic_byte(dev, (rOFDM0_XATxAFE+3), 0x20);
					#endif
				/*else if (priv->card_8192 == HARDWARE_TYPE_RTL8190P)
					write_nic_byte(dev, rOFDM0_RxDetector1, 0x42);
				*/

			}
			else
				write_nic_byte(dev, rOFDM0_RxDetector1, 0x44);
		}
	}

	reset_cnt_highpwr = priv->reset_count;

}	// dm_CtrlInitGainByRssiHighPwr */


static void dm_initial_gain(
	struct net_device * dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	u8					initial_gain=0;
	static u8				initialized=0, force_write=0;
	static u32			reset_cnt=0;

	if(dm_digtable.dig_algorithm_switch)
	{
		initialized = 0;
		reset_cnt = 0;
	}

	if(priv->rtllib->be_scan_inprogress == true)
	{
		force_write = 1;
		return;
	}

	if(dm_digtable.PreSTAConnectState == dm_digtable.CurSTAConnectState)
	{
		if(dm_digtable.CurSTAConnectState == DIG_STA_CONNECT)
		{
			if((dm_digtable.rssi_val+10-dm_digtable.backoff_val) > dm_digtable.rx_gain_range_max)
				dm_digtable.cur_ig_value = dm_digtable.rx_gain_range_max;
			else if((dm_digtable.rssi_val+10-dm_digtable.backoff_val) < dm_digtable.rx_gain_range_min)
				dm_digtable.cur_ig_value = dm_digtable.rx_gain_range_min;
			else
				dm_digtable.cur_ig_value = dm_digtable.rssi_val+10-dm_digtable.backoff_val;
		}
		else		//current state is disconnected
		{
			if(dm_digtable.cur_ig_value == 0)
				dm_digtable.cur_ig_value = priv->DefaultInitialGain[0];
			else
				dm_digtable.cur_ig_value = dm_digtable.pre_ig_value;
		}
	}
	else	// disconnected -> connected or connected -> disconnected
	{
		dm_digtable.cur_ig_value = priv->DefaultInitialGain[0];
		dm_digtable.pre_ig_value = 0;
	}
	//DbgPrint("DM_DigTable.CurIGValue = 0x%x, DM_DigTable.PreIGValue = 0x%x\n", DM_DigTable.CurIGValue, DM_DigTable.PreIGValue);

	// if silent reset happened, we should rewrite the values back
	if(priv->reset_count != reset_cnt)
	{
		force_write = 1;
		reset_cnt = priv->reset_count;
	}

	if(dm_digtable.pre_ig_value != read_nic_byte(dev, rOFDM0_XAAGCCore1))
		force_write = 1;

	{
		if((dm_digtable.pre_ig_value != dm_digtable.cur_ig_value)
			|| !initialized || force_write)
		{
			initial_gain = (u8)dm_digtable.cur_ig_value;
			//DbgPrint("Write initial gain = 0x%x\n", initial_gain);
			// Set initial gain.
			write_nic_byte(dev, rOFDM0_XAAGCCore1, initial_gain);
			write_nic_byte(dev, rOFDM0_XBAGCCore1, initial_gain);
			write_nic_byte(dev, rOFDM0_XCAGCCore1, initial_gain);
			write_nic_byte(dev, rOFDM0_XDAGCCore1, initial_gain);
			dm_digtable.pre_ig_value = dm_digtable.cur_ig_value;
			initialized = 1;
			force_write = 0;
		}
	}
}

void dm_initial_gain_STABeforeConnect(
	struct net_device * dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	u8			initial_gain=0;
	static u8		initialized=0, force_write=0;
	//u8			ret_value;

	RT_TRACE(COMP_DIG, "PreSTAConnectState = %x, CurSTAConnectState = %x\n",
				dm_digtable.PreSTAConnectState, dm_digtable.CurSTAConnectState);


	if((dm_digtable.PreSTAConnectState == dm_digtable.CurSTAConnectState) ||
		(dm_digtable.CurSTAConnectState == DIG_STA_BEFORE_CONNECT))
	{
		// beforeconnect -> beforeconnect or  (dis)connect -> beforeconnect
		if(dm_digtable.CurSTAConnectState == DIG_STA_BEFORE_CONNECT)
		{
			if(priv->rtllib->eRFPowerState != eRfOn)
				return;

			if(dm_digtable.Backoff_Enable_Flag == true)
			{
				if(priv->FalseAlmCnt.Cnt_all > dm_digtable.FAHighThresh)
				{
					if((dm_digtable.backoff_val -6) < dm_digtable.BackoffVal_range_min)
						dm_digtable.backoff_val = dm_digtable.BackoffVal_range_min;
					else
						dm_digtable.backoff_val -= 6;
				}
				else if(priv->FalseAlmCnt.Cnt_all < dm_digtable.FALowThresh)
				{
					if((dm_digtable.backoff_val+6) > dm_digtable.BackoffVal_range_max)
						dm_digtable.backoff_val = dm_digtable.BackoffVal_range_max;
					else
						dm_digtable.backoff_val +=6;
				}
			}
			else
				dm_digtable.backoff_val =DM_DIG_BACKOFF;

			if((dm_digtable.rssi_val+10-dm_digtable.backoff_val) > dm_digtable.rx_gain_range_max)
				dm_digtable.cur_ig_value = dm_digtable.rx_gain_range_max;
			else if((dm_digtable.rssi_val+10-dm_digtable.backoff_val) < dm_digtable.rx_gain_range_min)
				dm_digtable.cur_ig_value = dm_digtable.rx_gain_range_min;
			else
				dm_digtable.cur_ig_value = dm_digtable.rssi_val+10-dm_digtable.backoff_val;

			if(priv->FalseAlmCnt.Cnt_all > 10000)
				dm_digtable.cur_ig_value = (dm_digtable.cur_ig_value>0x33)?dm_digtable.cur_ig_value:0x33;

			if(priv->FalseAlmCnt.Cnt_all > 16000)
				dm_digtable.cur_ig_value = dm_digtable.rx_gain_range_max;

		}
		else // connected -> connected or disconnected -> disconnected
		{
			return;	// Firmware control DIG, do nothing in driver dm
		}
	}
	else	// disconnected -> connected or connected -> disconnected or beforeconnect->(dis)connected
	{
		// Enable FW DIG
		dm_digtable.Dig_Ext_Port_Stage = DIG_EXT_PORT_STAGE_MAX;
		priv->rtllib->SetFwCmdHandler(dev, FW_CMD_DIG_ENABLE);

		dm_digtable.backoff_val = DM_DIG_BACKOFF;
		dm_digtable.cur_ig_value = priv->DefaultInitialGain[0];
		dm_digtable.pre_ig_value = 0;
		return;
	}

	// Forced writing to prevent from fw-dig overwriting.
	if(dm_digtable.pre_ig_value != rtl8192_QueryBBReg(dev, rOFDM0_XAAGCCore1, bMaskByte0))
		force_write = 1;

	if((dm_digtable.pre_ig_value != dm_digtable.cur_ig_value) || !initialized || force_write)
	{
		// Disable FW DIG
		priv->rtllib->SetFwCmdHandler(dev, FW_CMD_DIG_DISABLE);

		initial_gain = (u8)dm_digtable.cur_ig_value;

		// Set initial gain.
		rtl8192_setBBreg(dev, rOFDM0_XAAGCCore1, bMaskByte0, initial_gain);
		rtl8192_setBBreg(dev, rOFDM0_XBAGCCore1, bMaskByte0, initial_gain);
		dm_digtable.pre_ig_value = dm_digtable.cur_ig_value;
		initialized = 1;
		force_write = 0;
	}

	RT_TRACE(COMP_DIG, "CurIGValue = 0x%x, pre_ig_value = 0x%x, backoff_val = %d\n",
				dm_digtable.cur_ig_value, dm_digtable.pre_ig_value, dm_digtable.backoff_val);

}

static void dm_pd_th(
	struct net_device * dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	static u8				initialized=0, force_write=0;
	static u32			reset_cnt = 0;

	if(dm_digtable.dig_algorithm_switch)
	{
		initialized = 0;
		reset_cnt = 0;
	}

	if(dm_digtable.PreSTAConnectState == dm_digtable.CurSTAConnectState)
	{
		if(dm_digtable.CurSTAConnectState == DIG_STA_CONNECT)
		{
			if (dm_digtable.rssi_val >= dm_digtable.rssi_high_power_highthresh)
				dm_digtable.curpd_thstate = DIG_PD_AT_HIGH_POWER;
			else if ((dm_digtable.rssi_val <= dm_digtable.rssi_low_thresh))
				dm_digtable.curpd_thstate = DIG_PD_AT_LOW_POWER;
			else if ((dm_digtable.rssi_val >= dm_digtable.rssi_high_thresh) &&
					(dm_digtable.rssi_val < dm_digtable.rssi_high_power_lowthresh))
				dm_digtable.curpd_thstate = DIG_PD_AT_NORMAL_POWER;
			else
				dm_digtable.curpd_thstate = dm_digtable.prepd_thstate;
		}
		else
		{
			dm_digtable.curpd_thstate = DIG_PD_AT_LOW_POWER;
		}
	}
	else	// disconnected -> connected or connected -> disconnected
	{
		dm_digtable.curpd_thstate = DIG_PD_AT_LOW_POWER;
	}

	// if silent reset happened, we should rewrite the values back
	if(priv->reset_count != reset_cnt)
	{
		force_write = 1;
		reset_cnt = priv->reset_count;
	}

	{
		if((dm_digtable.prepd_thstate != dm_digtable.curpd_thstate) ||
			(initialized<=3) || force_write)
		{
			//DbgPrint("Write PD_TH state = %d\n", DM_DigTable.CurPD_THState);
			if(dm_digtable.curpd_thstate == DIG_PD_AT_LOW_POWER)
			{
				// Lower PD_TH for OFDM.
				if (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20)
				{
					// 2008/01/11 MH 40MHZ 90/92 register are not the same. */
					// 2008/02/05 MH SD3-Jerry 92U/92E PD_TH are the same.
					#ifdef RTL8190P
					write_nic_byte(dev, rOFDM0_RxDetector1, 0x40);
					#else
						write_nic_byte(dev, (rOFDM0_XATxAFE+3), 0x00);
						#endif
					/*else if (priv->card_8192 == HARDWARE_TYPE_RTL8190P)
						write_nic_byte(dev, rOFDM0_RxDetector1, 0x40);
					*/
				}
				else
					write_nic_byte(dev, rOFDM0_RxDetector1, 0x42);
			}
			else if(dm_digtable.curpd_thstate == DIG_PD_AT_NORMAL_POWER)
			{
				// Higher PD_TH for OFDM.
				if (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20)
				{
					// 2008/01/11 MH 40MHZ 90/92 register are not the same. */
					// 2008/02/05 MH SD3-Jerry 92U/92E PD_TH are the same.
					#ifdef RTL8190P
					write_nic_byte(dev, rOFDM0_RxDetector1, 0x42);
					#else
						write_nic_byte(dev, (rOFDM0_XATxAFE+3), 0x20);
						#endif
					/*else if (priv->card_8192 == HARDWARE_TYPE_RTL8190P)
						write_nic_byte(dev, rOFDM0_RxDetector1, 0x42);
					*/
				}
				else
					write_nic_byte(dev, rOFDM0_RxDetector1, 0x44);
			}
			else if(dm_digtable.curpd_thstate == DIG_PD_AT_HIGH_POWER)
			{
				// Higher PD_TH for OFDM for high power state.
				if (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20)
				{
					#ifdef RTL8190P
					write_nic_byte(dev, rOFDM0_RxDetector1, 0x41);
					#else
						write_nic_byte(dev, (rOFDM0_XATxAFE+3), 0x10);
						#endif
					/*else if (priv->card_8192 == HARDWARE_TYPE_RTL8190P)
						write_nic_byte(dev, rOFDM0_RxDetector1, 0x41);
					*/
				}
				else
					write_nic_byte(dev, rOFDM0_RxDetector1, 0x43);
			}
			dm_digtable.prepd_thstate = dm_digtable.curpd_thstate;
			if(initialized <= 3)
				initialized++;
			force_write = 0;
		}
	}
}

static	void dm_cs_ratio(
	struct net_device * dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	static u8				initialized=0,force_write=0;
	static u32			reset_cnt = 0;

	if(dm_digtable.dig_algorithm_switch)
	{
		initialized = 0;
		reset_cnt = 0;
	}

	if(dm_digtable.PreSTAConnectState == dm_digtable.CurSTAConnectState)
	{
		if(dm_digtable.CurSTAConnectState == DIG_STA_CONNECT)
		{
			if ((dm_digtable.rssi_val <= dm_digtable.rssi_low_thresh))
				dm_digtable.curcs_ratio_state = DIG_CS_RATIO_LOWER;
			else if ((dm_digtable.rssi_val >= dm_digtable.rssi_high_thresh) )
				dm_digtable.curcs_ratio_state = DIG_CS_RATIO_HIGHER;
			else
				dm_digtable.curcs_ratio_state = dm_digtable.precs_ratio_state;
		}
		else
		{
			dm_digtable.curcs_ratio_state = DIG_CS_RATIO_LOWER;
		}
	}
	else	// disconnected -> connected or connected -> disconnected
	{
		dm_digtable.curcs_ratio_state = DIG_CS_RATIO_LOWER;
	}

	// if silent reset happened, we should rewrite the values back
	if(priv->reset_count != reset_cnt)
	{
		force_write = 1;
		reset_cnt = priv->reset_count;
	}


	{
		if((dm_digtable.precs_ratio_state != dm_digtable.curcs_ratio_state) ||
			!initialized || force_write)
		{
			//DbgPrint("Write CS_ratio state = %d\n", DM_DigTable.CurCS_ratioState);
			if(dm_digtable.curcs_ratio_state == DIG_CS_RATIO_LOWER)
			{
				// Lower CS ratio for CCK.
				write_nic_byte(dev, 0xa0a, 0x08);
			}
			else if(dm_digtable.curcs_ratio_state == DIG_CS_RATIO_HIGHER)
			{
				// Higher CS ratio for CCK.
				write_nic_byte(dev, 0xa0a, 0xcd);
			}
			dm_digtable.precs_ratio_state = dm_digtable.curcs_ratio_state;
			initialized = 1;
			force_write = 0;
		}
	}
}

extern void dm_init_edca_turbo(struct net_device * dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);

	priv->bcurrent_turbo_EDCA = false;
	priv->rtllib->bis_any_nonbepkts = false;
	priv->bis_cur_rdlstate = false;
}	// dm_init_edca_turbo

#if 1
static void dm_check_edca_turbo(
	struct net_device * dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	PRT_HIGH_THROUGHPUT	pHTInfo = priv->rtllib->pHTInfo;
	//PSTA_QOS			pStaQos = pMgntInfo->pStaQos;

	// Keep past Tx/Rx packet count for RT-to-RT EDCA turbo.
	static unsigned long			lastTxOkCnt = 0;
	static unsigned long			lastRxOkCnt = 0;
	unsigned long				curTxOkCnt = 0;
	unsigned long				curRxOkCnt = 0;

	if(priv->rtllib->iw_mode == IW_MODE_ADHOC)
	{
		goto dm_CheckEdcaTurbo_EXIT;
	}
	//
	// Do not be Turbo if it's under WiFi config and Qos Enabled, because the EDCA parameters
	// should follow the settings from QAP. By Bruce, 2007-12-07.
	//
	if(priv->rtllib->state != RTLLIB_LINKED)
	{
		goto dm_CheckEdcaTurbo_EXIT;
	}
	// We do not turn on EDCA turbo mode for some AP that has IOT issue
	if(priv->rtllib->pHTInfo->IOTAction & HT_IOT_ACT_DISABLE_EDCA_TURBO)
	{
	//	printk("%s(): IOTAction disable edca turbo\n",__FUNCTION__);
		goto dm_CheckEdcaTurbo_EXIT;
	}

	{
		u8* peername[11] = {"unknown", "realtek_90", "realtek_92se","broadcom", "ralink", "atheros", "cisco", "marvell", "92u_softap", "self_softap"};
		static int wb_tmp = 0;
		if (wb_tmp == 0){
			printk("========>%s():iot peer is %s, bssid:"MAC_FMT"\n",__FUNCTION__,peername[pHTInfo->IOTPeer], MAC_ARG(priv->rtllib->current_network.bssid));
			wb_tmp = 1;
		}
	}
	// Check the status for current condition.
	if(!priv->rtllib->bis_any_nonbepkts)
	{
		curTxOkCnt = priv->stats.txbytesunicast - lastTxOkCnt;
		curRxOkCnt = priv->stats.rxbytesunicast - lastRxOkCnt;
		//printk("%s:curTxOkCnt = %ld curRxOkCnt =%ld\n", __FUNCTION__, curTxOkCnt, curRxOkCnt);
		// Modify EDCA parameters selection bias
		// For some APs, use downlink EDCA parameters for uplink+downlink
		if(pHTInfo->IOTAction & HT_IOT_ACT_EDCA_BIAS_ON_RX)
		{
			if(curTxOkCnt > 4*curRxOkCnt)
			{// Uplink TP is present.
				if(priv->bis_cur_rdlstate || !priv->bcurrent_turbo_EDCA)
				{
					write_nic_dword(dev, EDCAPARA_BE, edca_setting_UL[pHTInfo->IOTPeer]);
					priv->bis_cur_rdlstate = false;
				}
			}
			else
			{// Balance TP is present.
				if(!priv->bis_cur_rdlstate || !priv->bcurrent_turbo_EDCA)
				{
					if(priv->rtllib->mode == WIRELESS_MODE_G)
						write_nic_dword(dev, EDCAPARA_BE, edca_setting_DL_GMode[pHTInfo->IOTPeer]);
					else
						write_nic_dword(dev, EDCAPARA_BE, edca_setting_DL[pHTInfo->IOTPeer]);
					priv->bis_cur_rdlstate = true;
				}
			}
			priv->bcurrent_turbo_EDCA = true;
		}
		else
		{
		// For RT-AP, we needs to turn it on when Rx>Tx
		if(curRxOkCnt > 4*curTxOkCnt)
		{
			//printk("%s: Use DL edca param 0x%x. bis_cur_rdlstate = %d bcurrent_turbo_EDCA = %d\n",__FUNCTION__, edca_setting_DL[pHTInfo->IOTPeer], priv->bis_cur_rdlstate, priv->bcurrent_turbo_EDCA);
			if(!priv->bis_cur_rdlstate || !priv->bcurrent_turbo_EDCA)
			{
				if(priv->rtllib->mode == WIRELESS_MODE_G)
					write_nic_dword(dev, EDCAPARA_BE, edca_setting_DL_GMode[pHTInfo->IOTPeer]);
				else
				write_nic_dword(dev, EDCAPARA_BE, edca_setting_DL[pHTInfo->IOTPeer]);
				priv->bis_cur_rdlstate = true;
			}
		}
		else
		{
			//printk("%s: Use UL edca param 0x%x. bis_cur_rdlstate = %d bcurrent_turbo_EDCA = %d\n",__FUNCTION__, edca_setting_UL[pHTInfo->IOTPeer], priv->bis_cur_rdlstate, priv->bcurrent_turbo_EDCA);
			if(priv->bis_cur_rdlstate || !priv->bcurrent_turbo_EDCA)
			{
				write_nic_dword(dev, EDCAPARA_BE, edca_setting_UL[pHTInfo->IOTPeer]);
				priv->bis_cur_rdlstate = false;
			}

		}

		priv->bcurrent_turbo_EDCA = true;
	}
	}
	else
	{
		//
		// Turn Off EDCA turbo here.
		// Restore original EDCA according to the declaration of AP.
		//
		 if(priv->bcurrent_turbo_EDCA)
		{

			{
				u8		u1bAIFS;
				u32		u4bAcParam;
				struct rtllib_qos_parameters *qos_parameters = &priv->rtllib->current_network.qos_data.parameters;
				u8 mode = priv->rtllib->mode;

			// For Each time updating EDCA parameter, reset EDCA turbo mode status.
				dm_init_edca_turbo(dev);
				u1bAIFS = qos_parameters->aifs[0] * ((mode&(IEEE_G|IEEE_N_24G)) ?9:20) + aSifsTime;
				u4bAcParam = ((((u32)(qos_parameters->tx_op_limit[0]))<< AC_PARAM_TXOP_LIMIT_OFFSET)|
					(((u32)(qos_parameters->cw_max[0]))<< AC_PARAM_ECW_MAX_OFFSET)|
					(((u32)(qos_parameters->cw_min[0]))<< AC_PARAM_ECW_MIN_OFFSET)|
					((u32)u1bAIFS << AC_PARAM_AIFS_OFFSET));
				printk("===>u4bAcParam:%x, ", u4bAcParam);
			//write_nic_dword(dev, WDCAPARA_ADD[i], u4bAcParam);
				write_nic_dword(dev, EDCAPARA_BE,  u4bAcParam);

			// Check ACM bit.
			// If it is set, immediately set ACM control bit to downgrading AC for passing WMM testplan. Annie, 2005-12-13.
				{
			// TODO:  Modified this part and try to set acm control in only 1 IO processing!!

					PACI_AIFSN	pAciAifsn = (PACI_AIFSN)&(qos_parameters->aifs[0]);
					u8		AcmCtrl = read_nic_byte( dev, AcmHwCtrl );
					if( pAciAifsn->f.ACM )
					{ // ACM bit is 1.
						AcmCtrl |= AcmHw_BeqEn;
					}
					else
					{ // ACM bit is 0.
						AcmCtrl &= (~AcmHw_BeqEn);
					}

					RT_TRACE( COMP_QOS,"SetHwReg8190pci(): [HW_VAR_ACM_CTRL] Write 0x%X\n", AcmCtrl ) ;
					write_nic_byte(dev, AcmHwCtrl, AcmCtrl );
				}
			}
			priv->bcurrent_turbo_EDCA = false;
		}
	}


dm_CheckEdcaTurbo_EXIT:
	// Set variables for next time.
	priv->rtllib->bis_any_nonbepkts = false;
	lastTxOkCnt = priv->stats.txbytesunicast;
	lastRxOkCnt = priv->stats.rxbytesunicast;
}	// dm_CheckEdcaTurbo
#endif

extern void DM_CTSToSelfSetting(struct net_device * dev,u32 DM_Type, u32 DM_Value)
{
	struct r8192_priv *priv = rtllib_priv((struct net_device *)dev);

	if (DM_Type == 0)	// CTS to self disable/enable
	{
		if(DM_Value > 1)
			DM_Value = 1;
		priv->rtllib->bCTSToSelfEnable = (bool)DM_Value;
		//DbgPrint("pMgntInfo->bCTSToSelfEnable = %d\n", pMgntInfo->bCTSToSelfEnable);
	}
	else if(DM_Type == 1) //CTS to self Th
	{
		if(DM_Value >= 50)
			DM_Value = 50;
		priv->rtllib->CTSToSelfTH = (u8)DM_Value;
		//DbgPrint("pMgntInfo->CTSToSelfTH = %d\n", pMgntInfo->CTSToSelfTH);
	}
}

static void dm_init_ctstoself(struct net_device * dev)
{
	struct r8192_priv *priv = rtllib_priv((struct net_device *)dev);

	priv->rtllib->bCTSToSelfEnable = true;
	priv->rtllib->CTSToSelfTH = CTSToSelfTHVal;
}

static void dm_ctstoself(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv((struct net_device *)dev);
	PRT_HIGH_THROUGHPUT	pHTInfo = priv->rtllib->pHTInfo;
	static unsigned long				lastTxOkCnt = 0;
	static unsigned long				lastRxOkCnt = 0;
	unsigned long						curTxOkCnt = 0;
	unsigned long						curRxOkCnt = 0;

	if(priv->rtllib->bCTSToSelfEnable != true)
	{
		pHTInfo->IOTAction &= ~HT_IOT_ACT_FORCED_CTS2SELF;
		return;
	}
	//
	// 1. Uplink
	// 2. Linksys350/Linksys300N
	// 3. <50 disable, >55 enable
	//
	if(pHTInfo->IOTPeer == HT_IOT_PEER_BROADCOM)
	{
		curTxOkCnt = priv->stats.txbytesunicast - lastTxOkCnt;
		curRxOkCnt = priv->stats.rxbytesunicast - lastRxOkCnt;
		if(curRxOkCnt > 4*curTxOkCnt)	//downlink, disable CTS to self
		{
			pHTInfo->IOTAction &= ~HT_IOT_ACT_FORCED_CTS2SELF;
			//DbgPrint("dm_CTSToSelf() ==> CTS to self disabled -- downlink\n");
		}
		else	//uplink
		{
		#if 1
			pHTInfo->IOTAction |= HT_IOT_ACT_FORCED_CTS2SELF;
		#else
			if(priv->undecorated_smoothed_pwdb < priv->rtllib->CTSToSelfTH)	// disable CTS to self
			{
				pHTInfo->IOTAction &= ~HT_IOT_ACT_FORCED_CTS2SELF;
				//DbgPrint("dm_CTSToSelf() ==> CTS to self disabled\n");
			}
			else if(priv->undecorated_smoothed_pwdb >= (priv->rtllib->CTSToSelfTH+5))	// enable CTS to self
			{
				pHTInfo->IOTAction |= HT_IOT_ACT_FORCED_CTS2SELF;
				//DbgPrint("dm_CTSToSelf() ==> CTS to self enabled\n");
			}
		#endif
		}

		lastTxOkCnt = priv->stats.txbytesunicast;
		lastRxOkCnt = priv->stats.rxbytesunicast;
	}
}


//
// Work Around for Broadcom IOT
// if ss>25%, then disable CCK in RRSR, else enable CCK in RRSR
// if ss>25%, then use OFDM 6M tx Mgnt/DHCP/EAPOL packet, else use CCK 1M.
//
static	void
dm_Init_WA_Broadcom_IOT(struct net_device * dev)
{
	struct r8192_priv *priv = rtllib_priv((struct net_device *)dev);
	PRT_HIGH_THROUGHPUT	pHTInfo = priv->rtllib->pHTInfo;

	pHTInfo->bWAIotBroadcom = false;
	pHTInfo->WAIotTH = WAIotTHVal;
}

#ifdef RTL8192SE
static	void
dm_WA_Broadcom_IOT(struct net_device * dev)
{
	struct r8192_priv *priv = rtllib_priv((struct net_device *)dev);
	PRT_HIGH_THROUGHPUT	pHTInfo = priv->rtllib->pHTInfo;
	u8					update=0;
	//static u8				EWC11NHTCap[] = {0x00, 0x90, 0x4c, 0x33};		// For 11n EWC definition, 2007.07.17, by Emily
	//u8*					pPeerHTCap;
	//static RT_MEDIA_STATUS	connectState=RT_MEDIA_DISCONNECT;
	static enum rtllib_state connectState = RTLLIB_NOLINK;

	if( (pHTInfo->bWAIotBroadcom != true) ||
		(priv->rtllib->mode == WIRELESS_MODE_B) ||
		(pHTInfo->bCurBW40MHz))
	{
		if(pHTInfo->IOTAction & HT_IOT_ACT_WA_IOT_Broadcom)
		{
			pHTInfo->IOTAction &= ~HT_IOT_ACT_WA_IOT_Broadcom;
			update = 1;
			printk(" dm_WA_Broadcom_IOT(), disable HT_IOT_ACT_WA_IOT_Broadcom\n");
		}
		else
			return;
	}

	if(connectState == RTLLIB_LINKED && priv->rtllib->state == RTLLIB_LINKED)
	{
		if(pHTInfo->IOTAction & HT_IOT_ACT_WA_IOT_Broadcom)
		{
			pHTInfo->IOTAction &= ~HT_IOT_ACT_WA_IOT_Broadcom;
			update = 1;
			pHTInfo->bWAIotBroadcom = false;
			printk("dm_WA_Broadcom_IOT(), from connect to disconnected, disable HT_IOT_ACT_WA_IOT_Broadcom\n");
		}
	}
	connectState = priv->rtllib->state;

	if(!update && pHTInfo->IOTPeer == HT_IOT_PEER_BROADCOM)
	{
		printk("dm_WA_Broadcom_IOT(), UndecoratedSmoothedPWDB = %ld\n", priv->undecorated_smoothed_pwdb);
		if(priv->undecorated_smoothed_pwdb < pHTInfo->WAIotTH)
		{
			if(pHTInfo->IOTAction & HT_IOT_ACT_WA_IOT_Broadcom)
			{
				pHTInfo->IOTAction &= ~HT_IOT_ACT_WA_IOT_Broadcom;
				update = 1;
				printk("dm_WA_Broadcom_IOT() ==> WA_IOT enable cck rates\n");
			}
		}
		else if(priv->undecorated_smoothed_pwdb >= (priv->rtllib->CTSToSelfTH+5))	// enable CTS to self
		{
			if((pHTInfo->IOTAction & HT_IOT_ACT_WA_IOT_Broadcom) == 0)
			{
				pHTInfo->IOTAction |= HT_IOT_ACT_WA_IOT_Broadcom;
				update = 1;
				printk("dm_WA_Broadcom_IOT() ==> WA_IOT disable cck rates\n");
			}
		}
	}

	if(update)
	{
#ifdef _RTL8192_EXT_PATCH_
		if(priv->rtllib->bUseRAMask){
			printk("=============>%s():bUseRAMask is true\n",__FUNCTION__);
			priv->rtllib->UpdateHalRAMaskHandler(
										dev,
										false,
										0,
										NULL,
										priv->rtllib->mode,
										0);
		}
		else{
			printk("=============>%s():bUseRAMask is false\n",__FUNCTION__);
			priv->ops->update_ratr_table(dev, priv->rtllib->dot11HTOperationalRateSet, NULL);
		}
#else
		if(!priv->rtllib->bUseRAMask){
			priv->ops->update_ratr_table(dev, priv->rtllib->dot11HTOperationalRateSet, NULL);
		}
#endif
		priv->rtllib->SetHwRegHandler( dev, HW_VAR_BASIC_RATE, (u8*)(&priv->basic_rate));//(pu1Byte)(&pMgntInfo->mBrates) );
	}
}
#endif


#if 0
//-----------------------------------------------------------------------------
// Function:	dm_rf_operation_test_callback()
//
// Overview:	Only for RF operation test now.
//
// Input:		NONE
//
// Output:		NONE
//
// Return:		NONE
//
// Revised History:
//	When		Who		Remark
//	05/29/2008	amy		Create Version 0 porting from windows code.
//
//---------------------------------------------------------------------------*/
extern void dm_rf_operation_test_callback(unsigned long dev)
{
//	struct r8192_priv *priv = rtllib_priv((struct net_device *)dev);
	u8 erfpath;


	for(erfpath=0; erfpath<4; erfpath++)
	{
		//DbgPrint("Set RF-%d\n\r", eRFPath);
		//PHY_SetRFReg(dev, (RF90_RADIO_PATH_E)eRFPath, 0x2c, bMask12Bits, 0x3d7);
		udelay(100);
	}

	{
		//PlatformSetPeriodicTimer(dev, &pHalData->RfTest1Timer, 500);
	}

	// For test
	{
		//u8 i;
		//PlatformSetPeriodicTimer(dev, &pHalData->RfTest1Timer, 500);
#if 0
		for(i=0; i<50; i++)
		{
			// Write Test
			PHY_SetRFReg(dev, RF90_PATH_A, 0x02, bMask12Bits, 0x4d);
			//delay_us(100);
			PHY_SetRFReg(dev, RF90_PATH_A, 0x02, bMask12Bits, 0x4f);
			//delay_us(100);
			PHY_SetRFReg(dev, RF90_PATH_C, 0x02, bMask12Bits, 0x4d);
			//delay_us(100);
			PHY_SetRFReg(dev, RF90_PATH_C, 0x02, bMask12Bits, 0x4f);
			//delay_us(100);

#if 0
			// Read test
			PHY_QueryRFReg(dev, RF90_PATH_A, 0x02, bMask12Bits);
			//delay_us(100);
			PHY_QueryRFReg(dev, RF90_PATH_A, 0x02, bMask12Bits);
			//delay_us(100);
			PHY_QueryRFReg(dev, RF90_PATH_A, 0x12, bMask12Bits);
			//delay_us(100);
			PHY_QueryRFReg(dev, RF90_PATH_A, 0x12, bMask12Bits);
			//delay_us(100);
			PHY_QueryRFReg(dev, RF90_PATH_A, 0x21, bMask12Bits);
			//delay_us(100);
			PHY_QueryRFReg(dev, RF90_PATH_A, 0x21, bMask12Bits);
			//delay_us(100);
#endif
		}
#endif
	}

}	// DM_RfOperationTestCallBack */
#endif

//-----------------------------------------------------------------------------
// Function:	dm_check_rfctrl_gpio()
//
// Overview:	Copy 8187B template for 9xseries.
//
// Input:		NONE
//
// Output:		NONE
//
// Return:		NONE
//
// Revised History:
//	When		Who		Remark
//	05/28/2008	amy		Create Version 0 porting from windows code.
//
//---------------------------------------------------------------------------*/
#if 0
static void dm_check_rfctrl_gpio(struct net_device * dev)
{
#ifdef RTL8192E
	struct r8192_priv *priv = rtllib_priv(dev);
#endif

	// Walk around for DTM test, we will not enable HW - radio on/off because r/w
	// page 1 register before Lextra bus is enabled cause system fails when resuming
	// from S4. 20080218, Emily

	// Stop to execute workitem to prevent S3/S4 bug.
#ifdef RTL8190P
	return;
#endif
#ifdef RTL8192U
	return;
#endif
#ifdef RTL8192E
	queue_delayed_work_rsl(priv->priv_wq,&priv->gpio_change_rf_wq,0);
#endif

}	// dm_CheckRfCtrlGPIO */

#endif
//-----------------------------------------------------------------------------
// Function:	dm_check_pbc_gpio()
//
// Overview:	Check if PBC button is pressed.
//
// Input:		NONE
//
// Output:		NONE
//
// Return:		NONE
//
// Revised History:
//	When		Who		Remark
//	05/28/2008	amy	Create Version 0 porting from windows code.
//
//---------------------------------------------------------------------------*/
static	void	dm_check_pbc_gpio(struct net_device *dev)
{
#ifdef RTL8192U
	struct r8192_priv *priv = rtllib_priv(dev);
	u8 tmp1byte;


	tmp1byte = read_nic_byte(dev,GPI);
	if(tmp1byte == 0xff)
	return;

	if (tmp1byte&BIT6 || tmp1byte&BIT0)
	{
		// Here we only set bPbcPressed to true
		// After trigger PBC, the variable will be set to false
		RT_TRACE(COMP_IO, "CheckPbcGPIO - PBC is pressed\n");
		priv->bpbc_pressed = true;
	}
#endif

}

#ifdef RTL8192E

//-----------------------------------------------------------------------------
// Function:	dm_GPIOChangeRF
// Overview:	PCI will not support workitem call back HW radio on-off control.
//
// Input:		NONE
//
// Output:		NONE
//
// Return:		NONE
//
// Revised History:
//	When		Who		Remark
//	02/21/2008	MHC		Create Version 0.
//
//---------------------------------------------------------------------------*/
extern	void	dm_gpio_change_rf_callback(void *data)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20))
       struct r8192_priv *priv = container_of_dwork_rsl(data,struct r8192_priv,gpio_change_rf_wq);
       struct net_device *dev = priv->rtllib->dev;
#else
	struct r8192_priv *priv = rtllib_priv((struct net_device *)data);
	struct net_device *dev = priv->rtllib->dev;
#endif
	u8 tmp1byte;
	RT_RF_POWER_STATE	eRfPowerStateToSet;
	//PRT_POWER_SAVE_CONTROL		pPSC = (PRT_POWER_SAVE_CONTROL)(&(priv->rtllib->PowerSaveControl));//GET_POWER_SAVE_CONTROL(pMgntInfo);
	bool bActuallySet = false;

	char *argv[3];
	static char *RadioPowerPath = "/etc/acpi/events/RadioPower.sh";
	static char *envp[] = {"HOME=/", "TERM=linux", "PATH=/usr/bin:/bin", NULL};

	bActuallySet=false;

	//printk("=======================>%s\n", __func__);
	if((priv->up_first_time == 1) || (priv->being_init_adapter))
	{
		//RT_TRACE((COMP_INIT | COMP_POWER | COMP_RF),"dm_gpio_change_rf_callback(): Callback function breaks out!!\n");
		//printk("dm_gpio_change_rf_callback(): Callback function breaks out!!\n");
		return;
	}

	//if(priv->ResetProgress == RESET_TYPE_SILENT)
	//{
	//	RT_TRACE((COMP_INIT | COMP_POWER | COMP_RF), "GPIOChangeRFWorkItemCallBack(): Silent Reseting!!!!!!!\n");
	//	return;
	//}

	//2008.12.31 Added by tynli
	//if (pPSC->bSwRfProcessing)
	//{
	//	RT_TRACE(COMP_SCAN, "GPIOChangeRFWorkItemCallBack(): Rf is in switching state.\n");
	//	return;
	//}

	{
		// 0x108 GPIO input register is read only
		//set 0x108 B1= 1: RF-ON; 0: RF-OFF.
		tmp1byte = read_nic_byte(dev,GPI);

		eRfPowerStateToSet = (tmp1byte&BIT1) ?  eRfOn : eRfOff;
		//printk("========================>GPI%#x,tmp1byte:%#x, %#x\n", GPI,tmp1byte, eRfPowerStateToSet);

		if( (priv->bHwRadioOff == true) && (eRfPowerStateToSet == eRfOn))
		{
			RT_TRACE(COMP_RF, "gpiochangeRF  - HW Radio ON\n");
			printk("gpiochangeRF  - HW Radio ON\n");
			priv->bHwRadioOff = false;
			bActuallySet = true;
		}
		else if ( (priv->bHwRadioOff == false) && (eRfPowerStateToSet == eRfOff))
		{
			RT_TRACE(COMP_RF, "gpiochangeRF  - HW Radio OFF\n");
			printk("gpiochangeRF  - HW Radio OFF\n");
			priv->bHwRadioOff = true;
			bActuallySet = true;
		}

		if(bActuallySet)
		{
			mdelay(1000); //add here temp for led issue
			priv->bHwRfOffAction = 1;
			MgntActSet_RF_State(dev, eRfPowerStateToSet, RF_CHANGE_BY_HW);
			//DrvIFIndicateCurrentPhyStatus(pAdapter);
			{
				// To update the UI status for Power status changed
				if(priv->bHwRadioOff == true)
					argv[1] = "RFOFF";
				else
					argv[1] = "RFON";

				argv[0] = RadioPowerPath;
				argv[2] = NULL;
				call_usermodehelper(RadioPowerPath,argv,envp,1);
			}

		}
#if 0 //sync with Windows. This will slower 2.4 system. WB.2009.02.05
		else
		{
			msleep(2000);
		}
#endif

	}

}	// dm_GPIOChangeRF */
#elif defined RTL8192SE

//-----------------------------------------------------------------------------
// Function:	Power_DomainInit92SE()
//
// Overview:	Exit from D3 and permit to R/W register.
//
// Input:		NONE
//
// Output:		NONE
//
// Return:		NONE
//
// Revised History:
//	When		Who		Remark
//	04/29/2008	MHC		The same as 92SE MAC initialization.
//
//---------------------------------------------------------------------------*/
extern void Power_DomainInit92SE(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	u8				tmpU1b;
	u16				tmpU2b;

	// 2008/05/14 MH For 92SE rest before init MAC data sheet not defined now.
	//RT_TRACE(COMP_INIT, "--->Power_DomainInit92SE()\n");

	// Switch to SW IO control
	tmpU1b = read_nic_byte(dev, (SYS_CLKR + 1));
	if(tmpU1b & BIT7)
	{
		tmpU1b &= ~(BIT6 | BIT7);
		if(!HalSetSysClk8192SE(dev, tmpU1b))
			return; // Set failed, return to prevent hang.
	}

	write_nic_byte(dev, AFE_PLL_CTRL, 0x0);
	write_nic_byte(dev, LDOA15_CTRL, 0x34);//0x30);
#if 0
	tmpU2b = read_nic_word(dev, SYS_CLKR);
	if (tmpU2b & BIT15)	// If not FW control, we may fail to return from HW SW to HW
	{
		tmpU2b &= ~(BIT14|BIT15);
		RT_TRACE(COMP_INIT, DBG_LOUD, ("Return to HW CTRL\n"));
		write_nic_word(dev, SYS_CLKR, tmpU2b);
	}
	// Prevent Hang on some platform!!!!
	udelay(200);
#endif

	// Reset MAC-IO and CPU and Core Digital BIT10/11/15
	//RT_TRACE(COMP_INIT, DBG_LOUD,		("Reset MAC-IO and CPU and Core Digital\r\n"));
	tmpU1b = read_nic_byte(dev, SYS_FUNC_EN+1);
	//
	// 2009/04/07 MH If IPS we need to turn LED on. So we not not disable BIT 3/7 of reg3.
	//
	if(priv->rtllib->RfOffReason == RF_CHANGE_BY_IPS )
	{
		tmpU1b &= 0xFB;
	}
	else
	{
	tmpU1b &= 0x73;
	}

	write_nic_byte(dev, SYS_FUNC_EN+1, tmpU1b);
	// wait for BIT 10/11/15 to pull high automatically!!
	udelay(1000);

	write_nic_byte(dev, CMDR, 0);
	write_nic_byte(dev, TCR, 0);

	// Data sheet not define 0x562!!! Copy from WMAC!!!!!
	tmpU1b = read_nic_byte(dev, 0x562);
	tmpU1b |= 0x08;
	write_nic_byte(dev, 0x562, tmpU1b);
	tmpU1b &= ~(BIT3);
	write_nic_byte(dev, 0x562, tmpU1b);

	//Enable AFE clock source
	//RT_TRACE(COMP_INIT, "Enable AFE clock source\r\n");
	tmpU1b = read_nic_byte(dev, AFE_XTAL_CTRL);
	write_nic_byte(dev, AFE_XTAL_CTRL, (tmpU1b|0x01));
	// Delay 1.5ms
	udelay(1500);
	tmpU1b = read_nic_byte(dev, AFE_XTAL_CTRL+1);
	write_nic_byte(dev, AFE_XTAL_CTRL+1, (tmpU1b&0xfb));


	//Enable AFE Macro Block's Bandgap
	//RT_TRACE(COMP_INIT, "Enable AFE Macro Block's Bandgap\r\n");
	tmpU1b = read_nic_byte(dev, AFE_MISC);
	write_nic_byte(dev, AFE_MISC, (tmpU1b|BIT0));
	udelay(1000);

	//Enable AFE Mbias
	//RT_TRACE(COMP_INIT, "Enable AFE Mbias\r\n");
	tmpU1b = read_nic_byte(dev, AFE_MISC);
	write_nic_byte(dev, AFE_MISC, (tmpU1b|0x02));
	udelay(1000);

	//Enable LDOA15 block
	tmpU1b = read_nic_byte(dev, LDOA15_CTRL);
	write_nic_byte(dev, LDOA15_CTRL, (tmpU1b|BIT0));

	// Set Digital Vdd to Retention isolation Path.
	tmpU2b = read_nic_word(dev, SYS_ISO_CTRL);
	write_nic_word(dev, SYS_ISO_CTRL, (tmpU2b|BIT11));


	// 2008/09/25 MH From SD1 Jong, For warm reboot NIC disappera bug.
	tmpU2b = read_nic_word(dev, SYS_FUNC_EN);
#ifdef _RTL8192_EXT_PATCH_
	write_nic_word(dev, SYS_FUNC_EN, tmpU2b |= BIT13);
#else
	write_nic_word(dev, SYS_FUNC_EN, (tmpU2b |BIT13));
#endif

	write_nic_byte(dev, SYS_ISO_CTRL+1, 0x68);

	//Enable AFE PLL Macro Block
	tmpU1b = read_nic_byte(dev, AFE_PLL_CTRL);
	write_nic_byte(dev, AFE_PLL_CTRL, (tmpU1b|BIT0|BIT4));
	// Enable MAC 80MHZ clock
	tmpU1b = read_nic_byte(dev, AFE_PLL_CTRL+1);
	write_nic_byte(dev, AFE_PLL_CTRL+1, (tmpU1b|BIT0));
	udelay(1000);

	// Release isolation AFE PLL & MD
	write_nic_byte(dev, SYS_ISO_CTRL, 0xA6);

	//Enable MAC clock
	tmpU2b = read_nic_word(dev, SYS_CLKR);
	//write_nic_word(dev, SYS_CLKR, (tmpU2b|SYS_MAC_CLK_EN));
	write_nic_word(dev, SYS_CLKR, (tmpU2b|BIT12|BIT11));

	//Enable Core digital and enable IOREG R/W
	tmpU2b = read_nic_word(dev, SYS_FUNC_EN);
	write_nic_word(dev, SYS_FUNC_EN, (tmpU2b|BIT11));
	//enable REG_EN
	write_nic_word(dev, SYS_FUNC_EN, (tmpU2b|BIT11|BIT15));

	 //Switch the control path.
	 tmpU2b = read_nic_word(dev, SYS_CLKR);
	write_nic_word(dev, SYS_CLKR, (tmpU2b&(~BIT2)));

	tmpU1b = read_nic_byte(dev, (SYS_CLKR + 1));
	tmpU1b = ((tmpU1b | BIT7) & (~BIT6));
	if(!HalSetSysClk8192SE(dev, tmpU1b))
	{
		//printk("CTRL Path switch fail\n");
		return; // Set failed, return to prevent hang.
	}
#if 0
	tmpU2b = read_nic_word(dev, SYS_CLKR);
	write_nic_word(dev, SYS_CLKR, ((tmpU2b|BIT15)&(~BIT14)));
#endif

	write_nic_word(dev, CMDR, 0x37FC);

	// After MACIO reset,we must refresh LED state.
	gen_RefreshLedState(dev);

	//RT_TRACE(COMP_INIT, "<---Power_DomainInit92SE()\n");

}	// Power_DomainInit92SE */

void	SET_RTL8192SE_RF_HALT(struct net_device *dev)
{
	u8		u1bTmp;
	struct r8192_priv *priv = rtllib_priv(dev);

	if(priv->rtllib->RfOffReason == RF_CHANGE_BY_IPS && priv->LedStrategy == SW_LED_MODE8)
	{
		SET_RTL8192SE_RF_SLEEP(dev);
		return;
	}

	u1bTmp = read_nic_byte(dev, LDOV12D_CTRL);
	u1bTmp |= BIT0;
	write_nic_byte(dev, LDOV12D_CTRL, u1bTmp);
	write_nic_byte(dev, SPS1_CTRL, 0x0);
	write_nic_byte(dev, TXPAUSE, 0xFF);
	write_nic_word(dev, CMDR, 0x57FC);
	udelay(100);
	write_nic_word(dev, CMDR, 0x77FC);
	write_nic_byte(dev, PHY_CCA, 0x0);
	udelay(10);
	write_nic_word(dev, CMDR, 0x37FC);
	udelay(10);
	write_nic_word(dev, CMDR, 0x77FC);
	udelay(10);
	write_nic_word(dev, CMDR, 0x57FC);
	write_nic_word(dev, CMDR, 0x0000);
	u1bTmp = read_nic_byte(dev, (SYS_CLKR + 1));
	if(u1bTmp & BIT7)
	{
		u1bTmp &= ~(BIT6 | BIT7);
		if(!HalSetSysClk8192SE(dev, u1bTmp))
			return;
	}
	//printk("Save MAC\n");
	//  03/27/2009 MH Add description. Power save for MAC */
	if(priv->rtllib->RfOffReason==RF_CHANGE_BY_IPS )
	{
		//  03/27/2009 MH For enable LED function. */
		//printk("Save except led\n");
		write_nic_byte(dev, 0x03, 0xF9);
	}
	else		// SW/HW radio off or halt adapter!! For example S3/S4
	{
		// 2009/03/27 MH LED function disable. Power range is about 8mA now.
		//printk("Save max pwr\n");
		write_nic_byte(dev, 0x03, 0x71);
	}
	write_nic_byte(dev, SYS_CLKR+1, 0x70);
	write_nic_byte(dev, AFE_PLL_CTRL+1, 0x68);
	write_nic_byte(dev, AFE_PLL_CTRL, 0x00);
	write_nic_byte(dev, LDOA15_CTRL, 0x34);//0x50);
	write_nic_byte(dev, AFE_XTAL_CTRL, 0x0E);

}

//-----------------------------------------------------------------------------
// Function:	GPIOChangeRFWorkItemCallBack()
//
// Overview:	Trun on/off RF according to GPIO1. In register 0x108 bit 1.
//
// Input:		NONE
//
// Output:		NONE
//
// Return:		NONE
//
// Revised History:
//	When		Who		Remark
//	01/10/2008	MHC		Copy from 818xB code to be template. And we only monitor
//						0x108 register bit 1 for HW RF-ON/OFF.
//	12/22/2008	MHC		If radio off is in D3 mode. We must modify some register
//						for us to read register.
//
//---------------------------------------------------------------------------*/
extern void dm_gpio_change_rf_callback(void *data)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20))
	struct r8192_priv *priv = container_of_dwork_rsl(data,struct r8192_priv,gpio_change_rf_wq);
	struct net_device *dev = priv->rtllib->dev;
#else
	struct net_device *dev = (struct net_device *)data;
	struct r8192_priv *priv = rtllib_priv(dev);
#endif

	u8					u1Tmp = 0;
	RT_RF_POWER_STATE	eRfPowerStateToSet, CurRfState;
	bool					bActuallySet = false;
	PRT_POWER_SAVE_CONTROL		pPSC = (PRT_POWER_SAVE_CONTROL)(&(priv->rtllib->PowerSaveControl));//GET_POWER_SAVE_CONTROL(pMgntInfo);
	unsigned long flag = 0;

	char *argv[3];
	static char *RadioPowerPath = "/etc/acpi/events/RadioPower.sh";
	static char *envp[] = {"HOME=/", "TERM=linux", "PATH=/usr/bin:/bin", NULL};
	u8 count = 0;

	if((priv->up_first_time == 1) || (priv->being_init_adapter))// || RT_IS_FUNC_DISABLED(pAdapter, DF_IO_BIT))
	{
		;//RT_TRACE((COMP_INIT | COMP_PS | COMP_RF), "GPIOChangeRFWorkItemCallBack(): Callback function breaks out!!\n");
		return;
	}

	if(priv->ResetProgress == RESET_TYPE_SILENT)
	{
		RT_TRACE((COMP_INIT | COMP_POWER | COMP_RF), "GPIOChangeRFWorkItemCallBack(): Silent Reseting!!!!!!!\n");
		return;
	}


	//2008.12.31 Added by tynli
	if (pPSC->bSwRfProcessing)
	{
		RT_TRACE(COMP_SCAN, "GPIOChangeRFWorkItemCallBack(): Rf is in switching state.\n");
		return;
	}

	RT_TRACE(COMP_RF, "GPIOChangeRFWorkItemCallBack() ---------> \n");

	//3 //
	//3 //<1>Prevent the race condition of RF state change.
	//3 //
	// Only one thread can change the RF state at one time, and others should wait to be executed. By Bruce, 2007-11-28.
	spin_lock_irqsave(&priv->rf_ps_lock,flag);
#if 1
	if(priv->RFChangeInProgress)
	{
		spin_unlock_irqrestore(&priv->rf_ps_lock,flag);
		RT_TRACE(COMP_RF, "GPIOChangeRFWorkItemCallBack(): RF Change in progress! \n");
		return;
	}
	else
	{
		priv->RFChangeInProgress = true;
		spin_unlock_irqrestore(&priv->rf_ps_lock,flag);
	}
#endif
	//pAdapter->HalFunc.GetHwRegHandler(pAdapter, HW_VAR_RF_STATE, (pu1Byte)&CurRfState);
	CurRfState = priv->rtllib->eRFPowerState;
#ifdef RTL8192SE_CONFIG_ASPM_OR_D3
	if((pPSC->RegRfPsLevel & RT_RF_OFF_LEVL_ASPM) && RT_IN_PS_LEVEL(pPSC, RT_RF_OFF_LEVL_ASPM))
	{
		RT_DISABLE_ASPM(dev);
		RT_CLEAR_PS_LEVEL(pPSC, RT_RF_OFF_LEVL_ASPM);
	}
	else if((pPSC->RegRfPsLevel & RT_RF_OFF_LEVL_PCI_D3) && RT_IN_PS_LEVEL(pPSC, RT_RF_OFF_LEVL_PCI_D3))
	{
#ifdef TODO
		RT_LEAVE_D3(dev, false);
		RT_CLEAR_PS_LEVEL(pPSC, RT_RF_OFF_LEVL_PCI_D3);
#endif
	}

#endif
	if(RT_IN_PS_LEVEL(pPSC, RT_RF_OFF_LEVL_HALT_NIC))
	{
		Power_DomainInit92SE(dev);
	}
	while(u1Tmp == 0){
		if(priv->BluetoothCoexist == 0) {
			// Advised from Jong SD1. Using GPIO 3.
			// Added by Bruce, 2008-12-01.
			write_nic_byte(dev, MAC_PINMUX_CFG, (GPIOMUX_EN | GPIOSEL_GPIO));
			//DbgPrint("write 0x%x\n", GPIOMUX_EN | GPIOSEL_GPIO);
		} else {
			// Cosa add for bluetooth coexistance
			write_nic_byte(dev, MAC_PINMUX_CFG, 0xa);
			printk("!!!!!!!!!!!!!!!%s:write 0x%x to 0x2F1\n", __FUNCTION__, 0xa);
		}

		u1Tmp = read_nic_byte(dev, GPIO_IO_SEL);
		if(count >= 100)
			break;
		count++;
	}
	u1Tmp &= HAL_8192S_HW_GPIO_OFF_MASK;
	write_nic_byte(dev, GPIO_IO_SEL, u1Tmp);

	// On some of the platform, driver cannot read correct value without delay
	// between Write_GPIO_SEL and Read_GPIO_IN, Bruce, 2009/1/19
	//udelay(100);
	mdelay(10);

	if(priv->pwrdown)
	{
		u1Tmp = read_nic_byte(dev, 0x06);
		printk( "GPIO 04h BIT6=%02x\n", u1Tmp);
		eRfPowerStateToSet = (u1Tmp & BIT6) ? eRfOn: eRfOff;
	}
	else
	{
	u1Tmp = read_nic_byte(dev, GPIO_IN);
	//printk("GPIO_IN=%02x\n", u1Tmp);
	eRfPowerStateToSet = (u1Tmp & HAL_8192S_HW_GPIO_OFF_BIT) ? eRfOn : eRfOff;
	}

	if(priv->bResetInProgress)//bSilentReseted)	// if silent reset, the polling gpio would error sometimes.
	{
		spin_lock_irqsave(&priv->rf_ps_lock,flag);
		priv->RFChangeInProgress = false;
		spin_unlock_irqrestore(&priv->rf_ps_lock,flag);
		return;
	}

	if( (priv->bHwRadioOff == true) && \
	   (((eRfPowerStateToSet == eRfOn)&&(priv->sw_radio_on == true))
#ifdef CONFIG_RTLWIFI_DEBUGFS
	    ||priv->debug->hw_holding
#endif
	    ))
	{
		RT_TRACE(COMP_RF, "GPIOChangeRF  - HW Radio ON, RF ON\n");
		printk("GPIOChangeRF  - HW Radio ON, RF ON\n");
                eRfPowerStateToSet = eRfOn;
		//priv->bHwRadioOff = false;
		bActuallySet = true;
	}
	else if ( (priv->bHwRadioOff == false) && ((eRfPowerStateToSet == eRfOff) || (priv->sw_radio_on == false)))
	{
		RT_TRACE(COMP_RF, "GPIOChangeRF  - HW Radio OFF\n");
		printk("GPIOChangeRF  - HW Radio OFF\n");
                eRfPowerStateToSet = eRfOff;
		//priv->bHwRadioOff = true;
		bActuallySet = true;
	}

	if(bActuallySet)
	{
		priv->bHwRfOffAction = 1;
#ifdef RTL8192SE_CONFIG_ASPM_OR_D3
		if(eRfPowerStateToSet == eRfOn)
		{
			if((pPSC->RegRfPsLevel & RT_RF_OFF_LEVL_ASPM) && RT_IN_PS_LEVEL(pPSC, RT_RF_OFF_LEVL_ASPM))
			{
				RT_DISABLE_ASPM(dev);
				RT_CLEAR_PS_LEVEL(pPSC, RT_RF_OFF_LEVL_ASPM);
			}
#ifdef TODO
			else if((pPSC->RegRfPsLevel & RT_RF_OFF_LEVL_PCI_D3) && RT_IN_PS_LEVEL(pPSC, RT_RF_OFF_LEVL_PCI_D3))
			{
				RT_LEAVE_D3(dev, false);
				RT_CLEAR_PS_LEVEL(pPSC, RT_RF_OFF_LEVL_PCI_D3);
			}
#endif
		}
#endif
		spin_lock_irqsave(&priv->rf_ps_lock,flag);
		priv->RFChangeInProgress = false;
		spin_unlock_irqrestore(&priv->rf_ps_lock,flag);
		MgntActSet_RF_State(dev, eRfPowerStateToSet, RF_CHANGE_BY_HW);

		//DrvIFIndicateCurrentPhyStatus(pAdapter);
		{
			// To update the UI status for Power status changed */
			if(priv->bHwRadioOff == true)
				argv[1] = "RFOFF";
			else
				argv[1] = "RFON";

			argv[0] = RadioPowerPath;
			argv[2] = NULL;
			call_usermodehelper(RadioPowerPath,argv,envp,1);
		}

		if(eRfPowerStateToSet == eRfOff)
		{
			if(priv->pwrdown){
				//
				// 2009/04/22 MH When we detect power down event, change RF state
				// at first and then close power state. For ECS T20.
				//
				MgntActSet_RF_State(dev, eRfOff, RF_CHANGE_BY_HW);

				write_nic_byte(dev, 0x03, 0x31);
			}
#ifdef RTL8192SE_CONFIG_ASPM_OR_D3
			if(pPSC->RegRfPsLevel & RT_RF_OFF_LEVL_ASPM)
			{
				RT_ENABLE_ASPM(dev);
				RT_SET_PS_LEVEL(pPSC, RT_RF_OFF_LEVL_ASPM);
			}
#ifdef TODO
			else if(pPSC->RegRfPsLevel & RT_RF_OFF_LEVL_PCI_D3)
			{
				RT_ENTER_D3(dev, false);
				RT_SET_PS_LEVEL(pPSC, RT_RF_OFF_LEVL_PCI_D3);
			}
#endif
#endif
		}
	}
	else if(eRfPowerStateToSet == eRfOff || CurRfState == eRfOff)
	{

		// The current state remains OFF, but because we have powered on the parts of components,
		// we still need to power down them and also enter D3 if it supports. By Bruce, 2008-12-30.
		if(pPSC->RegRfPsLevel & RT_RF_OFF_LEVL_HALT_NIC)
		{ // Disable all components.
			SET_RTL8192SE_RF_HALT(dev);
			RT_SET_PS_LEVEL(pPSC, RT_RF_OFF_LEVL_HALT_NIC);
		}
#ifdef RTL8192SE_CONFIG_ASPM_OR_D3
		// Enter D3 or ASPM after GPIO had been done.
		if(pPSC->RegRfPsLevel & RT_RF_OFF_LEVL_ASPM)
		{
			RT_ENABLE_ASPM(dev);
			RT_SET_PS_LEVEL(pPSC, RT_RF_OFF_LEVL_ASPM);
		}
#ifdef TODO
		else if(pPSC->RegRfPsLevel & RT_RF_OFF_LEVL_PCI_D3)
		{
			RT_ENTER_D3(dev, false);
			RT_SET_PS_LEVEL(pPSC, RT_RF_OFF_LEVL_PCI_D3);
		}
#endif
#endif
		spin_lock_irqsave(&priv->rf_ps_lock,flag);
		priv->RFChangeInProgress = false;
		spin_unlock_irqrestore(&priv->rf_ps_lock,flag);
	}
	else
	{
		spin_lock_irqsave(&priv->rf_ps_lock,flag);
		priv->RFChangeInProgress = false;
		spin_unlock_irqrestore(&priv->rf_ps_lock,flag);
	}
	RT_TRACE(COMP_RF, "GPIOChangeRFWorkItemCallBack() <--------- \n");
}
#endif
//-----------------------------------------------------------------------------
// Function:	DM_RFPathCheckWorkItemCallBack()
//
// Overview:	Check if Current RF RX path is enabled
//
// Input:		NONE
//
// Output:		NONE
//
// Return:		NONE
//
// Revised History:
//	When		Who		Remark
//	01/30/2008	MHC		Create Version 0.
//
//---------------------------------------------------------------------------*/
void	dm_rf_pathcheck_workitemcallback(void *data)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20))
	struct r8192_priv *priv = container_of_dwork_rsl(data,struct r8192_priv,rfpath_check_wq);
	struct net_device *dev =priv->rtllib->dev;
#else
	struct net_device *dev = (struct net_device *)data;
	struct r8192_priv *priv = rtllib_priv(dev);
#endif
	//bool bactually_set = false;
	u8 rfpath = 0, i;


	// 2008/01/30 MH After discussing with SD3 Jerry, 0xc04/0xd04 register will
	// always be the same. We only read 0xc04 now. */
	rfpath = read_nic_byte(dev, 0xc04);

	// Check Bit 0-3, it means if RF A-D is enabled.
	for (i = 0; i < RF90_PATH_MAX; i++)
	{
		if (rfpath & (0x01<<i))
			priv->brfpath_rxenable[i] = 1;
		else
			priv->brfpath_rxenable[i] = 0;
	}
	if(!DM_RxPathSelTable.Enable)
		return;

	dm_rxpath_sel_byrssi(dev);
}	// DM_RFPathCheckWorkItemCallBack */

static void dm_init_rxpath_selection(struct net_device * dev)
{
	u8 i;
	struct r8192_priv *priv = rtllib_priv(dev);
	DM_RxPathSelTable.Enable = 1;	//default enabled
	DM_RxPathSelTable.SS_TH_low = RxPathSelection_SS_TH_low;
	DM_RxPathSelTable.diff_TH = RxPathSelection_diff_TH;
	if(priv->CustomerID == RT_CID_819x_Netcore)
		DM_RxPathSelTable.cck_method = CCK_Rx_Version_2;
	else
		DM_RxPathSelTable.cck_method = CCK_Rx_Version_1;
	DM_RxPathSelTable.DbgMode = DM_DBG_OFF;
	DM_RxPathSelTable.disabledRF = 0;
	for(i=0; i<4; i++)
	{
		DM_RxPathSelTable.rf_rssi[i] = 50;
		DM_RxPathSelTable.cck_pwdb_sta[i] = -64;
		DM_RxPathSelTable.rf_enable_rssi_th[i] = 100;
	}
}

static void dm_rxpath_sel_byrssi(struct net_device * dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	u8				i, max_rssi_index=0, min_rssi_index=0, sec_rssi_index=0, rf_num=0;
	u8				tmp_max_rssi=0, tmp_min_rssi=0, tmp_sec_rssi=0;
	u8				cck_default_Rx=0x2;	//RF-C
	u8				cck_optional_Rx=0x3;//RF-D
	long				tmp_cck_max_pwdb=0, tmp_cck_min_pwdb=0, tmp_cck_sec_pwdb=0;
	u8				cck_rx_ver2_max_index=0, cck_rx_ver2_min_index=0, cck_rx_ver2_sec_index=0;
	u8				cur_rf_rssi;
	long				cur_cck_pwdb;
	static u8			disabled_rf_cnt=0, cck_Rx_Path_initialized=0;
	u8				update_cck_rx_path;

	if(priv->rf_type != RF_2T4R)
		return;

	if(!cck_Rx_Path_initialized)
	{
		DM_RxPathSelTable.cck_Rx_path = (read_nic_byte(dev, 0xa07)&0xf);
		cck_Rx_Path_initialized = 1;
	}

	DM_RxPathSelTable.disabledRF = 0xf;
	DM_RxPathSelTable.disabledRF &=~ (read_nic_byte(dev, 0xc04));

	if(priv->rtllib->mode == WIRELESS_MODE_B)
	{
		DM_RxPathSelTable.cck_method = CCK_Rx_Version_2;	//pure B mode, fixed cck version2
		//DbgPrint("Pure B mode, use cck rx version2 \n");
	}

	//decide max/sec/min rssi index
	for (i=0; i<RF90_PATH_MAX; i++)
	{
		if(!DM_RxPathSelTable.DbgMode)
			DM_RxPathSelTable.rf_rssi[i] = priv->stats.rx_rssi_percentage[i];

		if(priv->brfpath_rxenable[i])
		{
			rf_num++;
			cur_rf_rssi = DM_RxPathSelTable.rf_rssi[i];

			if(rf_num == 1)	// find first enabled rf path and the rssi values
			{	//initialize, set all rssi index to the same one
				max_rssi_index = min_rssi_index = sec_rssi_index = i;
				tmp_max_rssi = tmp_min_rssi = tmp_sec_rssi = cur_rf_rssi;
			}
			else if(rf_num == 2)
			{	// we pick up the max index first, and let sec and min to be the same one
				if(cur_rf_rssi >= tmp_max_rssi)
				{
					tmp_max_rssi = cur_rf_rssi;
					max_rssi_index = i;
				}
				else
				{
					tmp_sec_rssi = tmp_min_rssi = cur_rf_rssi;
					sec_rssi_index = min_rssi_index = i;
				}
			}
			else
			{
				if(cur_rf_rssi > tmp_max_rssi)
				{
					tmp_sec_rssi = tmp_max_rssi;
					sec_rssi_index = max_rssi_index;
					tmp_max_rssi = cur_rf_rssi;
					max_rssi_index = i;
				}
				else if(cur_rf_rssi == tmp_max_rssi)
				{	// let sec and min point to the different index
					tmp_sec_rssi = cur_rf_rssi;
					sec_rssi_index = i;
				}
				else if((cur_rf_rssi < tmp_max_rssi) &&(cur_rf_rssi > tmp_sec_rssi))
				{
					tmp_sec_rssi = cur_rf_rssi;
					sec_rssi_index = i;
				}
				else if(cur_rf_rssi == tmp_sec_rssi)
				{
					if(tmp_sec_rssi == tmp_min_rssi)
					{	// let sec and min point to the different index
						tmp_sec_rssi = cur_rf_rssi;
						sec_rssi_index = i;
					}
					else
					{
						// This case we don't need to set any index
					}
				}
				else if((cur_rf_rssi < tmp_sec_rssi) && (cur_rf_rssi > tmp_min_rssi))
				{
					// This case we don't need to set any index
				}
				else if(cur_rf_rssi == tmp_min_rssi)
				{
					if(tmp_sec_rssi == tmp_min_rssi)
					{	// let sec and min point to the different index
						tmp_min_rssi = cur_rf_rssi;
						min_rssi_index = i;
					}
					else
					{
						// This case we don't need to set any index
					}
				}
				else if(cur_rf_rssi < tmp_min_rssi)
				{
					tmp_min_rssi = cur_rf_rssi;
					min_rssi_index = i;
				}
			}
		}
	}

	rf_num = 0;
	// decide max/sec/min cck pwdb index
	if(DM_RxPathSelTable.cck_method == CCK_Rx_Version_2)
	{
		for (i=0; i<RF90_PATH_MAX; i++)
		{
			if(priv->brfpath_rxenable[i])
			{
				rf_num++;
				cur_cck_pwdb =  DM_RxPathSelTable.cck_pwdb_sta[i];

				if(rf_num == 1)	// find first enabled rf path and the rssi values
				{	//initialize, set all rssi index to the same one
					cck_rx_ver2_max_index = cck_rx_ver2_min_index = cck_rx_ver2_sec_index = i;
					tmp_cck_max_pwdb = tmp_cck_min_pwdb = tmp_cck_sec_pwdb = cur_cck_pwdb;
				}
				else if(rf_num == 2)
				{	// we pick up the max index first, and let sec and min to be the same one
					if(cur_cck_pwdb >= tmp_cck_max_pwdb)
					{
						tmp_cck_max_pwdb = cur_cck_pwdb;
						cck_rx_ver2_max_index = i;
					}
					else
					{
						tmp_cck_sec_pwdb = tmp_cck_min_pwdb = cur_cck_pwdb;
						cck_rx_ver2_sec_index = cck_rx_ver2_min_index = i;
					}
				}
				else
				{
					if(cur_cck_pwdb > tmp_cck_max_pwdb)
					{
						tmp_cck_sec_pwdb = tmp_cck_max_pwdb;
						cck_rx_ver2_sec_index = cck_rx_ver2_max_index;
						tmp_cck_max_pwdb = cur_cck_pwdb;
						cck_rx_ver2_max_index = i;
					}
					else if(cur_cck_pwdb == tmp_cck_max_pwdb)
					{	// let sec and min point to the different index
						tmp_cck_sec_pwdb = cur_cck_pwdb;
						cck_rx_ver2_sec_index = i;
					}
					else if((cur_cck_pwdb < tmp_cck_max_pwdb) &&(cur_cck_pwdb > tmp_cck_sec_pwdb))
					{
						tmp_cck_sec_pwdb = cur_cck_pwdb;
						cck_rx_ver2_sec_index = i;
					}
					else if(cur_cck_pwdb == tmp_cck_sec_pwdb)
					{
						if(tmp_cck_sec_pwdb == tmp_cck_min_pwdb)
						{	// let sec and min point to the different index
							tmp_cck_sec_pwdb = cur_cck_pwdb;
							cck_rx_ver2_sec_index = i;
						}
						else
						{
							// This case we don't need to set any index
						}
					}
					else if((cur_cck_pwdb < tmp_cck_sec_pwdb) && (cur_cck_pwdb > tmp_cck_min_pwdb))
					{
						// This case we don't need to set any index
					}
					else if(cur_cck_pwdb == tmp_cck_min_pwdb)
					{
						if(tmp_cck_sec_pwdb == tmp_cck_min_pwdb)
						{	// let sec and min point to the different index
							tmp_cck_min_pwdb = cur_cck_pwdb;
							cck_rx_ver2_min_index = i;
						}
						else
						{
							// This case we don't need to set any index
						}
					}
					else if(cur_cck_pwdb < tmp_cck_min_pwdb)
					{
						tmp_cck_min_pwdb = cur_cck_pwdb;
						cck_rx_ver2_min_index = i;
					}
				}

			}
		}
	}


	// Set CCK Rx path
	// reg0xA07[3:2]=cck default rx path, reg0xa07[1:0]=cck optional rx path.
	update_cck_rx_path = 0;
	if(DM_RxPathSelTable.cck_method == CCK_Rx_Version_2)
	{
		cck_default_Rx = cck_rx_ver2_max_index;
		cck_optional_Rx = cck_rx_ver2_sec_index;
		if(tmp_cck_max_pwdb != -64)
			update_cck_rx_path = 1;
	}

	if(tmp_min_rssi < DM_RxPathSelTable.SS_TH_low && disabled_rf_cnt < 2)
	{
		if((tmp_max_rssi - tmp_min_rssi) >= DM_RxPathSelTable.diff_TH)
		{
			//record the enabled rssi threshold
			DM_RxPathSelTable.rf_enable_rssi_th[min_rssi_index] = tmp_max_rssi+5;
			//disable the BB Rx path, OFDM
			rtl8192_setBBreg(dev, rOFDM0_TRxPathEnable, 0x1<<min_rssi_index, 0x0);	// 0xc04[3:0]
			rtl8192_setBBreg(dev, rOFDM1_TRxPathEnable, 0x1<<min_rssi_index, 0x0);	// 0xd04[3:0]
			disabled_rf_cnt++;
		}
		if(DM_RxPathSelTable.cck_method == CCK_Rx_Version_1)
		{
			cck_default_Rx = max_rssi_index;
			cck_optional_Rx = sec_rssi_index;
			if(tmp_max_rssi)
				update_cck_rx_path = 1;
		}
	}

	if(update_cck_rx_path)
	{
		DM_RxPathSelTable.cck_Rx_path = (cck_default_Rx<<2)|(cck_optional_Rx);
		rtl8192_setBBreg(dev, rCCK0_AFESetting, 0x0f000000, DM_RxPathSelTable.cck_Rx_path);
	}

	if(DM_RxPathSelTable.disabledRF)
	{
		for(i=0; i<4; i++)
		{
			if((DM_RxPathSelTable.disabledRF>>i) & 0x1)	//disabled rf
			{
				if(tmp_max_rssi >= DM_RxPathSelTable.rf_enable_rssi_th[i])
				{
					//enable the BB Rx path
					//DbgPrint("RF-%d is enabled. \n", 0x1<<i);
					rtl8192_setBBreg(dev, rOFDM0_TRxPathEnable, 0x1<<i, 0x1);	// 0xc04[3:0]
					rtl8192_setBBreg(dev, rOFDM1_TRxPathEnable, 0x1<<i, 0x1);	// 0xd04[3:0]
					DM_RxPathSelTable.rf_enable_rssi_th[i] = 100;
					disabled_rf_cnt--;
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Function:	dm_check_rx_path_selection()
//
// Overview:	Call a workitem to check current RXRF path and Rx Path selection by RSSI.
//
// Input:		NONE
//
// Output:		NONE
//
// Return:		NONE
//
// Revised History:
//	When		Who		Remark
//	05/28/2008	amy		Create Version 0 porting from windows code.
//
//---------------------------------------------------------------------------*/
static	void	dm_check_rx_path_selection(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	queue_delayed_work_rsl(priv->priv_wq,&priv->rfpath_check_wq,0);
}	// dm_CheckRxRFPath */


static void dm_init_fsync (struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);

	priv->rtllib->fsync_time_interval = 500;
	priv->rtllib->fsync_rate_bitmap = 0x0f000800;
	priv->rtllib->fsync_rssi_threshold = 30;
#ifdef RTL8190P
	priv->rtllib->bfsync_enable = true;
#elif defined RTL8192E || defined RTL8192SE
	priv->rtllib->bfsync_enable = false;
#endif
	priv->rtllib->fsync_multiple_timeinterval = 3;
	priv->rtllib->fsync_firstdiff_ratethreshold= 100;
	priv->rtllib->fsync_seconddiff_ratethreshold= 200;
	priv->rtllib->fsync_state = Default_Fsync;

#ifdef RTL8192SE
	priv->framesyncMonitor = 0;	// current default 0xc38 monitor on
#elif defined RTL8192E || defined RTL8190P
	priv->framesyncMonitor = 1;	// current default 0xc38 monitor on
#endif

	setup_timer(&priv->fsync_timer, dm_fsync_timer_callback,(unsigned long) dev);
}


static void dm_deInit_fsync(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	del_timer_sync(&priv->fsync_timer);
}

extern void dm_fsync_timer_callback(unsigned long data)
{
	struct net_device *dev = (struct net_device *)data;
	struct r8192_priv *priv = rtllib_priv((struct net_device *)data);
	u32 rate_index, rate_count = 0, rate_count_diff=0;
	bool		bSwitchFromCountDiff = false;
	bool		bDoubleTimeInterval = false;

	if(	priv->rtllib->state == RTLLIB_LINKED &&
		priv->rtllib->bfsync_enable &&
		(priv->rtllib->pHTInfo->IOTAction & HT_IOT_ACT_CDD_FSYNC))//for broadcom chip
	{
		 // Count rate 54, MCS [7], [12, 13, 14, 15]
		u32 rate_bitmap;
		for(rate_index = 0; rate_index <= 27; rate_index++)
		{
			rate_bitmap  = 1 << rate_index;
			if(priv->rtllib->fsync_rate_bitmap &  rate_bitmap)
				rate_count+= priv->stats.received_rate_histogram[1][rate_index];
		}

		if(rate_count < priv->rate_record)
			rate_count_diff = 0xffffffff - rate_count + priv->rate_record;
		else
			rate_count_diff = rate_count - priv->rate_record;
		if(rate_count_diff < priv->rateCountDiffRecord)
		{

			u32 DiffNum = priv->rateCountDiffRecord - rate_count_diff;
			// Contiune count
			if(DiffNum >= priv->rtllib->fsync_seconddiff_ratethreshold)
				priv->ContiuneDiffCount++;
			else
				priv->ContiuneDiffCount = 0;

			// Contiune count over
			if(priv->ContiuneDiffCount >=2)
			{
				bSwitchFromCountDiff = true;
				priv->ContiuneDiffCount = 0;
			}
		}
		else
		{
			// Stop contiune count
			priv->ContiuneDiffCount = 0;
		}

		//If Count diff <= FsyncRateCountThreshold
		if(rate_count_diff <= priv->rtllib->fsync_firstdiff_ratethreshold)
		{
			bSwitchFromCountDiff = true;
			priv->ContiuneDiffCount = 0;
		}
		priv->rate_record = rate_count;
		priv->rateCountDiffRecord = rate_count_diff;
		RT_TRACE(COMP_HALDM, "rateRecord %d rateCount %d, rateCountdiff %d bSwitchFsync %d\n", priv->rate_record, rate_count, rate_count_diff , priv->bswitch_fsync);
		// if we never receive those mcs rate and rssi > 30 % then switch fsyn
		if(priv->undecorated_smoothed_pwdb > priv->rtllib->fsync_rssi_threshold && bSwitchFromCountDiff)
		{
			bDoubleTimeInterval = true;
			priv->bswitch_fsync = !priv->bswitch_fsync;
			if(priv->bswitch_fsync)
			{
			#ifdef RTL8190P
				write_nic_byte(dev,0xC36, 0x00);
#elif defined RTL8192E
				write_nic_byte(dev,0xC36, 0x1c);
			#endif
				write_nic_byte(dev, 0xC3e, 0x90);
			}
			else
			{
			#ifdef RTL8190P
				write_nic_byte(dev, 0xC36, 0x40);
			#else
				write_nic_byte(dev, 0xC36, 0x5c);
			#endif
				write_nic_byte(dev, 0xC3e, 0x96);
			}
		}
		else if(priv->undecorated_smoothed_pwdb <= priv->rtllib->fsync_rssi_threshold)
		{
			if(priv->bswitch_fsync)
			{
				priv->bswitch_fsync  = false;
			#ifdef RTL8190P
				write_nic_byte(dev, 0xC36, 0x40);
#elif defined RTL8192E
				write_nic_byte(dev, 0xC36, 0x5c);
			#endif
				write_nic_byte(dev, 0xC3e, 0x96);
			}
		}
		if(bDoubleTimeInterval){
			if(timer_pending(&priv->fsync_timer))
				del_timer_sync(&priv->fsync_timer);
			priv->fsync_timer.expires = jiffies + MSECS(priv->rtllib->fsync_time_interval*priv->rtllib->fsync_multiple_timeinterval);
			add_timer(&priv->fsync_timer);
		}
		else{
			if(timer_pending(&priv->fsync_timer))
				del_timer_sync(&priv->fsync_timer);
			priv->fsync_timer.expires = jiffies + MSECS(priv->rtllib->fsync_time_interval);
			add_timer(&priv->fsync_timer);
		}
	}
	else
	{
		// Let Register return to default value;
		if(priv->bswitch_fsync)
		{
			priv->bswitch_fsync  = false;
		#ifdef RTL8190P
			write_nic_byte(dev, 0xC36, 0x40);
#elif defined RTL8192E
			write_nic_byte(dev, 0xC36, 0x5c);
		#endif
			write_nic_byte(dev, 0xC3e, 0x96);
		}
		priv->ContiuneDiffCount = 0;
	#ifdef RTL8190P
		write_nic_dword(dev, rOFDM0_RxDetector2, 0x164052cd);
#elif defined RTL8192E
		write_nic_dword(dev, rOFDM0_RxDetector2, 0x465c52cd);
	#endif
	}
	RT_TRACE(COMP_HALDM, "ContiuneDiffCount %d\n", priv->ContiuneDiffCount);
	RT_TRACE(COMP_HALDM, "rateRecord %d rateCount %d, rateCountdiff %d bSwitchFsync %d\n", priv->rate_record, rate_count, rate_count_diff , priv->bswitch_fsync);
}

static void dm_StartHWFsync(struct net_device *dev)
{
	RT_TRACE(COMP_HALDM, "%s\n", __FUNCTION__);
#if defined RTL8192E //|| defined RTL8190P
	write_nic_dword(dev, rOFDM0_RxDetector2, 0x465c12cf);
	write_nic_byte(dev, 0xc3b, 0x41);
#elif defined RTL8192SE
	write_nic_byte(dev, rOFDM0_RxDetector3, 0x96);
#endif
}

static void dm_EndHWFsync(struct net_device *dev)
{
	RT_TRACE(COMP_HALDM,"%s\n", __FUNCTION__);
#if defined RTL8192E //|| defined RTL8190P
	write_nic_dword(dev, rOFDM0_RxDetector2, 0x465c52cd);
	write_nic_byte(dev, 0xc3b, 0x49);
#elif defined RTL8192SE
	write_nic_byte(dev, rOFDM0_RxDetector3, 0x94);
#endif

}

static void dm_EndSWFsync(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);

	RT_TRACE(COMP_HALDM, "%s\n", __FUNCTION__);
	del_timer_sync(&(priv->fsync_timer));

	// Let Register return to default value;
	if(priv->bswitch_fsync)
	{
		priv->bswitch_fsync  = false;

		#ifdef RTL8190P
			write_nic_byte(dev, 0xC36, 0x40);
#elif defined RTL8192E
		write_nic_byte(dev, 0xC36, 0x5c);
#endif

		write_nic_byte(dev, 0xC3e, 0x96);
	}

	priv->ContiuneDiffCount = 0;
#ifdef RTL8192E
	write_nic_dword(dev, rOFDM0_RxDetector2, 0x465c52cd);
#endif

}

static void dm_StartSWFsync(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	u32			rateIndex;
	u32			rateBitmap;

	RT_TRACE(COMP_HALDM,"%s\n", __FUNCTION__);
	// Initial rate record to zero, start to record.
	priv->rate_record = 0;
	// Initial contiune diff count to zero, start to record.
	priv->ContiuneDiffCount = 0;
	priv->rateCountDiffRecord = 0;
	priv->bswitch_fsync  = false;

	if(priv->rtllib->mode == WIRELESS_MODE_N_24G)
	{
		priv->rtllib->fsync_firstdiff_ratethreshold= 600;
		priv->rtllib->fsync_seconddiff_ratethreshold = 0xffff;
	}
	else
	{
		priv->rtllib->fsync_firstdiff_ratethreshold= 200;
		priv->rtllib->fsync_seconddiff_ratethreshold = 200;
	}
	for(rateIndex = 0; rateIndex <= 27; rateIndex++)
	{
		rateBitmap  = 1 << rateIndex;
		if(priv->rtllib->fsync_rate_bitmap &  rateBitmap)
			priv->rate_record += priv->stats.received_rate_histogram[1][rateIndex];
	}
	if(timer_pending(&priv->fsync_timer))
		del_timer_sync(&priv->fsync_timer);
	priv->fsync_timer.expires = jiffies + MSECS(priv->rtllib->fsync_time_interval);
	add_timer(&priv->fsync_timer);

#ifdef RTL8192E
	write_nic_dword(dev, rOFDM0_RxDetector2, 0x465c12cd);
#endif

}

void dm_check_fsync(struct net_device *dev)
{
#define	RegC38_Default				0
#define	RegC38_NonFsync_Other_AP	1
#define	RegC38_Fsync_AP_BCM		2
	struct r8192_priv *priv = rtllib_priv(dev);
	//u32			framesyncC34;
	static u8		reg_c38_State=RegC38_Default;
	static u32	reset_cnt=0;

	RT_TRACE(COMP_HALDM, "RSSI %d TimeInterval %d MultipleTimeInterval %d\n", priv->rtllib->fsync_rssi_threshold, priv->rtllib->fsync_time_interval, priv->rtllib->fsync_multiple_timeinterval);
	RT_TRACE(COMP_HALDM, "RateBitmap 0x%x FirstDiffRateThreshold %d SecondDiffRateThreshold %d\n", priv->rtllib->fsync_rate_bitmap, priv->rtllib->fsync_firstdiff_ratethreshold, priv->rtllib->fsync_seconddiff_ratethreshold);

	if(	priv->rtllib->state == RTLLIB_LINKED &&
		(priv->rtllib->pHTInfo->IOTAction & HT_IOT_ACT_CDD_FSYNC))//for broadcom chip
	{
		if(priv->rtllib->bfsync_enable == 0)
		{
			switch(priv->rtllib->fsync_state)
			{
				case Default_Fsync:
					dm_StartHWFsync(dev);
					priv->rtllib->fsync_state = HW_Fsync;
					break;
				case SW_Fsync:
					dm_EndSWFsync(dev);
					dm_StartHWFsync(dev);
					priv->rtllib->fsync_state = HW_Fsync;
					break;
				case HW_Fsync:
				default:
					break;
			}
		}
		else
		{
			switch(priv->rtllib->fsync_state)
			{
				case Default_Fsync:
					dm_StartSWFsync(dev);
					priv->rtllib->fsync_state = SW_Fsync;
					break;
				case HW_Fsync:
					dm_EndHWFsync(dev);
					dm_StartSWFsync(dev);
					priv->rtllib->fsync_state = SW_Fsync;
					break;
				case SW_Fsync:
				default:
					break;

			}
		}
		if(priv->framesyncMonitor)
		{
			if(reg_c38_State != RegC38_Fsync_AP_BCM)
			{	//For broadcom AP we write different default value
				#ifdef RTL8190P
					write_nic_byte(dev, rOFDM0_RxDetector3, 0x15);
				#else
					write_nic_byte(dev, rOFDM0_RxDetector3, 0x95);
				#endif

				reg_c38_State = RegC38_Fsync_AP_BCM;
			}
		}
	}
	else
	{
		switch(priv->rtllib->fsync_state)
		{
			case HW_Fsync:
				dm_EndHWFsync(dev);
				priv->rtllib->fsync_state = Default_Fsync;
				break;
			case SW_Fsync:
				dm_EndSWFsync(dev);
				priv->rtllib->fsync_state = Default_Fsync;
				break;
			case Default_Fsync:
			default:
				break;
		}

		if(priv->framesyncMonitor)
		{
			if(priv->rtllib->state == RTLLIB_LINKED)
			{
				if(priv->undecorated_smoothed_pwdb <= RegC38_TH)
				{
					if(reg_c38_State != RegC38_NonFsync_Other_AP)
					{
						#ifdef RTL8190P
							write_nic_byte(dev, rOFDM0_RxDetector3, 0x10);
						#else
							write_nic_byte(dev, rOFDM0_RxDetector3, 0x90);
						#endif

						reg_c38_State = RegC38_NonFsync_Other_AP;
					#if 0//cosa
						if (dev->HardwareType == HARDWARE_TYPE_RTL8190P)
							DbgPrint("Fsync is idle, rssi<=35, write 0xc38 = 0x%x \n", 0x10);
						else
							DbgPrint("Fsync is idle, rssi<=35, write 0xc38 = 0x%x \n", 0x90);
					#endif
					}
				}
				else if(priv->undecorated_smoothed_pwdb >= (RegC38_TH+5))
				{
					if(reg_c38_State)
					{
						write_nic_byte(dev, rOFDM0_RxDetector3, priv->framesync);
						reg_c38_State = RegC38_Default;
						//DbgPrint("Fsync is idle, rssi>=40, write 0xc38 = 0x%x \n", pHalData->framesync);
					}
				}
			}
			else
			{
				if(reg_c38_State)
				{
					write_nic_byte(dev, rOFDM0_RxDetector3, priv->framesync);
					reg_c38_State = RegC38_Default;
					//DbgPrint("Fsync is idle, not connected, write 0xc38 = 0x%x \n", pHalData->framesync);
				}
			}
		}
	}
	if(priv->framesyncMonitor)
	{
		if(priv->reset_count != reset_cnt)
		{	//After silent reset, the reg_c38_State will be returned to default value
			write_nic_byte(dev, rOFDM0_RxDetector3, priv->framesync);
			reg_c38_State = RegC38_Default;
			reset_cnt = priv->reset_count;
			//DbgPrint("reg_c38_State = 0 for silent reset. \n");
		}
	}
	else
	{
		if(reg_c38_State)
		{
			write_nic_byte(dev, rOFDM0_RxDetector3, priv->framesync);
			reg_c38_State = RegC38_Default;
			//DbgPrint("framesync no monitor, write 0xc38 = 0x%x \n", pHalData->framesync);
		}
	}
}

#if 0
//-----------------------------------------------------------------------------
// Function:	DM_CheckLBusStatus()
//
// Overview:	For 9x series, we must make sure LBUS is active for IO.
//
// Input:		NONE
//
// Output:		NONE
//
// Return:		NONE
//
// Revised History:
//	When		Who		Remark
//	02/22/2008	MHC		Create Version 0.
//
//---------------------------------------------------------------------------*/
extern	s1Byte	DM_CheckLBusStatus(IN	PADAPTER	dev)
{
	PMGNT_INFO	pMgntInfo=&dev->MgntInfo;

#if (HAL_CODE_BASE & RTL819X)

#if (HAL_CODE_BASE == RTL8192)

#if( DEV_BUS_TYPE==PCI_INTERFACE)
	//return (pMgntInfo->bLbusEnable);	// For debug only
	return true;
#endif

#if( DEV_BUS_TYPE==USB_INTERFACE)
	return true;
#endif

#endif	// #if (HAL_CODE_BASE == RTL8192)

#if (HAL_CODE_BASE == RTL8190)
	return true;
#endif	// #if (HAL_CODE_BASE == RTL8190)

#endif	// #if (HAL_CODE_BASE & RTL819X)
}	// DM_CheckLBusStatus */

#endif

//-----------------------------------------------------------------------------
// Function:	dm_shadow_init()
//
// Overview:	Store all NIC MAC/BB register content.
//
// Input:		NONE
//
// Output:		NONE
//
// Return:		NONE
//
// Revised History:
//	When		Who		Remark
//	05/29/2008	amy		Create Version 0 porting from windows code.
//
//---------------------------------------------------------------------------*/
extern void dm_shadow_init(struct net_device *dev)
{
	u8	page;
	u16	offset;

	for (page = 0; page < 5; page++)
		for (offset = 0; offset < 256; offset++)
		{
			dm_shadow[page][offset] = read_nic_byte(dev, offset+page*256);
			//DbgPrint("P-%d/O-%02x=%02x\r\n", page, offset, DM_Shadow[page][offset]);
		}

	for (page = 8; page < 11; page++)
		for (offset = 0; offset < 256; offset++)
			dm_shadow[page][offset] = read_nic_byte(dev, offset+page*256);

	for (page = 12; page < 15; page++)
		for (offset = 0; offset < 256; offset++)
			dm_shadow[page][offset] = read_nic_byte(dev, offset+page*256);

}   // dm_shadow_init */

/*---------------------------Define function prototype------------------------*/
//-----------------------------------------------------------------------------
// Function:	DM_DynamicTxPower()
//
// Overview:	Detect Signal strength to control TX Registry
//			Tx Power Control For Near/Far Range
//
// Input:		NONE
//
// Output:		NONE
//
// Return:		NONE
//
// Revised History:
//	When		Who		Remark
//	03/06/2008	Jacken	Create Version 0.
//
//---------------------------------------------------------------------------*/
static void dm_init_dynamic_txpower(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);

	//Initial TX Power Control for near/far range , add by amy 2008/05/15, porting from windows code.
#ifdef RTL8192SE
	if(((priv->pFirmware->FirmwareVersion) >= 60) &&
		(priv->DM_Type == DM_Type_ByDriver)){
		priv->rtllib->bdynamic_txpower_enable = true;
		RT_TRACE(COMP_INIT, "Dynamic Tx Power is enabled by Driver \n");
	} else {
		priv->rtllib->bdynamic_txpower_enable = false;
		RT_TRACE(COMP_INIT, "Dynamic Tx Power is enabled by FW \n");
	}

	priv->LastDTPLvl = TxHighPwrLevel_Normal;
	priv->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;

#elif defined RTL8190P || defined RTL8192E
	priv->rtllib->bdynamic_txpower_enable = true;    //Default to enable Tx Power Control
	priv->bLastDTPFlag_High = false;
	priv->bLastDTPFlag_Low = false;
	priv->bDynamicTxHighPower = false;
	priv->bDynamicTxLowPower = false;
#endif
}

#if defined RTL8190P || defined RTL8192E
static void dm_dynamic_txpower(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	unsigned int txhipower_threshhold=0;
        unsigned int txlowpower_threshold=0;
	if(priv->rtllib->bdynamic_txpower_enable != true)
	{
		priv->bDynamicTxHighPower = false;
		priv->bDynamicTxLowPower = false;
		return;
	}
	//printk("priv->rtllib->current_network.unknown_cap_exist is %d ,priv->rtllib->current_network.broadcom_cap_exist is %d\n",priv->rtllib->current_network.unknown_cap_exist,priv->rtllib->current_network.broadcom_cap_exist);
        //if((priv->rtllib->current_network.atheros_cap_exist ) && (priv->rtllib->mode == IEEE_G)){
        if((priv->rtllib->pHTInfo->IOTPeer == HT_IOT_PEER_ATHEROS) && (priv->rtllib->mode == IEEE_G)){
		txhipower_threshhold = TX_POWER_ATHEROAP_THRESH_HIGH;
		txlowpower_threshold = TX_POWER_ATHEROAP_THRESH_LOW;
	}
	else
	{
		txhipower_threshhold = TX_POWER_NEAR_FIELD_THRESH_HIGH;
		txlowpower_threshold = TX_POWER_NEAR_FIELD_THRESH_LOW;
	}

//	printk("=======>%s(): txhipower_threshhold is %d,txlowpower_threshold is %d\n",__FUNCTION__,txhipower_threshhold,txlowpower_threshold);

	RT_TRACE(COMP_TXAGC,"priv->undecorated_smoothed_pwdb = %ld \n" , priv->undecorated_smoothed_pwdb);

	if(priv->rtllib->state == RTLLIB_LINKED)
	{
		if(priv->undecorated_smoothed_pwdb >= txhipower_threshhold)
		{
			priv->bDynamicTxHighPower = true;
			priv->bDynamicTxLowPower = false;
		}
		else
		{
			// high power state check
			if(priv->undecorated_smoothed_pwdb < txlowpower_threshold && priv->bDynamicTxHighPower == true)
			{
				priv->bDynamicTxHighPower = false;
			}
			// low power state check
			if(priv->undecorated_smoothed_pwdb < 35)
			{
				priv->bDynamicTxLowPower = true;
			}
			else if(priv->undecorated_smoothed_pwdb >= 40)
			{
				priv->bDynamicTxLowPower = false;
			}
		}
	}
	else
	{
		//pHalData->bTXPowerCtrlforNearFarRange = !pHalData->bTXPowerCtrlforNearFarRange;
		priv->bDynamicTxHighPower = false;
		priv->bDynamicTxLowPower = false;
	}

	if( (priv->bDynamicTxHighPower != priv->bLastDTPFlag_High ) ||
		(priv->bDynamicTxLowPower != priv->bLastDTPFlag_Low ) )
	{
		RT_TRACE(COMP_TXAGC,"SetTxPowerLevel8190()  channel = %d \n" , priv->rtllib->current_network.channel);

		rtl8192_phy_setTxPower(dev,priv->rtllib->current_network.channel);
	}
	priv->bLastDTPFlag_High = priv->bDynamicTxHighPower;
	priv->bLastDTPFlag_Low = priv->bDynamicTxLowPower;

}	// dm_dynamic_txpower */
#elif defined RTL8192SE
static void dm_dynamic_txpower(struct net_device *dev)
{
//#if (HAL_CODE_BASE == RTL8192_S)
	struct r8192_priv *priv = rtllib_priv(dev);
	long				UndecoratedSmoothedPWDB;

	// If dynamic high power is disabled.
	if( (priv->rtllib->bdynamic_txpower_enable != true) ||
		(priv->DMFlag & HAL_DM_HIPWR_DISABLE) ||
		priv->rtllib->pHTInfo->IOTAction & HT_IOT_ACT_DISABLE_HIGH_POWER) {
		priv->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;
		return;
	}

	// STA not connected and AP not connected
	if((!priv->rtllib->state != RTLLIB_LINKED) &&
		(priv->EntryMinUndecoratedSmoothedPWDB == 0)) {
		RT_TRACE(COMP_POWER, "Not connected to any \n");
		priv->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;
		return;
	}

	if(priv->rtllib->state == RTLLIB_LINKED) {// Default port
		if(priv->OpMode == RT_OP_MODE_IBSS) {
			UndecoratedSmoothedPWDB = priv->EntryMinUndecoratedSmoothedPWDB;
			RT_TRACE(COMP_POWER, "AP Client PWDB = %ld \n", UndecoratedSmoothedPWDB);
		} else {
			UndecoratedSmoothedPWDB = priv->undecorated_smoothed_pwdb;
			RT_TRACE(COMP_POWER, "STA Default Port PWDB = %ld \n", UndecoratedSmoothedPWDB);
		}
	} else {// associated entry pwdb
		UndecoratedSmoothedPWDB = priv->EntryMinUndecoratedSmoothedPWDB;
		RT_TRACE(COMP_POWER, "AP Ext Port PWDB = %ld \n", UndecoratedSmoothedPWDB);
	}

	if(UndecoratedSmoothedPWDB >= TX_POWER_NEAR_FIELD_THRESH_LVL2)	{
		priv->DynamicTxHighPowerLvl = TxHighPwrLevel_Level2;
		RT_TRACE(COMP_POWER, "TxHighPwrLevel_Level2 (TxPwr=0x0)\n");
	} else if((UndecoratedSmoothedPWDB < (TX_POWER_NEAR_FIELD_THRESH_LVL2-3)) &&
		(UndecoratedSmoothedPWDB >= TX_POWER_NEAR_FIELD_THRESH_LVL1) ) {
		priv->DynamicTxHighPowerLvl = TxHighPwrLevel_Level1;
		RT_TRACE(COMP_POWER, "TxHighPwrLevel_Level1 (TxPwr=0x10)\n");
	} else if(UndecoratedSmoothedPWDB < (TX_POWER_NEAR_FIELD_THRESH_LVL1-3)) {
		priv->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;
		RT_TRACE(COMP_POWER, "TxHighPwrLevel_Normal\n");
	}

	if( (priv->DynamicTxHighPowerLvl != priv->LastDTPLvl) ) {
		RT_TRACE(COMP_POWER, "PHY_SetTxPowerLevel8192S() Channel = %d \n" , priv->rtllib->current_network.channel);
		rtl8192_phy_setTxPower(dev, priv->rtllib->current_network.channel);
	}
	priv->LastDTPLvl = priv->DynamicTxHighPowerLvl;
//#endif
}	// dm_dynamic_txpower */
#endif

//
//added by vivi, for read tx rate and retrycount
// It is supported on 8190p/8192e/8192u only
//
static void dm_check_txrateandretrycount(struct net_device * dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	struct rtllib_device* ieee = priv->rtllib;

	//for 11n tx rate
#ifdef RTL8192SE
	static u8 LegacyRateSet[12] = {0x02 , 0x04 , 0x0b , 0x16 , 0x0c , 0x12 , 0x18 , 0x24 , 0x30 , 0x48 , 0x60 , 0x6c};
	u8 RateIdx = 0;
	RateIdx = read_nic_byte(dev, TX_RATE_REG);

	if(ieee->softmac_stats.CurrentShowTxate < 76)
		ieee->softmac_stats.CurrentShowTxate = (RateIdx<12)?(LegacyRateSet[RateIdx]):((RateIdx-12)|0x80);
	else
		ieee->softmac_stats.CurrentShowTxate = 0;
#else
	ieee->softmac_stats.CurrentShowTxate = read_nic_byte(dev, Current_Tx_Rate_Reg);
#endif

	//for initial tx rate
//	priv->stats.last_packet_rate = read_nic_byte(dev, Initial_Tx_Rate_Reg);
	ieee->softmac_stats.last_packet_rate = read_nic_byte(dev ,Initial_Tx_Rate_Reg);

	//for tx tx retry count
//	priv->stats.txretrycount = read_nic_dword(dev, Tx_Retry_Count_Reg);
	ieee->softmac_stats.txretrycount = read_nic_dword(dev, Tx_Retry_Count_Reg);
}

static void dm_send_rssi_tofw(struct net_device *dev)
{
#ifndef RTL8192SE
	DCMD_TXCMD_T			tx_cmd;
	struct r8192_priv *priv = rtllib_priv(dev);

	// If we test chariot, we should stop the TX command ?
	// Because 92E will always silent reset when we send tx command. We use register
	// 0x1e0(byte) to botify driver.
	write_nic_byte(dev, DRIVER_RSSI, (u8)priv->undecorated_smoothed_pwdb);
	return;
	tx_cmd.Op		= TXCMD_SET_RX_RSSI;
	tx_cmd.Length	= 4;
	tx_cmd.Value		= priv->undecorated_smoothed_pwdb;

	cmpk_message_handle_tx(dev, (u8*)&tx_cmd,
								DESC_PACKET_TYPE_INIT, sizeof(DCMD_TXCMD_T));
#endif
}

/*-----------------------------------------------------------------------------
 * Function:	dm_RefreshRateAdaptiveMask()
 *
 * Overview:	Update rate table mask according to rssi
 *
 * Input:		NONE
 *
 * Output:		NONE
 *
 * Return:		NONE
 *
 * Revised History:
 *	When		Who		Remark
 *	05/27/2009	hpfan	Create Version 0.
 *
 *---------------------------------------------------------------------------*/
#ifdef RTL8192SE
static void dm_RefreshRateAdaptiveMask(struct net_device *dev)
{
	struct r8192_priv *	priv = rtllib_priv(dev);
	prate_adaptive	pRA = (prate_adaptive)&priv->rate_adaptive;
	u32				LowRSSIThreshForRA = 0, HighRSSIThreshForRA = 0;
	u8				rssi_level;

	if(!priv->up)
	{
		RT_TRACE(COMP_RATE, "<---- dm_RefreshRateAdaptiveMask(): driver is going to unload\n");
		return;
	}

	if(!priv->rtllib->bUseRAMask)
	{
		RT_TRACE(COMP_RATE, "<---- dm_RefreshRateAdaptiveMask(): driver does not control rate adaptive mask\n");
		return;
	}

	// Inform fw driver control dm
	if(priv->pFirmware->FirmwareVersion >= 61 && !priv->bInformFWDriverControlDM)
	{
		RT_TRACE(COMP_RATE, "<---- dm_RefreshRateAdaptiveMask(): inform fw driver control dm\n");
		priv->rtllib->SetFwCmdHandler(dev, FW_CMD_CTRL_DM_BY_DRIVER);
		priv->bInformFWDriverControlDM = true;
	}

	// if default port is connected, update RA table for default port (infrastructure mode only)

	if(priv->rtllib->state == RTLLIB_LINKED )
	{

		// decide rastate according to rssi
		switch (pRA->PreRATRState)
		{
			case DM_RATR_STA_HIGH:
			{
				HighRSSIThreshForRA = 50;
				LowRSSIThreshForRA = 20;
			}
			break;

			case DM_RATR_STA_MIDDLE:
			{
				HighRSSIThreshForRA = 55;
				LowRSSIThreshForRA = 20;
			}
			break;

			case DM_RATR_STA_LOW:
			{
				HighRSSIThreshForRA = 50;
				LowRSSIThreshForRA = 25;
			}
			break;

			default:
			{
				HighRSSIThreshForRA = 50;
				LowRSSIThreshForRA = 20;
			}
		}

		if(priv->undecorated_smoothed_pwdb > (long)HighRSSIThreshForRA)
		{
			pRA->ratr_state = DM_RATR_STA_HIGH;
			rssi_level = 1;
		}
		else if(priv->undecorated_smoothed_pwdb > (long)LowRSSIThreshForRA)
		{
			pRA->ratr_state = DM_RATR_STA_MIDDLE;
			rssi_level = 2;
		}
		else
		{
			pRA->ratr_state = DM_RATR_STA_LOW;
			rssi_level = 3;
		}

		if(pRA->PreRATRState != pRA->ratr_state)
		{
			RT_TRACE(COMP_RATE, "RSSI = %ld\n", priv->undecorated_smoothed_pwdb);
			RT_TRACE(COMP_RATE, "RSSI_LEVEL = %d\n", rssi_level);
			RT_TRACE(COMP_RATE, "PreState = %d, CurState = %d\n", pRA->PreRATRState, pRA->ratr_state);
#ifdef _RTL8192_EXT_PATCH_
			priv->rtllib->UpdateHalRAMaskHandler(
									dev,
									false,
									0,
									NULL,
									priv->rtllib->mode,
									rssi_level);
#else
			priv->rtllib->UpdateHalRAMaskHandler(
									dev,
									false,
									0,
									NULL,
									NULL,
									rssi_level);
#endif
			pRA->PreRATRState = pRA->ratr_state;
		}
	}
}
#ifdef _RTL8192_EXT_PATCH_
static	void	dm_refresh_rate_adaptive_mask(struct net_device *dev)
{

	struct r8192_priv *priv = rtllib_priv(dev);
	prate_adaptive	pRA = (prate_adaptive)&priv->rate_adaptive;
	u32	LowRSSIThreshForRA = 0, HighRSSIThreshForRA = 0;
	u8	rssi_level;
#ifdef _RTL8192_EXT_PATCH_
	if((!priv->up) && (!priv->mesh_up))
#else
	if(!priv->up)
#endif
	{
		RT_TRACE(COMP_RATE,"<---- dm_refresh_rate_adaptive_mask(): driver is going to unload\n");
		return;
	}

	if(!priv->rtllib->bUseRAMask)
	{
		RT_TRACE(COMP_RATE, "<---- dm_refresh_rate_adaptive_mask(): driver does not control rate adaptive mask\n");
		return;
	}

	// Inform fw driver control dm
	if(priv->pFirmware->FirmwareVersion >= 61 && !priv->bInformFWDriverControlDM)
	{
		RT_TRACE(COMP_RATE, "<---- dm_refresh_rate_adaptive_mask(): inform fw driver control dm\n");
		priv->rtllib->SetFwCmdHandler(dev, FW_CMD_CTRL_DM_BY_DRIVER);
		priv->bInformFWDriverControlDM = true;
	}

	// if default port is connected, update RA table for default port (infrastructure mode only)
	if((priv->rtllib->state == RTLLIB_LINKED &&
			(priv->rtllib->iw_mode != IW_MODE_MESH)) || ((priv->rtllib->state == RTLLIB_LINKED) && (priv->rtllib->iw_mode == IW_MODE_MESH) && (priv->rtllib->only_mesh == 0)))
	{

		// decide rastate according to rssi
		switch (pRA->PreRATRState)
		{
			case DM_RATR_STA_HIGH:
			{
				HighRSSIThreshForRA = 50;
				LowRSSIThreshForRA = 20;
			}
			break;

			case DM_RATR_STA_MIDDLE:
			{
				HighRSSIThreshForRA = 55;
				LowRSSIThreshForRA = 20;
			}
			break;

			case DM_RATR_STA_LOW:
			{
				HighRSSIThreshForRA = 50;
				LowRSSIThreshForRA = 25;
			}
			break;

			default:
			{
				HighRSSIThreshForRA = 50;
				LowRSSIThreshForRA = 20;
			}
		}

		if(priv->undecorated_smoothed_pwdb > (long)HighRSSIThreshForRA)
		{
			pRA->ratr_state = DM_RATR_STA_HIGH;
			rssi_level = 1;
		}
		else if(priv->undecorated_smoothed_pwdb > (long)LowRSSIThreshForRA)
		{
			pRA->ratr_state = DM_RATR_STA_MIDDLE;
			rssi_level = 2;
		}
		else
		{
			pRA->ratr_state = DM_RATR_STA_LOW;
			rssi_level = 3;
		}
		//printk("===============>rssi_level is %d,priv->rssi_level is %d\n",rssi_level,priv->rssi_level);
		if((pRA->PreRATRState != pRA->ratr_state) || ((pRA->PreRATRState == pRA->ratr_state) && (rssi_level != priv->rssi_level)))
		{
			RT_TRACE(COMP_RATE, "Target AP addr : "MAC_FMT"\n", MAC_ARG(priv->rtllib->current_network.bssid));
			RT_TRACE(COMP_RATE, "RSSI = %ld\n", priv->undecorated_smoothed_pwdb);
			RT_TRACE(COMP_RATE, "RSSI_LEVEL = %d\n", rssi_level);
			RT_TRACE(COMP_RATE, "PreState = %d, CurState = %d\n", pRA->PreRATRState, pRA->ratr_state);
			priv->rtllib->UpdateHalRAMaskHandler(
									dev,
									false,
									0,
									NULL,
									priv->rtllib->mode,
									rssi_level);
			priv->rssi_level = rssi_level;
		//	printk("priv->rssi_level is %d\n",priv->rssi_level);
			pRA->PreRATRState = pRA->ratr_state;
		}
	}

	// if extension port (softap) is started, updaet RA table for more than one clients associate
	if(priv->rtllib->iw_mode == IW_MODE_MESH)
	{
		if(priv->mshobj->ext_refresh_rate_adaptive_mask)
			priv->mshobj->ext_refresh_rate_adaptive_mask(priv);
	}

}
#endif

void Adhoc_InitRateAdaptive(struct net_device *dev,struct sta_info  *pEntry)
{
	prate_adaptive	pRA = (prate_adaptive)&pEntry->rate_adaptive;
	struct r8192_priv *priv = rtllib_priv(dev);

	pRA->ratr_state = DM_RATR_STA_MAX;
	pRA->high2low_rssi_thresh_for_ra = RateAdaptiveTH_High;
	pRA->low2high_rssi_thresh_for_ra20M = RateAdaptiveTH_Low_20M+5;
	pRA->low2high_rssi_thresh_for_ra40M = RateAdaptiveTH_Low_40M+5;

	pRA->high_rssi_thresh_for_ra = RateAdaptiveTH_High+5;
	pRA->low_rssi_thresh_for_ra20M = RateAdaptiveTH_Low_20M;
	pRA->low_rssi_thresh_for_ra40M = RateAdaptiveTH_Low_40M;

	if (priv->rf_type == RF_2T4R)
	{
		// 07/10/08 MH Modify for RA smooth scheme.
		/* 2008/01/11 MH Modify 2T RATR table for different RSSI. */
		//pRA->UpperRSSIThresholdRATR		=	0x0f810000;
		pRA->upper_rssi_threshold_ratr		=	0x8f0f0000;
		//pRA->MiddleRSSIThresholdRATR		=	0x0f81f000;
		pRA->middle_rssi_threshold_ratr		=	0x8d0ff000;
		//pRA->LowRSSIThresholdRATR		=	0x0f81f003;
		pRA->low_rssi_threshold_ratr		=	0x8f0ff003;
		//pRA->LowRSSIThresholdRATR40M	=	0x0f81f007;
		pRA->low_rssi_threshold_ratr_40M	=	0x8f0ff007;
		//pRA->LowRSSIThresholdRATR20M	=	0x0f81f003;
		pRA->low_rssi_threshold_ratr_20M	=	0x8f0ff003;
	}
	else if (priv->rf_type == RF_1T2R)
	{
		pRA->upper_rssi_threshold_ratr		=	0x000f0000;
		pRA->middle_rssi_threshold_ratr		=	0x000ff000;
		pRA->low_rssi_threshold_ratr		=	0x000ff003;
		pRA->low_rssi_threshold_ratr_40M	=	0x000ff007;
		pRA->low_rssi_threshold_ratr_20M	=	0x000ff003;
	}

}	// InitRateAdaptive


void Adhoc_InitRateAdaptiveState(struct net_device *dev,struct sta_info  *pEntry)
{
	prate_adaptive	pRA = (prate_adaptive)&pEntry->rate_adaptive;

	pRA->ratr_state = DM_RATR_STA_MAX;
	pRA->PreRATRState = DM_RATR_STA_MAX;
}


static void Adhoc_dm_CheckRateAdaptive(struct net_device * dev)
{
	struct r8192_priv			*priv = rtllib_priv(dev);
	struct rtllib_device	*ieee = priv->rtllib;
	prate_adaptive			pRA;
	u32						currentRATR, targetRATR = 0;
	u32						LowRSSIThreshForRA = 0, HighRSSIThreshForRA = 0;
	bool						bShortGIEnabled = false;
	u8						i = 0;
	struct sta_info				*pEntry = NULL;

	if(!priv->up)
	{
		RT_TRACE(COMP_RATE, "<---- AP_dm_CheckRateAdaptive(): driver is going to unload\n");
		return;
	}

	for(i=0;i<PEER_MAX_ASSOC; i++)
	{
		pEntry = ieee->peer_assoc_list[i];
		if(NULL != pEntry)
		{
			pRA = (prate_adaptive)&pEntry->rate_adaptive;
			if(pRA->rate_adaptive_disabled)
				continue;
			// TODO: Only 11n mode is implemented currently,
			if((pEntry->wireless_mode!=WIRELESS_MODE_N_24G) && (pEntry->wireless_mode != WIRELESS_MODE_N_5G))
				continue;
			//bShortGIEnabled = (ieee->pHTInfo->bCurTxBW40MHz && ieee->pHTInfo->bCurShortGI40MHz) | ((!ieee->pHTInfo->bCurTxBW40MHz) && ieee->pHTInfo->bCurShortGI20MHz);
			bShortGIEnabled = (pEntry->htinfo.bCurTxBW40MHz && priv->rtllib->pHTInfo->bCurShortGI40MHz && pEntry->htinfo.bCurShortGI40MHz) |
				(!pEntry->htinfo.bCurTxBW40MHz && priv->rtllib->pHTInfo->bCurShortGI20MHz && pEntry->htinfo.bCurShortGI20MHz);
			pRA->upper_rssi_threshold_ratr =
				(pRA->upper_rssi_threshold_ratr & (~BIT31)) | ((bShortGIEnabled)? BIT31:0) ;

			pRA->middle_rssi_threshold_ratr =
				(pRA->middle_rssi_threshold_ratr & (~BIT31)) | ((bShortGIEnabled)? BIT31:0) ;


			if (pEntry->htinfo.bBw40MHz)
			{
				pRA->low_rssi_threshold_ratr =
					(pRA->low_rssi_threshold_ratr_40M & (~BIT31)) | ((bShortGIEnabled)? BIT31:0) ;
			}
			else
			{
				pRA->low_rssi_threshold_ratr =
					(pRA->low_rssi_threshold_ratr_20M & (~BIT31)) | ((bShortGIEnabled)? BIT31:0) ;
			}
			if (pRA->ratr_state == DM_RATR_STA_HIGH)
			{
				HighRSSIThreshForRA	= pRA->high2low_rssi_thresh_for_ra;
				LowRSSIThreshForRA	=
					(priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20)?
					(pRA->low_rssi_thresh_for_ra40M):(pRA->low_rssi_thresh_for_ra20M);
			}
			else if (pRA->ratr_state == DM_RATR_STA_LOW)
			{
				HighRSSIThreshForRA	= pRA->high_rssi_thresh_for_ra;
				LowRSSIThreshForRA	=
					(priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20)?
					(pRA->low2high_rssi_thresh_for_ra40M):(pRA->low2high_rssi_thresh_for_ra20M);
			}
			else
			{
				HighRSSIThreshForRA	= pRA->high_rssi_thresh_for_ra;
				LowRSSIThreshForRA	=
					(priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20)?
					(pRA->low_rssi_thresh_for_ra40M):(pRA->low_rssi_thresh_for_ra20M);
			}
			if(priv->undecorated_smoothed_pwdb >= (int)HighRSSIThreshForRA)
			{
				//printk("[DM] RSSI=%d STA=HIGH\n\r", priv->undecorated_smoothed_pwdb);
				pRA->ratr_state = DM_RATR_STA_HIGH;
				targetRATR = pRA->upper_rssi_threshold_ratr;
			}
			else if(priv->undecorated_smoothed_pwdb >= (int)LowRSSIThreshForRA)
			{
				//printk("[DM] RSSI=%d STA=Middle\n\r", priv->undecorated_smoothed_pwdb);
				pRA->ratr_state = DM_RATR_STA_MIDDLE;
				targetRATR = pRA->middle_rssi_threshold_ratr;
			}
			else
			{
				//printk("[DM] RSSI=%d STA=LOW\n\r", priv->undecorated_smoothed_pwdb);
				pRA->ratr_state = DM_RATR_STA_LOW;
				targetRATR = pRA->low_rssi_threshold_ratr;
			}
			currentRATR = read_nic_dword(dev, pEntry->ratr_index*4 + RATR0);
			if( targetRATR !=  currentRATR )
			{
				if(priv->rf_type == RF_1T2R)	// 1T2R, Spatial Stream 2 should be disabled
				{
					targetRATR &=~ (RATE_ALL_OFDM_2SS);
					//printk("HW_VAR_TATR_0 from 0x%x ==> 0x%x\n", ((pu4Byte)(val))[0], targetRATR);
				}
				printk("<<<<<<<<<<<currentRATR = %x, targetRATR = %x\n", currentRATR, targetRATR);
				write_nic_dword(dev, RATR0+pEntry->ratr_index*4, targetRATR);
				write_nic_byte(dev, UFWP, 1);
				pRA->last_ratr = targetRATR;
			}
		}
	}

}	// Adhoc_dm_CheckRateAdaptive
#endif

/*---------------------------Define function prototype------------------------*/
