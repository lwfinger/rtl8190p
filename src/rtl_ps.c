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
 *****************************************************************************/
#include <linux/interrupt.h>
#include "rtl_ps.h"
#include "rtl_core.h"
#ifdef RTL8192CE
#include "rtl_core.h"
#include "rtl8192c/r8192C_phy.h"
#include "rtl8192c/r8192C_phyreg.h"
#include "rtl8192c/r8192C_rtl6052.h"
#include "rtl8192c/r8192C_Efuse.h"
#elif defined RTL8192SE
#include "rtl_core.h"
#include "r8192S_phy.h"
#include "r8192S_phyreg.h"
#include "r8192S_rtl6052.h"
#include "r8192S_Efuse.h"
#else
#include "rtl_core.h"
#include "r819xE_phy.h"
#include "r819xE_phyreg.h"
#include "r8190_rtl8256.h" /* RTL8225 Radio frontend */
#include "r819xE_cmdpkt.h"
#endif

#if defined(RTL8192E) || defined(RTL8192SE) || defined RTL8192CE
void rtl8192_hw_sleep_down(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	unsigned long flags = 0;
#ifdef RTL8192SE_CONFIG_ASPM_OR_D3
	PRT_POWER_SAVE_CONTROL	pPSC = (PRT_POWER_SAVE_CONTROL)(&(priv->rtllib->PowerSaveControl));
#endif
	spin_lock_irqsave(&priv->rf_ps_lock,flags);
	if (priv->RFChangeInProgress) {
		spin_unlock_irqrestore(&priv->rf_ps_lock,flags);
		RT_TRACE(COMP_RF, "rtl8192_hw_sleep_down(): RF Change in progress! \n");
		printk("rtl8192_hw_sleep_down(): RF Change in progress!\n");
		return;
	}
	spin_unlock_irqrestore(&priv->rf_ps_lock,flags);
	RT_TRACE(COMP_PS, "%s()============>come to sleep down\n", __FUNCTION__);

#ifdef CONFIG_RTLWIFI_DEBUGFS
	// if debugfs try to hold, just leave it
	if(priv->debug->hw_holding) {
		return;
	}
#endif
	MgntActSet_RF_State(dev, eRfSleep, RF_CHANGE_BY_PS);
#ifdef RTL8192SE_CONFIG_ASPM_OR_D3
	if(pPSC->RegRfPsLevel & RT_RF_LPS_LEVEL_ASPM)
	{
		RT_ENABLE_ASPM(dev);
		RT_SET_PS_LEVEL(pPSC, RT_RF_LPS_LEVEL_ASPM);
	}
#endif
}

void rtl8192_hw_sleep_wq(void *data)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20))
	struct rtllib_device *ieee = container_of_dwork_rsl(data,struct rtllib_device,hw_sleep_wq);
	struct net_device *dev = ieee->dev;
#else
	struct net_device *dev = (struct net_device *)data;
#endif
	//printk("=========>%s()\n", __FUNCTION__);
        rtl8192_hw_sleep_down(dev);
}

