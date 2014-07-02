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
#include "rtl_core.h"
#include "r8192S_phy.h"
#include "r8192S_phyreg.h"
#include "r8192S_rtl6052.h"
#include "r8192S_Efuse.h"
#include "rtl_dm.h"
#include "rtl_wx.h"

extern int WDCAPARA_ADD[];
//Config hw with initial value.
static void rtl8192se_config_hw_for_load_fail(struct net_device* dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	u16			i;//,usValue;
	u8			sMacAddr[6] = {0x00, 0xE0, 0x4C, 0x81, 0x92, 0x00};
	u8			rf_path, index;

	RT_TRACE(COMP_INIT, "====> rtl8192se_config_hw_for_load_fail\n");

	write_nic_byte(dev, SYS_ISO_CTRL+1, 0xE8); // Isolation signals from Loader
	mdelay(10);
	write_nic_byte(dev, PMC_FSM, 0x02); // Enable Loader Data Keep

	// Initialize IC Version && Channel Plan
	priv->eeprom_vid= 0;
	priv->eeprom_did= 0;
	priv->eeprom_ChannelPlan= 0;
	priv->eeprom_CustomerID= 0;

       //
	//<Roger_Notes> In this case, we random assigh MAC address here. 2008.10.15.
	//
	//Initialize Permanent MAC address
	//if(!dev->bInHctTest)
       get_random_bytes(&sMacAddr[5], 1);
	for(i = 0; i < 6; i++)
		dev->dev_addr[i] = sMacAddr[i];


	// 2007/11/15 MH For RTL8192USB we assign as 1T2R now.
	priv->rf_type= RTL819X_DEFAULT_RF_TYPE;	// default : 1T2R
	priv->rf_chip = RF_6052;

	//
	// For TX power index
	//
#if (EEPROM_OLD_FORMAT_SUPPORT == 1)
	for(i=0; i<14; i++)
	{
		priv->EEPROMTxPowerLevelCCK[i] = (u8)(EEPROM_Default_TxPower & 0xff);
		RT_TRACE(COMP_INIT, "CCK 2.4G Tx Pwr Index %d = 0x%02x\n",
		i, priv->EEPROMTxPowerLevelCCK[i]);
	}

	for(i=0; i<14; i++)
	{
		priv->EEPROMTxPowerLevelOFDM24G[i] = (u8)(EEPROM_Default_TxPower & 0xff);
		RT_TRACE(COMP_INIT, "OFDM 2.4G Tx Pwr Index %d = 0x%02x\n",
		i, priv->EEPROMTxPowerLevelOFDM24G[i]);
	}

	//
	// Update HAL variables.
	//
	for(i=0; i<14; i++)
	{
		priv->TxPowerLevelOFDM24G[i] = priv->EEPROMTxPowerLevelOFDM24G[i];
		priv->TxPowerLevelCCK[i] = priv->EEPROMTxPowerLevelCCK[i];
	}
	 // Default setting for Empty EEPROM
	for(i=0; i<6; i++)
	{
		priv->EEPROMHT2T_TxPwr[i] = EEPROM_Default_HT2T_TxPwr;
	}

#else
	for (rf_path = 0; rf_path < 2; rf_path++)
	{
		for (i = 0; i < 3; i++)
		{
			// Read CCK RF A & B Tx power
			priv->RfCckChnlAreaTxPwr[rf_path][i] =
			priv->RfOfdmChnlAreaTxPwr1T[rf_path][i] =
			priv->RfOfdmChnlAreaTxPwr2T[rf_path][i] =
			EEPROM_Default_TxPowerLevel;
		}
	}

	for (rf_path = 0; rf_path < 2; rf_path++)
	{
		for(i=0; i<14; i++)	// channel 1~3 use the same Tx Power Level.
		{
			if (i < 3)			// Cjanel 1-3
				index = 0;
#ifdef _RTL8192_EXT_PATCH_
			else if (i < 9)		// Channel 4-9
#else
			else if (i < 8)		// Channel 4-8
#endif
				index = 1;
			else				// Channel 9-14
				index = 2;

			// Record A & B CCK /OFDM - 1T/2T Channel area tx power
			priv->RfTxPwrLevelCck[rf_path][i]  =
			priv->RfCckChnlAreaTxPwr[rf_path][index] =
			priv->RfTxPwrLevelOfdm1T[rf_path][i]  =
			priv->RfOfdmChnlAreaTxPwr1T[rf_path][index] =
			priv->RfTxPwrLevelOfdm2T[rf_path][i]  =
			priv->RfOfdmChnlAreaTxPwr2T[rf_path][index] =
			(u8)(EEPROM_Default_TxPower & 0xff);

			if (rf_path == 0)
			{
				priv->TxPowerLevelOFDM24G[i] = (u8)(EEPROM_Default_TxPower & 0xff);
				priv->TxPowerLevelCCK[i] = (u8)(EEPROM_Default_TxPower & 0xff);
			}
		}

	}

	RT_TRACE(COMP_INIT, "All TxPwr = 0x%x\n", EEPROM_Default_TxPower);

	// 2009/01/21 MH Support new EEPROM format from SD3 requirement for verification.
	for(i=0; i<14; i++)
	{
		// Set HT 20 <->40MHZ tx power diff
		priv->TxPwrHt20Diff[RF90_PATH_A][i] = DEFAULT_HT20_TXPWR_DIFF;
		priv->TxPwrHt20Diff[RF90_PATH_B][i] = DEFAULT_HT20_TXPWR_DIFF;

		priv->TxPwrLegacyHtDiff[0][i] = EEPROM_Default_LegacyHTTxPowerDiff;
		priv->TxPwrLegacyHtDiff[1][i] = EEPROM_Default_LegacyHTTxPowerDiff;
	}

	priv->TxPwrSafetyFlag = 0;
#endif

	priv->EEPROMTxPowerDiff = EEPROM_Default_LegacyHTTxPowerDiff;
	priv->LegacyHTTxPowerDiff = priv->EEPROMTxPowerDiff;
	RT_TRACE(COMP_INIT, "TxPowerDiff = %#x\n", priv->EEPROMTxPowerDiff);

#ifndef _RTL8192_EXT_PATCH_
	for (rf_path = 0; rf_path < 2; rf_path++)
	{
		for (i = 0; i < 3; i++)
		{
			// Read Power diff limit.
			priv->EEPROMPwrGroup[rf_path][i] = 0;
		}
	}

	for (rf_path = 0; rf_path < 2; rf_path++)
	{
		// Fill Pwr group
		for(i=0; i<14; i++)
		{
			if (i < 3)			// Cjanel 1-3
				index = 0;
			else if (i < 8)		// Channel 4-8
				index = 1;
			else				// Channel 9-13
				index = 2;
			priv->PwrGroupHT20[rf_path][i] = (priv->EEPROMPwrGroup[rf_path][index]&0xf);
			priv->PwrGroupHT40[rf_path][i] = ((priv->EEPROMPwrGroup[rf_path][index]&0xf0)>>4);
		}
	}
	priv->EEPROMRegulatory = 0;

#endif

	//
	//
	//
	priv->EEPROMTSSI_A = EEPROM_Default_TSSI;
	priv->EEPROMTSSI_B = EEPROM_Default_TSSI;

	for(i=0; i<6; i++)
	{
		priv->EEPROMHT2T_TxPwr[i] = EEPROM_Default_HT2T_TxPwr;
	}


	// Initialize Difference of gain index between legacy and HT OFDM from EEPROM.
	// Initialize ThermalMeter from EEPROM
	priv->EEPROMThermalMeter = EEPROM_Default_ThermalMeter;
	// ThermalMeter, bit0~3 for RFIC1, bit4~7 for RFIC2
	priv->ThermalMeter[0] = (priv->EEPROMThermalMeter&0x1f);	//[4:0]
	priv->TSSI_13dBm = priv->EEPROMThermalMeter *100;

	priv->BluetoothCoexist = EEPROM_Default_BlueToothCoexist;

	// Initialize CrystalCap from EEPROM
	priv->EEPROMCrystalCap = EEPROM_Default_CrystalCap;
	// CrystalCap, bit12~15
	priv->CrystalCap = priv->EEPROMCrystalCap;

	//
	// Read EEPROM Version && Channel Plan
	//
	// Default channel plan  =0
	priv->eeprom_ChannelPlan = 0;
	priv->eeprom_version = 1;		// Default version is 1
	priv->bTXPowerDataReadFromEEPORM = false;

	// 2007/11/15 MH For RTL8192S we assign as 1T2R now.
	priv->rf_type = RTL819X_DEFAULT_RF_TYPE;	// default : 1T2R
	priv->rf_chip = RF_6052;
	priv->eeprom_CustomerID = 0;
	RT_TRACE(COMP_INIT, "EEPROM Customer ID: 0x%2x\n", priv->eeprom_CustomerID);


	priv->EEPROMBoardType = EEPROM_Default_BoardType;
	RT_TRACE(COMP_INIT, "BoardType = %#x\n", priv->EEPROMBoardType);

#ifdef _RTL8192_EXT_PATCH_
	priv->LedStrategy = SW_LED_MODE0;
#else
	priv->LedStrategy = SW_LED_MODE7;
#endif

//	InitRateAdaptive(dev);

	RT_TRACE(COMP_INIT,"<==== rtl8192se_config_hw_for_load_fail\n");
}

static void rtl8192se_get_IC_Inferiority(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	u8  Efuse_ID;

	priv->IC_Class = IC_INFERIORITY_A; // We set class A as default and compatible EEPROM ones.
	// Only retrieving while using EFUSE.
	if((priv->epromtype == EEPROM_BOOT_EFUSE) && !priv->AutoloadFailFlag)
	{
		Efuse_ID = EFUSE_Read1Byte(dev, EFUSE_IC_ID_OFFSET);

		if(Efuse_ID == 0xfe)
		{
			priv->IC_Class = IC_INFERIORITY_B;

			//RT_TRACE(COMP_INIT|COMP_EFUSE, DBG_TRACE, ("HalGetICInferiority8192SU(): IC_INFERIORITY_B!!\n\n"));
		}
	}
}

#ifdef _RTL8192_EXT_PATCH_
void HalCustomizedBehavior8192S(struct net_device* dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	//pMgntInfo->bForcedShowRateStill = false;
	switch(priv->CustomerID)
	{
		case RT_CID_DEFAULT:
			priv->LedStrategy = SW_LED_MODE7;
			if(	priv->eeprom_vid == 0x10EC && priv->eeprom_did == 0x8171 &&
				priv->eeprom_svid == 0x1A3B && priv->eeprom_smid == 0x1A07)
			{
				priv->RegWirelessMode = WIRELESS_MODE_G;
				//pMgntInfo->bForcedShowRateStill = true;
			}
			else if(	priv->eeprom_vid == 0x10EC && priv->eeprom_did == 0x8172 &&
					priv->eeprom_svid == 0x1A3B && priv->eeprom_smid == 0x1A04)
			{
				priv->RegWirelessMode = WIRELESS_MODE_G;
				//pMgntInfo->bForcedShowRateStill = true;
			}
			break;

		case RT_CID_TOSHIBA:
			priv->rtllib->current_network.channel = 10;
			priv->LedStrategy = SW_LED_MODE7;
			if(priv->eeprom_smid >= 0x7000 && priv->eeprom_smid < 0x8000)
				priv->RegWirelessMode = WIRELESS_MODE_G;
			break;

		case RT_CID_CCX:
			priv->DMFlag |= (HAL_DM_DIG_DISABLE | HAL_DM_HIPWR_DISABLE);
			//pMgntInfo->RegROAMSensitiveLevel = 1;
			//pMgntInfo->IndicateByDeauth = true;
			break;

		case RT_CID_819x_Lenovo:
			// Customize Led mode
			priv->LedStrategy = SW_LED_MODE7;
			// Customize  Link any for auto connect
			// This Value should be set after InitializeMgntVariables
			//pMgntInfo->bAutoConnectEnable = false;
			break;

		case RT_CID_819x_QMI:
			priv->LedStrategy = SW_LED_MODE8;
			break;

		case RT_CID_819x_MSI:
			priv->LedStrategy = SW_LED_MODE9;
			break;

		case RT_CID_WHQL:
			//Adapter->bInHctTest = true;
			break;

		default:
			RT_TRACE(COMP_INIT,"Unkown hardware Type \n");
			break;
	}
}
#else
void
HalCustomizedBehavior8192S(struct net_device* dev)
{
	//PMGNT_INFO		pMgntInfo = &(Adapter->MgntInfo);
	//HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	struct r8192_priv* priv = rtllib_priv(dev);
	priv->rtllib->bForcedShowRateStill = false;
	switch(priv->CustomerID)
	{
		case RT_CID_DEFAULT:
			priv->LedStrategy = SW_LED_MODE7;
			if(	priv->eeprom_vid == 0x10EC && priv->eeprom_did == 0x8171 &&
				priv->eeprom_svid == 0x1A3B && priv->eeprom_smid == 0x1A07)
			{
				priv->RegWirelessMode = WIRELESS_MODE_G;
				priv->rtllib->mode = WIRELESS_MODE_G;
				priv->rtllib->bForcedBgMode = true;
				priv->rtllib->bForcedShowRateStill = true;
			}
			else if(	priv->eeprom_vid == 0x10EC && priv->eeprom_did == 0x8172 &&
					priv->eeprom_svid == 0x1A3B && priv->eeprom_smid == 0x1A04)
			{
				priv->RegWirelessMode = WIRELESS_MODE_G;
				priv->rtllib->mode = WIRELESS_MODE_G;
				priv->rtllib->bForcedBgMode = true;
				priv->rtllib->bForcedShowRateStill = true;
			}
			else if(priv->eeprom_svid == 0x1A3B && priv->eeprom_smid == 0x1104)
			{
				priv->rtllib->bForcedShowRateStill = true;
			}
			else if(priv->eeprom_svid == 0x1A3B && priv->eeprom_smid == 0x1107)
			{
				priv->rtllib->bForcedShowRateStill = true;
			}

			break;

		case RT_CID_TOSHIBA:
			priv->rtllib->current_network.channel = 10;
			priv->LedStrategy = SW_LED_MODE7;
			// For some customer will pg error regulatory value to 0, it will degree EVM
			priv->EEPROMRegulatory = 1;
			if(priv->eeprom_smid >=  0x7000 && priv->eeprom_smid < 0x8000){
				priv->RegWirelessMode = WIRELESS_MODE_G;
				priv->rtllib->mode = WIRELESS_MODE_G;
				priv->rtllib->bForcedBgMode = true;
			}
			break;
		case RT_CID_CCX:
			priv->DMFlag |= (HAL_DM_DIG_DISABLE | HAL_DM_HIPWR_DISABLE);
			//priv->RegROAMSensitiveLevel = 1;
			//priv->IndicateByDeauth = true;
			break;

		case RT_CID_819x_Lenovo:
			// Customize Led mode
			priv->LedStrategy = SW_LED_MODE7;
			// Customize  Link any for auto connect
			// This Value should be set after InitializeMgntVariables
			//pMgntInfo->bAutoConnectEnable = false;
			break;

		case RT_CID_819x_QMI:
			priv->LedStrategy = SW_LED_MODE8;
			break;

		case RT_CID_819x_MSI:
			priv->LedStrategy = SW_LED_MODE9;
			break;

		case RT_CID_819x_HP:
			priv->LedStrategy = SW_LED_MODE7;
			priv->bLedOpenDrain = true;// Support Open-drain arrangement for controlling the LED. Added by Roger, 2009.10.16.

		case RT_CID_WHQL:
			;//priv->bInHctTest = true;
			break;

		default:
			RT_TRACE(COMP_INIT,"Unkown hardware Type \n");
			break;
	}
}
#endif


