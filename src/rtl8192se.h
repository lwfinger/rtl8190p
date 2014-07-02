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
#ifndef _RTL8192SE_H
#define _RTL8192SE_H

#include "r8192SE_def.h"

void rtl8192se_get_eeprom_size(struct net_device* dev);
bool rtl8192se_adapter_start(struct net_device* dev);
void rtl8192se_link_change(struct net_device *dev);
void rtl8192se_tx_fill_desc(struct net_device *dev, tx_desc *pDesc, cb_desc *cb_desc, struct sk_buff *skb);
void rtl8192se_tx_fill_cmd_desc(struct net_device *dev, tx_desc_cmd *entry, cb_desc *cb_desc,
		struct sk_buff *skb);
bool rtl8192se_rx_query_status_desc(struct net_device* dev, struct rtllib_rx_stats*  stats,
		rx_desc *pdesc, struct sk_buff* skb);
void rtl8192se_halt_adapter(struct net_device *dev, bool bReset);
void rtl8192se_update_ratr_table(struct net_device* dev,u8* pMcsRate,struct sta_info* pEntry);
int r8192se_resume_firm(struct net_device *dev);
void PHY_SetRtl8192seRfHalt(struct net_device* dev);
#ifndef _RTL8192_EXT_PATCH_
void UpdateHalRAMask8192SE(struct net_device* dev,bool bMulticast, u8 macId, u8* MacAddr, u8* pEntry, u8 rssi_level);
#else
void UpdateHalRAMask8192SE(struct net_device* dev, bool bMulticast, u8 macId, u8* MacAddr, u8 wirelessMode,     u8 rssi_level);
#endif
u8 HalSetSysClk8192SE(struct net_device *dev, u8 Data);

#endif