void rtl8192_hw_wakeup(struct net_device* dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	unsigned long flags = 0;
#ifdef RTL8192SE_CONFIG_ASPM_OR_D3
	PRT_POWER_SAVE_CONTROL	pPSC = (PRT_POWER_SAVE_CONTROL)(&(priv->rtllib->PowerSaveControl));
#endif
//	spin_lock_irqsave(&priv->ps_lock,flags);
	spin_lock_irqsave(&priv->rf_ps_lock,flags);
	if (priv->RFChangeInProgress) {
		spin_unlock_irqrestore(&priv->rf_ps_lock,flags);
		RT_TRACE(COMP_RF, "rtl8192_hw_wakeup(): RF Change in progress! \n");
		printk("rtl8192_hw_wakeup(): RF Change in progress! schedule wake up task again\n");
		queue_delayed_work_rsl(priv->rtllib->wq,&priv->rtllib->hw_wakeup_wq,MSECS(10));//PowerSave is not supported if kernel version is below 2.6.20
		return;
	}
	spin_unlock_irqrestore(&priv->rf_ps_lock,flags);
#ifdef RTL8192SE_CONFIG_ASPM_OR_D3
	if (pPSC->RegRfPsLevel & RT_RF_LPS_LEVEL_ASPM) {
		RT_DISABLE_ASPM(dev);
		RT_CLEAR_PS_LEVEL(pPSC, RT_RF_LPS_LEVEL_ASPM);
	}
#endif
	RT_TRACE(COMP_PS, "%s()============>come to wake up\n", __FUNCTION__);
	MgntActSet_RF_State(dev, eRfOn, RF_CHANGE_BY_PS);
	//FIXME: will we send package stored while nic is sleep?
//	spin_unlock_irqrestore(&priv->ps_lock,flags);
}

void rtl8192_hw_wakeup_wq(void *data)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20))
	struct rtllib_device *ieee = container_of_dwork_rsl(data,struct rtllib_device,hw_wakeup_wq);
	struct net_device *dev = ieee->dev;
#else
	struct net_device *dev = (struct net_device *)data;
#endif
	rtl8192_hw_wakeup(dev);

}

#define MIN_SLEEP_TIME 50
#define MAX_SLEEP_TIME 10000
void rtl8192_hw_to_sleep(struct net_device *dev, u32 th, u32 tl)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	u32 rb = jiffies;
	unsigned long flags;

	spin_lock_irqsave(&priv->ps_lock,flags);

	// Writing HW register with 0 equals to disable
	// the timer, that is not really what we want
	//
	tl -= MSECS(8+16+7);

	// If the interval in witch we are requested to sleep is too
	// short then give up and remain awake
	// when we sleep after send null frame, the timer will be too short to sleep.
	//
	if(((tl>=rb)&& (tl-rb) <= MSECS(MIN_SLEEP_TIME))
			||((rb>tl)&& (rb-tl) < MSECS(MIN_SLEEP_TIME))) {
		spin_unlock_irqrestore(&priv->ps_lock,flags);
		printk("too short to sleep::%x, %x, %lx\n",tl, rb,  MSECS(MIN_SLEEP_TIME));
		return;
	}

	if(((tl > rb) && ((tl-rb) > MSECS(MAX_SLEEP_TIME)))||
			((tl < rb) && (tl>MSECS(69)) && ((rb-tl) > MSECS(MAX_SLEEP_TIME)))||
			((tl<rb)&&(tl<MSECS(69))&&((tl+0xffffffff-rb)>MSECS(MAX_SLEEP_TIME)))) {
		printk("========>too long to sleep:%x, %x, %lx\n", tl, rb,  MSECS(MAX_SLEEP_TIME));
		spin_unlock_irqrestore(&priv->ps_lock,flags);
		return;
	}
	{
		u32 tmp = (tl>rb)?(tl-rb):(rb-tl);
		queue_delayed_work_rsl(priv->rtllib->wq,
				&priv->rtllib->hw_wakeup_wq,tmp);
		//PowerSave not supported when kernel version less 2.6.20
	}
	queue_delayed_work_rsl(priv->rtllib->wq,
			(void *)&priv->rtllib->hw_sleep_wq,0);
	spin_unlock_irqrestore(&priv->ps_lock,flags);
}
#endif