static	void rtl8192se_read_eeprom_info(struct net_device* dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	u16			i,usValue;
	u16			EEPROMId;
#if (EEPROM_OLD_FORMAT_SUPPORT == 1)
	u8			tmpBuffer[30];
#endif
	u8			tempval;
	u8			hwinfo[HWSET_MAX_SIZE_92S];
	u8			rf_path, index;	// For EEPROM/EFUSE After V0.6_1117

	RT_TRACE(COMP_INIT, "====> rtl8192se_read_eeprom_info\n");

	if (priv->epromtype== EEPROM_93C46)
	{	// Read frin EEPROM
		write_nic_byte(dev, SYS_ISO_CTRL+1, 0xE8); // Isolation signals from Loader
		mdelay(10);
		write_nic_byte(dev, PMC_FSM, 0x02); // Enable Loader Data Keep

		RT_TRACE(COMP_INIT, "EEPROM\n");
		// Read all Content from EEPROM or EFUSE!!!
		for(i = 0; i < HWSET_MAX_SIZE_92S; i += 2)
		{
			usValue = eprom_read(dev, (u16) (i>>1));
			*((u16*)(&hwinfo[i])) = usValue;
		}
	}
	else if (priv->epromtype == EEPROM_BOOT_EFUSE)
	{	// Read from EFUSE
		RT_TRACE(COMP_INIT, "EFUSE\n");

		// Read EFUSE real map to shadow!!
		EFUSE_ShadowMapUpdate(dev);

		memcpy( hwinfo, &priv->EfuseMap[EFUSE_INIT_MAP][0], HWSET_MAX_SIZE_92S);
	}

	// Print current HW setting map!!
	//RT_DEBUG_DATA(COMP_INIT, hwinfo, HWSET_MAX_SIZE_92S);

	// Checl 0x8129 again for making sure autoload status!!
	EEPROMId = *((u16 *)&hwinfo[0]);
	if( EEPROMId != RTL8190_EEPROM_ID )
	{
		RT_TRACE(COMP_ERR, "EEPROM ID(%#x) is invalid!!\n", EEPROMId);
		priv->AutoloadFailFlag=true;
	}
	else
	{
		RT_TRACE(COMP_EPROM, "Autoload OK\n");
		priv->AutoloadFailFlag=false;
	}

	//
	if (priv->AutoloadFailFlag == true)
	{
		rtl8192se_config_hw_for_load_fail(dev);
		return;
	}

	rtl8192se_get_IC_Inferiority(dev);

	// Read IC Version && Channel Plan
	// VID, DID	 SE	0xA-D
	priv->eeprom_vid		= *(u16 *)&hwinfo[EEPROM_VID];
	priv->eeprom_did         = *(u16 *)&hwinfo[EEPROM_DID];
	priv->eeprom_svid		= *(u16 *)&hwinfo[EEPROM_SVID];
	priv->eeprom_smid		= *(u16 *)&hwinfo[EEPROM_SMID];
	priv->eeprom_version	= *(u16 *)&hwinfo[EEPROM_Version];

	RT_TRACE(COMP_EPROM, "EEPROMId = 0x%4x\n", EEPROMId);
	RT_TRACE(COMP_EPROM, "EEPROM VID = 0x%4x\n", priv->eeprom_vid);
	RT_TRACE(COMP_EPROM, "EEPROM DID = 0x%4x\n", priv->eeprom_did);
	RT_TRACE(COMP_EPROM, "EEPROM SVID = 0x%4x\n", priv->eeprom_svid);
	RT_TRACE(COMP_EPROM, "EEPROM SMID = 0x%4x\n", priv->eeprom_smid);

	// [For 92SE/90/92U/ USB single chip, read EEPROM to decied TxRx] 2009.03.31, by Bohn
	priv->EEPROMOptional = *(u8 *)&hwinfo[EEPROM_Optional];
	// [For 92SE/90/92U/ USB single chip, read EEPROM to decied TxRx] 2009.03.31, by Bohn
	priv->ShowRateMode = 2;// 0:determine by E-fuse, 1:forced Tx, 2:forced Rx
	priv->rtllib->bForcedShowRxRate = false;

	if(priv->ShowRateMode == 0) {
	    if((priv->EEPROMOptional & BIT3) == 0x08/*0000_1000*/) {
		priv->rtllib->bForcedShowRxRate = true;
	    }
	} else if(priv->ShowRateMode == 2){
		  priv->rtllib->bForcedShowRxRate = true;
	}

      //Read Permanent MAC address
	for(i = 0; i < 6; i += 2)
	{
		usValue = *(u16 *)&hwinfo[EEPROM_MAC_ADDR+i];
		*((u16*)(&dev->dev_addr[i])) = usValue;
	}
	for (i=0;i<6;i++)
		write_nic_byte(dev, MACIDR0+i, dev->dev_addr[i]);

	RT_TRACE(COMP_EPROM, "ReadAdapterInfo8192S(), Permanent Address = %02x-%02x-%02x-%02x-%02x-%02x\n",
	dev->dev_addr[0], dev->dev_addr[1], dev->dev_addr[2], dev->dev_addr[3],
	dev->dev_addr[4], dev->dev_addr[5]);

#if (EEPROM_OLD_FORMAT_SUPPORT == 1)
	//
	// Buffer TxPwIdx(i.e., from offset 0x50~0x6D, total 28Bytes)
	// Update CCK, OFDM Tx Power Index from above buffer.
	//
	for(i = 0; i < EEPROM_TX_PWR_INDEX_RANGE; i += 2)
	{
		usValue = *(u16 *)&hwinfo[EEPROM_TxPowerBase+i];
		*((u16 *)(&tmpBuffer[i])) = usValue;
	}
	for(i=0; i<14; i++)
	{
		priv->EEPROMTxPowerLevelCCK[i] = (u8)tmpBuffer[i];
		priv->EEPROMTxPowerLevelOFDM24G[i] = (u8)tmpBuffer[i+14];
	}


	for(i=0; i<14; i++)
	{
		priv->TxPowerLevelOFDM24G[i] = priv->EEPROMTxPowerLevelOFDM24G[i];
		priv->TxPowerLevelCCK[i] = priv->EEPROMTxPowerLevelCCK[i];

		RT_TRACE(COMP_EPROM, "CH%d CCK Tx PWR IDX = 0x%02x\n", i, priv->EEPROMTxPowerLevelCCK[i]);
		RT_TRACE(COMP_EPROM, "CH%d OFDM Tx PWR IDX = 0x%02x\n", i, priv->EEPROMTxPowerLevelOFDM24G[i]);
	}

	//
	// Get HT 2T Path A and B Power Index.
	//
	for(i = 0; i < 6; i += 2)
	{
		usValue = *(u16 *)&hwinfo[EEPROM_HT2T_CH1_A+i];
		*((u16*)(&priv->EEPROMHT2T_TxPwr[i])) = usValue;
	}
	for(i=0; i<6; i++)
	{
		RT_TRACE(COMP_EPROM, "EEPROMHT2T_TxPwr, Index %d = 0x%02x\n", i, priv->EEPROMHT2T_TxPwr[i]);
	}

#else	// For 93C56 like EEPROM or EFUSe TX power index definition
	//
	// Buffer TxPwIdx(i.e., from offset 0x50~0x61, total 18Bytes)
	// Update CCK, OFDM (1T/2T)Tx Power Index from above buffer.
	//

	//
	// Get Tx Power Level by Channel
	//
	// Read Tx power of Channel 1 ~ 14 from EEPROM.
	// 92S suupport RF A & B
	for (rf_path = 0; rf_path < 2; rf_path++)
	{
		for (i = 0; i < 3; i++)
		{
			// Read CCK RF A & B Tx power
			priv->RfCckChnlAreaTxPwr[rf_path][i] =
			hwinfo[EEPROM_TxPowerBase+rf_path*3+i];

			// Read OFDM RF A & B Tx power for 1T
			priv->RfOfdmChnlAreaTxPwr1T[rf_path][i] =
			hwinfo[EEPROM_TxPowerBase+6+rf_path*3+i];

			// Read OFDM RF A & B Tx power for 2T
			priv->RfOfdmChnlAreaTxPwr2T[rf_path][i] =
			hwinfo[EEPROM_TxPowerBase+12+rf_path*3+i];
		}
	}

	for (rf_path = 0; rf_path < 2; rf_path++)
	{
		for (i = 0; i < 3; i++)
		{
			RT_TRACE(COMP_EPROM,"CCK RF-%d CHan_Area-%d = 0x%x\n",  rf_path, i,
			priv->RfCckChnlAreaTxPwr[rf_path][i]);
			RT_TRACE(COMP_EPROM, "OFDM-1T RF-%d CHan_Area-%d = 0x%x\n",  rf_path, i,
			priv->RfOfdmChnlAreaTxPwr1T[rf_path][i]);
			RT_TRACE(COMP_EPROM,"OFDM-2T RF-%d CHan_Area-%d = 0x%x\n",  rf_path, i,
			priv->RfOfdmChnlAreaTxPwr2T[rf_path][i]);
		}

		// Assign dedicated channel tx power
		for(i=0; i<14; i++)	// channel 1~3 use the same Tx Power Level.
		{
			if (i < 3)			// Chanel 1-3
				index = 0;
#ifdef _RTL8192_EXT_PATCH_
			else if (i < 9)		// Channel 4-9
#else
			else if (i < 8)		// Channel 4-8
#endif
				index = 1;
			else				// Channel 9-14
				index = 2;

			// Record A & B CCK /OFDM - 1T/2T Channel area tx power
			priv->RfTxPwrLevelCck[rf_path][i]  =
			priv->RfCckChnlAreaTxPwr[rf_path][index];
			priv->RfTxPwrLevelOfdm1T[rf_path][i]  =
			priv->RfOfdmChnlAreaTxPwr1T[rf_path][index];
			priv->RfTxPwrLevelOfdm2T[rf_path][i]  =
			priv->RfOfdmChnlAreaTxPwr2T[rf_path][index];

			if (rf_path == 0)
			{
				priv->TxPowerLevelOFDM24G[i] = priv->RfTxPwrLevelOfdm1T[rf_path][i] ;
				priv->TxPowerLevelCCK[i] = priv->RfTxPwrLevelCck[rf_path][i];
                    }
		}

		for(i=0; i<14; i++)
		{
			RT_TRACE(COMP_EPROM, "Rf-%d TxPwr CH-%d CCK OFDM_1T OFDM_2T= 0x%x/0x%x/0x%x\n",
			rf_path, i, priv->RfTxPwrLevelCck[rf_path][i],
			priv->RfTxPwrLevelOfdm1T[rf_path][i] ,
			priv->RfTxPwrLevelOfdm2T[rf_path][i] );
		}
	}

#ifndef _RTL8192_EXT_PATCH_
	for (rf_path = 0; rf_path < 2; rf_path++)
	{
		for (i = 0; i < 3; i++)
		{
			// Read Power diff limit.
			priv->EEPROMPwrGroup[rf_path][i] = hwinfo[EEPROM_TxPWRGroup+rf_path*3+i];
		}
	}

	for (rf_path = 0; rf_path < 2; rf_path++)
	{
		// Fill Pwr group
		for(i=0; i<14; i++)
		{
			if (i < 3)			// Cjanel 1-3
				index = 0;
			else if (i < 8)		// Channel 4-8
				index = 1;
			else				// Channel 9-13
				index = 2;
			priv->PwrGroupHT20[rf_path][i] = (priv->EEPROMPwrGroup[rf_path][index]&0xf);
			priv->PwrGroupHT40[rf_path][i] = ((priv->EEPROMPwrGroup[rf_path][index]&0xf0)>>4);
			RT_TRACE(COMP_INIT, "RF-%d PwrGroupHT20[%d] = 0x%x\n", rf_path, i, priv->PwrGroupHT20[rf_path][i]);
			RT_TRACE(COMP_INIT, "RF-%d PwrGroupHT40[%d] = 0x%x\n", rf_path, i, priv->PwrGroupHT40[rf_path][i]);
		}
	}
#endif

// 2009/01/20 MH add for new EEPROM format
	//
	for(i=0; i<14; i++)	// channel 1~3 use the same Tx Power Level.
	{
		// Read tx power difference between HT OFDM 20/40 MHZ
		if (i < 3)			// Cjanel 1-3
			index = 0;
#ifdef _RTL8192_EXT_PATCH_
			else if (i < 9)		// Channel 4-9
#else
			else if (i < 8)		// Channel 4-8
#endif
			index = 1;
		else				// Channel 9-14
			index = 2;

		tempval = (*(u8 *)&hwinfo[EEPROM_TX_PWR_HT20_DIFF+index])&0xff;
		priv->TxPwrHt20Diff[RF90_PATH_A][i] = (tempval&0xF);
		priv->TxPwrHt20Diff[RF90_PATH_B][i] = ((tempval>>4)&0xF);

		// Read OFDM<->HT tx power diff
		if (i < 3)			// Cjanel 1-3
			index = 0;
#ifdef _RTL8192_EXT_PATCH_
			else if (i < 9)		// Channel 4-9
#else
			else if (i < 8)		// Channel 4-8
#endif
			index = 0x11;
		else				// Channel 9-14
			index = 1;

		tempval = (*(u8 *)&hwinfo[EEPROM_TX_PWR_OFDM_DIFF+index])&0xff;
		priv->TxPwrLegacyHtDiff[RF90_PATH_A][i] = (tempval&0xF);
		priv->TxPwrLegacyHtDiff[RF90_PATH_B][i] = ((tempval>>4)&0xF);

		tempval = (*(u8 *)&hwinfo[TX_PWR_SAFETY_CHK]);
		priv->TxPwrSafetyFlag = (tempval&0x01);
	}

#ifndef _RTL8192_EXT_PATCH_
	priv->EEPROMRegulatory = 0;
	if(priv->eeprom_version >= 2)
	{
		if(priv->eeprom_version >= 4)
			priv->EEPROMRegulatory = (hwinfo[EEPROM_Regulatory]&0x7);	//bit0~2
		else
			priv->EEPROMRegulatory = (hwinfo[EEPROM_Regulatory]&0x1);	//bit0
	}
	RT_TRACE(COMP_INIT, "EEPROMRegulatory = 0x%x\n", priv->EEPROMRegulatory);
#endif
	for(i=0; i<14; i++)
		RT_TRACE(COMP_EPROM, "RF-A Ht20 to HT40 Diff[%d] = 0x%x\n", i, priv->TxPwrHt20Diff[RF90_PATH_A][i]);
	for(i=0; i<14; i++)
		RT_TRACE(COMP_EPROM, "RF-A Legacy to Ht40 Diff[%d] = 0x%x\n", i, priv->TxPwrLegacyHtDiff[RF90_PATH_A][i]);
	for(i=0; i<14; i++)
		RT_TRACE(COMP_EPROM, "RF-B Ht20 to HT40 Diff[%d] = 0x%x\n", i, priv->TxPwrHt20Diff[RF90_PATH_B][i]);
	for(i=0; i<14; i++)
		RT_TRACE(COMP_EPROM, "RF-B Legacy to HT40 Diff[%d] = 0x%x\n", i, priv->TxPwrLegacyHtDiff[RF90_PATH_B][i]);
#endif	// #if (EEPROM_OLD_FORMAT_SUPPORT == 1)
	RT_TRACE(COMP_EPROM, "TxPwrSafetyFlag = %d\n", priv->TxPwrSafetyFlag);

	//
	// Read RF-indication and Tx Power gain index diff of legacy to HT OFDM rate.
	tempval = (*(u8 *)&hwinfo[EEPROM_RFInd_PowerDiff])&0xff;
	priv->EEPROMTxPowerDiff = tempval;
#ifdef _RTL8192_EXT_PATCH_
	priv->LegacyHTTxPowerDiff = priv->EEPROMTxPowerDiff;
#else
	priv->LegacyHTTxPowerDiff = priv->TxPwrLegacyHtDiff[RF90_PATH_A][0];
#endif

	RT_TRACE(COMP_EPROM, "TxPowerDiff = %#x\n", priv->EEPROMTxPowerDiff);

	//
	// Get TSSI value for each path.
	//
	usValue = *(u16 *)&hwinfo[EEPROM_TSSI_A];
	priv->EEPROMTSSI_A = (u8)((usValue&0xff00)>>8);
	usValue = *(u8 *)&hwinfo[EEPROM_TSSI_B];
	priv->EEPROMTSSI_B = (u8)(usValue&0xff);
	RT_TRACE(COMP_EPROM, "TSSI_A = %#x, TSSI_B = %#x\n",
			priv->EEPROMTSSI_A, priv->EEPROMTSSI_B);

	//
	// Read antenna tx power offset of B/C/D to A  from EEPROM
	// and read ThermalMeter from EEPROM
	//
	tempval = *(u8 *)&hwinfo[EEPROM_ThermalMeter];
	priv->EEPROMThermalMeter = tempval;
	RT_TRACE(COMP_EPROM, "ThermalMeter = %#x\n", priv->EEPROMThermalMeter);
	// ThermalMeter, bit0~3 for RFIC1, bit4~7 for RFIC2
	priv->ThermalMeter[0] =(priv->EEPROMThermalMeter&0x1f);
	//pHalData->ThermalMeter[1] = ((pHalData->EEPROMThermalMeter & 0xf0)>>4);
	priv->TSSI_13dBm = priv->EEPROMThermalMeter *100;

	// Bluetooth coexistance
	tempval = *(u8 *)&hwinfo[EEPROM_BLUETOOTH_COEXIST];
	priv->EEPROMBluetoothCoexist = ((tempval&0x2)>>1);	// 92se, 0x78[1]
	priv->BluetoothCoexist = priv->EEPROMBluetoothCoexist;
	RT_TRACE(COMP_EPROM, "BlueTooth Coexistance = %#x\n", priv->BluetoothCoexist);

	//
	// Read CrystalCap from EEPROM
	//
	tempval = (*(u8 *)&hwinfo[EEPROM_CrystalCap])>>4;
	priv->EEPROMCrystalCap =tempval;
	RT_TRACE(COMP_EPROM, "CrystalCap = %#x\n", priv->EEPROMCrystalCap);
	// CrystalCap, bit12~15
	priv->CrystalCap = priv->EEPROMCrystalCap;

	//
	// Read IC Version && Channel Plan
	//
	// Version ID, Channel plan
	priv->eeprom_ChannelPlan = *(u8 *)&hwinfo[EEPROM_ChannelPlan];
#ifdef _RTL8192_EXT_PATCH_
	priv->eeprom_version = *(u16 *)&hwinfo[EEPROM_Version];
#endif
	priv->bTXPowerDataReadFromEEPORM = true;
	RT_TRACE(COMP_EPROM, "EEPROM ChannelPlan = 0x%4x\n", priv->eeprom_ChannelPlan);
	//RT_TRACE(COMP_INIT, DBG_LOUD, ("EEPROM Version ID: 0x%2x\n", pHalData->VersionID));

	//
	// Read Customer ID or Board Type!!!
	//
	tempval = *(u8*)&hwinfo[EEPROM_BoardType];
	// 2008/11/14 MH RTL8192SE_PCIE_EEPROM_SPEC_V0.6_20081111.doc
	// 2008/12/09 MH RTL8192SE_PCIE_EEPROM_SPEC_V0.7_20081201.doc
	// Change RF type definition
	if (tempval == 0)	// 2x2           RTL8192SE (QFN68)
		priv->rf_type= RF_2T2R;
	else if (tempval == 1)	 // 1x2           RTL8191RE (QFN68)
		priv->rf_type = RF_1T2R;
	else if (tempval == 2)	 //  1x2		RTL8191SE (QFN64) Crab
		priv->rf_type = RF_1T2R;
	else if (tempval == 3)	 //  1x1		RTL8191SE (QFN64) Unicorn
		priv->rf_type = RF_1T1R;

	//add for 92se 1x1 IOT issue
	// 2009.04.16 Add for 1T2R but 1SS (1x1 receive combining)
	priv->rtllib->RF_Type = priv->rf_type;
	priv->rtllib->b1x1RecvCombine = false;
	if (priv->rf_type == RF_1T2R)
	{
		tempval = read_nic_byte(dev, 0x07);
		if (!(tempval & BIT0))
		{
			priv->rtllib->b1x1RecvCombine = true;
			RT_TRACE(COMP_INIT, "RF_TYPE=1T2R but only 1SS\n");
		}
	}
	priv->rtllib->b1SSSupport =	priv->rtllib->b1x1RecvCombine;
	//add for 92se 1x1 IOT issue

	priv->rf_chip = RF_6052;

	priv->eeprom_CustomerID = *(u8 *)&hwinfo[EEPROM_CustomID];

	RT_TRACE(COMP_EPROM, "EEPROM Customer ID: 0x%2x, rf_chip:%x\n", priv->eeprom_CustomerID, priv->rf_chip);

	//send to stack for HT_proc use tmp.
	priv->rtllib->b_customer_lenovo_id = false;

	switch(priv->eeprom_CustomerID)
	{
		case EEPROM_CID_DEFAULT:
			if(priv->eeprom_svid == 0x10EC && priv->eeprom_smid == 0xE020){
				priv->CustomerID = RT_CID_819x_Lenovo;
				priv->rtllib->b_customer_lenovo_id = true;
                        }
			else if(priv->eeprom_svid == 0x1462 && priv->eeprom_smid == 0x6897)
				priv->CustomerID = RT_CID_819x_MSI;
			else if(priv->eeprom_svid == 0x1462 && priv->eeprom_smid == 0x3821)
				priv->CustomerID = RT_CID_819x_MSI;
			else if(	priv->eeprom_vid == 0x10EC && priv->eeprom_did == 0x8171 &&
					priv->eeprom_svid == 0x10EC && priv->eeprom_smid == 0x8186)
				priv->CustomerID = RT_CID_819x_Acer;
			else if(	priv->eeprom_vid == 0x10EC && priv->eeprom_did == 0x8171 &&
					priv->eeprom_svid == 0x10EC && priv->eeprom_smid == 0x8187)
				priv->CustomerID = RT_CID_819x_Acer;
			else if(	priv->eeprom_vid == 0x10EC && priv->eeprom_did == 0x8171 &&
					priv->eeprom_svid == 0x10EC && priv->eeprom_smid == 0x8156)
				priv->CustomerID = RT_CID_819x_Acer;
			else if(	priv->eeprom_vid == 0x10EC && priv->eeprom_did == 0x8171 &&
					priv->eeprom_svid == 0x10EC && priv->eeprom_smid == 0x8157)
				priv->CustomerID = RT_CID_819x_Acer;
			else if(	priv->eeprom_vid == 0x10EC && priv->eeprom_did == 0x8172 &&
					priv->eeprom_svid == 0x10EC && priv->eeprom_smid == 0xE021)
				priv->CustomerID = RT_CID_819x_Acer;
			else if(	priv->eeprom_vid == 0x10EC && priv->eeprom_did == 0x8172 &&
					priv->eeprom_svid == 0x10EC && priv->eeprom_smid == 0xE022)
				priv->CustomerID = RT_CID_819x_Acer;
			else if(	priv->eeprom_vid == 0x10EC && priv->eeprom_did == 0x8172 &&
					priv->eeprom_svid == 0x10EC && priv->eeprom_smid == 0x8158)
				priv->CustomerID = RT_CID_819x_Acer;
			else if(	priv->eeprom_vid == 0x10EC && priv->eeprom_did == 0x8172 &&
					priv->eeprom_svid == 0x10EC && priv->eeprom_smid == 0x8159)
				priv->CustomerID = RT_CID_819x_Acer;
			else if(	priv->eeprom_vid == 0x10EC && priv->eeprom_did == 0x8174 &&
					priv->eeprom_svid == 0x10EC && priv->eeprom_smid == 0x8186)
				priv->CustomerID = RT_CID_819x_Acer;
			else if(	priv->eeprom_vid == 0x10EC && priv->eeprom_did == 0x8174 &&
					priv->eeprom_svid == 0x10EC && priv->eeprom_smid == 0x8187)
				priv->CustomerID = RT_CID_819x_Acer;
				else if(	priv->eeprom_vid == 0x10EC && priv->eeprom_did == 0x8174 &&
					priv->eeprom_svid == 0x10EC && priv->eeprom_smid == 0x8156)
				priv->CustomerID = RT_CID_819x_Acer;
			else if(	priv->eeprom_vid == 0x10EC && priv->eeprom_did == 0x8174 &&
					priv->eeprom_svid == 0x10EC && priv->eeprom_smid == 0x8157)
				priv->CustomerID = RT_CID_819x_Acer;
			else if(	priv->eeprom_vid == 0x10EC && priv->eeprom_did == 0x8171 &&
					priv->eeprom_svid == 0x103C && priv->eeprom_smid == 0x1467)
			 priv->CustomerID = RT_CID_819x_HP;
			else
			priv->CustomerID = RT_CID_DEFAULT;
			break;

		case EEPROM_CID_TOSHIBA:
			priv->CustomerID = RT_CID_TOSHIBA;
			break;

		case EEPROM_CID_QMI:
			priv->CustomerID = RT_CID_819x_QMI;
			break;

		case EEPROM_CID_WHQL:
#ifdef TO_DO_LIST
			priv->bInHctTest = true;
			priv->bSupportTurboMode = false;
			priv->bAutoTurboBy8186 = false;
			priv->PowerSaveControl.bInactivePs = false;
			priv->PowerSaveControl.bIPSModeBackup = false;
			priv->PowerSaveControl.bLeisurePs = false;
			priv->keepAliveLevel = 0;
			priv->bUnloadDriverwhenS3S4 = false;
#endif
			break;

		default:
			priv->CustomerID = RT_CID_DEFAULT;
			break;

	}

#ifdef _RTL8192_EXT_PATCH_
	HalCustomizedBehavior8192S(dev);
#endif

	// 2008/01/16 MH We can only know RF type in the function. So we have to init
	// DIG RATR table again.
	//FIXME:YJ,090317
	//InitRateAdaptive(dev);

	RT_TRACE(COMP_INIT, "<==== rtl8192se_read_eeprom_info\n");
}

void rtl8192se_get_eeprom_size(struct net_device* dev)
{
	struct r8192_priv* priv = rtllib_priv(dev);
	struct rtllib_device* ieee = priv->rtllib;
	u8 curCR = 0;
	curCR = read_nic_byte(dev, EPROM_CMD);
	// For debug test now!!!!!
	PHY_RFShadowRefresh(dev);
	if (curCR & BIT4){
		RT_TRACE(COMP_EPROM, "Boot from EEPROM\n");
		priv->epromtype = EEPROM_93C46;
	}
	else{
		RT_TRACE(COMP_EPROM, "Boot from EFUSE\n");
		priv->epromtype = EEPROM_BOOT_EFUSE;
	}
	if (curCR & BIT5){
		RT_TRACE(COMP_EPROM,"Autoload OK\n");
		priv->AutoloadFailFlag=false;
		rtl8192se_read_eeprom_info(dev);
	}
	else{// Auto load fail.
		RT_TRACE(COMP_INIT, "AutoLoad Fail reported from CR9346!!\n");
		priv->AutoloadFailFlag=true;
		rtl8192se_config_hw_for_load_fail(dev);

		if (priv->epromtype == EEPROM_BOOT_EFUSE)
		{
#if (RTL92SE_FPGA_VERIFY == 0)
			EFUSE_ShadowMapUpdate(dev);
#endif
		}
	}
#ifdef TO_DO_LIST
	if(Adapter->bInHctTest)
	{
		pMgntInfo->PowerSaveControl.bInactivePs = false;
		pMgntInfo->PowerSaveControl.bIPSModeBackup = false;
		pMgntInfo->PowerSaveControl.bLeisurePs = false;
		pMgntInfo->keepAliveLevel = 0;
	}

	switch(pHalData->eeprom_CustomerID)
	{
		case EEPROM_CID_DEFAULT:
			pMgntInfo->CustomerID = RT_CID_DEFAULT;
			break;
		case EEPROM_CID_TOSHIBA:
			pMgntInfo->CustomerID = RT_CID_TOSHIBA;
			break;
		default:
			// value from RegCustomerID
			break;
	}

#ifdef _RTL8192_EXT_PATCH_
	if(pHalData->eeprom_svid == 0x10EC && pHalData->eeprom_smid == 0xE020)
#else
	if(pHalData->EEPROMSVID == 0x10EC && pHalData->EEPROMSMID == 0xE020)
#endif
		pMgntInfo->CustomerID = RT_CID_819x_Lenovo;
#endif
#ifdef ENABLE_DOT11D
	// force the channel plan to world wide 13
	priv->ChannelPlan = COUNTRY_CODE_WORLD_WIDE_13;
#endif

	if(priv->ChannelPlan == COUNTRY_CODE_GLOBAL_DOMAIN) {
		GET_DOT11D_INFO(ieee)->bEnabled = 1;
		RT_TRACE(COMP_INIT, "%s: Enable dot11d when RT_CHANNEL_DOMAIN_GLOBAL_DOAMIN!\n", __FUNCTION__);
	}

	RT_TRACE(COMP_INIT, "RegChannelPlan(%d) EEPROMChannelPlan(%d)", \
			priv->RegChannelPlan, priv->eeprom_ChannelPlan);
	RT_TRACE(COMP_INIT, "ChannelPlan = %d\n" , priv->ChannelPlan);
#ifndef _RTL8192_EXT_PATCH_
	HalCustomizedBehavior8192S(dev);
#endif
}

