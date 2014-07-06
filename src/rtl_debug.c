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
#include "rtl_debug.h"
#include "rtl_core.h"
#include "r819xE_phy.h"
#include "r819xE_phyreg.h"
#include "r8190_rtl8256.h" /* RTL8225 Radio frontend */
#include "r819xE_cmdpkt.h"

/* this is only for debugging */
void print_buffer(u32 *buffer, int len)
{
	int i;
	u8 *buf = (u8*)buffer;

	printk("ASCII BUFFER DUMP (len: %x):\n", len);

	for (i = 0;i<len;i++)
		printk("%c", buf[i]);

	printk("\nBINARY BUFFER DUMP (len: %x):\n", len);

	for (i = 0;i<len;i++)
		printk("%x", buf[i]);

	printk("\n");
}

/* this is only for debug */
void dump_eprom(struct net_device *dev)
{
	int i;

	for (i = 0; i < 0xff; i++) {
		RT_TRACE(COMP_INIT, "EEPROM addr %x : %x", i, eprom_read(dev, i));
	}
}

/* this is only for debug */
void rtl8192_dump_reg(struct net_device *dev)
{
	int i;
	int n;
	int max = 0x5ff;

	RT_TRACE(COMP_INIT, "Dumping NIC register map");

	for (n = 0; n <= max; ) {
		printk( "\nD: %2x> ", n);
		for (i = 0; i < 16 && n <= max; i++, n++)
			printk("%2x ", read_nic_byte(dev, n));
	}
	printk("\n");
}