#ifdef ENABLE_IPS
void InactivePsWorkItemCallback(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	PRT_POWER_SAVE_CONTROL	pPSC = (PRT_POWER_SAVE_CONTROL)(&(priv->rtllib->PowerSaveControl));
	//u8							index = 0;

	RT_TRACE(COMP_PS, "InactivePsWorkItemCallback() ---------> \n");
	//
	// This flag "bSwRfProcessing", indicates the status of IPS procedure, should be set if the IPS workitem
	// is really scheduled.
	// The old code, sets this flag before scheduling the IPS workitem and however, at the same time the
	// previous IPS workitem did not end yet, fails to schedule the current workitem. Thus, bSwRfProcessing
	// blocks the IPS procedure of switching RF.
	// By Bruce, 2007-12-25.
	//
	pPSC->bSwRfProcessing = true;

	RT_TRACE(COMP_PS, "InactivePsWorkItemCallback(): Set RF to %s.\n", \
			pPSC->eInactivePowerState == eRfOff?"OFF":"ON");
	//added by amy 090331
#ifdef RTL8192SE_CONFIG_ASPM_OR_D3
	if(pPSC->eInactivePowerState == eRfOn)
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
	//added by amy 090331 end
	MgntActSet_RF_State(dev, pPSC->eInactivePowerState, RF_CHANGE_BY_IPS);

	//added by amy 090331
#ifdef RTL8192SE_CONFIG_ASPM_OR_D3
	if(pPSC->eInactivePowerState == eRfOff)
	{
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
	}
#endif
	//added by amy 090331 end

	//
	// To solve CAM values miss in RF OFF, rewrite CAM values after RF ON. By Bruce, 2007-09-20.
	//
#if 0
	if(pPSC->eInactivePowerState == eRfOn)
	{
		//
		// To solve CAM values miss in RF OFF, rewrite CAM values after RF ON. By Bruce, 2007-09-20.
		//
		while( index < 4 )
		{
			if( ( pMgntInfo->SecurityInfo.PairwiseEncAlgorithm == WEP104_Encryption ) ||
				(pMgntInfo->SecurityInfo.PairwiseEncAlgorithm == WEP40_Encryption) )
			{
				if( pMgntInfo->SecurityInfo.KeyLen[index] != 0)
				pAdapter->HalFunc.SetKeyHandler(pAdapter, index, 0, false, pMgntInfo->SecurityInfo.PairwiseEncAlgorithm, true, false);

			}
			index++;
		}
	}
#endif
	pPSC->bSwRfProcessing = false;
	RT_TRACE(COMP_PS, "InactivePsWorkItemCallback() <--------- \n");
}

//
//	Description:
//		Enter the inactive power save mode. RF will be off
//	2007.08.17, by shien chang.
//
void
IPSEnter(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	PRT_POWER_SAVE_CONTROL		pPSC = (PRT_POWER_SAVE_CONTROL)(&(priv->rtllib->PowerSaveControl));
	RT_RF_POWER_STATE			rtState;

	if (pPSC->bInactivePs)
	{
		rtState = priv->rtllib->eRFPowerState;
		//
		// Added by Bruce, 2007-12-25.
		// Do not enter IPS in the following conditions:
		// (1) RF is already OFF or Sleep
		// (2) bSwRfProcessing (indicates the IPS is still under going)
		// (3) Connectted (only disconnected can trigger IPS)
		// (4) IBSS (send Beacon)
		// (5) AP mode (send Beacon)
		//
		if (rtState == eRfOn && !pPSC->bSwRfProcessing &&\
			(priv->rtllib->state != RTLLIB_LINKED)&&\
			(priv->rtllib->iw_mode != IW_MODE_MASTER))
		{
			RT_TRACE(COMP_PS,"IPSEnter(): Turn off RF.\n");
			pPSC->eInactivePowerState = eRfOff;
			priv->isRFOff = true;//added by amy 090331
			priv->bInPowerSaveMode = true;//added by amy 090331
//			queue_work(priv->priv_wq,&(pPSC->InactivePsWorkItem));
			InactivePsWorkItemCallback(dev);
		}
	}
}