#if (RTL92SE_FPGA_VERIFY == 1)
// * Function:	MacConfigBeforeFwDownload()
// *
// * Overview:	Copy from WMAc for init setting. MAC config before FW download
// *
// * Input:		NONE
// *
// * Output:		NONE
// *
// * Return:		NONE
// *
// * Revised History:
// *	When		Who		Remark
// *	04/29/2008	MHC		The same as 92SE MAC initialization.
// *	07/11/2008	MHC		MAC config before FW download
// *
// *---------------------------------------------------------------------------*/
static void MacConfigBeforeFwDownload(struct net_device* dev)
{
	struct r8192_priv* priv = rtllib_priv(dev);
	u8				i;
	u8				tmpU1b;
	u16				tmpU2b;
	u32				addr;

	// 2008/05/14 MH For 92SE rest before init MAC data sheet not defined now.
	// HW provide the method to prevent press reset bbutton every time.
	RT_TRACE(COMP_INIT, "Set some register before enable NIC\r\n");

	tmpU1b = read_nic_byte(dev, 0x562);
	tmpU1b |= 0x08;
	write_nic_byte(dev, 0x562, tmpU1b);
	tmpU1b &= ~(BIT3);
	write_nic_byte(dev, 0x562, tmpU1b);

	tmpU1b = read_nic_byte(dev, SYS_FUNC_EN+1);
	tmpU1b &= 0x73;
	write_nic_byte(dev, SYS_FUNC_EN+1, tmpU1b);

	tmpU1b = read_nic_byte(dev, SYS_CLKR);
	tmpU1b &= 0xfa;
	write_nic_byte(dev, SYS_CLKR, tmpU1b);

	RT_TRACE(COMP_INIT, "Delay 1000ms before reset NIC. I dont know how long should we delay!!!!!\r\n");
	ssleep(1);

	// Switch to 80M clock
	write_nic_byte(dev, SYS_CLKR, SYS_CLKSEL_80M);

	 // Enable VSPS12 LDO Macro block
	tmpU1b = read_nic_byte(dev, SPS1_CTRL);
	write_nic_byte(dev, SPS1_CTRL, (tmpU1b|SPS1_LDEN));

	//Enable AFE Macro Block's Bandgap
	tmpU1b = read_nic_byte(dev, AFE_MISC);
	write_nic_byte(dev, AFE_MISC, (tmpU1b|AFE_BGEN));

	//Enable LDOA15 block
	tmpU1b = read_nic_byte(dev, LDOA15_CTRL);
	write_nic_byte(dev, LDOA15_CTRL, (tmpU1b|LDA15_EN));

	 //Enable VSPS12_SW Macro Block
	tmpU1b = read_nic_byte(dev, SPS1_CTRL);
	write_nic_byte(dev, SPS1_CTRL, (tmpU1b|SPS1_SWEN));

	//Enable AFE Macro Block's Mbias
	tmpU1b = read_nic_byte(dev, AFE_MISC);
	write_nic_byte(dev, AFE_MISC, (tmpU1b|AFE_MBEN));

	// Set Digital Vdd to Retention isolation Path.
	tmpU2b = read_nic_word(dev, SYS_ISO_CTRL);
	write_nic_word(dev, SYS_ISO_CTRL, (tmpU2b|ISO_PWC_DV2RP));

	// Attatch AFE PLL to MACTOP/BB/PCIe Digital
	tmpU2b = read_nic_word(dev, SYS_ISO_CTRL);
	write_nic_word(dev, SYS_ISO_CTRL, (tmpU2b &(~ISO_PWC_RV2RP)));

	//Enable AFE clock
	tmpU2b = read_nic_word(dev, AFE_XTAL_CTRL);
	write_nic_word(dev, AFE_XTAL_CTRL, (tmpU2b &(~XTAL_GATE_AFE)));

	//Enable AFE PLL Macro Block
	tmpU1b = read_nic_byte(dev, AFE_PLL_CTRL);
	write_nic_byte(dev, AFE_PLL_CTRL, (tmpU1b|APLL_EN));

	// Release isolation AFE PLL & MD
	write_nic_byte(dev, SYS_ISO_CTRL, 0xEE);

	//Enable MAC clock
	tmpU2b = read_nic_word(dev, SYS_CLKR);
	write_nic_word(dev, SYS_CLKR, (tmpU2b|SYS_MAC_CLK_EN));

	//Enable Core digital and enable IOREG R/W
	tmpU2b = read_nic_word(dev, SYS_FUNC_EN);
	write_nic_word(dev, SYS_FUNC_EN, (tmpU2b|FEN_DCORE|FEN_MREGEN));

	 //Switch the control path.
	tmpU2b = read_nic_word(dev, SYS_CLKR);
	write_nic_word(dev, SYS_CLKR, ((tmpU2b|SYS_FWHW_SEL)&(~SYS_SWHW_SEL)));

	// Reset RF temporarily!!!!
	write_nic_byte(dev, RF_CTRL, 0);
	write_nic_byte(dev, RF_CTRL, 7);

	write_nic_word(dev, CMDR, 0x37FC);

#if 1
	// Load MAC register from WMAc temporarily We simulate macreg.txt HW will provide
	// MAC txt later
	write_nic_byte(dev, 0x6, 0x30);
	//write_nic_byte(dev, 0x48, 0x2f);
	write_nic_byte(dev, 0x49, 0xf0);

	write_nic_byte(dev, 0x4b, 0x81);

	write_nic_byte(dev, 0xb5, 0x21);

	write_nic_byte(dev, 0xdc, 0xff);
	write_nic_byte(dev, 0xdd, 0xff);
	write_nic_byte(dev, 0xde, 0xff);
	write_nic_byte(dev, 0xdf, 0xff);

	write_nic_byte(dev, 0x11a, 0x00);
	write_nic_byte(dev, 0x11b, 0x00);

	for (i = 0; i < 32; i++)
		write_nic_byte(dev, INIMCS_SEL+i, 0x1b);

	write_nic_byte(dev, 0x236, 0xff);

	write_nic_byte(dev, 0x503, 0x22);

	if(priv->bIntelBridgeExist) {
		write_nic_byte(dev, 0x560, 0x40);
#ifndef _RTL8192_EXT_PATCH_
	} else {
		write_nic_byte(dev, 0x560, 0x00);
#endif
	}

	write_nic_byte(dev, DBG_PORT, 0x91);
#endif

	//Set RX Desc Address
#if TODO_LIST //rx cmd packet?
	write_nic_dword(dev, RCDA,
	pHalData->RxDescMemory[RX_CMD_QUEUE].PhysicalAddressLow);
#endif
	write_nic_dword(dev, RDQDA, priv->rx_ring_dma);
	rtl8192_tx_enable(dev);

	RT_TRACE(COMP_INIT, "<---MacConfig8192SE()\n");

}	/* MacConfigBeforeFwDownload */
#else
//
// * Function:	gen_RefreshLedState()
// *
// * Overview:	When we call the function, media status is no link. It must be in SW/HW
// *			radio off. Or IPS state. If IPS no link we will turn on LED, otherwise, we must turn off.
// *			After MAC IO reset, we must write LED control 0x2f2 again.
// *
// * Input:		IN	PADAPTER			Adapter)
// *
// * Output:		NONE
// *
// * Return:		NONE
// *
// * Revised History:
// *	When		Who		Remark
// *	03/27/2009	MHC		Create for LED judge only~!!
// *
void gen_RefreshLedState(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	PLED_8190 pLed0 = &(priv->SwLed0);

	if(priv->bfirst_init)
	{
		printk("gen_RefreshLedState first init\n");
		return;
	}

	//printk("priv->rtllib->RfOffReason=%x\n", priv->rtllib->RfOffReason);
	if(priv->rtllib->RfOffReason == RF_CHANGE_BY_IPS )
	{
		//PlatformEFIOWrite1Byte(Adapter, 0x2f2, 0x00);
		SwLedOn(dev, pLed0);
//		Adapter->HalFunc.LedControlHandler(Adapter,LED_CTL_NO_LINK);
	}
	else		// SW/HW radio off
	{
		// Turn off LED if RF is not ON.
		SwLedOff(dev, pLed0);
//		Adapter->HalFunc.LedControlHandler(Adapter, LED_CTL_POWER_OFF);
	}

}	// gen_RefreshLedState
//-----------------------------------------------------------------------------
// Function:	MacConfigBeforeFwDownload()
//
// Overview:	Copy from WMAc for init setting. MAC config before FW download
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
//	07/11/2008	MHC		MAC config before FW download
//	09/02/2008	MHC		The initialize sequence can preven NIC reload fail
//						NIC will not disappear when power domain init twice.
//	11/04/2008	MHC		Modify power on seq, we must reset MACIO/CPU/Digital Core
//						Pull low and then high.
//
//---------------------------------------------------------------------------*/
static void MacConfigBeforeFwDownload(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	u8				i;
	u8				tmpU1b;
	u16				tmpU2b;
	//u32				addr;
	u8				PollingCnt = 20;

	// 2008/05/14 MH For 92SE rest before init MAC data sheet not defined now.
	RT_TRACE(COMP_INIT, "--->MacConfigBeforeFwDownload()\n");
//	printk("--->MacConfigBeforeFwDownload()\n");
	//added by amy 090408 from windows
	if(priv->bfirst_init)
	{
		// 2009/03/24 MH Reset PCIE Digital
		tmpU1b = read_nic_byte(dev, SYS_FUNC_EN+1);
		tmpU1b &= 0xFE;
		write_nic_byte(dev, SYS_FUNC_EN+1, tmpU1b);
		udelay(1);
		write_nic_byte(dev, SYS_FUNC_EN+1, tmpU1b|BIT0);
	}
	//added by amy 090408 from windows end
	// Switch to SW IO control
	tmpU1b = read_nic_byte(dev, (SYS_CLKR + 1));
	if(tmpU1b & BIT7)
	{
		tmpU1b &= ~(BIT6 | BIT7);
		if(!HalSetSysClk8192SE(dev, tmpU1b))
			return; // Set failed, return to prevent hang.
	}
        write_nic_byte(dev, AFE_PLL_CTRL, 0x0);
        write_nic_byte(dev, LDOA15_CTRL, 0x34);//0x30);//0x50);

	// Clear FW RPWM for FW control LPS. by tynli. 2009.02.23
	write_nic_byte(dev, RPWM, 0x0);

	// Reset MAC-IO and CPU and Core Digital BIT10/11/15
	tmpU1b = read_nic_byte(dev, SYS_FUNC_EN+1);
	tmpU1b &= 0x73;
	write_nic_byte(dev, SYS_FUNC_EN+1, tmpU1b);
	// wait for BIT 10/11/15 to pull high automatically!!
	mdelay(1);

	write_nic_byte(dev, CMDR, 0);
	write_nic_byte(dev, TCR, 0);

#if (DEMO_BOARD_SUPPORT == 0)
	tmpU1b = read_nic_byte(dev, SPS1_CTRL);
	tmpU1b &= 0xfc;
	write_nic_byte(dev, SPS1_CTRL, tmpU1b);
#endif

	// Data sheet not define 0x562!!! Copy from WMAC!!!!!
	tmpU1b = read_nic_byte(dev, 0x562);
	tmpU1b |= 0x08;
	write_nic_byte(dev, 0x562, tmpU1b);
	tmpU1b &= ~(BIT3);
	write_nic_byte(dev, 0x562, tmpU1b);

	//tmpU1b = read_nic_byte(dev, SYS_FUNC_EN+1);
	//tmpU1b &= 0x73;
	//write_nic_byte(dev, SYS_FUNC_EN+1, tmpU1b);

	//tmpU1b = read_nic_byte(dev, SYS_CLKR);
	//tmpU1b &= 0xfa;
	//write_nic_byte(dev, SYS_CLKR, tmpU1b);

	// Switch to 80M clock
	//write_nic_byte(dev, SYS_CLKR, SYS_CLKSEL_80M);

	 // Enable VSPS12 LDO Macro block
	//tmpU1b = read_nic_byte(dev, SPS1_CTRL);
	//write_nic_byte(dev, SPS1_CTRL, (tmpU1b|SPS1_LDEN));

	//Enable AFE clock source
	RT_TRACE(COMP_INIT, "Enable AFE clock source\r\n");
	tmpU1b = read_nic_byte(dev, AFE_XTAL_CTRL);
	write_nic_byte(dev, AFE_XTAL_CTRL, (tmpU1b|0x01));
	// Delay 1.5ms
	udelay(2000);
	tmpU1b = read_nic_byte(dev, AFE_XTAL_CTRL+1);
	write_nic_byte(dev, AFE_XTAL_CTRL+1, (tmpU1b&0xfb));


	//Enable AFE Macro Block's Bandgap
	RT_TRACE(COMP_INIT, "Enable AFE Macro Block's Bandgap\r\n");
	tmpU1b = read_nic_byte(dev, AFE_MISC);
	write_nic_byte(dev, AFE_MISC, (tmpU1b|BIT0));
	mdelay(1);

	//Enable AFE Mbias
	RT_TRACE(COMP_INIT, "Enable AFE Mbias\r\n");
	tmpU1b = read_nic_byte(dev, AFE_MISC);
	write_nic_byte(dev, AFE_MISC, (tmpU1b|0x02));
	mdelay(1);

	//Enable LDOA15 block
	tmpU1b = read_nic_byte(dev, LDOA15_CTRL);
	write_nic_byte(dev, LDOA15_CTRL, (tmpU1b|BIT0));

	//Enable VSPS12_SW Macro Block
	//tmpU1b = read_nic_byte(dev, SPS1_CTRL);
	//write_nic_byte(dev, SPS1_CTRL, (tmpU1b|SPS1_SWEN));

	//Enable AFE Macro Block's Mbias
	//tmpU1b = read_nic_byte(dev, AFE_MISC);
	//write_nic_byte(dev, AFE_MISC, (tmpU1b|AFE_MBEN));

	// Set Digital Vdd to Retention isolation Path.
	tmpU2b = read_nic_word(dev, SYS_ISO_CTRL);
	write_nic_word(dev, SYS_ISO_CTRL, (tmpU2b|BIT11));

	// Attatch AFE PLL to MACTOP/BB/PCIe Digital
	//tmpU2b = read_nic_word(dev, SYS_ISO_CTRL);
	//write_nic_word(dev, SYS_ISO_CTRL, (tmpU2b &(~BIT12)));

	// 2008/09/25 MH From SD1 Jong, For warm reboot NIC disappera bug.
	tmpU2b = read_nic_word(dev, SYS_FUNC_EN);
#ifdef _RTL8192_EXT_PATCH_
	write_nic_word(dev, SYS_FUNC_EN, tmpU2b |= BIT13);
#else
	write_nic_word(dev, SYS_FUNC_EN, (tmpU2b |BIT13));
#endif

	//Enable AFE clock
	//tmpU2b = read_nic_word(dev, AFE_XTAL_CTRL);
	//write_nic_word(dev, AFE_XTAL_CTRL, (tmpU2b &(~XTAL_GATE_AFE)));

	write_nic_byte(dev, SYS_ISO_CTRL+1, 0x68);

	//Enable AFE PLL Macro Block
	// For low temperature test!!!! We need to delay 100u before enabling PLL.
	udelay(200);
	tmpU1b = read_nic_byte(dev, AFE_PLL_CTRL);
	write_nic_byte(dev, AFE_PLL_CTRL, (tmpU1b|BIT0|BIT4));

#if 1 // for divider reset
	udelay(100);
	write_nic_byte(dev, AFE_PLL_CTRL, (tmpU1b|BIT0|BIT4|BIT6));
	udelay(10);
	write_nic_byte(dev, AFE_PLL_CTRL, (tmpU1b|BIT0|BIT4));
	udelay(10);
#endif
	// Enable MAC 80MHZ clock  //added by amy 090408
	tmpU1b = read_nic_byte(dev, AFE_PLL_CTRL+1);
	write_nic_byte(dev, AFE_PLL_CTRL+1, (tmpU1b|BIT0));
	mdelay(1);

	// Release isolation AFE PLL & MD
	write_nic_byte(dev, SYS_ISO_CTRL, 0xA6);

	//Enable MAC clock
	tmpU2b = read_nic_word(dev, SYS_CLKR);
	//write_nic_word(dev, SYS_CLKR, (tmpU2b|SYS_MAC_CLK_EN));
	write_nic_word(dev, SYS_CLKR, (tmpU2b|BIT12|BIT11));

	//Enable Core digital and enable IOREG R/W
	tmpU2b = read_nic_word(dev, SYS_FUNC_EN);
	write_nic_word(dev, SYS_FUNC_EN, (tmpU2b|BIT11));

	//added by amy 090408
	tmpU1b = read_nic_byte(dev, SYS_FUNC_EN+1);
	write_nic_byte(dev, SYS_FUNC_EN+1, tmpU1b&~(BIT7));
	//added by amy 090408 end

	//enable REG_EN
	write_nic_word(dev, SYS_FUNC_EN, (tmpU2b|BIT11|BIT15));

	 //Switch the control path.
	 tmpU2b = read_nic_word(dev, SYS_CLKR);
	write_nic_word(dev, SYS_CLKR, (tmpU2b&(~BIT2)));

	tmpU1b = read_nic_byte(dev, (SYS_CLKR + 1));
	tmpU1b = ((tmpU1b | BIT7) & (~BIT6));
	if(!HalSetSysClk8192SE(dev, tmpU1b))
		return; // Set failed, return to prevent hang.

#if 0
	tmpU2b = read_nic_word(dev, SYS_CLKR);
	write_nic_word(dev, SYS_CLKR, ((tmpU2b|BIT15)&(~BIT14)));
#endif

	//write_nic_word(dev, CMDR, 0x37FC);
	write_nic_word(dev, CMDR, 0x07FC);//added by amy 090408

#if 1
	// 2008/09/02 MH We must enable the section of code to prevent load IMEM fail.
	// Load MAC register from WMAc temporarily We simulate macreg.txt HW will provide
	// MAC txt later
	write_nic_byte(dev, 0x6, 0x30);
	//write_nic_byte(dev, 0x48, 0x2f);
	write_nic_byte(dev, 0x49, 0xf0);

	write_nic_byte(dev, 0x4b, 0x81);

	write_nic_byte(dev, 0xb5, 0x21);

	write_nic_byte(dev, 0xdc, 0xff);
	write_nic_byte(dev, 0xdd, 0xff);
	write_nic_byte(dev, 0xde, 0xff);
	write_nic_byte(dev, 0xdf, 0xff);

	write_nic_byte(dev, 0x11a, 0x00);
	write_nic_byte(dev, 0x11b, 0x00);

	for (i = 0; i < 32; i++)
		write_nic_byte(dev, INIMCS_SEL+i, 0x1b);

	write_nic_byte(dev, 0x236, 0xff);

	write_nic_byte(dev, 0x503, 0x22);
	//write_nic_byte(dev, 0x560, 0x09);
	// 2008/11/26 MH From SD1 Jong's suggestion for clock request bug.
	//write_nic_byte(dev, 0x560, 0x79);
	// 2008/12/26 MH From SD1 Victor's suggestion for clock request bug.
	if (priv->pci_bridge_vendor & (PCI_BRIDGE_VENDOR_INTEL | PCI_BRIDGE_VENDOR_SIS)) {
		write_nic_byte(dev, 0x560, 0x40);
	} else {
		write_nic_byte(dev, 0x560, 0x00);
	}

	write_nic_byte(dev, DBG_PORT, 0x91);
#endif

	//Set RX Desc Address
	write_nic_dword(dev, RDQDA, priv->rx_ring_dma);
#if 0
	write_nic_dword(dev, RCDA,
	pHalData->RxDescMemory[RX_CMD_QUEUE].PhysicalAddressLow);
#endif
	//Set TX Desc Address
	rtl8192_tx_enable(dev);

	write_nic_word(dev, CMDR, 0x37FC);
	//
	// <Roger_EXP> To make sure that TxDMA can ready to download FW.
	// We should reset TxDMA if IMEM RPT was not ready.
	// Suggested by SD1 Alex. 2008.10.23.
	//
	do {
		tmpU1b = read_nic_byte(dev, TCR);
		if((tmpU1b & TXDMA_INIT_VALUE) == TXDMA_INIT_VALUE)
			break;

		udelay(5);
	} while(PollingCnt--);	// Delay 1ms

	if(PollingCnt <= 0 )
	{
		RT_TRACE(COMP_ERR, "MacConfigBeforeFwDownloadASIC(): Polling TXDMA_INIT_VALUE timeout!! Current TCR(%#x)\n", tmpU1b);
		tmpU1b = read_nic_byte(dev, CMDR);
		write_nic_byte(dev, CMDR, tmpU1b&(~TXDMA_EN));
		udelay(2);
		write_nic_byte(dev, CMDR, tmpU1b|TXDMA_EN);// Reset TxDMA
	}

	//added by amy 090408
	// After MACIO reset,we must refresh LED state.
	gen_RefreshLedState(dev);
	//added by amy 090408 end

	RT_TRACE(COMP_INIT, "<---MacConfigBeforeFwDownload()\n");

}	/* MacConfigBeforeFwDownload */
#endif

