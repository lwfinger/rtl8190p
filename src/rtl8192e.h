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
#ifndef _RTL8192E_H
#define _RTL8192E_H

#include "r8190P_def.h"
void rtl8192e_SetHwReg(struct net_device *dev,u8 variable,u8* val);
void rtl8192_get_eeprom_size(struct net_device* dev);
bool rtl8192_adapter_start(struct net_device *dev);
void rtl8192_link_change(struct net_device *dev);
void  rtl8192_tx_fill_desc(struct net_device* dev, tx_desc * pdesc, cb_desc * cb_desc,
		           struct sk_buff* skb);
void  rtl8192_tx_fill_cmd_desc(struct net_device* dev, tx_desc_cmd * entry,
			       cb_desc * cb_desc, struct sk_buff* skb);
bool rtl8192_rx_query_status_desc(struct net_device* dev, struct rtllib_rx_stats *stats,
				  rx_desc *pdesc, struct sk_buff* skb);
void rtl8192_halt_adapter(struct net_device *dev, bool reset);
void rtl8192_update_ratr_table(struct net_device* dev);
#endif