//
//	Description:
//		Leave the inactive power save mode, RF will be on.
//	2007.08.17, by shien chang.
//
void
IPSLeave(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	PRT_POWER_SAVE_CONTROL	pPSC = (PRT_POWER_SAVE_CONTROL)(&(priv->rtllib->PowerSaveControl));
	RT_RF_POWER_STATE	rtState;

	if (pPSC->bInactivePs)
	{
		rtState = priv->rtllib->eRFPowerState;
		if (rtState != eRfOn  && !pPSC->bSwRfProcessing && priv->rtllib->RfOffReason <= RF_CHANGE_BY_IPS)
		{
			RT_TRACE(COMP_PS, "IPSLeave(): Turn on RF.\n");
			pPSC->eInactivePowerState = eRfOn;
			priv->bInPowerSaveMode = false;
//			queue_work(priv->priv_wq,&(pPSC->InactivePsWorkItem));
			InactivePsWorkItemCallback(dev);
		}
	}
}
//added by amy 090331
void IPSLeave_wq(void *data)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20))
	struct rtllib_device *ieee = container_of_work_rsl(data,struct rtllib_device,ips_leave_wq);
	struct net_device *dev = ieee->dev;
#else
	struct net_device *dev = (struct net_device *)data;
#endif
	struct r8192_priv *priv = (struct r8192_priv *)rtllib_priv(dev);
	down(&priv->rtllib->ips_sem);
	IPSLeave(dev);
	up(&priv->rtllib->ips_sem);
}
void rtllib_ips_leave_wq(struct net_device *dev)
{
	struct r8192_priv *priv = (struct r8192_priv *)rtllib_priv(dev);
	RT_RF_POWER_STATE	rtState;
	rtState = priv->rtllib->eRFPowerState;

	if(priv->rtllib->PowerSaveControl.bInactivePs){
		if(rtState == eRfOff){
			if(priv->rtllib->RfOffReason > RF_CHANGE_BY_IPS)
			{
				RT_TRACE(COMP_ERR, "%s(): RF is OFF.\n",__FUNCTION__);
				return;
			}
			else{
				printk("=========>%s(): IPSLeave\n",__FUNCTION__);
				queue_work_rsl(priv->rtllib->wq,&priv->rtllib->ips_leave_wq);
			}
		}
	}
}
//added by amy 090331 end
void rtllib_ips_leave(struct net_device *dev)
{
	struct r8192_priv *priv = (struct r8192_priv *)rtllib_priv(dev);
	down(&priv->rtllib->ips_sem);
	IPSLeave(dev);
	up(&priv->rtllib->ips_sem);
}
#endif

#ifdef ENABLE_LPS
//
// Change current and default preamble mode.
// 2005.01.06, by rcnjko.
//
bool MgntActSet_802_11_PowerSaveMode(struct net_device *dev,	u8 rtPsMode)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	//PRT_POWER_SAVE_CONTROL pPSC = (PRT_POWER_SAVE_CONTROL)(&(priv->rtllib->PowerSaveControl));
	//u8 RpwmVal, FwPwrMode;

	// Currently, we do not change power save mode on IBSS mode.
	if(priv->rtllib->iw_mode == IW_MODE_ADHOC)
		return false;

	//
	// <RJ_NOTE> If we make HW to fill up the PwrMgt bit for us,
	// some AP will not response to our mgnt frames with PwrMgt bit set,
	// e.g. cannot associate the AP.
	// So I commented out it. 2005.02.16, by rcnjko.
	//
//	// Change device's power save mode.
//	Adapter->HalFunc.SetPSModeHandler( Adapter, rtPsMode );

	// Update power save mode configured.
	RT_TRACE(COMP_LPS,"%s(): set ieee->ps = %x\n",__FUNCTION__,rtPsMode);
	if(!priv->ps_force) {
		priv->rtllib->ps = rtPsMode;
	}
#if 0
	priv->rtllib->dot11PowerSaveMode = rtPsMode;

	// Determine ListenInterval.
	if(priv->rtllib->dot11PowerSaveMode == eMaxPs)
	{
	//	priv->rtllib->ListenInterval = priv->rtllib->dot11DtimPeriod;
	}
	else
	{
	//	priv->rtllib->ListenInterval = 2;
	}