static void MacConfigAfterFwDownload(struct net_device* dev)
{
	u8				i;
	//u8				tmpU1b;
	u16				tmpU2b;
	struct r8192_priv* priv = rtllib_priv(dev);

	//
	// 1. System Configure Register (Offset: 0x0000 - 0x003F)
	//

	//
	// 2. Command Control Register (Offset: 0x0040 - 0x004F)
	//
	// Turn on 0x40 Command register
#ifdef _RTL8192_EXT_PATCH_	//FIXME:YJ: marked in sta driver.
	write_nic_word(dev, CMDR,
	BBRSTn|BB_GLB_RSTn|SCHEDULE_EN|MACRXEN|MACTXEN|DDMA_EN|FW2HW_EN|
	RXDMA_EN|TXDMA_EN|HCI_RXDMA_EN|HCI_TXDMA_EN);
#else
	write_nic_byte(dev, CMDR,
	(u8)(BBRSTn|BB_GLB_RSTn|SCHEDULE_EN|MACRXEN|MACTXEN|DDMA_EN|FW2HW_EN|
	RXDMA_EN|TXDMA_EN|HCI_RXDMA_EN|HCI_TXDMA_EN));
#endif
	// Set TCR TX DMA pre 2 FULL enable bit
	write_nic_dword(dev, TCR,
	read_nic_dword(dev, TCR)|TXDMAPRE2FULL);
	// Set RCR
	write_nic_dword(dev, RCR, priv->ReceiveConfig);

	//
	// 3. MACID Setting Register (Offset: 0x0050 - 0x007F)
	//
//move to start adapter
#if 0
	for (i=0;i<6;i++)
	write_nic_byte(dev, MACIDR0+i, dev->dev_addr[i]);
#endif
	//
	// 4. Timing Control Register  (Offset: 0x0080 - 0x009F)
	//
	// Set CCK/OFDM SIFS
	write_nic_word(dev, SIFS_CCK, 0x0a0a); // CCK SIFS shall always be 10us.
	write_nic_word(dev, SIFS_OFDM, 0x1010);
	//3
	//Set AckTimeout
	write_nic_byte(dev, ACK_TIMEOUT, 0x40);

	//Beacon related
	write_nic_word(dev, BCN_INTERVAL, 100);
	write_nic_word(dev, ATIMWND, 2);
#ifdef _ENABLE_SW_BEACON
        write_nic_word(dev, BCN_DRV_EARLY_INT, BIT15);
#endif
	//write_nic_word(dev, BCN_DMATIME, 0xC8);
	//write_nic_word(dev, BCN_ERR_THRESH, 0xFF);
	//write_nic_byte(dev, MLT, 8);

	//
	// 5. FIFO Control Register (Offset: 0x00A0 - 0x015F)
	//
	// 5.1 Initialize Number of Reserved Pages in Firmware Queue
	// Firmware allocate now, associate with FW internal setting.!!!
#if 0
	write_nic_dword(dev, RQPN1,
	NUM_OF_PAGE_IN_FW_QUEUE_BK<<0 | NUM_OF_PAGE_IN_FW_QUEUE_BE<<8 |\
	NUM_OF_PAGE_IN_FW_QUEUE_VI<<16 | NUM_OF_PAGE_IN_FW_QUEUE_VO<<24);
	write_nic_dword(dev, RQPN2,
	NUM_OF_PAGE_IN_FW_QUEUE_HCCA << 0 | NUM_OF_PAGE_IN_FW_QUEUE_CMD << 8|\
	NUM_OF_PAGE_IN_FW_QUEUE_MGNT << 16 |NUM_OF_PAGE_IN_FW_QUEUE_HIGH << 24);
	write_nic_dword(dev, RQPN3,
	NUM_OF_PAGE_IN_FW_QUEUE_BCN<<0 | NUM_OF_PAGE_IN_FW_QUEUE_PUB<<8);
	// Active FW/MAC to load the new RQPN setting
	write_nic_byte(dev, LD_RQPN, BIT7);
#endif

	// 5.2 Setting TX/RX page size 0/1/2/3/4=64/128/256/512/1024
	//write_nic_byte(dev, PBP, 0x22);


	// 5.3 Set driver info, we only accept PHY status now.
	//write_nic_byte(dev, RXDRVINFO_SZ, 4);

	// 5.4 Set RXDMA arbitration to control RXDMA/MAC/FW R/W for RXFIFO
	write_nic_byte(dev, RXDMA,
							read_nic_byte(dev, RXDMA)|BIT6);

	//
	// 6. Adaptive Control Register  (Offset: 0x0160 - 0x01CF)
	//
	// Set RRSR to all legacy rate and HT rate
	// CCK rate is supported by default.
	// CCK rate will be filtered out only when associated AP does not support it.
	// Only enable ACK rate to OFDM 24M
	// 2008/09/24 MH Disable RRSR for CCK rate in A-Cut
//#if (DISABLE_CCK == 1)
	if (priv->card_8192_version== VERSION_8192S_ACUT)
		write_nic_byte(dev, RRSR, 0xf0);
#ifdef _RTL8192_EXT_PATCH_
	else if (priv->card_8192_version == VERSION_8192S_BCUT)
		write_nic_byte(dev, RRSR, 0xff);
#endif
	write_nic_byte(dev, RRSR+1, 0x01);
	write_nic_byte(dev, RRSR+2, 0x00);

	// 2008/09/22 MH A-Cut IC do not support CCK rate. We forbid ARFR to
	// fallback to CCK rate
	for (i = 0; i < 8; i++)
	{
	// 2008/09/24 MH Disable RRSR for CCK rate in A-Cut

//#if (DISABLE_CCK == 1)
		if (priv->card_8192_version == VERSION_8192S_ACUT)
			write_nic_dword(dev, ARFR0+i*4, 0x1f0ff0f0);
		else if (priv->card_8192_version == VERSION_8192S_BCUT)
			write_nic_dword(dev, ARFR0+i*4, 0x1f0ff0f0);
	}

	// Different rate use different AMPDU size
	// MCS32/ MCS15_SG use max AMPDU size 15*2=30K
	write_nic_byte(dev, AGGLEN_LMT_H, 0x0f);
	// MCS0/1/2/3 use max AMPDU size 4*2=8K
	write_nic_word(dev, AGGLEN_LMT_L, 0x7442);
	// MCS4/5 use max AMPDU size 8*2=16K 6/7 use 10*2=20K
	write_nic_word(dev, AGGLEN_LMT_L+2, 0xddd7);
	// MCS8/9 use max AMPDU size 8*2=16K 10/11 use 10*2=20K
	write_nic_word(dev, AGGLEN_LMT_L+4, 0xd772);
	// MCS12/13/14/15 use max AMPDU size 15*2=30K
	write_nic_word(dev, AGGLEN_LMT_L+6, 0xfffd);
	//write_nic_word(dev, AGGLEN_LMT_L+6, 0xFFFF);

	// Set Data / Response auto rate fallack retry count
	write_nic_dword(dev, DARFRC, 0x04010000);
	write_nic_dword(dev, DARFRC+4, 0x09070605);
	write_nic_dword(dev, RARFRC, 0x04010000);
	write_nic_dword(dev, RARFRC+4, 0x09070605);

	// MCS/CCK TX Auto Gain Control Register
	//write_nic_dword(dev, MCS_TXAGC, 0x0D0C0C0C);
	//write_nic_dword(dev, MCS_TXAGC+4, 0x0F0E0E0D);
	//write_nic_word(dev, CCK_TXAGC, 0x0000);


	//
	// 7. EDCA Setting Register (Offset: 0x01D0 - 0x01FF)
	//
	// BCNTCFG
	//write_nic_word(dev, BCNTCFG, 0x0000);
	// Set all rate to support SG
	write_nic_word(dev, SG_RATE, 0xFFFF);


	//
	// 8. WMAC, BA, and CCX related Register (Offset: 0x0200 - 0x023F)
	//
	// Set NAV protection length
	write_nic_word(dev, NAV_PROT_LEN, 0x0080);
	// CF-END Threshold
	write_nic_byte(dev, CFEND_TH, 0xFF);
	// Set AMPDU minimum space
	write_nic_byte(dev, AMPDU_MIN_SPACE, 0x07);
	// Set TXOP stall control for several queue/HI/BCN/MGT/
	write_nic_byte(dev, TXOP_STALL_CTRL, 0x00);

	// 9. Security Control Register (Offset: 0x0240 - 0x025F)
	// 10. Power Save Control Register (Offset: 0x0260 - 0x02DF)
	// 11. General Purpose Register (Offset: 0x02E0 - 0x02FF)
	// 12. Host Interrupt Status Register (Offset: 0x0300 - 0x030F)
	// 13. Test Mode and Debug Control Register (Offset: 0x0310 - 0x034F)

	//
	// 13. HOST/MAC PCIE Interface Setting
	//
	// Set Tx dma burst
	/*write_nic_byte(dev, PCIF, ((MXDMA2_NoLimit<<MXDMA2_RX_SHIFT) | \
											(MXDMA2_NoLimit<<MXDMA2_TX_SHIFT) | \
											(1<<MULRW_SHIFT)));*/

	//
	// 14. Set driver info, we only accept PHY status now.
	//
	write_nic_byte(dev, RXDRVINFO_SZ, 4);

	//
	// 15. For EEPROM R/W Workaround
	// 16. For EFUSE to share SYS_FUNC_EN with EEPROM!!!
	//
	tmpU2b= read_nic_byte(dev, SYS_FUNC_EN);
	write_nic_byte(dev, SYS_FUNC_EN, tmpU2b | BIT13);
	tmpU2b= read_nic_byte(dev, SYS_ISO_CTRL);
	write_nic_byte(dev, SYS_ISO_CTRL, tmpU2b & (~BIT8));

	//
	// 16. For EFUSE
	//
	if (priv->epromtype == EEPROM_BOOT_EFUSE)	// We may R/W EFUSE in EEPROM mode
	{
		u8	tempval;

		tempval = read_nic_byte(dev, SYS_ISO_CTRL+1);
		tempval &= 0xFE;
		write_nic_byte(dev, SYS_ISO_CTRL+1, tempval);

		// Enable LDO 2.5V for write action
		//tempval = read_nic_byte(dev, EFUSE_TEST+3);
		//write_nic_byte(dev, EFUSE_TEST+3, (tempval | 0x80));

		// Change Efuse Clock for write action
		//write_nic_byte(dev, EFUSE_CLK, 0x03);

		// Change Program timing
		write_nic_byte(dev, EFUSE_CTRL+3, 0x72);
		RT_TRACE(COMP_INIT, "EFUSE CONFIG OK\n");
	}
	RT_TRACE(COMP_INIT, "MacConfigAfterFwDownload OK\n");

}	/* MacConfigAfterFwDownload */

static void rtl8192se_HalDetectPwrDownMode(struct net_device*dev)
{
    u8 tmpvalue;
    struct r8192_priv *priv = rtllib_priv(dev);
    EFUSE_ShadowRead(dev, 1, 0x78, (u32 *)&tmpvalue);

    if (tmpvalue & BIT0) {
        priv->pwrdown = true;
    } else {
        priv->pwrdown = false;
    }
}

//added by amy 090330 end
void HwConfigureRTL8192SE(struct net_device *dev)
{

	struct r8192_priv* priv = rtllib_priv(dev);

	u8	regBwOpMode = 0;
	u32	regRATR = 0, regRRSR = 0;
	u8	regTmp = 0;


	//1 This part need to modified according to the rate set we filtered!!
	//
	// Set RRSR, RATR, and BW_OPMODE registers
	//
	switch(priv->rtllib->mode)
	{
	case WIRELESS_MODE_B:
		regBwOpMode = BW_OPMODE_20MHZ;
		regRATR = RATE_ALL_CCK;
		regRRSR = RATE_ALL_CCK;
		break;
	case WIRELESS_MODE_A:
		regBwOpMode = BW_OPMODE_5G |BW_OPMODE_20MHZ;
		regRATR = RATE_ALL_OFDM_AG;
		regRRSR = RATE_ALL_OFDM_AG;
		break;
	case WIRELESS_MODE_G:
		regBwOpMode = BW_OPMODE_20MHZ;
		regRATR = RATE_ALL_CCK | RATE_ALL_OFDM_AG;
		regRRSR = RATE_ALL_CCK | RATE_ALL_OFDM_AG;
		break;
	case WIRELESS_MODE_AUTO:
	case WIRELESS_MODE_N_24G:
		// It support CCK rate by default.
		// CCK rate will be filtered out only when associated AP does not support it.
			regBwOpMode = BW_OPMODE_20MHZ;
			regRATR = RATE_ALL_CCK | RATE_ALL_OFDM_AG | RATE_ALL_OFDM_1SS | RATE_ALL_OFDM_2SS;
			regRRSR = RATE_ALL_CCK | RATE_ALL_OFDM_AG;
		break;
	case WIRELESS_MODE_N_5G:
		regBwOpMode = BW_OPMODE_5G;
		regRATR = RATE_ALL_OFDM_AG | RATE_ALL_OFDM_1SS | RATE_ALL_OFDM_2SS;
		regRRSR = RATE_ALL_OFDM_AG;
		break;
	default://(for B|G)
		regBwOpMode = BW_OPMODE_20MHZ;
		regRATR = RATE_ALL_CCK | RATE_ALL_OFDM_AG;
		regRRSR = RATE_ALL_CCK | RATE_ALL_OFDM_AG;
		break;
	}

	regTmp = read_nic_byte(dev, INIRTSMCS_SEL);
#if (RTL8192SU_DISABLE_CCK_RATE == 1)
	regRRSR = ((regRRSR & 0x000ffff0)<<8) | regTmp;
#else
	regRRSR = ((regRRSR & 0x000fffff)<<8) | regTmp;
#endif
	write_nic_dword(dev, INIRTSMCS_SEL, regRRSR);

	// 2007/02/07 Mark by Emily becasue we have not verify whether this register works
	write_nic_byte(dev, BW_OPMODE, regBwOpMode);
	//
	// Set Retry Limit here
	//
	priv->rtllib->SetHwRegHandler(dev, HW_VAR_RETRY_LIMIT, (u8*)(&priv->ShortRetryLimit));

	write_nic_byte(dev, MLT, 0x8f);
	// Set Contention Window here

	// Set Tx AGC

	// Set Tx Antenna including Feedback control

	// Set Auto Rate fallback control
	//
	// For Min Spacing configuration.
	//
#if 1
	switch(priv->rf_type)
	{
		case RF_1T2R:
		case RF_1T1R:
			RT_TRACE(COMP_INIT, "Initializeadapter: RF_Type%s\n", priv->rf_type==RF_1T1R? "(1T1R)":"(1T2R)");
                priv->rtllib->MinSpaceCfg = (MAX_MSS_DENSITY_1T<<3);
			break;
		case RF_2T2R:
		case RF_2T2R_GREEN:
			RT_TRACE(COMP_INIT, "Initializeadapter:RF_Type(2T2R)\n");
                priv->rtllib->MinSpaceCfg = (MAX_MSS_DENSITY_2T<<3);
			break;
	}
	write_nic_byte(dev, AMPDU_MIN_SPACE, priv->rtllib->MinSpaceCfg);
#else
	priv->rtllib->MinSpaceCfg = 0x90;	//cosa, asked by scott, for MCS15 short GI, padding patch, 0x237[7:3] = 0x12.
	SetHwReg8192SE(dev, HW_VAR_AMPDU_MIN_SPACE, (u8*)(&priv->rtllib->MinSpaceCfg));
#endif
}

void
RF_RECOVERY(struct net_device*dev)
{
	u8 offset = 0x25;
	for(;offset<0x29;offset++)
	{
		PHY_RFShadowCompareFlagSet(dev, (RF90_RADIO_PATH_E)0, offset,true);
		if(PHY_RFShadowCompare(dev, (RF90_RADIO_PATH_E)0, offset))
		{
			PHY_RFShadowRecorverFlagSet(dev, (RF90_RADIO_PATH_E)0, offset, true);
			PHY_RFShadowRecorver( dev, (RF90_RADIO_PATH_E)0, offset);
		}
	}
}

bool rtl8192se_adapter_start(struct net_device* dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	//struct rtllib_device* ieee = priv->rtllib;
	//u32				ulRegRead;
	bool rtStatus	= true;
	u8				tmpU1b;
	//u16				tmpU2b;
	u8				eRFPath;
	u8				fw_download_times = 1;
	u8				i=0;
	RT_TRACE(COMP_INIT, "rtl8192se_adapter_start()\n");
	priv->being_init_adapter = true;
       //
       //disable aspm before initialize HW because we
       // use dma to download fw
       //
#ifdef RTL8192SE_CONFIG_ASPM_OR_D3
	RT_DISABLE_ASPM(dev);
#endif
start:
	rtl8192_pci_resetdescring(dev);
	//
	// 1. MAC Initialize
	//
	// Before FW download, we have to set some MAC register
	MacConfigBeforeFwDownload(dev);

	// Read Version ID from Reg4 bit 16-19. EEPROM may not PGed, we will asign as ACUT
	priv->card_8192_version
	= (VERSION_8192S)((read_nic_dword(dev, PMC_FSM)>>16)&0xF);

	printk("=================> NIC version : %s\n", ((read_nic_dword(dev, PMC_FSM)>>15)&0x1)?"C-cut":"B-cut");

	// Read macreg.txt before firmware download !!???
	// Firmware download
	rtStatus = FirmwareDownload92S((struct net_device*)dev); //WB
	if(rtStatus != true)
	{
		if(fw_download_times <= 10){
			RT_TRACE(COMP_INIT, "rtl8192se_adapter_start(): Download Firmware failed %d times, Download again!!\n",fw_download_times);
			fw_download_times = fw_download_times + 1;
			goto start;
		}else{
			RT_TRACE(COMP_INIT, "rtl8192se_adapter_start(): Download Firmware failed 10, end!!\n");
			goto end;
		}
	}

	// After FW download, we have to reset MAC register
	MacConfigAfterFwDownload(dev);

	// <Roger_Notes> Retrieve default FW Cmd IO map. 2009.05.08.
	priv->FwCmdIOMap =	read_nic_word(dev, LBUS_MON_ADDR);
	priv->FwCmdIOParam = read_nic_dword(dev, LBUS_ADDR_MASK);

	// Read Efuse after MAC init OK, we can switch efuse clock from 40M to 500K.
	//ReadAdapterInfo8192S(dev);

#if (RTL8192S_DISABLE_FW_DM == 1)
	write_nic_dword(dev, WFM5, FW_DM_DISABLE);
	ChkFwCmdIoDone(dev);
	write_nic_dword(dev, WFM5, FW_TXANT_SWITCH_DISABLE);
	ChkFwCmdIoDone(dev);
#endif

	//
	// 2. Initialize MAC/PHY Config by MACPHY_reg.txt
	//
#if (HAL_MAC_ENABLE == 1)
	RT_TRACE(COMP_INIT, "MAC Config Start!\n");
	if (PHY_MACConfig8192S(dev) != true)
	{
		RT_TRACE(COMP_ERR, "MAC Config failed\n");
		return rtStatus;
	}
	RT_TRACE(COMP_INIT, "MAC Config Finished!\n");
#endif	// #if (HAL_MAC_ENABLE == 1)

	//
	// 2008/12/19 MH Make sure BB/RF write OK. We should prevent enter IPS. radio off.
	// We must set flag avoid BB/RF config period later!!
	//
	write_nic_dword(dev, CMDR, 0x37FC);

	//
	// 3. Initialize BB After MAC Config PHY_reg.txt, AGC_Tab.txt
	//
#if (HAL_BB_ENABLE == 1)
	RT_TRACE(COMP_INIT, "BB Config Start!\n");
	if (PHY_BBConfig8192S(dev) != true)
	{
		RT_TRACE(COMP_INIT, "BB Config failed\n");
		return rtStatus;
	}
	RT_TRACE(COMP_INIT, "BB Config Finished!\n");
#endif	// #if (HAL_BB_ENABLE == 1)


	//
	// 4. Initiailze RF RAIO_A.txt RF RAIO_B.txt
	//
	// 2007/11/02 MH Before initalizing RF. We can not use FW to do RF-R/W.
	priv->Rf_Mode = RF_OP_By_SW_3wire;
#if (HAL_RF_ENABLE == 1)
	RT_TRACE(COMP_INIT, "RF Config started!\n");

#if (RTL92SE_FPGA_VERIFY == 0)
	// Before RF-R/W we must execute the IO from Scott's suggestion.
	write_nic_byte(dev, AFE_XTAL_CTRL+1, 0xDB);
	if(priv->card_8192_version== VERSION_8192S_ACUT)
		write_nic_byte(dev, SPS1_CTRL+3, 0x07);
	else
		write_nic_byte(dev, RF_CTRL, 0x07);
#endif
	if(PHY_RFConfig8192S(dev) != true)
	{
		RT_TRACE(COMP_ERR, "RF Config failed\n");
		return rtStatus;
	}
	RT_TRACE(COMP_INIT, "RF Config Finished!\n");
#endif	// #if (HAL_RF_ENABLE == 1)

	// After read predefined TXT, we must set BB/MAC/RF register as our requirement

	priv->RfRegChnlVal[0] = rtl8192_phy_QueryRFReg(dev, (RF90_RADIO_PATH_E)0, RF_CHNLBW, bRFRegOffsetMask);
	priv->RfRegChnlVal[1] = rtl8192_phy_QueryRFReg(dev, (RF90_RADIO_PATH_E)1, RF_CHNLBW, bRFRegOffsetMask);

	//---- Set CCK and OFDM Block "ON"----*/
	rtl8192_setBBreg(dev, rFPGA0_RFMOD, bCCKEn, 0x1);
	rtl8192_setBBreg(dev, rFPGA0_RFMOD, bOFDMEn, 0x1);

	//3 Set Hardware(Do nothing now)
	HwConfigureRTL8192SE(dev);

	//3 Set Wireless Mode
	// TODO: Emily 2006.07.13. Wireless mode should be set according to registry setting and RF type
	//Default wireless mode is set to "WIRELESS_MODE_N_24G|WIRELESS_MODE_G",
	//and the RRSR is set to Legacy OFDM rate sets. We do not include the bit mask
	//of WIRELESS_MODE_B currently. Emily, 2006.11.13
	//For wireless mode setting from mass.
	if(priv->ResetProgress == RESET_TYPE_NORESET)
		rtl8192_SetWirelessMode(dev, priv->rtllib->mode);
	//-----------------------------------------------------------------------------
	// Set up security related. 070106, by rcnjko:
	// 1. Clear all H/W keys.
	// 2. Enable H/W encryption/decryption.
	//-----------------------------------------------------------------------------
	CamResetAllEntry(dev);
	{
		u8 SECR_value = 0x0;
		SECR_value |= SCR_TxEncEnable;
		SECR_value |= SCR_RxDecEnable;
		SECR_value |= SCR_NoSKMC;
		write_nic_byte(dev, SECR, SECR_value);
	}

	{
		int i;
		for (i=0; i<4; i++)
		 write_nic_dword(dev, WDCAPARA_ADD[i], 0x5e4322);
	}
	//
	// Read EEPROM TX power index and PHY_REG_PG.txt to capture correct
	// TX power index for different rate set.
	//
//	if(priv->card_8192_version>= VERSION_8192S_ACUT) //always true
	{
		// Get original hw reg values */
		PHY_GetHWRegOriginalValue(dev);
		// Write correct tx power index */
#ifndef CONFIG_MP
		rtl8192_phy_setTxPower(dev, priv->chan);
#endif
	}

	//2=======================================================
	// RF Power Save
	//2=======================================================
	//
	// We need to check power down mode before HW radio detect!!.
	//
	rtl8192se_HalDetectPwrDownMode(dev);
#if 1
	if(priv->RegRfOff == true)
	{ // User disable RF via registry.
		RT_TRACE((COMP_INIT|COMP_RF), "InitializeAdapter8190(): Turn off RF for RegRfOff ----------\n");
//		printk("InitializeAdapter8190(): Turn off RF for RegRfOff ----------\n");
		MgntActSet_RF_State(dev, eRfOff, RF_CHANGE_BY_SW);

		// Those action will be discard in MgntActSet_RF_State because off the same state
		for(eRFPath = 0; eRFPath <priv->NumTotalRFPath; eRFPath++)
			rtl8192_phy_SetRFReg(dev, (RF90_RADIO_PATH_E)eRFPath, 0x4, 0xC00, 0x0);

	}
	else if(priv->rtllib->RfOffReason > RF_CHANGE_BY_PS)
	{ // H/W or S/W RF OFF before sleep.
		RT_TRACE((COMP_INIT|COMP_RF), "InitializeAdapter8190(): Turn off RF for RfOffReason(%d) ----------\n", priv->rtllib->RfOffReason);
//		printk("InitializeAdapter8190(): Turn off RF for RfOffReason(%d) ----------\n", priv->rtllib->RfOffReason);
		MgntActSet_RF_State(dev, eRfOff, priv->rtllib->RfOffReason);
	}
	else
	{
            //lzm gpio radio on/off is out of adapter start
            if(priv->bHwRadioOff == false){
		priv->rtllib->eRFPowerState = eRfOn;
		priv->rtllib->RfOffReason = 0;
		// LED control
		if(priv->rtllib->LedControlHandler)
			priv->rtllib->LedControlHandler(dev, LED_CTL_POWER_ON);
	}
	}
#endif

	//
	// Execute TX power tracking later
	//

	//  For test.
	//DM_ShadowInit(dev);

	// We must set MAC address after firmware download. HW do not support MAC addr
	// autoload now.
	{
		int i;
		for (i=0;i<6;i++)
			write_nic_byte(dev, MACIDR0+i, dev->dev_addr[i]);
	}

	// EEPROM R/W workaround
	tmpU1b = read_nic_byte(dev, MAC_PINMUX_CFG);
	write_nic_byte(dev, MAC_PINMUX_CFG, tmpU1b&(~BIT3));

	// Enable CCX related HW configurations. By Bruce, 2009-02-27.
	if(priv->CustomerID == RT_CID_CCX)
	{
		//
		// Enable FW Tx feedback report.
		//
		RT_TRACE(COMP_INIT ,"InitializeAdapter8192SE(): Set FW Cmd FW_TX_FEEDBACK_CCX_ENABLE\n");
		write_nic_dword(dev, WFM5, FW_TX_FEEDBACK_CCX_ENABLE);
		ChkFwCmdIoDone(dev);

		// Disable DIG and High Power for CCX test. These mechanism would affect the test result.
		write_nic_dword(dev, WFM5, FW_HIGH_PWR_DISABLE); //break;
		ChkFwCmdIoDone(dev);
		write_nic_dword(dev, WFM5, FW_DIG_HALT);
		ChkFwCmdIoDone(dev);

		// According to SD3 luke's suggest. Initial gain should be set 0x1C.
		write_nic_byte(dev, 0xC50, 0x1C);
		write_nic_byte(dev, 0xC58, 0x1C);
	}

	//
	// <Roger_Notes> We enable high power and RA related mechanism after NIC initialized.
	// 2008.11.27.
	//
	if(priv->pFirmware->FirmwareVersion >= 0x35)
	{// Fw v.53 and later.
		priv->rtllib->SetFwCmdHandler(dev, FW_CMD_RA_INIT);
	}
       else if(priv->pFirmware->FirmwareVersion >= 0x34)
	{// Fw v.52 and later.
		write_nic_dword(dev, WFM5, FW_RA_INIT);
		ChkFwCmdIoDone(dev);
	}
	else
	{// Compatible earlier FW version.
	write_nic_dword(dev, WFM5, FW_RA_RESET);
	ChkFwCmdIoDone(dev);
	write_nic_dword(dev, WFM5, FW_RA_ACTIVE);
	ChkFwCmdIoDone(dev);
	write_nic_dword(dev, WFM5, FW_RA_REFRESH);
		ChkFwCmdIoDone(dev);
        }

	// Execute RF check with last rounf resume!!!!

	//
	// 2008/12/22 MH Wroaround to rePG section1 data.
	//
	//EFUSE_RePgSection1(dev);
	//Remove to make sure not to make chip to a stone!!!!!!!!!!!!!!!!!!!!!!!

	//
	// 2008/12/26 MH Add to prevent ASPM bug.
	// 2008/12/27 MH Always enable hst and NIC clock request.
	//
	//if(!priv->isRFOff)//FIXME: once IPS enter,the isRFOff is set to true,but never set false, is it a bug?? amy 090331
		PHY_SwitchEphyParameter(dev);
	//PHY_EnableHostClkReq(Adapter);
	RF_RECOVERY(dev);

	//
	// Restore all of the initialization or silent reset related count
	//
	priv->SilentResetRxSoltNum = 4;	// if the value is 4, driver detect rx stuck after 8 seconds without receiving packets
	priv->SilentResetRxSlotIndex = 0;
	for( i=0; i < MAX_SILENT_RESET_RX_SLOT_NUM; i++ )
	{
		priv->SilentResetRxStuckEvent[i] = 0;
	}

	if(priv->BluetoothCoexist)
	{
		printk("Write reg 0x%x = 1 for Bluetooth Co-existance\n", SYSF_CFG);
		write_nic_byte(dev, SYSF_CFG, 0x1);
	}

	priv->bIgnoreSilentReset = true;

//	printk("==================>original RCR:%x, need to rewrite to fix aes bug:%x\n", read_nic_dword(dev, RCR), priv->ReceiveConfig);
//FIXME:YJ,090316
	//write_nic_dword(dev, RCR, priv->ReceiveConfig);	//YJ,del,090320. priv->ReceiveConfig is set zero in rtl8192se_link_change after rtl8192_down, so when up again, RCR will be set to zero.
	// Execute RF check with last rounf resume!!!!
	rtl8192_irq_enable(dev);
end:
	priv->being_init_adapter = false;
	return rtStatus;


}

