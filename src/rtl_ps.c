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
#include "rtl_core.h"
#include "r819xE_phy.h"
#include "r819xE_phyreg.h"
#include "r8190_rtl8256.h" /* RTL8225 Radio frontend */
#include "r819xE_cmdpkt.h"

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
	//added by amy 090331 end
	MgntActSet_RF_State(dev, pPSC->eInactivePowerState, RF_CHANGE_BY_IPS);

	//added by amy 090331
	//added by amy 090331 end

	//
	// To solve CAM values miss in RF OFF, rewrite CAM values after RF ON. By Bruce, 2007-09-20.
	//
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

	if (priv->rtllib->PowerSaveControl.bInactivePs){
		if (rtState == eRfOff){
			if (priv->rtllib->RfOffReason > RF_CHANGE_BY_IPS)
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
	if (priv->rtllib->iw_mode == IW_MODE_ADHOC)
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
	if (!priv->ps_force) {
		priv->rtllib->ps = rtPsMode;
	}
	// Awake immediately
	if (priv->rtllib->sta_sleep != 0 && rtPsMode == RTLLIB_PS_DISABLED)
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
		if (pPSC->LpsIdleCount >= RT_CHECK_FOR_HANG_PERIOD) //  4 Sec
		{

			if (priv->rtllib->ps == RTLLIB_PS_DISABLED)
			{

				RT_TRACE(COMP_LPS, "LeisurePSEnter(): Enter 802.11 power save mode...\n");

				if (!pPSC->bFwCtrlLPS)
				{
					// Firmware workaround fix for PS-Poll issue. 2009/1/4, Emily
					if (priv->rtllib->SetFwCmdHandler)
					{
						priv->rtllib->SetFwCmdHandler(dev, FW_CMD_LPS_ENTER);
					} //FIXME:YJ: Delete in sta driver. Done by firmware.
				}
				MgntActSet_802_11_PowerSaveMode(dev, RTLLIB_PS_MBCAST|RTLLIB_PS_UNICAST);

				/*if (pPSC->RegRfPsLevel & RT_RF_LPS_LEVEL_ASPM)
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
		if (priv->rtllib->ps != RTLLIB_PS_DISABLED)
		{
			// move to lps_wakecomplete()
			RT_TRACE(COMP_LPS, "LeisurePSLeave(): Busy Traffic , Leave 802.11 power save..\n");
			MgntActSet_802_11_PowerSaveMode(dev, RTLLIB_PS_DISABLED);

			if (!pPSC->bFwCtrlLPS) // Fw will control this by itself in powersave v2.
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