#endif
	// Awake immediately
	if(priv->rtllib->sta_sleep != 0 && rtPsMode == RTLLIB_PS_DISABLED)
	{
                unsigned long flags;

		//PlatformSetTimer(Adapter, &(pMgntInfo->AwakeTimer), 0);
		// Notify the AP we awke.
		rtl8192_hw_wakeup(dev);
		priv->rtllib->sta_sleep = 0;

                spin_lock_irqsave(&(priv->rtllib->mgmt_tx_lock), flags);
		printk("LPS leave: notify AP we are awaked ++++++++++ SendNullFunctionData\n");
		rtllib_sta_ps_send_null_frame(priv->rtllib, 0);
                spin_unlock_irqrestore(&(priv->rtllib->mgmt_tx_lock), flags);
	}

#if 0
	// <FW control LPS>
	// 1. Enter PS mode
	//    Set RPWM to Fw to turn RF off and send H2C FwPwrMode cmd to set Fw into PS mode.
	// 2. Leave PS mode
	//    Send H2C FwPwrMode cmd to Fw to set Fw into Active mode and set RPWM to turn RF on.
	// By tynli. 2009.01.19.
	if((pPSC->bFwCtrlLPS) && (pPSC->bLeisurePs))
	{
		if(priv->rtllib->dot11PowerSaveMode == eActive)
		{
			RpwmVal = 0x0C; // RF on
			FwPwrMode = FW_PS_ACTIVE_MODE;
			//DbgPrint("****RPWM--->RF On\n");
			Adapter->HalFunc.SetHwRegHandler(Adapter, HW_VAR_SET_RPWM, (pu1Byte)(&RpwmVal));
			Adapter->HalFunc.SetHwRegHandler(Adapter, HW_VAR_H2C_FW_PWRMODE, (pu1Byte)(&FwPwrMode));
		}
		else
		{
			if(GetFwLPS_Doze(Adapter))
			{
				RpwmVal = 0x02; // RF off
				Adapter->HalFunc.SetHwRegHandler(Adapter, HW_VAR_H2C_FW_PWRMODE, (pu1Byte)(&pPSC->FWCtrlPSMode));
				Adapter->HalFunc.SetHwRegHandler(Adapter, HW_VAR_SET_RPWM, (pu1Byte)(&RpwmVal));
				//DbgPrint("****RPWM--->RF Off\n");
			}
			else
			{
				// Reset the power save related parameters.
				pMgntInfo->dot11PowerSaveMode = eActive;
				Adapter->bInPowerSaveMode = false;
				//DbgPrint("LeisurePSEnter() was return.\n");
			}
		}
	}
#endif
	return true;
}

//================================================================================
// Leisure Power Save in linked state.
//================================================================================