void rtl8192se_net_update(struct net_device *dev)
{

	struct r8192_priv *priv = rtllib_priv(dev);
//	struct rtllib_device* ieee = priv->rtllib;
	struct rtllib_network *net = &priv->rtllib->current_network;
	//u16 BcnTimeCfg = 0, BcnCW = 6, BcnIFS = 0xf;
	u16 rate_config = 0;
	u32 regTmp = 0;
	u8 rateIndex = 0;
	u8	RetryLimit = 0x30;
	u16 cap = net->capability;

	priv->short_preamble = cap & WLAN_CAPABILITY_SHORT_PREAMBLE;

	// 1. Basic rate: RR, BRSR
	//Adapter->HalFunc.SetHwRegHandler( Adapter, HW_VAR_BASIC_RATE, (pu1Byte)(&pMgntInfo->mBrates) );
	{
	//update Basic rate: RR, BRSR
	rtl8192_config_rate(dev, &rate_config);
	if (priv->card_8192_version== VERSION_8192S_ACUT)
		priv->basic_rate = rate_config  = rate_config & 0x150;
	else if (priv->card_8192_version == VERSION_8192S_BCUT)
		priv->basic_rate= rate_config = rate_config & 0x15f;

#if 1
	if(priv->rtllib->pHTInfo->IOTPeer == HT_IOT_PEER_CISCO && ((rate_config &0x150)==0))
	{
                 // if peer is cisco and didn't use ofdm rate,
                 // we use 6M for ack.
		rate_config |=0x010;
	}
	if(priv->rtllib->pHTInfo->IOTPeer & HT_IOT_ACT_WA_IOT_Broadcom)
	{
		rate_config &= 0x1f0;
		printk("HW_VAR_BASIC_RATE, HT_IOT_ACT_WA_IOT_Broadcom, BrateCfg = 0x%x\n", rate_config);
	}

			// Set RRSR rate table.
			write_nic_byte(dev, RRSR, rate_config&0xff);
			write_nic_byte(dev, RRSR+1, (rate_config>>8)&0xff);

			// Set RTS initial rate
			while(rate_config > 0x1)
			{
				rate_config = (rate_config>> 1);
				rateIndex++;
			}
			write_nic_byte(dev, INIRTSMCS_SEL, rateIndex);

			//set ack preample
			regTmp = (priv->nCur40MhzPrimeSC) << 5;
			if (priv->short_preamble)
				regTmp |= 0x80;
			write_nic_byte(dev, RRSR+2, regTmp);
#endif
	}

	// 2. BSSID
	write_nic_dword(dev,BSSIDR,((u32*)net->bssid)[0]);
	write_nic_word(dev,BSSIDR+4,((u16*)net->bssid)[2]);

	if (priv->rtllib->iw_mode == IW_MODE_ADHOC){
		RetryLimit = HAL_RETRY_LIMIT_AP_ADHOC;
	} else {
		RetryLimit = (priv->CustomerID == RT_CID_CCX) ? 7 : HAL_RETRY_LIMIT_INFRA;
	}
	priv->rtllib->SetHwRegHandler(dev, HW_VAR_RETRY_LIMIT, (u8*)(&RetryLimit));

	// 4. BcnIntv
	if (priv->rtllib->iw_mode == IW_MODE_ADHOC){
		priv->rtllib->SetHwRegHandler( dev, HW_VAR_BEACON_INTERVAL, (u8*)(&net->beacon_interval));
	}

	rtl8192_update_cap(dev, cap);
}

#ifdef _RTL8192_EXT_PATCH_
void rtl8192se_mesh_net_update(struct net_device *dev)
{

	struct r8192_priv *priv = rtllib_priv(dev);
	struct rtllib_network *net = &priv->rtllib->current_mesh_network;
	u16 rate_config = 0;
	u32 regTmp = 0;
	u8 rateIndex = 0;
	u8	retrylimit = 0x7;
	u16 cap = net->capability;
	/* At the peer mesh mode, the peer MP shall recognize the short preamble */
	priv->short_preamble = 1;

	//update Basic rate: RR, BRSR
	if (priv->card_8192_version== VERSION_8192S_ACUT)
		priv->basic_rate = rate_config  = 0x150;
	else if (priv->card_8192_version == VERSION_8192S_BCUT)
		priv->basic_rate= rate_config = 0x15f;

	// Set RRSR rate table.
	write_nic_byte(dev, RRSR, rate_config&0xff);
	write_nic_byte(dev, RRSR+1, (rate_config>>8)&0xff);

	// Set RTS initial rate
	while(rate_config > 0x1)
	{
		rate_config = (rate_config>> 1);
		rateIndex++;
	}
	write_nic_byte(dev, INIRTSMCS_SEL, rateIndex);

	//set ack preample
	regTmp = (priv->nCur40MhzPrimeSC) << 5;
	if (priv->short_preamble)
		regTmp |= 0x80;
	write_nic_byte(dev, RRSR+2, regTmp);

//	write_nic_dword(dev,BSSIDR,((u32*)net->bssid)[0]);
//	write_nic_word(dev,BSSIDR+4,((u16*)net->bssid)[2]);

	write_nic_word(dev, BCN_INTERVAL, net->beacon_interval);
	//2008.10.24 added by tynli for beacon changed.
	PHY_SetBeaconHwReg( dev, net->beacon_interval);
	rtl8192_update_cap(dev, cap);

	priv->ShortRetryLimit =
		priv->LongRetryLimit = retrylimit;

	write_nic_word(dev,RETRY_LIMIT, \
			retrylimit << RETRY_LIMIT_SHORT_SHIFT | \
			retrylimit << RETRY_LIMIT_LONG_SHIFT);
}
#endif

void rtl8192se_link_change(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	struct rtllib_device* ieee = priv->rtllib;
	u32 reg = 0;
	reg = read_nic_dword(dev, RCR);
#ifdef _RTL8192_EXT_PATCH_
	printk("===>%s():ieee->iw_mode is %d\n",__FUNCTION__,ieee->iw_mode);
	if (ieee->state == RTLLIB_LINKED) {
                // <Roger_Notes> High power is disabled in default. 2009.05.06.
		if(ieee->pHTInfo->IOTAction & HT_IOT_ACT_DISABLE_HIGH_POWER)
			ieee->SetFwCmdHandler(dev, FW_CMD_HIGH_PWR_DISABLE);
		else
			ieee->SetFwCmdHandler(dev, FW_CMD_HIGH_PWR_ENABLE);
		rtl8192se_net_update(dev);
		if(ieee->bUseRAMask){
			//printk("=============>%s():bUseRAMask is true\n",__FUNCTION__);
			ieee->UpdateHalRAMaskHandler(
									dev,
									false,
									0,
									NULL,
									ieee->mode,
									0);
			priv->rssi_level = 0;
		//	printk("=======>priv->rssi_level is %d\n",priv->rssi_level);
		}
		else{
			//printk("=============>%s():bUseRAMask is false\n",__FUNCTION__);
			rtl8192se_update_ratr_table(dev,ieee->dot11HTOperationalRateSet,NULL);
		}
		priv->ReceiveConfig = reg |= RCR_CBSSID;
	} else if ((ieee->mesh_state == RTLLIB_MESH_LINKED) && ieee->only_mesh) {
		rtl8192se_mesh_net_update(dev);
		priv->ReceiveConfig = reg &= ~RCR_CBSSID;
		//priv->ReceiveConfig = reg |= RCR_CBSSID;
	} else{
		priv->ReceiveConfig = reg &= ~RCR_CBSSID;
	}
	write_nic_dword(dev, RCR, reg);
#else
	if (ieee->state == RTLLIB_LINKED) {
		if(priv->DM_Type == DM_Type_ByFW) {
		// <Roger_Notes> High power is disabled in default. 2009.05.06.
		if(ieee->pHTInfo->IOTAction & HT_IOT_ACT_DISABLE_HIGH_POWER)
			ieee->SetFwCmdHandler(dev, FW_CMD_HIGH_PWR_DISABLE);
		else
			ieee->SetFwCmdHandler(dev, FW_CMD_HIGH_PWR_ENABLE);
		}

		rtl8192se_net_update(dev);
		rtl8192se_update_ratr_table(dev,ieee->dot11HTOperationalRateSet,NULL);
		priv->ReceiveConfig = reg |= RCR_CBSSID;
	} else {
		priv->ReceiveConfig = reg &= ~RCR_CBSSID;
	}
	write_nic_dword(dev, RCR, reg);
#endif
	rtl8192_update_msr(dev);
	{		//// For 92SE test we must reset TST after setting MSR
		u32	temp = read_nic_dword(dev, TCR);
		write_nic_dword(dev, TCR, temp&(~BIT8));
		write_nic_dword(dev, TCR, temp|BIT8);
	}

}

u8 MRateToHwRate8192SE(struct net_device*dev, u8 rate)
{
	u8	ret = DESC92S_RATE1M;
	u16	max_sg_rate;
	static	u16	multibss_sg_old = 0x1234;// Can not assign as zero for init value
	u16	multibss_sg;

	switch(rate)
	{
	case MGN_1M:	ret = DESC92S_RATE1M;		break;
	case MGN_2M:	ret = DESC92S_RATE2M;		break;
	case MGN_5_5M:	ret = DESC92S_RATE5_5M;	break;
	case MGN_11M:	ret = DESC92S_RATE11M;	break;
	case MGN_6M:	ret = DESC92S_RATE6M;		break;
	case MGN_9M:	ret = DESC92S_RATE9M;		break;
	case MGN_12M:	ret = DESC92S_RATE12M;	break;
	case MGN_18M:	ret = DESC92S_RATE18M;	break;
	case MGN_24M:	ret = DESC92S_RATE24M;	break;
	case MGN_36M:	ret = DESC92S_RATE36M;	break;
	case MGN_48M:	ret = DESC92S_RATE48M;	break;
	case MGN_54M:	ret = DESC92S_RATE54M;	break;

	// HT rate since here
	case MGN_MCS0:		ret = DESC92S_RATEMCS0;	break;
	case MGN_MCS1:		ret = DESC92S_RATEMCS1;	break;
	case MGN_MCS2:		ret = DESC92S_RATEMCS2;	break;
	case MGN_MCS3:		ret = DESC92S_RATEMCS3;	break;
	case MGN_MCS4:		ret = DESC92S_RATEMCS4;	break;
	case MGN_MCS5:		ret = DESC92S_RATEMCS5;	break;
	case MGN_MCS6:		ret = DESC92S_RATEMCS6;	break;
	case MGN_MCS7:		ret = DESC92S_RATEMCS7;	break;
	case MGN_MCS8:		ret = DESC92S_RATEMCS8;	break;
	case MGN_MCS9:		ret = DESC92S_RATEMCS9;	break;
	case MGN_MCS10:	ret = DESC92S_RATEMCS10;	break;
	case MGN_MCS11:	ret = DESC92S_RATEMCS11;	break;
	case MGN_MCS12:	ret = DESC92S_RATEMCS12;	break;
	case MGN_MCS13:	ret = DESC92S_RATEMCS13;	break;
	case MGN_MCS14:	ret = DESC92S_RATEMCS14;	break;
	case MGN_MCS15:	ret = DESC92S_RATEMCS15;	break;

	// Set the highest SG rate
	case MGN_MCS0_SG:
	case MGN_MCS1_SG:
	case MGN_MCS2_SG:
	case MGN_MCS3_SG:
	case MGN_MCS4_SG:
	case MGN_MCS5_SG:
	case MGN_MCS6_SG:
	case MGN_MCS7_SG:
	case MGN_MCS8_SG:
	case MGN_MCS9_SG:
	case MGN_MCS10_SG:
	case MGN_MCS11_SG:
	case MGN_MCS12_SG:
	case MGN_MCS13_SG:
	case MGN_MCS14_SG:
	case MGN_MCS15_SG:
			ret = DESC92S_RATEMCS15_SG;
			//#if(RTL8192S_PREPARE_FOR_NORMAL_RELEASE==0)
			max_sg_rate = rate&0xf;
			multibss_sg = max_sg_rate | (max_sg_rate<<4) | (max_sg_rate<<8) | (max_sg_rate<<12);
			if (multibss_sg_old != multibss_sg)
			{
				write_nic_dword(dev, SG_RATE, multibss_sg);
				multibss_sg_old = multibss_sg;
			}
			break;
			//#endif


	case (0x80|0x20):	ret = DESC92S_RATEMCS32; break;

	default:				ret = DESC92S_RATEMCS15;	break;

	}

	return ret;
}