//
//	Description:
//		Enter the leisure power save mode.
//
void LeisurePSEnter(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	PRT_POWER_SAVE_CONTROL pPSC = (PRT_POWER_SAVE_CONTROL)(&(priv->rtllib->PowerSaveControl));

	RT_TRACE(COMP_PS, "LeisurePSEnter()...\n");
	RT_TRACE(COMP_PS, "pPSC->bLeisurePs = %d, ieee->ps = %d,pPSC->LpsIdleCount is %d,RT_CHECK_FOR_HANG_PERIOD is %d\n",
		pPSC->bLeisurePs, priv->rtllib->ps,pPSC->LpsIdleCount,RT_CHECK_FOR_HANG_PERIOD);

	if (!((priv->rtllib->iw_mode == IW_MODE_INFRA) &&
	    (priv->rtllib->state == RTLLIB_LINKED)) ||
	    (priv->rtllib->iw_mode == IW_MODE_ADHOC) ||
	    (priv->rtllib->iw_mode == IW_MODE_MASTER))
		return;

	if (pPSC->bLeisurePs) {
		// Idle for a while if we connect to AP a while ago.
		if(pPSC->LpsIdleCount >= RT_CHECK_FOR_HANG_PERIOD) //  4 Sec
		{

			if(priv->rtllib->ps == RTLLIB_PS_DISABLED)
			{

				RT_TRACE(COMP_LPS, "LeisurePSEnter(): Enter 802.11 power save mode...\n");

				if(!pPSC->bFwCtrlLPS)
				{
					// Firmware workaround fix for PS-Poll issue. 2009/1/4, Emily
					if (priv->rtllib->SetFwCmdHandler)
					{
						priv->rtllib->SetFwCmdHandler(dev, FW_CMD_LPS_ENTER);
					} //FIXME:YJ: Delete in sta driver. Done by firmware.
				}
				MgntActSet_802_11_PowerSaveMode(dev, RTLLIB_PS_MBCAST|RTLLIB_PS_UNICAST);

				/*if(pPSC->RegRfPsLevel & RT_RF_LPS_LEVEL_ASPM)
				{
					RT_ENABLE_ASPM(pAdapter);
					RT_SET_PS_LEVEL(pAdapter, RT_RF_LPS_LEVEL_ASPM);
				}*/

			}
		}
		else
			pPSC->LpsIdleCount++;
	}
}


//
//	Description:
//		Leave the leisure power save mode.
//
void LeisurePSLeave(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	PRT_POWER_SAVE_CONTROL pPSC = (PRT_POWER_SAVE_CONTROL)(&(priv->rtllib->PowerSaveControl));


	RT_TRACE(COMP_PS, "LeisurePSLeave()...\n");
	RT_TRACE(COMP_PS, "pPSC->bLeisurePs = %d, ieee->ps = %d\n",
		pPSC->bLeisurePs, priv->rtllib->ps);

	if (pPSC->bLeisurePs)
	{
		if(priv->rtllib->ps != RTLLIB_PS_DISABLED)
		{
#ifdef RTL8192SE_CONFIG_ASPM_OR_D3
			if(pPSC->RegRfPsLevel & RT_RF_LPS_LEVEL_ASPM && RT_IN_PS_LEVEL(pPSC, RT_RF_LPS_LEVEL_ASPM))
			{
				RT_DISABLE_ASPM(dev);
				RT_CLEAR_PS_LEVEL(pPSC, RT_RF_LPS_LEVEL_ASPM);
			}
#endif
			// move to lps_wakecomplete()
			RT_TRACE(COMP_LPS, "LeisurePSLeave(): Busy Traffic , Leave 802.11 power save..\n");
			MgntActSet_802_11_PowerSaveMode(dev, RTLLIB_PS_DISABLED);

			if(!pPSC->bFwCtrlLPS) // Fw will control this by itself in powersave v2.
			{
				// Firmware workaround fix for PS-Poll issue. 2009/1/4, Emily
				if (priv->rtllib->SetFwCmdHandler)
				{
					priv->rtllib->SetFwCmdHandler(dev, FW_CMD_LPS_LEAVE);
				} //FIXME:YJ: Delete in sta driver. Done by firmware.
                    }
		}
	}
}
#endif

#ifdef RTL8192SE_CONFIG_ASPM_OR_D3
bool PlatformSwitchDevicePciASPM(struct net_device *dev, u8 value)
{
	struct r8192_priv *priv = (struct r8192_priv *)rtllib_priv(dev);
	bool bResult = false;

	pci_write_config_byte(priv->pdev, priv->ASPMRegOffset, value);
	//printk("PlatformSwitchASPM= %x\n", value);
	udelay(100);

	return bResult;
}