void  rtl8192se_tx_fill_desc(struct net_device* dev, tx_desc * pDesc, cb_desc * cb_desc, struct sk_buff* skb)
{
	u8				*pSeq;
	u16				Temp;
	struct r8192_priv* priv = rtllib_priv(dev);

	struct rtllib_hdr_1addr * header = NULL;

	dma_addr_t mapping = pci_map_single(priv->pdev, skb->data, skb->len, PCI_DMA_TODEVICE);

	u16 fc=0, stype=0;
	header = (struct rtllib_hdr_1addr *)(((u8*)skb->data));
	fc = header->frame_ctl;
	stype = WLAN_FC_GET_STYPE(fc);
	// Clear all status
	memset((void*)pDesc, 0, 32);		// 92SE Descriptor size to MAC

	// This part can just fill to the first descriptor of the frame.
//	if(bFirstSeg) //always we set no fragment, so disable here temporarily. WB. 2008.12.25
	{

		//pTxFwInfo->EnableCPUDur = pTcb->bTxEnableFwCalcDur;
#ifdef _RTL8192_EXT_PATCH_
		if(priv->rtllib->bUseRAMask){
			//printk("=============>%s():bUseRAMask is true\n",__FUNCTION__);
			//set txdesc macId
			if(cb_desc->macId < 32)
			{
				pDesc->MacID = cb_desc->macId;
				pDesc->Rsvd_MacID = cb_desc->macId;
			}
		}
#endif
		pDesc->TXHT		= (cb_desc->data_rate&0x80)?1:0;

#if (RTL92SE_FPGA_VERIFY == 0)
		if (priv->card_8192_version== VERSION_8192S_ACUT)
		{
			if (cb_desc->data_rate== MGN_1M || cb_desc->data_rate == MGN_2M ||
				cb_desc->data_rate == MGN_5_5M || cb_desc->data_rate == MGN_11M)
			{
				cb_desc->data_rate = MGN_12M;
			}
		}
#endif
		pDesc->TxRate	= MRateToHwRate8192SE(dev,cb_desc->data_rate);
		pDesc->TxShort	= QueryIsShort(((cb_desc->data_rate&0x80)?1:0), MRateToHwRate8192SE(dev,cb_desc->data_rate), cb_desc);

		//
		// Aggregation related
		//
		if(cb_desc->bAMPDUEnable)
		{
			pDesc->AggEn = 1;
			//SET_TX_DESC_AGG_ENABLE(pDesc, 1);
		}
		else
		{
			pDesc->AggEn = 0;
			//SET_TX_DESC_AGG_ENABLE(pDesc, 0);
		}

		{
			pSeq = (u8 *)(skb->data+22);
			Temp = pSeq[0];
			Temp <<= 12;
			Temp |= (*(u16 *)pSeq)>>4;
			pDesc->Seq = Temp;
		}

		//
		// Protection mode related
		//
		// For 92S, if RTS/CTS are set, HW will execute RTS.
		// We choose only one protection mode to execute
		//pDesc->RTSEn	= (pTcb->bRTSEnable)?1:0;
		pDesc->RTSEn	= (cb_desc->bRTSEnable && cb_desc->bCTSEnable==false)?1:0;
		pDesc->CTS2Self	= (cb_desc->bCTSEnable)?1:0;
		pDesc->RTSSTBC	= (cb_desc->bRTSSTBC)?1:0;
		pDesc->RTSHT	= (cb_desc->rts_rate&0x80)?1:0;

#if (RTL92SE_FPGA_VERIFY == 0)
		if (priv->card_8192_version== VERSION_8192S_ACUT)
		{
			if (cb_desc->rts_rate == MGN_1M || cb_desc->rts_rate == MGN_2M ||
				cb_desc->rts_rate == MGN_5_5M || cb_desc->rts_rate == MGN_11M)
			{
				cb_desc->rts_rate = MGN_12M;
			}
		}
#endif
		pDesc->RTSRate	= MRateToHwRate8192SE(dev,cb_desc->rts_rate);
		pDesc->RTSRate	= MRateToHwRate8192SE(dev,MGN_24M);
		pDesc->RTSBW	= 0;
		pDesc->RTSSC	= cb_desc->RTSSC;
		pDesc->RTSShort	= (pDesc->RTSHT==0)?(cb_desc->bRTSUseShortPreamble?1:0):(cb_desc->bRTSUseShortGI?1:0);
		//
		// Set Bandwidth and sub-channel settings.
		//
		if(priv->CurrentChannelBW == HT_CHANNEL_WIDTH_20_40)
		{
			if(cb_desc->bPacketBW)
			{
				pDesc->TxBw		= 1;
				pDesc->TXSC		= 0;
				//SET_TX_DESC_TX_BANDWIDTH(pDesc, 1);
				//SET_TX_DESC_TX_SUB_CARRIER(pDesc, 0);//By SD3's Jerry suggestion, use duplicated mode
			}
			else
			{
				pDesc->TxBw		= 0;
				pDesc->TXSC		= priv->nCur40MhzPrimeSC;
				//SET_TX_DESC_TX_BANDWIDTH(pDesc, 0);
				//SET_TX_DESC_TX_SUB_CARRIER(pDesc, pHalData->nCur40MhzPrimeSC);
			}
		}
		else
		{
			pDesc->TxBw		= 0;
			pDesc->TXSC		= 0;
			//SET_TX_DESC_TX_BANDWIDTH(pDesc, 0);
			//SET_TX_DESC_TX_SUB_CARRIER(pDesc, 0);
		}

		//3(3)Fill necessary field in First Descriptor
		//DWORD 0*/
		pDesc->LINIP = 0;//bLastInitPacket;
		pDesc->Offset = 32;//USB_HWDESC_HEADER_LEN;
		pDesc->PktSize = (u16)skb->len;

		//DWORD 1*/
		pDesc->RaBRSRID = cb_desc->RATRIndex;
#if 0
printk("*************TXDESC:\n");
printk("\tTxRate: %d\n", pDesc->TxRate);
printk("\tAMPDUEn: %d\n", pDesc->AggEn);
printk("\tTxBw: %d\n", pDesc->TxBw);
printk("\tTXSC: %d\n", pDesc->TXSC);
printk("\tPktSize: %d\n", pDesc->PktSize);
printk("\tRatrIdx: %d\n", pDesc->RaBRSRID);
#endif
		//
		// Fill security related
		//
		 if (cb_desc->bHwSec) {
		 static u8 tmp =0;
		 if (!tmp) {
			 tmp = 1;
			}
#ifdef _RTL8192_EXT_PATCH_
			if(cb_desc->mesh_pkt == 0)
#endif
			{
				switch (priv->rtllib->pairwise_key_type) {
					case KEY_TYPE_WEP40:
					case KEY_TYPE_WEP104:
						//	printk("=====>wlan WEP encrypt\n");
						pDesc->SecType = 0x1;
						break;
					case KEY_TYPE_TKIP:
						//	printk("=====>wlan TKIP encrypt\n");
						pDesc->SecType = 0x2;
						break;
					case KEY_TYPE_CCMP:
						//	printk("=====>wlan CCMP encrypt\n");
						pDesc->SecType = 0x3;
						break;
					case KEY_TYPE_NA:
						//	printk("======>wlan NO encrypt\n");
						pDesc->SecType = 0x0;
						break;
				}
			}
#ifdef _RTL8192_EXT_PATCH_
			else if(cb_desc->mesh_pkt == 1)
			{
				switch (priv->rtllib->mesh_pairwise_key_type) {
					case KEY_TYPE_WEP40:
					case KEY_TYPE_WEP104:
						//	printk("=====>mesh WEP encrypt\n");
						pDesc->SecType = 0x1;
						break;
					case KEY_TYPE_TKIP:
						//	printk("=====>mesh TKIP encrypt\n");
						pDesc->SecType = 0x2;
						break;
					case KEY_TYPE_CCMP:
						//	printk("=====>mesh CCMP encrypt\n");
						pDesc->SecType = 0x3;
						break;
					case KEY_TYPE_NA:
						//	printk("======>mesh NO encrypt\n");
						pDesc->SecType = 0x0;
						break;
				}
			}
#endif
		}

		//
		// Set Packet ID
		//
		pDesc->PktID			= 0x0;
		// 2008/11/07 MH For SD4 requirement !!!!!
		// We will assign magement queue to BK.
		//if (IsMgntFrame(VirtualAddress))
			//QueueIndex = BK_QUEUE;
		pDesc->QueueSel		= MapHwQueueToFirmwareQueue(cb_desc->queue_index, cb_desc->priority);
		//SET_TX_DESC_QUEUE_SEL(pDesc,  MapHwQueueToFirmwareQueue(QueueIndex));

		pDesc->DataRateFBLmt	= 0x1F;		// Alwasy enable all rate fallback range
		pDesc->DISFB	= cb_desc->bTxDisableRateFallBack;
		pDesc->UserRate	= cb_desc->bTxUseDriverAssingedRate;
		//SET_TX_DESC_DISABLE_FB(pDesc, pTcb->bTxDisableRateFallBack);
		//SET_TX_DESC_USER_RATE(pDesc, pTcb->bTxUseDriverAssingedRate);

		//
		// 2008/09/25 MH We must change TX gain table for 1T or 2T. If we set force rate
		// We must change gain table by driver. Otherwise, firmware will handle.
		//
		if (pDesc->UserRate == true && pDesc->TXHT == true)
			RF_ChangeTxPath(dev, cb_desc->data_rate);

	}


	//1 Fill fields that are required to be initialized in all of the descriptors
	//DWORD 0
	pDesc->FirstSeg	= 1;//(bFirstSeg)? 1:0;
	pDesc->LastSeg	= 1;//(bLastSeg)?1:0;
	// For test only
	//SET_TX_DESC_FIRST_SEG(pDesc, (bFirstSeg? 1:0));
	//SET_TX_DESC_LAST_SEG(pDesc, (bLastSeg? 1:0));

	//DWORD 7
	pDesc->TxBufferSize= (u16)skb->len;
	//SET_TX_DESC_TX_BUFFER_SIZE(pDesc, (u16) BufferLen);

	//DOWRD 8
	pDesc->TxBuffAddr = cpu_to_le32(mapping);

}

void  rtl8192se_tx_fill_cmd_desc(struct net_device* dev, tx_desc_cmd * entry, cb_desc * cb_desc, struct sk_buff* skb)
{
    struct r8192_priv *priv = rtllib_priv(dev);
    dma_addr_t mapping = pci_map_single(priv->pdev, skb->data, skb->len, PCI_DMA_TODEVICE);
	// Clear all status
	memset((void*)entry, 0, 32);

	// For firmware downlaod we only need to set LINIP
	entry->LINIP = cb_desc->bLastIniPkt;
	// From Scott --> 92SE must set as 1 for firmware download HW DMA error
	entry->FirstSeg = 1;//bFirstSeg;
	entry->LastSeg = 1;//bLastSeg;
	//pDesc->Offset	= 0x20;

	// 92SE need not to set TX packet size when firmware download
	entry->TxBufferSize= (u16)(skb->len);
	entry->TxBufferAddr = cpu_to_le32(mapping);
	entry->PktSize = (u16)(skb->len);

	//if (bSetOwnBit)
	{
		entry->OWN = 1;
	}
//	RT_DEBUG_DATA(COMP_ERR, entry, sizeof(*entry));
}

u8 HwRateToMRate92S(bool bIsHT,	u8 rate)
{
	u8	ret_rate = 0x02;

	if (!bIsHT) {
		switch (rate) {
		case DESC92S_RATE1M:
			ret_rate = MGN_1M;
			break;
		case DESC92S_RATE2M:
			ret_rate = MGN_2M;
			break;
		case DESC92S_RATE5_5M:
			ret_rate = MGN_5_5M;
			break;
		case DESC92S_RATE11M:
			ret_rate = MGN_11M;
			break;
		case DESC92S_RATE6M:
			ret_rate = MGN_6M;
			break;
		case DESC92S_RATE9M:
			ret_rate = MGN_9M;
			break;
		case DESC92S_RATE12M:
			ret_rate = MGN_12M;
			break;
		case DESC92S_RATE18M:
			ret_rate = MGN_18M;
			break;
		case DESC92S_RATE24M:
			ret_rate = MGN_24M;
			break;
		case DESC92S_RATE36M:
			ret_rate = MGN_36M;
			break;
		case DESC92S_RATE48M:
			ret_rate = MGN_48M;
			break;
		case DESC92S_RATE54M:
			ret_rate = MGN_54M;
			break;
		default:
			ret_rate = 0xff;
			break;
		}
	} else {
		switch (rate) {
		case DESC92S_RATEMCS0:
			ret_rate = MGN_MCS0;
			break;
		case DESC92S_RATEMCS1:
			ret_rate = MGN_MCS1;
			break;
		case DESC92S_RATEMCS2:
			ret_rate = MGN_MCS2;
			break;
		case DESC92S_RATEMCS3:
			ret_rate = MGN_MCS3;
			break;
		case DESC92S_RATEMCS4:
			ret_rate = MGN_MCS4;
			break;
		case DESC92S_RATEMCS5:
			ret_rate = MGN_MCS5;
			break;
		case DESC92S_RATEMCS6:
			ret_rate = MGN_MCS6;
			break;
		case DESC92S_RATEMCS7:
			ret_rate = MGN_MCS7;
			break;
		case DESC92S_RATEMCS8:
			ret_rate = MGN_MCS8;
			break;
		case DESC92S_RATEMCS9:
			ret_rate = MGN_MCS9;
			break;
		case DESC92S_RATEMCS10:
			ret_rate = MGN_MCS10;
			break;
		case DESC92S_RATEMCS11:
			ret_rate = MGN_MCS11;
			break;
		case DESC92S_RATEMCS12:
			ret_rate = MGN_MCS12;
			break;
		case DESC92S_RATEMCS13:
			ret_rate = MGN_MCS13;
			break;
		case DESC92S_RATEMCS14:
			ret_rate = MGN_MCS14;
			break;
		case DESC92S_RATEMCS15:
			ret_rate = MGN_MCS15;
			break;
		case DESC92S_RATEMCS32:
			ret_rate = (0x80|0x20);
			break;
		default:
			ret_rate = 0xff;
			break;
		}

	}
	return ret_rate;
}

bool rtl8192se_rx_query_status_desc(struct net_device* dev, struct rtllib_rx_stats*  stats, rx_desc *pdesc, struct sk_buff* skb)
{
	struct r8192_priv* priv = rtllib_priv(dev);
	struct rtllib_device* ieee = priv->rtllib;
	u32	PHYStatus	= pdesc->PHYStatus;
	rx_fwinfo*		pDrvInfo;
	stats->Length		= (u16)pdesc->Length;
	stats->RxDrvInfoSize = (u8)pdesc->DrvInfoSize*8;
	stats->RxBufShift	= (u8)((pdesc->Shift)&0x03);
	stats->bICV			= (u16)pdesc->ICVError;
	stats->bCRC			= (u16)pdesc->CRC32;
	stats->bHwError			= (u16)(pdesc->CRC32|pdesc->ICVError);
	stats->Decrypted	= !pdesc->SWDec;
	// Debug !!!
	stats->rate = (u8)pdesc->RxMCS;
	stats->bShortPreamble= (u16)pdesc->SPLCP;
	stats->bIsAMPDU			= (bool)(pdesc->PAGGR==1);
	stats->bFirstMPDU	= (bool)((pdesc->PAGGR==1) && (pdesc->FAGGR==1));
	stats->TimeStampLow	= pdesc->TSFL;
	stats->RxIs40MHzPacket= (bool)pdesc->BandWidth;
	if(IS_UNDER_11N_AES_MODE(ieee))
	{
		// Always received ICV error packets in AES mode.
		// This fixed HW later MIC write bug.
	//	if (stats->bICV)
	//		printk("received icv error packet:%d, %d\n", stats->bICV, stats->bCRC);
		if(stats->bICV && !stats->bCRC)
		{
			stats->bICV = false;
			stats->bHwError = false;
		}
	}


	// 2008/09/30 MH For Marvel/Atheros/Broadcom AMPDU RX, we may receive wrong
	// beacon packet length, we do test here!!!!
	if(stats->Length > 0x2000 || stats->Length < 24)
	{
		RT_TRACE(COMP_ERR, "Err RX pkt len = 0x%x\n", stats->Length);
		stats->bHwError |= 1;
	}
	// Collect rx rate before it transform to MRate
	UpdateReceivedRateHistogramStatistics8190(dev, stats);

	// Transform HwRate to MRate
	if(!stats->bHwError)
		stats->rate = HwRateToMRate92S((bool)(pdesc->RxHT), (u8)(pdesc->RxMCS));
	else
	{
		stats->rate = MGN_1M;
		return false;
	}
	//
	// Be care the below code will influence TP. We should not enable the code if
	// it is useless.
	//

	//
	// 3. Debug only, collect rx rate/ AMPDU/ TSFL
	//
	//UpdateRxdRateHistogramStatistics8192S(Adapter, pRfd);
//	UpdateRxAMPDUHistogramStatistics8192S(Adapter, stats);
	UpdateRxPktTimeStamp8190(dev, stats);

	//
	// Get Total offset of MPDU Frame Body
	//
	if((stats->RxBufShift + stats->RxDrvInfoSize) > 0)
		stats->bShift = 1;

	//
	// 3-4. Get Driver Info
	// Driver info are written to the begining of the RxBuffer, it will be
	// attached after RX DESC(24 or 32 bytes). PHY status 32 bytes and other
	// variable info size.!!
	//
	//RTPRINT(FRX, RX_PHY_STS, ("pDesc->PHYStatus = %d\n", PHYStatus));
	if (PHYStatus == true)
	{
		// 92SE PHY status is appended before real data frame.!!
		pDrvInfo = (rx_fwinfo*)(skb->data + stats->RxBufShift);

		//
		// 4. Get signal quality for this packet, and record aggregation info for
		//    next packet
		//
		TranslateRxSignalStuff819xpci(dev, skb, stats, pdesc, pDrvInfo); //

	}
	return true;
}

void rtl8192se_halt_adapter(struct net_device *dev, bool bReset)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	//PRT_POWER_SAVE_CONTROL	pPSC = (PRT_POWER_SAVE_CONTROL)(&(priv->rtllib->PowerSaveControl));
        int i;
//	u8	OpMode;
	//u8	u1bTmp;
	//u16	u2bTmp;
//	u32	ulRegRead;

	RT_TRACE(COMP_INIT, "==> rtl8192se_halt_adapter()\n");

	priv->rtllib->state = RTLLIB_NOLINK;
	rtl8192_update_msr(dev);
	if(priv->rtllib->RfOffReason == RF_CHANGE_BY_IPS && priv->LedStrategy == SW_LED_MODE8)
	{
		SET_RTL8192SE_RF_SLEEP(dev);
		return;
	}
	if(priv->bDriverIsGoingToUnload)
	write_nic_byte(dev,0x560,0x0);
#if 1
	PHY_SetRtl8192seRfHalt(dev);
#endif
	udelay(100);

	udelay(20);
	if (!bReset) {
		mdelay(20);
	}

        for(i = 0; i < MAX_QUEUE_SIZE; i++) {
                skb_queue_purge(&priv->rtllib->skb_waitQ [i]);
        }
        for(i = 0; i < MAX_QUEUE_SIZE; i++) {
                skb_queue_purge(&priv->rtllib->skb_aggQ [i]);
        }
#ifdef _RTL8192_EXT_PATCH_
		for(i = 0; i < MAX_QUEUE_SIZE; i++) {
                skb_queue_purge(&priv->rtllib->skb_meshaggQ [i]);
        }
#endif

	skb_queue_purge(&priv->skb_queue);
	RT_TRACE(COMP_INIT, "<== HaltAdapter8192SE()\n");
	return;
}

u8 GetFreeRATRIndex8192SE (struct net_device* dev)
{
	struct r8192_priv* priv = rtllib_priv(dev);
	u8 bitmap = priv->RATRTableBitmap;
	u8 ratr_index = 0;
	for(	;ratr_index<7; ratr_index++)
	{
		if((bitmap & BIT0) == 0)
			{
				priv->RATRTableBitmap |= BIT0<<ratr_index;
				return ratr_index;
			}
		bitmap = bitmap >>1;
	}
	return ratr_index;
}

void rtl8192se_update_ratr_table(struct net_device* dev,u8* pMcsRate,struct sta_info* pEntry)
{
	struct r8192_priv* priv = rtllib_priv(dev);
	struct rtllib_device* ieee = priv->rtllib;
	//u8* pMcsRate = ieee->dot11HTOperationalRateSet;
	//struct rtllib_network *net = &ieee->current_network;
	u32 ratr_value = 0;
	u8 ratr_index = 0;
	u8 bNMode = 0;
	u16 shortGI_rate = 0;
	u32 tmp_ratr_value = 0;
	u8 MimoPs;
	WIRELESS_MODE WirelessMode;
	u8 bCurTxBW40MHz, bCurShortGI40MHz, bCurShortGI20MHz;

	if(ieee->iw_mode == IW_MODE_ADHOC){	// AP and AdHoc

		if(pEntry == NULL){
			printk("Doesn't have match Entry\n");
			return;
		}

		if(pEntry->ratr_index != 8)
			ratr_index = pEntry->ratr_index;
		else
		        ratr_index = GetFreeRATRIndex8192SE(dev);

		if(ratr_index == 7){
			RT_TRACE(COMP_RATE, "Ratrtable are full");
			return;
		}
		MimoPs = pEntry->htinfo.MimoPs;

		if((ieee->mode == WIRELESS_MODE_G) && (pEntry->wireless_mode == WIRELESS_MODE_N_24G))
			WirelessMode = ieee->mode;
		else
			WirelessMode = pEntry->wireless_mode;

		//WirelessMode = pEntry->wireless_mode;
		bCurTxBW40MHz = pEntry->htinfo.bCurTxBW40MHz;
		bCurShortGI40MHz = pEntry->htinfo.bCurShortGI40MHz;
		bCurShortGI20MHz = pEntry->htinfo.bCurShortGI20MHz;
		pEntry->ratr_index = ratr_index;
	}
	else  // STA mode
	{
		ratr_index = 0;
		WirelessMode = ieee->mode;
		MimoPs = ieee->pHTInfo->PeerMimoPs;
		bCurTxBW40MHz = ieee->pHTInfo->bCurTxBW40MHz;
		bCurShortGI40MHz = ieee->pHTInfo->bCurShortGI40MHz;
		bCurShortGI20MHz = ieee->pHTInfo->bCurShortGI20MHz;					//to be modified
	}

	rtl8192_config_rate(dev, (u16*)(&ratr_value));
	ratr_value |= (*(u16*)(pMcsRate)) << 12;
	switch (WirelessMode)
	{
		case IEEE_A:
			ratr_value &= 0x00000FF0;
			break;
		case IEEE_B:
			ratr_value &= 0x0000000D;
			break;
		case IEEE_G:
			ratr_value &= 0x00000FF5;
			break;
		case IEEE_N_24G:
		case IEEE_N_5G:
			bNMode = 1;
			if (MimoPs == 0) //MIMO_PS_STATIC
				ratr_value &= 0x0007F005;
			else{
				if (priv->rf_type == RF_1T2R ||priv->rf_type  == RF_1T1R || (ieee->pHTInfo->IOTAction & HT_IOT_ACT_DISABLE_TX_2SS)){
					if ((bCurTxBW40MHz) && !(ieee->pHTInfo->IOTAction & HT_IOT_ACT_DISABLE_TX_40_MHZ))
						ratr_value &= 0x000FF015;
					else
						ratr_value &= 0x000ff005;
				}else{
					if ((bCurTxBW40MHz) && !(ieee->pHTInfo->IOTAction & HT_IOT_ACT_DISABLE_TX_40_MHZ))
						ratr_value &= 0x0f0ff015;
					else
						ratr_value &= 0x0f0ff005;
					}
			}
			break;
		default:
			printk("====>%s(), mode is not correct:%x\n", __FUNCTION__,WirelessMode);
			break;
	}
	if (priv->card_8192_version>= VERSION_8192S_BCUT)
		ratr_value &= 0x0FFFFFFF;
	else if (priv->card_8192_version == VERSION_8192S_ACUT)
		ratr_value &= 0x0FFFFFF0;	// Normal ACUT must disable CCK rate

	// Get MAX MCS available.
	if (((ieee->pHTInfo->IOTAction & HT_IOT_ACT_DISABLE_SHORT_GI)==0) &&
		bNMode && ((bCurTxBW40MHz && bCurShortGI40MHz) ||
	    (!bCurTxBW40MHz && bCurShortGI20MHz)))
	{
		ratr_value |= 0x10000000;  //Short GI
		tmp_ratr_value = (ratr_value>>12);
		for(shortGI_rate=15; shortGI_rate>0; shortGI_rate--)
		{
			if((1<<shortGI_rate) & tmp_ratr_value)
				break;
		}
		shortGI_rate = (shortGI_rate<<12)|(shortGI_rate<<8)|\
			       (shortGI_rate<<4)|(shortGI_rate);
		write_nic_byte(dev, SG_RATE, shortGI_rate);
	}

	if(ieee->pHTInfo->IOTAction & HT_IOT_ACT_WA_IOT_Broadcom)
	{
		ratr_value &= 0xfffffff0; // disable all CCK rates.
		printk("UpdateHalRATRTable8192SE(), for HT_IOT_ACT_WA_IOT_Broadcom, ratr_value = 0x%x\n", ratr_value);
	}

	write_nic_dword(dev, ARFR0+ratr_index*4, ratr_value);
	printk("===========%s: %x\n", __FUNCTION__, read_nic_dword(dev, ARFR0));
	//2 UFWP
	if (ratr_value & 0xfffff000){
		printk("===>set to N mode\n");
		rtl8192se_set_fw_cmd(dev, FW_CMD_RA_REFRESH_N);
	}
	else{
		printk("===>set to B/G mode\n");
		rtl8192se_set_fw_cmd(dev, FW_CMD_RA_REFRESH_BG);
	}
}