//
// 2008/12/23 MH When we set 0x01 to enable clk request. Set 0x0 to disable clk req.
//
bool PlatformSwitchClkReq(struct net_device *dev, u8 value)
{
	bool bResult = false;
	struct r8192_priv *priv = (struct r8192_priv *)rtllib_priv(dev);
	u8	Buffer;

	Buffer= value;

	pci_write_config_byte(priv->pdev,priv->ClkReqOffset,value);
	//printk("PlatformSwitchClkReq = %x\n", value);
	bResult = true;
#ifdef TODO
	// 0x01: Enable Clk request */
	if(Buffer) {
		priv->ClkReqState = true;
	} else {
		priv->ClkReqState = false;
	}
#endif
	udelay(100);
	return bResult;
}

//Description:
//  Disable RTL8192SE ASPM & Disable Pci Bridge ASPM
//2008.12.19 tynli porting from 818xB
void
PlatformDisableASPM(struct net_device *dev)
{
	struct r8192_priv *priv = (struct r8192_priv *)rtllib_priv(dev);
	PRT_POWER_SAVE_CONTROL	pPSC = (PRT_POWER_SAVE_CONTROL)(&(priv->rtllib->PowerSaveControl));

	u8	LinkCtrlReg;
	u16	PciBridgeLinkCtrlReg, ASPMLevel=0;

	// Retrieve original configuration settings.
	LinkCtrlReg = priv->LinkCtrlReg;
	ASPMLevel |= BIT0|BIT1;
	LinkCtrlReg &=~ASPMLevel;

	// Set corresponding value.
	PciBridgeLinkCtrlReg = priv->PciBridgeLinkCtrlReg;
	PciBridgeLinkCtrlReg &=~(BIT0|BIT1);

	//When there exists anyone's BusNum, DevNum, and FuncNum that are set to 0xff,
	// we do not execute any action and return. Added by tynli.
	if (priv->aspm_clkreq_enable) {

		// Disable Pci Bridge ASPM  */
		pci_write_config_byte(priv->bridge_pdev,priv->PciBridgeASPMRegOffset,PciBridgeLinkCtrlReg);
		RT_TRACE(COMP_POWER, "Write reg[%x]=%x\n", (priv->PciBridgeASPMRegOffset), PciBridgeLinkCtrlReg);
		udelay(100);
	}

	PlatformSwitchDevicePciASPM(dev, priv->LinkCtrlReg);

	PlatformSwitchClkReq(dev, 0x0);
	if (pPSC->RegRfPsLevel & RT_RF_OFF_LEVL_CLK_REQ)
		RT_CLEAR_PS_LEVEL(pPSC, RT_RF_OFF_LEVL_CLK_REQ);
	udelay(100);
}

//[ASPM]
//Description:
//		Enable RTL8192SE ASPM & Enable Pci Bridge ASPM for power saving
//		We should follow the sequence to enable RTL8192SE first then enable Pci Bridge ASPM
//		or the system will show bluescreen.
//2008.12.19 tynli porting from 818xB
void PlatformEnableASPM(struct net_device *dev)
{
	struct r8192_priv *priv = (struct r8192_priv *)rtllib_priv(dev);
	PRT_POWER_SAVE_CONTROL pPSC = (PRT_POWER_SAVE_CONTROL)(&(priv->rtllib->PowerSaveControl));
	u16	ASPMLevel = 0;

	// When there exists anyone's BusNum, DevNum, and FuncNum that are set to 0xff,
	//   we do not execute any action and return. Added by tynli.
	// if it is not intel bus then don't enable ASPM.
	if (!priv->aspm_clkreq_enable) {
		RT_TRACE(COMP_INIT, "%s: Fail to enable ASPM. Cannot find the Bus of PCI(Bridge).\n",\
				__FUNCTION__);
		return;
	}

	ASPMLevel |= priv->RegDevicePciASPMSetting;
	PlatformSwitchDevicePciASPM(dev, (priv->LinkCtrlReg | ASPMLevel));

	if (pPSC->RegRfPsLevel & RT_RF_OFF_LEVL_CLK_REQ) {
		PlatformSwitchClkReq(dev,(pPSC->RegRfPsLevel & RT_RF_OFF_LEVL_CLK_REQ) ? 1 : 0);
		RT_SET_PS_LEVEL(pPSC, RT_RF_OFF_LEVL_CLK_REQ);
	}
	udelay(100);

	// Enable Pci Bridge ASPM */
	// now grab data port with device|vendor 4 byte dword */
	pci_write_config_byte(priv->bridge_pdev,priv->PciBridgeASPMRegOffset,\
			((priv->PciBridgeLinkCtrlReg | priv->RegHostPciASPMSetting)&~BIT0)&0xff);
	RT_TRACE(COMP_INIT, "Write reg[%x] = %x\n",
		priv->PciBridgeASPMRegOffset,
		(priv->PciBridgeLinkCtrlReg|priv->RegHostPciASPMSetting));
	udelay(100);
}

//
// 2008/12/26 MH When we exit from D3 we must reset PCI mmory space. Otherwise,
// ASPM and clock request will not work at next time.
//
u32 PlatformResetPciSpace(struct net_device *dev,u8 Value)
{
	struct r8192_priv *priv = (struct r8192_priv *)rtllib_priv(dev);

	pci_write_config_byte(priv->pdev,0x04,Value);

	return 1;

}
//
// 2008/12/23 MH For 9x series D3 mode switch, we do not support D3 spin lock.
// Because we only permit to enter D3 mode and exit when we switch RF state. If
// state is the same, we will not switch D3 mode.
//
bool PlatformSetPMCSR(struct net_device *dev,u8 value,bool bTempSetting)
{
	bool bResult = false;
	struct r8192_priv *priv = (struct r8192_priv *)rtllib_priv(dev);
	u8  Buffer;
	bool bActuallySet=false, bSetFunc=false;
	unsigned long flag;
	// We do not check if we can Do IO. If we can perform the code, it means we can change
	// configuration space for PCI-(E)

	Buffer= value;
	spin_lock_irqsave(&priv->D3_lock,flag);
#ifdef TODO
	if(bTempSetting) //temp open
	{
		if(Buffer==0x00) //temp leave D3
		{
			priv->LeaveD3Cnt++;

			//if(pDevice->LeaveD3Cnt == 1)
			{
				bActuallySet =true;
			}
		}
		else //0x03: enter D3
		{
			priv->LeaveD3Cnt--;

			if(priv->LeaveD3Cnt == 0)
			{
				bActuallySet=true;
			}
		}
	}
	else
	{
		priv->LeaveD3Cnt=0;
		bActuallySet=true;
		bSetFunc=true;
	}
#endif
	if (bActuallySet) {
		if (Buffer) {
			// Enable clock request to lower down  the current.
			PlatformSwitchClkReq(dev, 0x01);
		} else {
			PlatformSwitchClkReq(dev, 0x00);
		}

		pci_write_config_byte(priv->pdev,0x44,Buffer);
		RT_TRACE(COMP_POWER, "PlatformSetPMCSR(): D3(value: %d)\n", Buffer);

		bResult = true;
		if (!Buffer) {
			//
			// Reset PCIE to simulate host behavior. to prevent ASPM power
			// save problem.
			//
			PlatformResetPciSpace(dev, 0x06);
			PlatformResetPciSpace(dev, 0x07);
		}

		if (bSetFunc) {
			if(Buffer) //0x03: enter D3
			{
#ifdef TO_DO_LIST
				RT_DISABLE_FUNC(Adapter, DF_IO_D3_BIT);
#endif
			} else {
#ifdef TO_DO_LIST
				RT_ENABLE_FUNC(Adapter, DF_IO_D3_BIT);
#endif
			}
		}

	}
	spin_unlock_irqrestore(&priv->D3_lock,flag);
	return bResult;
}
#endif