int r8192se_resume_firm(struct net_device *dev)
{
	write_nic_byte(dev, 0x42, 0xFF);
	write_nic_word(dev, 0x40, 0x77FC);
	write_nic_word(dev, 0x40, 0x57FC);
	write_nic_word(dev, 0x40, 0x37FC);
	write_nic_word(dev, 0x40, 0x77FC);

	udelay(100);

	write_nic_word(dev, 0x40, 0x57FC);
	write_nic_word(dev, 0x40, 0x37FC);
	write_nic_byte(dev, 0x42, 0x00);

	return 0;
}

void PHY_SetRtl8192seRfHalt(struct net_device* dev)
{
	struct r8192_priv* priv = rtllib_priv(dev);
	PRT_POWER_SAVE_CONTROL	pPSC = (PRT_POWER_SAVE_CONTROL)(&(priv->rtllib->PowerSaveControl));

	u8 u1bTmp;
	if(priv->rtllib->RfOffReason == RF_CHANGE_BY_IPS && priv->LedStrategy == SW_LED_MODE8)
	{
		SET_RTL8192SE_RF_SLEEP(dev);
		return;
	}
	//  03/27/2009 MH Add description. Power save for BB/RF */
	printk("PHY_SetRtl8192seRfHalt save BB/RF\n");
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

	//  03/27/2009 MH Add description. After switch control path. register
	//  after page1 will be invisible. We can not do any IO for register>0x40.
        //  After resume&MACIO reset, we need to remember previous reg content.
	if (u1bTmp & BIT7) {
		u1bTmp &= ~(BIT6 | BIT7);
		if(!HalSetSysClk8192SE(dev, u1bTmp)) {
			printk("Switch ctrl path fail\n");
			return;
		}
	}

	//printk("Save MAC\n");
	//  03/27/2009 MH Add description. Power save for MAC */
	if(priv->rtllib->RfOffReason==RF_CHANGE_BY_IPS  && !priv->bDriverIsGoingToUnload)
	{
		//  03/27/2009 MH For enable LED function. */
		//printk("Save except led\n");
		write_nic_byte(dev, 0x03, 0xF9);
	} else		// SW/HW radio off or halt adapter!! For example S3/S4
	{
		// 2009/03/27 MH LED function disable. Power range is about 8mA now.
		printk("Save max pwr\n");
		write_nic_byte(dev, 0x03, 0x71);
	}
	write_nic_byte(dev, SYS_CLKR+1, 0x70);//0x09, 0x70);
	write_nic_byte(dev, AFE_PLL_CTRL+1, 0x68);//0x29, 0x68);
	write_nic_byte(dev,  AFE_PLL_CTRL, 0x00);	//0x28, 0x00);
	write_nic_byte(dev, LDOA15_CTRL, 0x34);//0x30);//0x20, 0x30)//0x50);
	write_nic_byte(dev, AFE_XTAL_CTRL, 0x0E);//0x26, 0x0E);
	RT_SET_PS_LEVEL(pPSC, RT_RF_OFF_LEVL_HALT_NIC);
	udelay(100);
	udelay(20);
}	// PHY_SetRtl8192seRfHalt


/*
*	Description:
*		Set RateAdaptive Mask
*	/param	Adapter		Pionter to Adapter entity
*	/param	bMulticast	TURE if broadcast or multicast, used for softAP basic rate
*	/param	macId		macID to set
*	/param	wirelessMode	wireless mode of associated AP/client
*	/return	void
*
*/
#ifdef _RTL8192_EXT_PATCH_
void UpdateHalRAMask8192SE(
	struct net_device* dev,
	bool bMulticast,
	u8   macId,
	u8*  MacAddr,
	u8   wirelessMode,
	u8   rssi_level)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	struct rtllib_device *ieee = priv->rtllib;
	PRT_HIGH_THROUGHPUT	pHTInfo =ieee->pHTInfo;
	u32	ratr_bitmap, RateSet;
	u8	shortGI_rate = 0;
	u32	tmp_ratr_value = 0;
	u32	mask=0, band = 0;
	u8	bShortGI = 0;

	rtl8192_config_rate(dev, (u16*)(&RateSet));
	//HalSetBrateCfg(Adapter, &pMgntInfo->dot11OperationalRateSet, (u16*)&RateSet);
	RateSet |= (*((u16*)(ieee->dot11HTOperationalRateSet))) << 12;
	ratr_bitmap = RateSet;

	// assign band mask and rate bitmap
	switch (wirelessMode)
	{
		case WIRELESS_MODE_B:
		{
			band |= WIRELESS_11B;
			ratr_bitmap &= 0x0000000d;
		}
		break;

		case WIRELESS_MODE_G:
		{
			band |= (WIRELESS_11G | WIRELESS_11B);
			if(rssi_level == 1)
				ratr_bitmap &= 0x00000f00;
			else if(rssi_level == 2)
				ratr_bitmap &= 0x00000ff0;
			else
				ratr_bitmap &= 0x00000ff5;
		}
		break;

		case WIRELESS_MODE_A:
		{
			band |= BIT2;
			ratr_bitmap &= 0x00000ff0;
		}
		break;

		case WIRELESS_MODE_N_24G:
		case WIRELESS_MODE_N_5G:
		{
			band |= (WIRELESS_11N| WIRELESS_11G| WIRELESS_11B);
			if(macId == 0)	// default port
			{
				if(ieee->pHTInfo->PeerMimoPs == MIMO_PS_STATIC)
				{
					if(rssi_level == 1)
						ratr_bitmap &= 0x00070000;
					else if(rssi_level == 2)
						ratr_bitmap &= 0x0007f000;
					else
						ratr_bitmap &= 0x0007f005;
				}
				else
				{
					if (	priv->rf_type == RF_1T2R ||
						priv->rf_type == RF_1T1R	)
					{
						if (pHTInfo->bCurTxBW40MHz)
						{
							if(rssi_level == 1)
								ratr_bitmap &= 0x000f0000;
							else if(rssi_level == 2)
								ratr_bitmap &= 0x000ff000;
							else
								ratr_bitmap &= 0x000ff015;
						}
						else
						{
							if(rssi_level == 1)
								ratr_bitmap &= 0x000f0000;
							else if(rssi_level == 2)
								ratr_bitmap &= 0x000ff000;
							else
								ratr_bitmap &= 0x000ff005;
						}
					}
					else
					{
						if (pHTInfo->bCurTxBW40MHz)
						{
							if(rssi_level == 1)
								ratr_bitmap &= 0x0f0f0000;
							else if(rssi_level == 2)
								ratr_bitmap &= 0x0f0ff000;
							else
								ratr_bitmap &= 0x0f0ff015;
						}
						else
						{
							if(rssi_level == 1)
								ratr_bitmap &= 0x0f0f0000;
							else if(rssi_level == 2)
								ratr_bitmap &= 0x0f0ff000;
							else
								ratr_bitmap &= 0x0f0ff005;
						}
					}
				}

				if( (pHTInfo->bCurTxBW40MHz && pHTInfo->bCurShortGI40MHz) ||
					(!pHTInfo->bCurTxBW40MHz && pHTInfo->bCurShortGI20MHz)	)
				{
					bShortGI = true;

					// Get MAX MCS available.
					tmp_ratr_value = (ratr_bitmap>>12);
					for(shortGI_rate=15; shortGI_rate>0; shortGI_rate--)
					{
						if((1<<shortGI_rate) & tmp_ratr_value)
							break;
					}
					shortGI_rate = (shortGI_rate<<12)|(shortGI_rate<<8)|(shortGI_rate<<4)|(shortGI_rate);
					write_nic_byte(dev, SG_RATE, shortGI_rate);
				}
			}
			else if(macId == 1)
			{
				// for soft basic rate
				if ( priv->rf_type == RF_1T2R || priv->rf_type == RF_1T1R )
				{
					if (pHTInfo->bCurBW40MHz)
					{
						ratr_bitmap &= 0x000ff015;
					}
					else
					{
						ratr_bitmap &= 0x000ff005;
					}
				}
				else
				{
					if (pHTInfo->bCurBW40MHz)
					{
						ratr_bitmap &= 0x0f0ff015;
					}
					else
					{
						ratr_bitmap &= 0x0f0ff005;
					}
				}
			}
			else
			{
				if(ieee->iw_mode == IW_MODE_MESH)
				{
					if(ieee->ext_patch_rtllib_update_ratr_mask)
						ieee->ext_patch_rtllib_update_ratr_mask(dev,MacAddr,rssi_level,&ratr_bitmap,&bShortGI);
				}
			}
		}
		break;

		default:
		{
			band |= (WIRELESS_11N| WIRELESS_11G| WIRELESS_11B);
			if(priv->rf_type == RF_1T2R)
			{
				ratr_bitmap &= 0x000ff0ff;
			}
			else
			{
				ratr_bitmap &= 0x0f0ff0ff;
			}
		}
	}

	if (priv->card_8192_version >= VERSION_8192S_BCUT)
		ratr_bitmap &= 0x0FFFFFFF;
	else if (priv->card_8192_version == VERSION_8192S_ACUT)
		ratr_bitmap &= 0x0FFFFFF0;	// Normal ACUT must disable CCK rate

	if(bShortGI)
		ratr_bitmap |= 0x10000000;

	//
	// For IOT related action, add here
	//
	if(macId == 0)
	{
		if(pHTInfo->IOTAction & HT_IOT_ACT_WA_IOT_Broadcom)
		{
			ratr_bitmap &= 0xfffffff0; // disable all CCK rates.
			RT_TRACE(COMP_RATE, "UpdateHalRATRTable8192SE(), for HT_IOT_ACT_WA_IOT_Broadcom, ratr_value = 0x%x\n", ratr_bitmap);
		}
		if(pHTInfo->IOTAction & HT_IOT_ACT_DISABLE_SHORT_GI)
		{
			ratr_bitmap &=0x0fffffff;
		}
	}
	mask |= (bMulticast ? 1 : 0)<<9 | (macId & 0x1f)<<4 | (band & 0xf);

	RT_TRACE(COMP_RATE, "UpdateHalRAMask92SE(): mask = %x, bitmap = %x\n", mask, ratr_bitmap);
	write_nic_dword(dev, 0x2c4, ratr_bitmap);
	write_nic_dword(dev, WFM5, (FW_RA_UPDATE_MASK | (mask << 8)));

}
#else
void
UpdateHalRAMask8192SE(
	struct net_device* dev,
	bool				bMulticast,
	u8				macId, //o
	u8*				MacAddr, //null
	u8*				pEntry, //for softAP//null
	u8				rssi_level)

{
	struct r8192_priv*	priv = rtllib_priv(dev);
	PRT_HIGH_THROUGHPUT	pHTInfo = priv->rtllib->pHTInfo;
	WIRELESS_MODE		WirelessMode = WIRELESS_MODE_G;
	u32			ratr_bitmap, RateSet;
	u8			shortGI_rate = 0, MimoPs = 0, ratr_index = 8;
	u32			tmp_ratr_value = 0;
	u32			mask=0, band = 0;
	bool			bShortGI = false, bCurTxBW40MHz = 0;
	struct	rtllib_device* ieee = priv->rtllib;
	u8*	pMcsRate = ieee->dot11HTOperationalRateSet;

	if(macId == 0)
	{
		MimoPs = pHTInfo->PeerMimoPs;
		WirelessMode = priv->rtllib->mode;
		bCurTxBW40MHz = pHTInfo->bCurTxBW40MHz;
	}
	else if (macId == 1)
	{
		WirelessMode = WIRELESS_MODE_B;
	}
	else
	{
#ifdef DO_DO_LIST
		if(pEntry == NULL)
		{
			printk("Doesn't have match Entry\n");
			return;
		}

		MimoPs = pEntry->HTInfo.MimoPs;

		if(IS_WIRELESS_MODE_G(Adapter) && (pEntry->WirelessMode == WIRELESS_MODE_N_24G))
			WirelessMode = pMgntInfo->dot11CurrentWirelessMode;
		else
			WirelessMode = pEntry->WirelessMode;

		bCurTxBW40MHz = pEntry->HTInfo.bBw40MHz;
#endif
	}
	rtl8192_config_rate(dev, (u16*)&RateSet);
	RateSet |= (*(u16*)(pMcsRate)) << 12;
	ratr_bitmap = RateSet;
	// assign band mask and rate bitmap
	switch (WirelessMode)
	{
		case WIRELESS_MODE_B:
		{
			band |= WIRELESS_11B;
			ratr_index = RATR_INX_WIRELESS_B;
			ratr_bitmap &= 0x0000000d;
		}
		break;

		case WIRELESS_MODE_G:
		case (WIRELESS_MODE_G |WIRELESS_MODE_B):
		{
			band |= (WIRELESS_11G | WIRELESS_11B);
			ratr_index = RATR_INX_WIRELESS_GB;

			if(rssi_level == 1)
				ratr_bitmap &= 0x00000f00;
			else if(rssi_level == 2)
				ratr_bitmap &= 0x00000ff0;
			else
				ratr_bitmap &= 0x00000ff5;
		}
		break;

		case WIRELESS_MODE_A:
		{
			band |= WIRELESS_11A;
			ratr_index = RATR_INX_WIRELESS_A;
			ratr_bitmap &= 0x00000ff0;
		}
		break;

		case WIRELESS_MODE_N_24G:
		case WIRELESS_MODE_N_5G:
		{
			band |= (WIRELESS_11N| WIRELESS_11G| WIRELESS_11B);
			ratr_index = RATR_INX_WIRELESS_NGB;

			if(MimoPs == MIMO_PS_STATIC)
				{
					if(rssi_level == 1)
						ratr_bitmap &= 0x00070000;
					else if(rssi_level == 2)
						ratr_bitmap &= 0x0007f000;
					else
						ratr_bitmap &= 0x0007f005;
				}
				else
				{
				if (	priv->rf_type == RF_1T2R || priv->rf_type == RF_1T1R	)
					{
					if (bCurTxBW40MHz)
						{
							if(rssi_level == 1)
								ratr_bitmap &= 0x000f0000;
							else if(rssi_level == 2)
								ratr_bitmap &= 0x000ff000;
							else
								ratr_bitmap &= 0x000ff015;
						}
						else
						{
							if(rssi_level == 1)
								ratr_bitmap &= 0x000f0000;
							else if(rssi_level == 2)
								ratr_bitmap &= 0x000ff000;
							else
								ratr_bitmap &= 0x000ff005;
						}
					}
					else
					{
					if (bCurTxBW40MHz)
						{
							if(rssi_level == 1)
								ratr_bitmap &= 0x0f0f0000;
							else if(rssi_level == 2)
								ratr_bitmap &= 0x0f0ff000;
							else
								ratr_bitmap &= 0x0f0ff015;
						}
						else
						{
							if(rssi_level == 1)
								ratr_bitmap &= 0x0f0f0000;
							else if(rssi_level == 2)
								ratr_bitmap &= 0x0f0ff000;
							else
								ratr_bitmap &= 0x0f0ff005;
						}
					}
				}
				if( (pHTInfo->bCurTxBW40MHz && pHTInfo->bCurShortGI40MHz) ||
					(!pHTInfo->bCurTxBW40MHz && pHTInfo->bCurShortGI20MHz)	)
				{
				if(macId == 0)
					bShortGI = true;
				else
					bShortGI = false;
			}
		}
		break;

		default:
			band |= (WIRELESS_11N| WIRELESS_11G| WIRELESS_11B);
			ratr_index = RATR_INX_WIRELESS_NGB;

			if(priv->rf_type == RF_1T2R)
				ratr_bitmap &= 0x000ff0ff;
			else
				ratr_bitmap &= 0x0f0ff0ff;
			break;
	}

	if (priv->card_8192_version >= VERSION_8192S_BCUT)
		ratr_bitmap &= 0x0FFFFFFF;
	else if (priv->card_8192_version == VERSION_8192S_ACUT)
		ratr_bitmap &= 0x0FFFFFF0;	// Normal ACUT must disable CCK rate

	if(bShortGI)
	{
		ratr_bitmap |= 0x10000000;
		// Get MAX MCS available.
		tmp_ratr_value = (ratr_bitmap>>12);
		for(shortGI_rate=15; shortGI_rate>0; shortGI_rate--)
		{
			if((1<<shortGI_rate) & tmp_ratr_value)
				break;
		}
		shortGI_rate = (shortGI_rate<<12)|(shortGI_rate<<8)|(shortGI_rate<<4)|(shortGI_rate);
		write_nic_byte(dev, SG_RATE, shortGI_rate);
	}

	//
	// For IOT related action, add here
	//
	if(macId == 0)	{
		if(priv->rtllib->pHTInfo->IOTAction & HT_IOT_ACT_WA_IOT_Broadcom)
			ratr_bitmap &= 0xfffffff0; // disable all CCK rates.
		if(priv->rtllib->pHTInfo->IOTAction & HT_IOT_ACT_DISABLE_SHORT_GI)
			ratr_bitmap &=0x0fffffff;
	}
	//else if (pEntry != NULL)
	//	pEntry->ratr_index = ratr_index;


	mask |= (bMulticast ? 1 : 0)<<9 | (macId & 0x1f)<<4 | (band & 0xf);

	RT_TRACE(COMP_RATE, "UpdateHalRAMask92SE(): mask = %x, bitmap = %x ratr_index = %x\n", mask, ratr_bitmap, ratr_index);
	write_nic_dword(dev, 0x2c4, ratr_bitmap);
	write_nic_dword(dev, WFM5, (FW_RA_UPDATE_MASK | (mask << 8)));

}
#endif

//
// Description:
//	Set the MAC offset [0x09] and prevent all I/O for a while (about 20us~200us, suggested from SD4 Scott).
//	If the protection is not performed well or the value is not set complete, the next I/O will cause the system hang.
// Note:
//	This procudure is designed specifically for 8192S and references the platform based variables
//	which violates the stucture of multi-platform.
//	Thus, we shall not extend this procedure to common handler.
// By Bruce, 2009-01-08.
//
u8 HalSetSysClk8192SE( struct net_device *dev, u8 Data)
{
//#if PLATFORM != PLATFORM_WINDOWS
#if 0
	write_nic_byte(dev, (SYS_CLKR + 1), Data);
	udelay(200);;
	return 1;
#else
	{
		//PMP_ADAPTER			pDevice = &(Adapter->NdisAdapter);
		//struct r8192_priv* priv = rtllib_priv(dev);
		u8				WaitCount = 100;
		bool bResult = false;

#ifdef TO_DO_LIST
		RT_DISABLE_FUNC(Adapter, DF_IO_BIT);

		do
		{
			// Wait until all I/Os are complete.
			if(pDevice->IOCount == 0)
				break;
			delay_us(10);
		}while(WaitCount -- > 0);

		if(WaitCount == 0)
		{ // Wait too long, give up this procedure.
			RT_ENABLE_FUNC(Adapter, DF_IO_BIT);
			RT_TRACE(COMP_POWER, DBG_WARNING, ("HalSetSysClk8192SE(): Wait too long! Skip ....\n"));
			return false;
		}
		#endif
		write_nic_byte(dev,SYS_CLKR + 1,Data);

		// Wait the MAC synchronized.
		udelay(400);


		// Check if it is set ready.
		{
			u8 TmpValue;
			TmpValue=read_nic_byte(dev,SYS_CLKR + 1);
			bResult = ((TmpValue&BIT7)== (Data & BIT7));
			//DbgPrint("write 38 and return value %x\n",TmpValue);
			if((Data &(BIT6|BIT7)) == false)
			{
				WaitCount = 100;
				TmpValue = 0;
				while(1)
				{
					WaitCount--;
					TmpValue=read_nic_byte(dev, SYS_CLKR + 1);
					if((TmpValue &BIT6))
						break;
					printk("wait for BIT6 return value %x\n",TmpValue);
					if(WaitCount==0)
						break;
					udelay(10);
				}
				if(WaitCount == 0)
					bResult = false;
				else
					bResult = true;
			}
		}
#ifdef TO_DO_LIST
		// should be wait for BIT6 to 1 Neo Test // BIT7 0 BIT6 1
		RT_ENABLE_FUNC(Adapter, DF_IO_BIT);
#endif
		RT_TRACE(COMP_PS,"HalSetSysClk8192SE():Value = %02X, return: %d\n", Data, bResult);
		return bResult;
	}
#endif
}

#ifndef _RTL8192_EXT_PATCH_
static u8 LegacyRateSet[12] = {0x02 , 0x04 , 0x0b , 0x16 , 0x0c , 0x12 , 0x18 , 0x24 , 0x30 , 0x48 , 0x60 , 0x6c};
void GetHwReg8192SE(struct net_device *dev,u8 variable,u8* val)
{
	//struct r8192_priv* priv = rtllib_priv(dev);
	switch(variable)
	{
		case HW_VAR_INIT_TX_RATE: //2008.12.10 added by tynli.
			{
				u8 RateIdx = read_nic_byte(dev, TX_RATE_REG);
				if(RateIdx < 76)
					*((u8*)(val)) = (RateIdx<12)?(LegacyRateSet[RateIdx]):((RateIdx-12)|0x80);
				else
					*((u8*)(val)) = 0;
			}
			break;

		default:
			break;
	}
}
#endif
//added by amy 090330
void SetHwReg8192SE(struct net_device *dev,u8 variable,u8* val)
{
	struct r8192_priv* priv = rtllib_priv(dev);
	switch(variable)
	{
		case HW_VAR_AMPDU_MIN_SPACE:
		{
			u8	MinSpacingToSet;
			u8	SecMinSpace;

			MinSpacingToSet = *((u8*)val);
			if(MinSpacingToSet <= 7)
			{
				if((priv->rtllib->current_network.capability & WLAN_CAPABILITY_PRIVACY) == 0)  //NO_Encryption
					SecMinSpace = 0;
				else if (priv->rtllib->rtllib_ap_sec_type &&
						(priv->rtllib->rtllib_ap_sec_type(priv->rtllib)
							 & (SEC_ALG_WEP|SEC_ALG_TKIP)))
					SecMinSpace = 7;
				else
					SecMinSpace = 1;

				if(MinSpacingToSet < SecMinSpace)
					MinSpacingToSet = SecMinSpace;
				priv->rtllib->MinSpaceCfg = ((priv->rtllib->MinSpaceCfg&0xf8) |MinSpacingToSet);
				RT_TRACE(COMP_MLME, "Set HW_VAR_AMPDU_MIN_SPACE: %#x\n", priv->rtllib->MinSpaceCfg);
				write_nic_byte(dev, AMPDU_MIN_SPACE, priv->rtllib->MinSpaceCfg);
			}
		}
		break;
		case HW_VAR_SHORTGI_DENSITY:
		{
			u8	DensityToSet;

			DensityToSet = *((u8*)val);
			//DensityToSet &= 0x1f;
			priv->rtllib->MinSpaceCfg|= (DensityToSet<<3);
			RT_TRACE(COMP_MLME, "Set HW_VAR_SHORTGI_DENSITY: %#x\n", priv->rtllib->MinSpaceCfg);
			write_nic_byte(dev, AMPDU_MIN_SPACE, priv->rtllib->MinSpaceCfg);
			break;
		}
		case HW_VAR_AMPDU_FACTOR:
		{
			u8	FactorToSet;
			u8	RegToSet;
			u8	FactorLevel[18] = {2, 4, 4, 7, 7, 13, 13, 13, 2, 7, 7, 13, 13, 15, 15, 15, 15, 0};
			u8	index = 0;

			FactorToSet = *((u8*)val);
			if(FactorToSet <= 3)
			{
				FactorToSet = (1<<(FactorToSet + 2));
				if(FactorToSet>0xf)
					FactorToSet = 0xf;

				for(index=0; index<17; index++)
				{
					if(FactorLevel[index] > FactorToSet)
						FactorLevel[index] = FactorToSet;
				}

				for(index=0; index<8; index++)
				{
					RegToSet = ((FactorLevel[index*2]) | (FactorLevel[index*2+1]<<4));
					write_nic_byte(dev, AGGLEN_LMT_L+index, RegToSet);
				}
				RegToSet = ((FactorLevel[16]) | (FactorLevel[17]<<4));
				write_nic_byte(dev, AGGLEN_LMT_H, RegToSet);

				RT_TRACE(COMP_MLME, "Set HW_VAR_AMPDU_FACTOR: %#x\n", FactorToSet);
			}
		}
		break;
		case HW_VAR_BASIC_RATE:
		{
			//OCTET_STRING		*mBratesOS;
			u16				BrateCfg = 0;
			//u32				regTmp = 0;
			u8				RateIndex = 0;

			//mBratesOS = (OCTET_STRING *)(val);

			// 2007.01.16, by Emily
			// Select RRSR (in Legacy-OFDM and CCK)
			// For 8190, we select only 24M, 12M, 6M, 11M, 5.5M, 2M, and 1M from the Basic rate.
			// We do not use other rates.
			//HalSetBrateCfg( Adapter, mBratesOS, &BrateCfg );
			rtl8192_config_rate(dev, &BrateCfg);

			if (priv->card_8192_version == VERSION_8192S_ACUT)
				priv->basic_rate = BrateCfg = BrateCfg & 0x150;
			else if (priv->card_8192_version == VERSION_8192S_BCUT)
				priv->basic_rate = BrateCfg = BrateCfg & 0x15f;

	                if(priv->rtllib->pHTInfo->IOTPeer == HT_IOT_PEER_CISCO && ((BrateCfg &0x150)==0))
			{
                                // if peer is cisco and didn't use ofdm rate,
                                // we use 6M for ack.
				BrateCfg |=0x010;
			}
			if(priv->rtllib->pHTInfo->IOTAction & HT_IOT_ACT_WA_IOT_Broadcom)
			{
				BrateCfg &= 0x1f0;
				printk("HW_VAR_BASIC_RATE, HT_IOT_ACT_WA_IOT_Broadcom, BrateCfg = 0x%x\n", BrateCfg);
			}
			// Set RRSR rate table.
			write_nic_byte(dev, RRSR, BrateCfg&0xff);
			write_nic_byte(dev, RRSR+1, (BrateCfg>>8)&0xff);

			// Set RTS initial rate
			while(BrateCfg > 0x1)
			{
				BrateCfg = (BrateCfg >> 1);
				RateIndex++;
			}
			write_nic_byte(dev, INIRTSMCS_SEL, RateIndex);
		}
		break;
		case HW_VAR_RETRY_LIMIT:
		{
			u8 RetryLimit = ((u8*)(val))[0];

			priv->ShortRetryLimit = RetryLimit;
			priv->LongRetryLimit = RetryLimit;

			write_nic_word(dev, RETRY_LIMIT,
							RetryLimit << RETRY_LIMIT_SHORT_SHIFT | \
							RetryLimit << RETRY_LIMIT_LONG_SHIFT);
		}
		break;
		case HW_VAR_BEACON_INTERVAL:
		{
			// Beacon interval (in unit of TU).
			write_nic_word(dev, BCN_INTERVAL, *((u16*)val));
			//2008.10.24 added by tynli for beacon changed.
			PHY_SetBeaconHwReg(dev, *((u16*)val));
		}
		break;

		case HW_VAR_BSSID:
			write_nic_dword(dev, BSSIDR, ((u32*)(val))[0]);
			write_nic_word(dev, BSSIDR+4, ((u16*)(val+4))[0]);
		break;

		case HW_VAR_MEDIA_STATUS:
		{
			//In 92C, not the register MSR.Use TCR--0x100--17:16.
			RT_OP_MODE	OpMode = *((RT_OP_MODE *)(val));
			LED_CTL_MODE	LedAction = LED_CTL_NO_LINK;
			u8		btMsr = read_nic_byte(dev, MSR);

			btMsr &= 0xfc;
			//vivi, for win7, 0423.vivi special for 92ce
			//if(IS_ADAPTER_SENDS_BEACON(Adapter) )
			//{// One of the adapter needs to send beacon.
			//	// And current operation is to stop beacon.
			//	if(OpMode == RT_OP_MODE_INFRASTRUCTURE ||
			//		OpMode == RT_OP_MODE_NO_LINK)
			//		break;
			//	/*
			//	if(OpMode != RT_OP_MODE_AP ||
			//		OpMode != RT_OP_MODE_IBSS) // And one adapter is going to stop sending beacon.
			//		break;
			//	*/
			//}
			switch(OpMode)
			{
			case RT_OP_MODE_INFRASTRUCTURE:
				btMsr |= MSR_INFRA;
				LedAction = LED_CTL_LINK;
				break;

			case RT_OP_MODE_IBSS:
				btMsr |= MSR_ADHOC;
				// led link set seperate
				break;

			case RT_OP_MODE_AP:
				btMsr |= MSR_AP;
				LedAction = LED_CTL_LINK;
				break;

			default:
				btMsr |= MSR_NOLINK;
				break;
			}

			write_nic_byte(dev, MSR, btMsr);

			// For 92SE test we must reset TST after setting MSR
			//mask off for 92C
			{
				u32	temp = read_nic_dword(dev, TCR);
				write_nic_dword(dev, TCR, temp&(~BIT8));
				write_nic_dword(dev, TCR, temp|BIT8);
			}
			// LED control
			priv->rtllib->LedControlHandler(dev, LedAction);
		}
		break;

		case HW_VAR_CECHK_BSSID:
		{
			u32	RegRCR, Type;

			Type = ((u8*)(val))[0];
			//priv->rtllib->GetHwRegHandler(dev, HW_VAR_RCR, (u8*)(&RegRCR));
			RegRCR = read_nic_dword(dev,RCR);
			priv->ReceiveConfig = RegRCR;

#if (RTL92SE_FPGA_VERIFY == 1)
			// For 92SE test.
			RegRCR &= (~RCR_CBSSID);
#else
#if 1
			if (Type == true)
				RegRCR |= (RCR_CBSSID);
			else if (Type == false)
				RegRCR &= (~RCR_CBSSID);
#endif
			// 2008/09/04 MH ASIC test, if set CBSSID, adhoc will rx no data.
			//RegRCR &= (~RCR_CBSSID);
			//priv->rtllib->SetHwRegHandler( dev, HW_VAR_RCR, (u8*)(&RegRCR) );
			write_nic_dword(dev, RCR,RegRCR);
			priv->ReceiveConfig = RegRCR;
#endif
		}
		break;

		default:
			break;
	}
}

//
//Description:
//	Set Beacon related registersetting
//param		Adapter		Pionter to Adapter entity
//param	OpMode		The configuration value is different in AP mode and Ad-hoc mode
//param	BcnInterval	Beacon interval in units of TU
//param		AtimWindow	ATIM window width in units of TU.
//return	void
//
//
void SetBeaconRelatedRegisters8192SE(struct net_device *dev)
{
	struct r8192_priv *priv = (struct r8192_priv *)rtllib_priv(dev);
	struct rtllib_network *net = &priv->rtllib->current_network;
	u16			BcnTimeCfg = 0;
	u16			BcnCW = 6, BcnIFS = 0xf;
	u16			AtimWindow = 2;	// ATIM Window (in unit of TU).
	int			OpMode = priv->rtllib->iw_mode;
	u16			BcnInterval = net->beacon_interval;
	//
	// ATIM window
	//
	write_nic_word(dev, ATIMWND, AtimWindow);

	//
	// Beacon interval (in unit of TU).
	//
	write_nic_word(dev, BCN_INTERVAL, BcnInterval);
	//2008.10.24 added by tynli for beacon changed.
	PHY_SetBeaconHwReg( dev, BcnInterval);

	//
	// DrvErlyInt (in unit of TU). (Time to send interrupt to notify driver to change beacon content)
	//
	write_nic_word(dev, BCN_DRV_EARLY_INT, 10<<4);

	//
	// BcnDMATIM(in unit of us). Indicates the time before TBTT to perform beacon queue DMA
	//
	write_nic_word(dev, BCN_DMATIME, 256); // HWSD suggest this value 2006.11.14

	//
	// Force beacon frame transmission even after receiving beacon frame from other ad hoc STA
	//
	write_nic_byte(dev, BCN_ERR_THRESH, 100); // Reference from WMAC code 2006.11.14


	//
	// Beacon Time Configuration
	//
	switch(OpMode)
	{
		case IW_MODE_ADHOC:
			BcnTimeCfg |= (BcnCW<<BCN_TCFG_CW_SHIFT);
			break;
		default:
			printk("Invalid Operation Mode!!\n");
			break;
	}

	// TODO: BcnIFS may required to be changed on ASIC
	BcnTimeCfg |= BcnIFS<<BCN_TCFG_IFS;

	// For 92Se we still not know the corresponding register address.
	//PlatformEFIOWrite2Byte(Adapter, BCN_TCFG, BcnTimeCfg);

	{
		//FIXME
		//u8 u1Temp = (u8)(EF2Byte(BcnInterval));
		u8 u1Temp = (u8)(BcnInterval);
		write_nic_dword(dev, WFM5, 0xF1000000 |((u16)( u1Temp) << 8));
		//PlatformEFIOWrite4Byte(Adapter, WFM5, 0xF1000000 |(u2Byte)( (BcnInterval&0xff) << 8));
		ChkFwCmdIoDone(dev);
	}
}

void UpdateHalRATRTableIndex(struct net_device *dev)
{
	struct r8192_priv* priv = rtllib_priv(dev);
	struct rtllib_device* ieee = priv->rtllib;
	u8		bitmap = 0;
	int		i;

	for (i = 0; i < PEER_MAX_ASSOC; i++) {
		if (NULL != ieee->peer_assoc_list[i]) {
			bitmap |= BIT0 << ieee->peer_assoc_list[i]->ratr_index;
		}
	}

	priv->RATRTableBitmap = bitmap;
	return;
}

//added by amy for adhoc 090402
bool rtl8192se_check_ht_cap(struct net_device* dev, struct sta_info *sta,
		struct rtllib_network* net)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	struct rtllib_device *ieee = priv->rtllib;
	PHT_CAPABILITY_ELE  pHTCapIE = NULL;
	PHT_INFORMATION_ELE  pPeerHTInfo = NULL;
	u8 ExtChlOffset=0;
	u8	*pMcsFilter = NULL;
	u16	nMaxAMSDUSize = 0;
	// For 11n EWC definition, 2007.07.17, by Emily
	static u8	EWC11NHTCap[] = {0x00, 0x90, 0x4c, 0x33};
	// For 11n EWC definition, 2007.07.17, by Emily
	static u8	EWC11NHTInfo[] = {0x00, 0x90, 0x4c, 0x34};

	if ((ieee->mode != WIRELESS_MODE_N_24G) &&
			(ieee->mode != WIRELESS_MODE_N_5G)) {
		printk("%s():i am G mode ,do not need to check Cap IE\n",
				__FUNCTION__);
		if (net->mode == IEEE_N_5G)
			sta->wireless_mode = IEEE_A;
		else if (net->mode == IEEE_N_24G) {
			if (net->rates_ex_len > 0)
				sta->wireless_mode = IEEE_G;
			else
				sta->wireless_mode = IEEE_B;
		} else
			sta->wireless_mode = net->mode;
		return false;
	}
	//2 Reject PEER MP to connect in N mode
	// If we do not support CCK rate in N mode, reject the B mode
	if ((ieee->mode ==WIRELESS_MODE_N_24G)
		&& ieee->pHTInfo->bRegSuppCCK== false) {
		if(net->mode == IEEE_B){
			sta->wireless_mode = net->mode;
			printk("%s(): peer is B MODE return\n", __FUNCTION__);
			return false;
		}
	}
	//2 Set PEER MP Wireless Mode
	if(net->bssht.bdHTCapLen  != 0)
	{
		sta->htinfo.bEnableHT = true;
		sta->htinfo.bCurRxReorderEnable = ieee->pHTInfo->bRegRxReorderEnable;
		if(net->mode == IEEE_A)
			sta->wireless_mode = IEEE_N_5G;
		else
			sta->wireless_mode = IEEE_N_24G;
	} else {
		printk("%s(): have no HTCap IE, mode is %d\n",__FUNCTION__,net->mode);
		sta->wireless_mode = net->mode;
		sta->htinfo.bEnableHT = false;
		return true;
	}

	if (!memcmp(net->bssht.bdHTCapBuf ,EWC11NHTCap, sizeof(EWC11NHTCap)))
		pHTCapIE = (PHT_CAPABILITY_ELE)(&(net->bssht.bdHTCapBuf[4]));
	else
		pHTCapIE = (PHT_CAPABILITY_ELE)(net->bssht.bdHTCapBuf);

	if (!memcmp(net->bssht.bdHTInfoBuf, EWC11NHTInfo, sizeof(EWC11NHTInfo)))
		pPeerHTInfo = (PHT_INFORMATION_ELE)(&net->bssht.bdHTInfoBuf[4]);
	else
		pPeerHTInfo = (PHT_INFORMATION_ELE)(net->bssht.bdHTInfoBuf);

	ExtChlOffset=((ieee->pHTInfo->bRegBW40MHz == false)?HT_EXTCHNL_OFFSET_NO_EXT:
					(ieee->current_network.channel<=6)?
					HT_EXTCHNL_OFFSET_UPPER:HT_EXTCHNL_OFFSET_LOWER);
	printk("******** STA wireless mode %d\n", sta->wireless_mode);

	//1 Get HT properties of the STA
	// Get CCK rate support property
	if (ieee->pHTInfo->bRegSuppCCK)
		sta->htinfo.bSupportCck = (pHTCapIE->DssCCk==1)?true:false;
	else {
		if(pHTCapIE->DssCCk==1)
			return false;
	}

	// Get Mimo power save settings
	sta->htinfo.MimoPs= pHTCapIE->MimoPwrSave;

	printk("******** PEER MP MimoPs %d\n", sta->htinfo.MimoPs);
	// Get operation bandwidth
	if(ieee->pHTInfo->bRegBW40MHz)
		sta->htinfo.bBw40MHz= (pHTCapIE->ChlWidth==1)?true:false;
	else
		sta->htinfo.bBw40MHz = false;

	if((pPeerHTInfo->ExtChlOffset) != ExtChlOffset)
		sta->htinfo.bBw40MHz = false;

	ieee->Peer_bCurBW40M = sta->htinfo.bBw40MHz;
	printk("******** PEER MP bCurBW40M %d\n", sta->htinfo.bBw40MHz);
	if(ieee->pHTInfo->bRegBW40MHz == true)
		sta->htinfo.bCurTxBW40MHz = sta->htinfo.bBw40MHz;

	printk("******** PEER MP bCurTxBW40MHz %d\n", sta->htinfo.bCurTxBW40MHz);
	sta->htinfo.bCurShortGI20MHz=
		((ieee->pHTInfo->bRegShortGI20MHz)?((pHTCapIE->ShortGI20Mhz==1)?true:false):false);
	sta->htinfo.bCurShortGI40MHz=
		((ieee->pHTInfo->bRegShortGI40MHz)?((pHTCapIE->ShortGI40Mhz==1)?true:false):false);

	printk("******** PEER MP bCurShortGI20MHz %d, bCurShortGI40MHz %d\n",sta->htinfo.bCurShortGI20MHz,sta->htinfo.bCurShortGI40MHz);
	// Get A-MSDU property
	nMaxAMSDUSize = (pHTCapIE->MaxAMSDUSize==0)?3839:7935;
	if(ieee->pHTInfo->nAMSDU_MaxSize >= nMaxAMSDUSize)
		sta->htinfo.AMSDU_MaxSize = nMaxAMSDUSize;
	else
		sta->htinfo.AMSDU_MaxSize = ieee->pHTInfo->nAMSDU_MaxSize;

	printk("****************AMSDU_MaxSize=%d\n",sta->htinfo.AMSDU_MaxSize);
	//sta->htinfo.bSupportGreenField = pHtCap->GreenField;

	// Get A-MPDU Factor
	// Decide AMPDU Factor according to protocol handshake
	if(ieee->pHTInfo->AMPDU_Factor >= pHTCapIE->MaxRxAMPDUFactor)
		sta->htinfo.AMPDU_Factor = pHTCapIE->MaxRxAMPDUFactor;
	else
		sta->htinfo.AMPDU_Factor = ieee->pHTInfo->AMPDU_Factor;

	// Get AMPDU Density
	if(ieee->pHTInfo->MPDU_Density >= pHTCapIE->MPDUDensity)
		sta->htinfo.MPDU_Density = pHTCapIE->MPDUDensity;
	else
		sta->htinfo.MPDU_Density = ieee->pHTInfo->MPDU_Density;

	// Filter MCS rate set that we support.
	HTFilterMCSRate(ieee, pHTCapIE->MCS, sta->htinfo.McsRateSet);
	//{
	//	int k=0;
	//	printk("***********Peer MCS rate\n");
	//	for(k=0;k<16;k++)
	//		printk("%x ",ext_entry->htinfo.McsRateSet[k]);
	//}
	// This is for static mimo power save
	if(sta->htinfo.MimoPs == 0)  //MIMO_PS_STATIC
		pMcsFilter = MCS_FILTER_1SS;
	else
		pMcsFilter = MCS_FILTER_ALL;

	sta->htinfo.HTHighestOperaRate = HTGetHighestMCSRate(ieee, sta->htinfo.McsRateSet, pMcsFilter);
	printk("******** PEER MP HTHighestOperaRate %x\n",sta->htinfo.HTHighestOperaRate);
	//printf("AP_CheckHTCap(): STA has HT cap, AMSDUSize = %d, AMPDUFactor = %d, MPDU Density = %d\n",
	//				sta->htinfo.AMSDU_MaxSize,
	//				sta->htinfo.AMPDU_Factor,
	//				sta->htinfo.MPDU_Density);

	return true;

}

//added by thomas for adhoc 090804
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,20)
void rtl8192se_check_tsf_wq(struct work_struct * work)
{
	struct delayed_work *dwork = container_of(work,struct delayed_work,work);
	struct rtllib_device *ieee = container_of(dwork, struct rtllib_device, check_tsf_wq);
	struct net_device *dev = ieee->dev;
#else
void rtl8192se_check_tsf_wq(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	struct rtllib_device *ieee = priv->rtllib;
#endif
	u32	CurrTsfHigh,CurrTsfLow;
	u32	TargetTsfHigh,TargetTsfLow;

	CurrTsfHigh = read_nic_dword(dev, TSFR+4);
	CurrTsfLow = (u32)(ieee->CurrTsf & 0xffff);
	TargetTsfHigh = (u32)(ieee->TargetTsf >> 32);
	TargetTsfLow = (u32)(ieee->TargetTsf & 0xffff);

	printk("Current TSF Low = %x, Hight = %x\n",CurrTsfLow,CurrTsfHigh);
	printk("Target TSF Low = %x, Hight = %x\n",TargetTsfLow,TargetTsfHigh);

	ieee->CurrTsf |= (u64)CurrTsfHigh << 32;

	if(ieee->CurrTsf < ieee->TargetTsf)
	{
		down(&ieee->wx_sem);

		rtllib_stop_protocol(ieee,true);

		ieee->ssid_set = 1;

		rtllib_start_protocol(ieee);

		up(&ieee->wx_sem);
	}
}

#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,20)
void rtl8192se_update_peer_ratr_table_wq(struct work_struct * work)
{
	struct delayed_work *dwork = container_of(work,struct delayed_work,work);
	struct rtllib_device *ieee = container_of(dwork, struct rtllib_device, update_assoc_sta_info_wq);
        struct net_device *dev = ieee->dev;
        struct r8192_priv *priv = rtllib_priv(dev);
#else
void rtl8192se_update_peer_ratr_table_wq(struct net_device *dev)
{
        struct r8192_priv *priv = rtllib_priv(dev);
	struct rtllib_device *ieee = priv->rtllib;
#endif
	int idx = 0;
	for(idx=0; idx<PEER_MAX_ASSOC; idx++)
	{
		if(NULL != priv->rtllib->peer_assoc_list[idx])
		{
			u8 * addr = priv->rtllib->peer_assoc_list[idx]->macaddr;
			printk("%s: STA:%x:%x:%x:%x:%x:%x\n",__FUNCTION__,addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
			rtl8192se_update_ratr_table(dev,priv->rtllib->peer_assoc_list[idx]->htinfo.McsRateSet,priv->rtllib->peer_assoc_list[idx]);
		}
	}
	UpdateHalRATRTableIndex(dev);

	//update bandwith based on peer HT cap
	if(ieee->Peer_bCurBW40M)
		HTSetConnectBwMode(ieee, HT_CHANNEL_WIDTH_20_40, (ieee->current_network.channel<=6)?HT_EXTCHNL_OFFSET_UPPER:HT_EXTCHNL_OFFSET_LOWER);  //20/40M
	else
		HTSetConnectBwMode(ieee, HT_CHANNEL_WIDTH_20, (ieee->current_network.channel<=6)?HT_EXTCHNL_OFFSET_UPPER:HT_EXTCHNL_OFFSET_LOWER);  //20M
}
