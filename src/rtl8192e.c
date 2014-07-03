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
#include <linux/interrupt.h>
#include "rtl_core.h"
#include "r819xE_phy.h"
#include "r819xE_phyreg.h"
#include "r8190_rtl8256.h" // RTL8225 Radio frontend */
#include "r819xE_cmdpkt.h"
#include "rtl_dm.h"
#include "rtl_wx.h"

extern int WDCAPARA_ADD[];

void
rtl8192e_SetHwReg(struct net_device *dev,u8 variable,u8* val)
{
	struct r8192_priv* priv = rtllib_priv(dev);

	switch(variable)
	{

		case HW_VAR_BSSID:
			write_nic_dword(dev, BSSIDR, ((u32*)(val))[0]);
			write_nic_word(dev, BSSIDR+2, ((u16*)(val+2))[0]);
		break;

		case HW_VAR_MEDIA_STATUS:
		{
			RT_OP_MODE	OpMode = *((RT_OP_MODE *)(val));
			LED_CTL_MODE	LedAction = LED_CTL_NO_LINK;
			u8		btMsr = read_nic_byte(dev, MSR);

			btMsr &= 0xfc;

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

			//priv->rtllib->LedControlHandler(dev, LedAction);
		}
		break;

		case HW_VAR_CECHK_BSSID:
		{
			u32	RegRCR, Type;

			Type = ((u8*)(val))[0];
			//priv->rtllib->GetHwRegHandler(dev, HW_VAR_RCR, (u8*)(&RegRCR));
			RegRCR = read_nic_dword(dev,RCR);
			priv->ReceiveConfig = RegRCR;

			if (Type == true)
				RegRCR |= (RCR_CBSSID);
			else if (Type == false)
				RegRCR &= (~RCR_CBSSID);

			//priv->rtllib->SetHwRegHandler( dev, HW_VAR_RCR, (u8*)(&RegRCR) );
			write_nic_dword(dev, RCR,RegRCR);
			priv->ReceiveConfig = RegRCR;

		}
		break;

		default:
		break;
	}

}

static void rtl8192_read_eeprom_info(struct net_device* dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);

	u8			tempval;
	u16			i,usValue, IC_Version;
	u16			EEPROMId;
	u8			offset;//, tmpAFR;
	u8			EepromTxPower[100];
	u8 bMac_Tmp_Addr[6] = {0x00, 0xe0, 0x4c, 0x00, 0x00, 0x01};
	RT_TRACE(COMP_INIT, "====> rtl8192_read_eeprom_info\n");


	// TODO: I don't know if we need to apply EF function to EEPROM read function

	//2 Read EEPROM ID to make sure autoload is success
	EEPROMId = eprom_read(dev, 0);
	if( EEPROMId != RTL8190_EEPROM_ID )
	{
		RT_TRACE(COMP_ERR, "EEPROM ID is invalid:%x, %x\n", EEPROMId, RTL8190_EEPROM_ID);
		priv->AutoloadFailFlag=true;
	}
	else
	{
		priv->AutoloadFailFlag=false;
	}

	//
	// Assign Chip Version ID
	//
	// Read IC Version && Channel Plan
	if(!priv->AutoloadFailFlag)
	{
		// VID, PID
		priv->eeprom_vid = eprom_read(dev, (EEPROM_VID >> 1));
		priv->eeprom_did = eprom_read(dev, (EEPROM_DID >> 1));

		usValue = eprom_read(dev, (u16)(EEPROM_Customer_ID>>1)) >> 8 ;
		priv->eeprom_CustomerID = (u8)( usValue & 0xff);
		usValue = eprom_read(dev, (EEPROM_ICVersion_ChannelPlan>>1));
		priv->eeprom_ChannelPlan = usValue&0xff;
		IC_Version = ((usValue&0xff00)>>8);

		priv->card_8192_version = (VERSION_8190)(IC_Version);
		switch(priv->card_8192_version) {
			case VERSION_8190_BD:
			case VERSION_8190_BE:
				break;
			default:
				priv->card_8192_version = VERSION_8190_BD;
				break;
		}
		RT_TRACE(COMP_INIT, "\nIC Version = 0x%x\n", priv->card_8192_version);
	}
	else
	{
		priv->card_8192_version = VERSION_8190_BD;
		priv->eeprom_vid = 0;
		priv->eeprom_did = 0;
		priv->eeprom_CustomerID = 0;
		priv->eeprom_ChannelPlan = 0;
		RT_TRACE(COMP_INIT, "\nIC Version = 0x%x\n", 0xff);
	}

	RT_TRACE(COMP_INIT, "EEPROM VID = 0x%4x\n", priv->eeprom_vid);
	RT_TRACE(COMP_INIT, "EEPROM DID = 0x%4x\n", priv->eeprom_did);
	RT_TRACE(COMP_INIT,"EEPROM Customer ID: 0x%2x\n", priv->eeprom_CustomerID);

	//2 Read Permanent MAC address
	if(!priv->AutoloadFailFlag)
	{
		for(i = 0; i < 6; i += 2)
		{
			usValue = eprom_read(dev, (u16) ((EEPROM_NODE_ADDRESS_BYTE_0+i)>>1));
			*(u16*)(&dev->dev_addr[i]) = usValue;
		}
	} else {
		// when auto load failed,  the last address byte set to be a random one.
		// added by david woo.2007/11/7
		memcpy(dev->dev_addr, bMac_Tmp_Addr, 6);
		#if 0
		for(i = 0; i < 6; i++)
		{
			dev->PermanentAddress[i] = sMacAddr[i];
			write_nic_byte(dev, IDR0+i, sMacAddr[i]);
		}
		#endif
	}

	RT_TRACE(COMP_INIT, "Permanent Address = %02x-%02x-%02x-%02x-%02x-%02x\n",
			dev->dev_addr[0], dev->dev_addr[1],
			dev->dev_addr[2], dev->dev_addr[3],
			dev->dev_addr[4], dev->dev_addr[5]);

		//2 TX Power Check EEPROM Fail or not
	if(priv->card_8192_version > VERSION_8190_BD) {
		priv->bTXPowerDataReadFromEEPORM = true;
	} else {
		priv->bTXPowerDataReadFromEEPORM = false;
	}

	// 2007/11/15 MH 8190PCI Default=2T4R, 8192PCIE dafault=1T2R
	priv->rf_type = RTL819X_DEFAULT_RF_TYPE;

	if(priv->card_8192_version > VERSION_8190_BD)
	{
		// Read RF-indication and Tx Power gain index diff of legacy to HT OFDM rate.
		if(!priv->AutoloadFailFlag)
		{
			tempval = (eprom_read(dev, (EEPROM_RFInd_PowerDiff>>1))) & 0xff;
			priv->EEPROMLegacyHTTxPowerDiff = tempval & 0xf;	// bit[3:0]

			if (tempval&0x80)	//RF-indication, bit[7]
				priv->rf_type = RF_1T2R;
			else
				priv->rf_type = RF_2T4R;
		}
		else
		{
			priv->EEPROMLegacyHTTxPowerDiff = 0x04;//EEPROM_Default_LegacyHTTxPowerDiff;
		}
		RT_TRACE(COMP_INIT, "EEPROMLegacyHTTxPowerDiff = %d\n",
			priv->EEPROMLegacyHTTxPowerDiff);

		// Read ThermalMeter from EEPROM
		if(!priv->AutoloadFailFlag)
		{
			priv->EEPROMThermalMeter = (u8)(((eprom_read(dev, (EEPROM_ThermalMeter>>1))) & 0xff00)>>8);
		}
		else
		{
			priv->EEPROMThermalMeter = EEPROM_Default_ThermalMeter;
		}
		RT_TRACE(COMP_INIT, "ThermalMeter = %d\n", priv->EEPROMThermalMeter);
		//vivi, for tx power track
		priv->TSSI_13dBm = priv->EEPROMThermalMeter *100;

		if(priv->epromtype == EEPROM_93C46)
		{
		// Read antenna tx power offset of B/C/D to A and CrystalCap from EEPROM
		if(!priv->AutoloadFailFlag)
		{
				usValue = eprom_read(dev, (EEPROM_TxPwDiff_CrystalCap>>1));
				priv->EEPROMAntPwDiff = (usValue&0x0fff);
				priv->EEPROMCrystalCap = (u8)((usValue&0xf000)>>12);
		}
		else
		{
				priv->EEPROMAntPwDiff = EEPROM_Default_AntTxPowerDiff;
				priv->EEPROMCrystalCap = EEPROM_Default_TxPwDiff_CrystalCap;
		}
			RT_TRACE(COMP_INIT, "EEPROMAntPwDiff = %d\n", priv->EEPROMAntPwDiff);
			RT_TRACE(COMP_INIT, "EEPROMCrystalCap = %d\n", priv->EEPROMCrystalCap);

		//
		// Get per-channel Tx Power Level
		//
		for(i=0; i<14; i+=2)
		{
			if(!priv->AutoloadFailFlag)
			{
				usValue = eprom_read(dev, (u16) ((EEPROM_TxPwIndex_CCK+i)>>1) );
			}
			else
			{
				usValue = EEPROM_Default_TxPower;
			}
			*((u16*)(&priv->EEPROMTxPowerLevelCCK[i])) = usValue;
			RT_TRACE(COMP_INIT,"CCK Tx Power Level, Index %d = 0x%02x\n", i, priv->EEPROMTxPowerLevelCCK[i]);
			RT_TRACE(COMP_INIT, "CCK Tx Power Level, Index %d = 0x%02x\n", i+1, priv->EEPROMTxPowerLevelCCK[i+1]);
		}
		for(i=0; i<14; i+=2)
		{
			if(!priv->AutoloadFailFlag)
			{
				usValue = eprom_read(dev, (u16) ((EEPROM_TxPwIndex_OFDM_24G+i)>>1) );
			}
			else
			{
				usValue = EEPROM_Default_TxPower;
			}
			*((u16*)(&priv->EEPROMTxPowerLevelOFDM24G[i])) = usValue;
			RT_TRACE(COMP_INIT, "OFDM 2.4G Tx Power Level, Index %d = 0x%02x\n", i, priv->EEPROMTxPowerLevelOFDM24G[i]);
			RT_TRACE(COMP_INIT, "OFDM 2.4G Tx Power Level, Index %d = 0x%02x\n", i+1, priv->EEPROMTxPowerLevelOFDM24G[i+1]);
		}
		}
		else if(priv->epromtype== EEPROM_93C56)
		{
			// Read CrystalCap from EEPROM
			if(!priv->AutoloadFailFlag)
			{
				priv->EEPROMAntPwDiff = EEPROM_Default_AntTxPowerDiff;
				priv->EEPROMCrystalCap = (u8)(((eprom_read(dev, (EEPROM_C56_CrystalCap>>1))) & 0xf000)>>12);
			}
			else
			{
				priv->EEPROMAntPwDiff = EEPROM_Default_AntTxPowerDiff;
				priv->EEPROMCrystalCap = EEPROM_Default_TxPwDiff_CrystalCap;
			}
			RT_TRACE(COMP_INIT,"EEPROMAntPwDiff = %d\n", priv->EEPROMAntPwDiff);
			RT_TRACE(COMP_INIT, "EEPROMCrystalCap = %d\n", priv->EEPROMCrystalCap);

			// Get Tx Power Level by Channel
			if(!priv->AutoloadFailFlag)
			{
				// Read Tx power of Channel 1 ~ 14 from EEPROM.
				for(i = 0; i < 12; i+=2)
				{
					if (i <6)
						offset = EEPROM_C56_RfA_CCK_Chnl1_TxPwIndex + i;
					else
						offset = EEPROM_C56_RfC_CCK_Chnl1_TxPwIndex + i - 6;
					usValue = eprom_read(dev, (offset>>1));
					*((u16*)(&EepromTxPower[i])) = usValue;
				}

				for(i = 0; i < 12; i++)
				{
					if (i <= 2)
						priv->EEPROMRfACCKChnl1TxPwLevel[i] = EepromTxPower[i];
					else if ((i >=3 )&&(i <= 5))
						priv->EEPROMRfAOfdmChnlTxPwLevel[i-3] = EepromTxPower[i];
					else if ((i >=6 )&&(i <= 8))
						priv->EEPROMRfCCCKChnl1TxPwLevel[i-6] = EepromTxPower[i];
					else
						priv->EEPROMRfCOfdmChnlTxPwLevel[i-9] = EepromTxPower[i];
				}
			}
			else
			{
				priv->EEPROMRfACCKChnl1TxPwLevel[0] = EEPROM_Default_TxPowerLevel;
				priv->EEPROMRfACCKChnl1TxPwLevel[1] = EEPROM_Default_TxPowerLevel;
				priv->EEPROMRfACCKChnl1TxPwLevel[2] = EEPROM_Default_TxPowerLevel;

				priv->EEPROMRfAOfdmChnlTxPwLevel[0] = EEPROM_Default_TxPowerLevel;
				priv->EEPROMRfAOfdmChnlTxPwLevel[1] = EEPROM_Default_TxPowerLevel;
				priv->EEPROMRfAOfdmChnlTxPwLevel[2] = EEPROM_Default_TxPowerLevel;

				priv->EEPROMRfCCCKChnl1TxPwLevel[0] = EEPROM_Default_TxPowerLevel;
				priv->EEPROMRfCCCKChnl1TxPwLevel[1] = EEPROM_Default_TxPowerLevel;
				priv->EEPROMRfCCCKChnl1TxPwLevel[2] = EEPROM_Default_TxPowerLevel;

				priv->EEPROMRfCOfdmChnlTxPwLevel[0] = EEPROM_Default_TxPowerLevel;
				priv->EEPROMRfCOfdmChnlTxPwLevel[1] = EEPROM_Default_TxPowerLevel;
				priv->EEPROMRfCOfdmChnlTxPwLevel[2] = EEPROM_Default_TxPowerLevel;
			}
			RT_TRACE(COMP_INIT, "priv->EEPROMRfACCKChnl1TxPwLevel[0] = 0x%x\n", priv->EEPROMRfACCKChnl1TxPwLevel[0]);
			RT_TRACE(COMP_INIT, "priv->EEPROMRfACCKChnl1TxPwLevel[1] = 0x%x\n", priv->EEPROMRfACCKChnl1TxPwLevel[1]);
			RT_TRACE(COMP_INIT, "priv->EEPROMRfACCKChnl1TxPwLevel[2] = 0x%x\n", priv->EEPROMRfACCKChnl1TxPwLevel[2]);
			RT_TRACE(COMP_INIT, "priv->EEPROMRfAOfdmChnlTxPwLevel[0] = 0x%x\n", priv->EEPROMRfAOfdmChnlTxPwLevel[0]);
			RT_TRACE(COMP_INIT, "priv->EEPROMRfAOfdmChnlTxPwLevel[1] = 0x%x\n", priv->EEPROMRfAOfdmChnlTxPwLevel[1]);
			RT_TRACE(COMP_INIT, "priv->EEPROMRfAOfdmChnlTxPwLevel[2] = 0x%x\n", priv->EEPROMRfAOfdmChnlTxPwLevel[2]);
			RT_TRACE(COMP_INIT, "priv->EEPROMRfCCCKChnl1TxPwLevel[0] = 0x%x\n", priv->EEPROMRfCCCKChnl1TxPwLevel[0]);
			RT_TRACE(COMP_INIT, "priv->EEPROMRfCCCKChnl1TxPwLevel[1] = 0x%x\n", priv->EEPROMRfCCCKChnl1TxPwLevel[1]);
			RT_TRACE(COMP_INIT, "priv->EEPROMRfCCCKChnl1TxPwLevel[2] = 0x%x\n", priv->EEPROMRfCCCKChnl1TxPwLevel[2]);
			RT_TRACE(COMP_INIT, "priv->EEPROMRfCOfdmChnlTxPwLevel[0] = 0x%x\n", priv->EEPROMRfCOfdmChnlTxPwLevel[0]);
			RT_TRACE(COMP_INIT, "priv->EEPROMRfCOfdmChnlTxPwLevel[1] = 0x%x\n", priv->EEPROMRfCOfdmChnlTxPwLevel[1]);
			RT_TRACE(COMP_INIT, "priv->EEPROMRfCOfdmChnlTxPwLevel[2] = 0x%x\n", priv->EEPROMRfCOfdmChnlTxPwLevel[2]);

		}
		//
		// Update HAL variables.
		//
		if(priv->epromtype == EEPROM_93C46)
		{
			for(i=0; i<14; i++)
			{
				priv->TxPowerLevelCCK[i] = priv->EEPROMTxPowerLevelCCK[i];
				priv->TxPowerLevelOFDM24G[i] = priv->EEPROMTxPowerLevelOFDM24G[i];
			}
			priv->LegacyHTTxPowerDiff = priv->EEPROMLegacyHTTxPowerDiff;
		// Antenna B gain offset to antenna A, bit0~3
			priv->AntennaTxPwDiff[0] = (priv->EEPROMAntPwDiff & 0xf);
		// Antenna C gain offset to antenna A, bit4~7
			priv->AntennaTxPwDiff[1] = ((priv->EEPROMAntPwDiff & 0xf0)>>4);
		// Antenna D gain offset to antenna A, bit8~11
			priv->AntennaTxPwDiff[2] = ((priv->EEPROMAntPwDiff & 0xf00)>>8);
		// CrystalCap, bit12~15
			priv->CrystalCap = priv->EEPROMCrystalCap;
		// ThermalMeter, bit0~3 for RFIC1, bit4~7 for RFIC2
			priv->ThermalMeter[0] = (priv->EEPROMThermalMeter & 0xf);
			priv->ThermalMeter[1] = ((priv->EEPROMThermalMeter & 0xf0)>>4);
		}
		else if(priv->epromtype == EEPROM_93C56)
		{
			//char	cck_pwr_diff_a=0, cck_pwr_diff_c=0;

			//cck_pwr_diff_a = pHalData->EEPROMRfACCKChnl7TxPwLevel - pHalData->EEPROMRfAOfdmChnlTxPwLevel[1];
			//cck_pwr_diff_c = pHalData->EEPROMRfCCCKChnl7TxPwLevel - pHalData->EEPROMRfCOfdmChnlTxPwLevel[1];
			for(i=0; i<3; i++)	// channel 1~3 use the same Tx Power Level.
			{
				priv->TxPowerLevelCCK_A[i]  = priv->EEPROMRfACCKChnl1TxPwLevel[0];
				priv->TxPowerLevelOFDM24G_A[i] = priv->EEPROMRfAOfdmChnlTxPwLevel[0];
				priv->TxPowerLevelCCK_C[i] =  priv->EEPROMRfCCCKChnl1TxPwLevel[0];
				priv->TxPowerLevelOFDM24G_C[i] = priv->EEPROMRfCOfdmChnlTxPwLevel[0];
			}
			for(i=3; i<9; i++)	// channel 4~9 use the same Tx Power Level
			{
				priv->TxPowerLevelCCK_A[i]  = priv->EEPROMRfACCKChnl1TxPwLevel[1];
				priv->TxPowerLevelOFDM24G_A[i] = priv->EEPROMRfAOfdmChnlTxPwLevel[1];
				priv->TxPowerLevelCCK_C[i] =  priv->EEPROMRfCCCKChnl1TxPwLevel[1];
				priv->TxPowerLevelOFDM24G_C[i] = priv->EEPROMRfCOfdmChnlTxPwLevel[1];
			}
			for(i=9; i<14; i++)	// channel 10~14 use the same Tx Power Level
			{
				priv->TxPowerLevelCCK_A[i]  = priv->EEPROMRfACCKChnl1TxPwLevel[2];
				priv->TxPowerLevelOFDM24G_A[i] = priv->EEPROMRfAOfdmChnlTxPwLevel[2];
				priv->TxPowerLevelCCK_C[i] =  priv->EEPROMRfCCCKChnl1TxPwLevel[2];
				priv->TxPowerLevelOFDM24G_C[i] = priv->EEPROMRfCOfdmChnlTxPwLevel[2];
			}
			for(i=0; i<14; i++)
				RT_TRACE(COMP_INIT, "priv->TxPowerLevelCCK_A[%d] = 0x%x\n", i, priv->TxPowerLevelCCK_A[i]);
			for(i=0; i<14; i++)
				RT_TRACE(COMP_INIT,"priv->TxPowerLevelOFDM24G_A[%d] = 0x%x\n", i, priv->TxPowerLevelOFDM24G_A[i]);
			for(i=0; i<14; i++)
				RT_TRACE(COMP_INIT, "priv->TxPowerLevelCCK_C[%d] = 0x%x\n", i, priv->TxPowerLevelCCK_C[i]);
			for(i=0; i<14; i++)
				RT_TRACE(COMP_INIT, "priv->TxPowerLevelOFDM24G_C[%d] = 0x%x\n", i, priv->TxPowerLevelOFDM24G_C[i]);
			priv->LegacyHTTxPowerDiff = priv->EEPROMLegacyHTTxPowerDiff;
			priv->AntennaTxPwDiff[0] = 0;
			priv->AntennaTxPwDiff[1] = 0;
			priv->AntennaTxPwDiff[2] = 0;
			priv->CrystalCap = priv->EEPROMCrystalCap;
			// ThermalMeter, bit0~3 for RFIC1, bit4~7 for RFIC2
			priv->ThermalMeter[0] = (priv->EEPROMThermalMeter & 0xf);
			priv->ThermalMeter[1] = ((priv->EEPROMThermalMeter & 0xf0)>>4);
		}
	}

	if(priv->rf_type == RF_1T2R)
	{
		RT_TRACE(COMP_INIT, "\n1T2R config\n");
	}
	else if (priv->rf_type == RF_2T4R)
	{
		RT_TRACE(COMP_INIT, "\n2T4R config\n");
	}

	// 2008/01/16 MH We can only know RF type in the function. So we have to init
	// DIG RATR table again.
	init_rate_adaptive(dev);

	//1 Make a copy for following variables and we can change them if we want

	priv->rf_chip= RF_8256;

	if(priv->RegChannelPlan == 0xf)
	{
		priv->ChannelPlan = priv->eeprom_ChannelPlan;
	}
	else
	{
		priv->ChannelPlan = priv->RegChannelPlan;
	}

	//
	//  Used PID and DID to Set CustomerID
	//
	if( priv->eeprom_vid == 0x1186 &&  priv->eeprom_did == 0x3304 )
	{
		priv->CustomerID =  RT_CID_DLINK;
	}

	switch(priv->eeprom_CustomerID)
	{
		case EEPROM_CID_DEFAULT:
			priv->CustomerID = RT_CID_DEFAULT;
			break;
		case EEPROM_CID_CAMEO:
			priv->CustomerID = RT_CID_819x_CAMEO;
			break;
		case  EEPROM_CID_RUNTOP:
			priv->CustomerID = RT_CID_819x_RUNTOP;
			break;
		case EEPROM_CID_NetCore:
			priv->CustomerID = RT_CID_819x_Netcore;
			break;
		case EEPROM_CID_TOSHIBA:        // Merge by Jacken, 2008/01/31
			priv->CustomerID = RT_CID_TOSHIBA;
			if(priv->eeprom_ChannelPlan&0x80)
				priv->ChannelPlan = priv->eeprom_ChannelPlan&0x7f;
			else
				priv->ChannelPlan = 0x0;
			RT_TRACE(COMP_INIT, "Toshiba ChannelPlan = 0x%x\n",
				priv->ChannelPlan);
			break;
		case EEPROM_CID_Nettronix:
			priv->ScanDelay = 100;	//cosa add for scan
			priv->CustomerID = RT_CID_Nettronix;
			break;
		case EEPROM_CID_Pronet:
			priv->CustomerID = RT_CID_PRONET;
			break;
		case EEPROM_CID_DLINK:
			priv->CustomerID = RT_CID_DLINK;
			break;

		case EEPROM_CID_WHQL:
			//dev->bInHctTest = true;//do not supported

			//priv->bSupportTurboMode = false;
			//priv->bAutoTurboBy8186 = false;

			//pMgntInfo->PowerSaveControl.bInactivePs = false;
			//pMgntInfo->PowerSaveControl.bIPSModeBackup = false;
			//pMgntInfo->PowerSaveControl.bLeisurePs = false;

			break;
		default:
			// value from RegCustomerID
			break;
	}

	//Avoid the channel plan array overflow, by Bruce, 2007-08-27.
	if(priv->ChannelPlan > CHANNEL_PLAN_LEN - 1)
		priv->ChannelPlan = 0; //FCC
	// force to world wide 13
	priv->ChannelPlan = COUNTRY_CODE_WORLD_WIDE_13;

#ifdef TO_DO_LIST
	switch(priv->CustomerID) {
	case RT_CID_DEFAULT:
		priv->LedStrategy = HW_LED;
		break;
	case RT_CID_819x_CAMEO:
		priv->LedStrategy = SW_LED_MODE2;
		break;
	case RT_CID_819x_RUNTOP:
		priv->LedStrategy = SW_LED_MODE3;
		break;
	case RT_CID_819x_Netcore:
		priv->LedStrategy = SW_LED_MODE4;
		break;
	case RT_CID_Nettronix:
		priv->LedStrategy = SW_LED_MODE5;
		break;
	case RT_CID_PRONET:
		priv->LedStrategy = SW_LED_MODE6;
		break;
	case RT_CID_TOSHIBA:   //Modify by Jacken 2008/01/31
		// Do nothing.
		//break;
	default:
		priv->LedStrategy = HW_LED;
		break;
	}
	RT_TRACE(COMP_INIT, "LedStrategy = %d \n", priv->LedStrategy);
#endif
	//2008.06.03, for WOL
        //if( priv->eeprom_vid == 0x1186 &&  priv->eeprom_did == 0x3304)
        //priv->rtllib->bSupportRemoteWakeUp = true;
        //else
        //priv->rtllib->bSupportRemoteWakeUp = false;
	RT_TRACE(COMP_INIT, "RegChannelPlan(%d)\n", priv->RegChannelPlan);
	RT_TRACE(COMP_INIT, "ChannelPlan = %d \n", priv->ChannelPlan);
	RT_TRACE(COMP_TRACE, "<==== ReadAdapterInfo\n");

	return ;
}

void rtl8192_get_eeprom_size(struct net_device* dev)
{
	u16 curCR = 0;
	struct r8192_priv *priv = rtllib_priv(dev);
	RT_TRACE(COMP_INIT, "===========>%s()\n", __FUNCTION__);
	curCR = read_nic_dword(dev, EPROM_CMD);
	RT_TRACE(COMP_INIT, "read from Reg Cmd9346CR(%x):%x\n", EPROM_CMD, curCR);
	//whether need I consider BIT5?
	priv->epromtype = (curCR & EPROM_CMD_9356SEL) ? EEPROM_93C56 : EEPROM_93C46;
	RT_TRACE(COMP_INIT, "<===========%s(), epromtype:%d\n", __FUNCTION__, priv->epromtype);
	rtl8192_read_eeprom_info(dev);
}

//*****************************************************************************
//function:  This function actually only set RRSR, RATR and BW_OPMODE registers
//	     not to do all the hw config as its name says
//   input:  net_device dev
//  output:  none
//  return:  none
//  notice:  This part need to modified according to the rate set we filtered
// ****************************************************************************/
static void rtl8192_hwconfig(struct net_device* dev)
{
	u32 regRATR = 0, regRRSR = 0;
	u8 regBwOpMode = 0, regTmp = 0;
	struct r8192_priv *priv = rtllib_priv(dev);

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

	write_nic_byte(dev, BW_OPMODE, regBwOpMode);
	{
		u32 ratr_value = 0;
		ratr_value = regRATR;
		if (priv->rf_type == RF_1T2R)
		{
			ratr_value &= ~(RATE_ALL_OFDM_2SS);
		}
		write_nic_dword(dev, RATR0, ratr_value);
		write_nic_byte(dev, UFWP, 1);
	}
	regTmp = read_nic_byte(dev, 0x313);
	regRRSR = ((regTmp) << 24) | (regRRSR & 0x00ffffff);
	write_nic_dword(dev, RRSR, regRRSR);

	//
	// Set Retry Limit here
	//
	write_nic_word(dev, RETRY_LIMIT,
			priv->ShortRetryLimit << RETRY_LIMIT_SHORT_SHIFT | \
			priv->LongRetryLimit << RETRY_LIMIT_LONG_SHIFT);
	// Set Contention Window here

	// Set Tx AGC

	// Set Tx Antenna including Feedback control

	// Set Auto Rate fallback control
}

bool rtl8192_adapter_start(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
//	struct rtllib_device *ieee = priv->rtllib;
	u32 ulRegRead;
	bool rtStatus = true;
//	static char szMACPHYRegFile[] = RTL819X_PHY_MACPHY_REG;
//	static char szMACPHYRegPGFile[] = RTL819X_PHY_MACPHY_REG_PG;
	//u8 eRFPath;
	u8 tmpvalue;
	bool bfirmwareok = true;
	u8 ucRegRead;
	u32 tmpRegA, tmpRegC, TempCCk;
	int i = 0;
	u32 retry_times = 0;

	RT_TRACE(COMP_INIT, "====>%s()\n", __FUNCTION__);
	priv->being_init_adapter = true;
start:
        rtl8192_pci_resetdescring(dev);
	// 2007/11/02 MH Before initalizing RF. We can not use FW to do RF-R/W.
	priv->Rf_Mode = RF_OP_By_SW_3wire;
	//PlatformSleepUs(10000);
	// For any kind of InitializeAdapter process, we shall use system now!!
	priv->pFirmware->firmware_status = FW_STATUS_0_INIT;

	// Set to eRfoff in order not to count receive count.
	if(priv->RegRfOff == true)
		priv->rtllib->eRFPowerState = eRfOff;

	//
	//3 //Config CPUReset Register
	//3//
	//3 Firmware Reset Or Not
	ulRegRead = read_nic_dword(dev, CPU_GEN);
	if(priv->pFirmware->firmware_status == FW_STATUS_0_INIT)
	{	//called from MPInitialized. do nothing
		ulRegRead |= CPU_GEN_SYSTEM_RESET;
	}else if(priv->pFirmware->firmware_status == FW_STATUS_5_READY)
		ulRegRead |= CPU_GEN_FIRMWARE_RESET;	// Called from MPReset
	else
		RT_TRACE(COMP_ERR, "ERROR in %s(): undefined firmware state(%d)\n", __FUNCTION__,   priv->pFirmware->firmware_status);

	//2008.06.03, for WOL 90 hw bug
	ulRegRead &= (~(CPU_GEN_GPIO_UART));

	write_nic_dword(dev, CPU_GEN, ulRegRead);
	//mdelay(100);

	//3//
	//3// Initialize BB before MAC
	//3//
	//rtl8192_dump_reg(dev);
	RT_TRACE(COMP_INIT, "BB Config Start!\n");
	rtStatus = rtl8192_BBConfig(dev);
	if(rtStatus != true)
	{
		RT_TRACE(COMP_ERR, "BB Config failed\n");
		return rtStatus;
	}
	RT_TRACE(COMP_INIT,"BB Config Finished!\n");

	//rtl8192_dump_reg(dev);
	//
	//3//Set Loopback mode or Normal mode
	//3//
	//2006.12.13 by emily. Note!We should not merge these two CPU_GEN register writings
	//	because setting of System_Reset bit reset MAC to default transmission mode.
		//Loopback mode or not
	priv->LoopbackMode = RTL819X_NO_LOOPBACK;
	//priv->LoopbackMode = RTL819X_MAC_LOOPBACK;
	if(priv->ResetProgress == RESET_TYPE_NORESET)
	{
	ulRegRead = read_nic_dword(dev, CPU_GEN);
	if(priv->LoopbackMode == RTL819X_NO_LOOPBACK)
	{
		ulRegRead = ((ulRegRead & CPU_GEN_NO_LOOPBACK_MSK) | CPU_GEN_NO_LOOPBACK_SET);
	}
	else if (priv->LoopbackMode == RTL819X_MAC_LOOPBACK )
	{
		ulRegRead |= CPU_CCK_LOOPBACK;
	}
	else
	{
		RT_TRACE(COMP_ERR,"Serious error: wrong loopback mode setting\n");
	}

	//2008.06.03, for WOL
	//ulRegRead &= (~(CPU_GEN_GPIO_UART));
	write_nic_dword(dev, CPU_GEN, ulRegRead);

	// 2006.11.29. After reset cpu, we sholud wait for a second, otherwise, it may fail to write registers. Emily
	udelay(500);
	}
	//3Set Hardware(Do nothing now)
	rtl8192_hwconfig(dev);
	//2=======================================================
	// Common Setting for all of the FPGA platform. (part 1)
	//2=======================================================
	// If there is changes, please make sure it applies to all of the FPGA version
	//3 Turn on Tx/Rx
	write_nic_byte(dev, CMDR, CR_RE|CR_TE);

	//2Set Tx dma burst
	write_nic_byte(dev, PCIF, ((MXDMA2_NoLimit<<MXDMA2_RX_SHIFT) | \
				(MXDMA2_NoLimit<<MXDMA2_TX_SHIFT) | \
				(1<<MULRW_SHIFT)));
	//set IDR0 here
	write_nic_dword(dev, MAC0, ((u32*)dev->dev_addr)[0]);
	write_nic_word(dev, MAC4, ((u16*)(dev->dev_addr + 4))[0]);
	//set RCR
	write_nic_dword(dev, RCR, priv->ReceiveConfig);

	//3 Initialize Number of Reserved Pages in Firmware Queue
#ifdef TO_DO_LIST
	if(priv->bInHctTest)
	{
		write_nic_dword(dev, RQPN1,  NUM_OF_PAGE_IN_FW_QUEUE_BK_DTM << RSVD_FW_QUEUE_PAGE_BK_SHIFT |\
				NUM_OF_PAGE_IN_FW_QUEUE_BE_DTM << RSVD_FW_QUEUE_PAGE_BE_SHIFT | \
				NUM_OF_PAGE_IN_FW_QUEUE_VI_DTM << RSVD_FW_QUEUE_PAGE_VI_SHIFT | \
				NUM_OF_PAGE_IN_FW_QUEUE_VO_DTM <<RSVD_FW_QUEUE_PAGE_VO_SHIFT);
		write_nic_dword(dev, RQPN2, NUM_OF_PAGE_IN_FW_QUEUE_MGNT << RSVD_FW_QUEUE_PAGE_MGNT_SHIFT);
		write_nic_dword(dev, RQPN3, APPLIED_RESERVED_QUEUE_IN_FW| \
				NUM_OF_PAGE_IN_FW_QUEUE_BCN<<RSVD_FW_QUEUE_PAGE_BCN_SHIFT|\
				NUM_OF_PAGE_IN_FW_QUEUE_PUB_DTM<<RSVD_FW_QUEUE_PAGE_PUB_SHIFT);
	}
	else
#endif
	{
		write_nic_dword(dev, RQPN1,  NUM_OF_PAGE_IN_FW_QUEUE_BK << RSVD_FW_QUEUE_PAGE_BK_SHIFT |\
					NUM_OF_PAGE_IN_FW_QUEUE_BE << RSVD_FW_QUEUE_PAGE_BE_SHIFT | \
					NUM_OF_PAGE_IN_FW_QUEUE_VI << RSVD_FW_QUEUE_PAGE_VI_SHIFT | \
					NUM_OF_PAGE_IN_FW_QUEUE_VO <<RSVD_FW_QUEUE_PAGE_VO_SHIFT);
		write_nic_dword(dev, RQPN2, NUM_OF_PAGE_IN_FW_QUEUE_MGNT << RSVD_FW_QUEUE_PAGE_MGNT_SHIFT);
		write_nic_dword(dev, RQPN3, APPLIED_RESERVED_QUEUE_IN_FW| \
					NUM_OF_PAGE_IN_FW_QUEUE_BCN<<RSVD_FW_QUEUE_PAGE_BCN_SHIFT|\
					NUM_OF_PAGE_IN_FW_QUEUE_PUB<<RSVD_FW_QUEUE_PAGE_PUB_SHIFT);
	}

	rtl8192_tx_enable(dev);
	rtl8192_rx_enable(dev);
	//3Set Response Rate Setting Register
	// CCK rate is supported by default.
	// CCK rate will be filtered out only when associated AP does not support it.
	ulRegRead = (0xFFF00000 & read_nic_dword(dev, RRSR))  | RATE_ALL_OFDM_AG | RATE_ALL_CCK;
	write_nic_dword(dev, RRSR, ulRegRead);
	write_nic_dword(dev, RATR0+4*7, (RATE_ALL_OFDM_AG | RATE_ALL_CCK));

	//2Set AckTimeout
	// TODO: (it value is only for FPGA version). need to be changed!!2006.12.18, by Emily
	write_nic_byte(dev, ACK_TIMEOUT, 0x30);

	//rtl8192_actset_wirelessmode(dev,priv->RegWirelessMode);
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
	//3Beacon related
	write_nic_word(dev, ATIMWND, 2);
	write_nic_word(dev, BCN_INTERVAL, 100);
	{
		int i;
		for (i=0; i<QOS_QUEUE_NUM; i++)
		write_nic_dword(dev, WDCAPARA_ADD[i], 0x005e4332);
	}
	//
	// Switching regulator controller: This is set temporarily.
	// It's not sure if this can be removed in the future.
	// PJ advised to leave it by default.
	//
	write_nic_byte(dev, 0xbe, 0xc0);

	//2=======================================================
	// Set PHY related configuration defined in MAC register bank
	//2=======================================================
	rtl8192_phy_configmac(dev);

	if (priv->card_8192_version > (u8) VERSION_8190_BD) {
		rtl8192_phy_getTxPower(dev);
		rtl8192_phy_setTxPower(dev, priv->chan);
	}

	//if D or C cut
	tmpvalue = read_nic_byte(dev, IC_VERRSION);
	priv->IC_Cut= tmpvalue;
	RT_TRACE(COMP_INIT, "priv->IC_Cut= 0x%x\n", priv->IC_Cut);
	if(priv->IC_Cut>= IC_VersionCut_D)
	{
		//pHalData->bDcut = true;
		if (priv->IC_Cut== IC_VersionCut_D) {
			RT_TRACE(COMP_INIT, "D-cut\n");
		} else if (priv->IC_Cut== IC_VersionCut_E) {
			RT_TRACE(COMP_INIT, "E-cut\n");
			// HW SD suggest that we should not wirte this register too often, so driver
			// should readback this register. This register will be modified only when
			// power on reset
		}
	} else {
		//pHalData->bDcut = false;
		RT_TRACE(COMP_INIT, "Before C-cut\n");
	}

	//Firmware download
	RT_TRACE(COMP_INIT, "Load Firmware!\n");
	bfirmwareok = init_firmware(dev);
	if (!bfirmwareok) {
		if (retry_times < 10) {
			retry_times++;
			goto start;
		} else {
			rtStatus = false;
			goto end;
		}
	}
	RT_TRACE(COMP_INIT, "Load Firmware finished!\n");
	//RF config
	if (priv->ResetProgress == RESET_TYPE_NORESET) {
		RT_TRACE(COMP_INIT, "RF Config Started!\n");
		rtStatus = rtl8192_phy_RFConfig(dev);
		if (rtStatus != true) {
			RT_TRACE(COMP_ERR, "RF Config failed\n");
			return rtStatus;
		}
		RT_TRACE(COMP_INIT, "RF Config Finished!\n");
	}
	rtl8192_phy_updateInitGain(dev);

	//---- Set CCK and OFDM Block "ON"----*/
	rtl8192_setBBreg(dev, rFPGA0_RFMOD, bCCKEn, 0x1);
	rtl8192_setBBreg(dev, rFPGA0_RFMOD, bOFDMEn, 0x1);

	//2008.06.03, for WOL
	ucRegRead = read_nic_byte(dev, GPE);
	ucRegRead |= BIT0;
	write_nic_byte(dev, GPE, ucRegRead);

	ucRegRead = read_nic_byte(dev, GPO);
	ucRegRead &= ~BIT0;
	write_nic_byte(dev, GPO, ucRegRead);

	//2=======================================================
	// RF Power Save
	//2=======================================================
#ifdef ENABLE_IPS
	if(priv->RegRfOff == true)
	{ // User disable RF via registry.
		RT_TRACE((COMP_INIT|COMP_RF|COMP_POWER), "%s(): Turn off RF for RegRfOff ----------\n",__FUNCTION__);
		MgntActSet_RF_State(dev, eRfOff, RF_CHANGE_BY_SW);
#if 0//cosa, ask SD3 willis and he doesn't know what is this for
		// Those action will be discard in MgntActSet_RF_State because off the same state
	for(eRFPath = 0; eRFPath <pHalData->NumTotalRFPath; eRFPath++)
		PHY_SetRFReg(dev, (RF90_RADIO_PATH_E)eRFPath, 0x4, 0xC00, 0x0);
#endif
	}
	else if(priv->rtllib->RfOffReason > RF_CHANGE_BY_PS)
	{ // H/W or S/W RF OFF before sleep.
		RT_TRACE((COMP_INIT|COMP_RF|COMP_POWER), "%s(): Turn off RF for RfOffReason(%d) ----------\n", __FUNCTION__,priv->rtllib->RfOffReason);
		MgntActSet_RF_State(dev, eRfOff, priv->rtllib->RfOffReason);
	}
	else if(priv->rtllib->RfOffReason >= RF_CHANGE_BY_IPS)
	{ // H/W or S/W RF OFF before sleep.
		RT_TRACE((COMP_INIT|COMP_RF|COMP_POWER), "%s(): Turn off RF for RfOffReason(%d) ----------\n", __FUNCTION__,priv->rtllib->RfOffReason);
		MgntActSet_RF_State(dev, eRfOff, priv->rtllib->RfOffReason);
	}
	else
	{
		RT_TRACE((COMP_INIT|COMP_RF|COMP_POWER), "%s(): RF-ON \n",__FUNCTION__);
		priv->rtllib->eRFPowerState = eRfOn;
		priv->rtllib->RfOffReason = 0;
		//DrvIFIndicateCurrentPhyStatus(dev);
	// LED control
	//dev->HalFunc.LedControlHandler(dev, LED_CTL_POWER_ON);

	//
	// If inactive power mode is enabled, disable rf while in disconnected state.
	// But we should still tell upper layer we are in rf on state.
	// 2007.07.16, by shien chang.
	//
		//if(!dev->bInHctTest)
	//IPSEnter(dev);

	}
#endif

	priv->Rf_Mode = RF_OP_By_SW_3wire;

	if (priv->ResetProgress == RESET_TYPE_NORESET) {
		dm_initialize_txpower_tracking(dev);

		tmpRegA= rtl8192_QueryBBReg(dev,rOFDM0_XATxIQImbalance,bMaskDWord);
		tmpRegC= rtl8192_QueryBBReg(dev,rOFDM0_XCTxIQImbalance,bMaskDWord);

		if (priv->rf_type == RF_2T4R){
			for (i = 0; i<TxBBGainTableLength; i++) {
				if (tmpRegA == priv->txbbgain_table[i].txbbgain_value) {
					priv->rfa_txpowertrackingindex= (u8)i;
					priv->rfa_txpowertrackingindex_real= (u8)i;
					priv->rfa_txpowertracking_default = priv->rfa_txpowertrackingindex;
					break;
				}
			}
		}
		for (i = 0; i<TxBBGainTableLength; i++) {
			if(tmpRegC == priv->txbbgain_table[i].txbbgain_value)
			{
				priv->rfc_txpowertrackingindex= (u8)i;
				priv->rfc_txpowertrackingindex_real= (u8)i;
				priv->rfc_txpowertracking_default = priv->rfc_txpowertrackingindex;
				break;
			}
		}
		TempCCk = rtl8192_QueryBBReg(dev, rCCK0_TxFilter1, bMaskByte2);

		for (i = 0 ; i < CCKTxBBGainTableLength ; i++) {
			if (TempCCk == priv->cck_txbbgain_table[i].ccktxbb_valuearray[0]) {
				priv->CCKPresentAttentuation_20Mdefault =(u8) i;
				break;
			}
		}
		priv->CCKPresentAttentuation_40Mdefault = 0;
		priv->CCKPresentAttentuation_difference = 0;
		priv->CCKPresentAttentuation = priv->CCKPresentAttentuation_20Mdefault;
		RT_TRACE(COMP_POWER_TRACKING, "priv->rfa_txpowertrackingindex_initial = %d\n", priv->rfa_txpowertrackingindex);
		RT_TRACE(COMP_POWER_TRACKING, "priv->rfa_txpowertrackingindex_real__initial = %d\n", priv->rfa_txpowertrackingindex_real);
		RT_TRACE(COMP_POWER_TRACKING, "priv->rfc_txpowertrackingindex_initial = %d\n", priv->rfc_txpowertrackingindex);
		RT_TRACE(COMP_POWER_TRACKING, "priv->rfc_txpowertrackingindex_real_initial = %d\n", priv->rfc_txpowertrackingindex_real);
		RT_TRACE(COMP_POWER_TRACKING, "priv->CCKPresentAttentuation_difference_initial = %d\n", priv->CCKPresentAttentuation_difference);
		RT_TRACE(COMP_POWER_TRACKING, "priv->CCKPresentAttentuation_initial = %d\n", priv->CCKPresentAttentuation);
	}
	rtl8192_irq_enable(dev);
end:
	priv->being_init_adapter = false;
	return rtStatus;

}

void rtl8192_net_update(struct net_device *dev)
{

	struct r8192_priv *priv = rtllib_priv(dev);
	struct rtllib_network *net;
	u16 BcnTimeCfg = 0, BcnCW = 6, BcnIFS = 0xf;
	u16 rate_config = 0;
	net = &priv->rtllib->current_network;
	//update Basic rate: RR, BRSR
	rtl8192_config_rate(dev, &rate_config);
	// 2007.01.16, by Emily
	// Select RRSR (in Legacy-OFDM and CCK)
	// For 8190, we select only 24M, 12M, 6M, 11M, 5.5M, 2M, and 1M from the Basic rate.
	// We do not use other rates.
	 priv->basic_rate = rate_config &= 0x15f;
	//BSSID
	write_nic_dword(dev,BSSIDR,((u32*)net->bssid)[0]);
	write_nic_word(dev,BSSIDR+4,((u16*)net->bssid)[2]);

//	rtl8192_update_cap(dev, net->capability);
	if (priv->rtllib->iw_mode == IW_MODE_ADHOC)
	{
		write_nic_word(dev, ATIMWND, 2);
		write_nic_word(dev, BCN_DMATIME, 256);
		write_nic_word(dev, BCN_INTERVAL, net->beacon_interval);
	//	write_nic_word(dev, BcnIntTime, 100);
	//BIT15 of BCN_DRV_EARLY_INT will indicate whether software beacon or hw beacon is applied.
		write_nic_word(dev, BCN_DRV_EARLY_INT, 10);
		write_nic_byte(dev, BCN_ERR_THRESH, 100);

		BcnTimeCfg |= (BcnCW<<BCN_TCFG_CW_SHIFT);
	// TODO: BcnIFS may required to be changed on ASIC
		BcnTimeCfg |= BcnIFS<<BCN_TCFG_IFS;

		write_nic_word(dev, BCN_TCFG, BcnTimeCfg);
	}
}

void rtl8192_link_change(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	struct rtllib_device* ieee = priv->rtllib;

	if (ieee->state == RTLLIB_LINKED) {
		rtl8192_net_update(dev);
		priv->ops->update_ratr_table(dev);
		//add this as in pure N mode, wep encryption will use software way, but there is no chance to set this as wep will not set group key in wext. WB.2008.07.08
		if ((KEY_TYPE_WEP40 == ieee->pairwise_key_type) || (KEY_TYPE_WEP104 == ieee->pairwise_key_type))
			EnableHWSecurityConfig8192(dev);
	} else {
		write_nic_byte(dev, 0x173, 0);
	}
	//update timing params*/
	//rtl8192_set_chan(dev, priv->chan);
	//MSR
	rtl8192_update_msr(dev);

	// 2007/10/16 MH MAC Will update TSF according to all received beacon, so we have
	//      // To set CBSSID bit when link with any AP or STA.
	if (ieee->iw_mode == IW_MODE_INFRA || ieee->iw_mode == IW_MODE_ADHOC) {
		u32 reg = 0;
		reg = read_nic_dword(dev, RCR);
		if (priv->rtllib->state == RTLLIB_LINKED)
			priv->ReceiveConfig = reg |= RCR_CBSSID;
		else
			priv->ReceiveConfig = reg &= ~RCR_CBSSID;
		write_nic_dword(dev, RCR, reg);
	}
}

static u8 MRateToHwRate8190Pci(u8 rate)
{
	u8  ret = DESC90_RATE1M;

	switch(rate) {
		case MGN_1M:	ret = DESC90_RATE1M;		break;
		case MGN_2M:	ret = DESC90_RATE2M;		break;
		case MGN_5_5M:	ret = DESC90_RATE5_5M;	break;
		case MGN_11M:	ret = DESC90_RATE11M;	break;
		case MGN_6M:	ret = DESC90_RATE6M;		break;
		case MGN_9M:	ret = DESC90_RATE9M;		break;
		case MGN_12M:	ret = DESC90_RATE12M;	break;
		case MGN_18M:	ret = DESC90_RATE18M;	break;
		case MGN_24M:	ret = DESC90_RATE24M;	break;
		case MGN_36M:	ret = DESC90_RATE36M;	break;
		case MGN_48M:	ret = DESC90_RATE48M;	break;
		case MGN_54M:	ret = DESC90_RATE54M;	break;

		// HT rate since here
		case MGN_MCS0:	ret = DESC90_RATEMCS0;	break;
		case MGN_MCS1:	ret = DESC90_RATEMCS1;	break;
		case MGN_MCS2:	ret = DESC90_RATEMCS2;	break;
		case MGN_MCS3:	ret = DESC90_RATEMCS3;	break;
		case MGN_MCS4:	ret = DESC90_RATEMCS4;	break;
		case MGN_MCS5:	ret = DESC90_RATEMCS5;	break;
		case MGN_MCS6:	ret = DESC90_RATEMCS6;	break;
		case MGN_MCS7:	ret = DESC90_RATEMCS7;	break;
		case MGN_MCS8:	ret = DESC90_RATEMCS8;	break;
		case MGN_MCS9:	ret = DESC90_RATEMCS9;	break;
		case MGN_MCS10:	ret = DESC90_RATEMCS10;	break;
		case MGN_MCS11:	ret = DESC90_RATEMCS11;	break;
		case MGN_MCS12:	ret = DESC90_RATEMCS12;	break;
		case MGN_MCS13:	ret = DESC90_RATEMCS13;	break;
		case MGN_MCS14:	ret = DESC90_RATEMCS14;	break;
		case MGN_MCS15:	ret = DESC90_RATEMCS15;	break;
		case (0x80|0x20): ret = DESC90_RATEMCS32; break;

		default:       break;
	}
	return ret;
}

void  rtl8192_tx_fill_desc(struct net_device* dev, tx_desc * pdesc, cb_desc * cb_desc, struct sk_buff* skb)
{
    struct r8192_priv *priv = rtllib_priv(dev);
    dma_addr_t mapping = pci_map_single(priv->pdev, skb->data, skb->len, PCI_DMA_TODEVICE);
    TX_FWINFO_8190PCI *pTxFwInfo = NULL;
	 // fill tx firmware */
    pTxFwInfo = (PTX_FWINFO_8190PCI)skb->data;
    memset(pTxFwInfo,0,sizeof(TX_FWINFO_8190PCI));
    pTxFwInfo->TxHT = (cb_desc->data_rate&0x80)?1:0;
    pTxFwInfo->TxRate = MRateToHwRate8190Pci((u8)cb_desc->data_rate);
    pTxFwInfo->EnableCPUDur = cb_desc->bTxEnableFwCalcDur;
    pTxFwInfo->Short	= QueryIsShort(pTxFwInfo->TxHT, pTxFwInfo->TxRate, cb_desc);

    // Aggregation related */
    if(cb_desc->bAMPDUEnable) {
        pTxFwInfo->AllowAggregation = 1;
        pTxFwInfo->RxMF = cb_desc->ampdu_factor;
        pTxFwInfo->RxAMD = cb_desc->ampdu_density;
    } else {
        pTxFwInfo->AllowAggregation = 0;
        pTxFwInfo->RxMF = 0;
        pTxFwInfo->RxAMD = 0;
    }

    //
    // Protection mode related
    //
    pTxFwInfo->RtsEnable =	(cb_desc->bRTSEnable)?1:0;
    pTxFwInfo->CtsEnable =	(cb_desc->bCTSEnable)?1:0;
    pTxFwInfo->RtsSTBC =	(cb_desc->bRTSSTBC)?1:0;
    pTxFwInfo->RtsHT=		(cb_desc->rts_rate&0x80)?1:0;
    pTxFwInfo->RtsRate =		MRateToHwRate8190Pci((u8)cb_desc->rts_rate);
    pTxFwInfo->RtsBandwidth = 0;
    pTxFwInfo->RtsSubcarrier = cb_desc->RTSSC;
    pTxFwInfo->RtsShort =	(pTxFwInfo->RtsHT==0)?(cb_desc->bRTSUseShortPreamble?1:0):(cb_desc->bRTSUseShortGI?1:0);
    //
    // Set Bandwidth and sub-channel settings.
    //
    if(priv->CurrentChannelBW == HT_CHANNEL_WIDTH_20_40)
    {
        if(cb_desc->bPacketBW) {
            pTxFwInfo->TxBandwidth = 1;
            pTxFwInfo->TxSubCarrier = 3;
        } else {
            pTxFwInfo->TxBandwidth = 0;
            pTxFwInfo->TxSubCarrier = priv->nCur40MhzPrimeSC;
        }
    } else {
        pTxFwInfo->TxBandwidth = 0;
        pTxFwInfo->TxSubCarrier = 0;
    }

    // fill tx descriptor */
    memset((u8*)pdesc,0,12);
    //DWORD 0*/
    pdesc->LINIP = 0;
    pdesc->CmdInit = 1;
    pdesc->Offset = sizeof(TX_FWINFO_8190PCI) + 8; //We must add 8!! Emily
    pdesc->PktSize = (u16)skb->len-sizeof(TX_FWINFO_8190PCI);

    //DWORD 1*/
    pdesc->SecCAMID= 0;
    pdesc->RATid = cb_desc->RATRIndex;


    pdesc->NoEnc = 1;
    pdesc->SecType = 0x0;
    if (cb_desc->bHwSec) {
        static u8 tmp =0;
        if (!tmp) {
            printk("==>================hw sec\n");
            tmp = 1;
        }
        switch (priv->rtllib->pairwise_key_type) {
            case KEY_TYPE_WEP40:
            case KEY_TYPE_WEP104:
                pdesc->SecType = 0x1;
                pdesc->NoEnc = 0;
                break;
            case KEY_TYPE_TKIP:
                pdesc->SecType = 0x2;
                pdesc->NoEnc = 0;
                break;
            case KEY_TYPE_CCMP:
                pdesc->SecType = 0x3;
                pdesc->NoEnc = 0;
                break;
            case KEY_TYPE_NA:
                pdesc->SecType = 0x0;
                pdesc->NoEnc = 1;
                break;
        }
    }

    //
    // Set Packet ID
    //
    pdesc->PktId = 0x0;

    pdesc->QueueSelect = MapHwQueueToFirmwareQueue(cb_desc->queue_index, cb_desc->priority);
    pdesc->TxFWInfoSize = sizeof(TX_FWINFO_8190PCI);

    pdesc->DISFB = cb_desc->bTxDisableRateFallBack;
    pdesc->USERATE = cb_desc->bTxUseDriverAssingedRate;

    pdesc->FirstSeg =1;
    pdesc->LastSeg = 1;
    pdesc->TxBufferSize = skb->len;

    pdesc->TxBuffAddr = cpu_to_le32(mapping);
}

void  rtl8192_tx_fill_cmd_desc(struct net_device* dev, tx_desc_cmd * entry,
		cb_desc * cb_desc, struct sk_buff* skb)
{
    struct r8192_priv *priv = rtllib_priv(dev);
    dma_addr_t mapping = pci_map_single(priv->pdev, skb->data, skb->len, PCI_DMA_TODEVICE);

    memset(entry,0,12);
    entry->LINIP = cb_desc->bLastIniPkt;
    entry->FirstSeg = 1;//first segment
    entry->LastSeg = 1; //last segment
    if(cb_desc->bCmdOrInit == DESC_PACKET_TYPE_INIT) {
        entry->CmdInit = DESC_PACKET_TYPE_INIT;
    } else {
	tx_desc* entry_tmp = (tx_desc*)entry;
        entry_tmp->CmdInit = DESC_PACKET_TYPE_NORMAL;
        entry_tmp->Offset = sizeof(TX_FWINFO_8190PCI) + 8;
        entry_tmp->PktSize = (u16)(cb_desc->pkt_size + entry_tmp->Offset);
        entry_tmp->QueueSelect = QSLT_CMD;
        entry_tmp->TxFWInfoSize = 0x08;
        entry_tmp->RATid = (u8)DESC_PACKET_TYPE_INIT;
    }
    entry->TxBufferSize = skb->len;
    entry->TxBuffAddr = cpu_to_le32(mapping);
    entry->OWN = 1;
}

u8 HwRateToMRate90(bool bIsHT, u8 rate)
{
	u8  ret_rate = 0x02;

	if(!bIsHT) {
		switch(rate) {
			case DESC90_RATE1M:   ret_rate = MGN_1M;         break;
			case DESC90_RATE2M:   ret_rate = MGN_2M;         break;
			case DESC90_RATE5_5M: ret_rate = MGN_5_5M;       break;
			case DESC90_RATE11M:  ret_rate = MGN_11M;        break;
			case DESC90_RATE6M:   ret_rate = MGN_6M;         break;
			case DESC90_RATE9M:   ret_rate = MGN_9M;         break;
			case DESC90_RATE12M:  ret_rate = MGN_12M;        break;
			case DESC90_RATE18M:  ret_rate = MGN_18M;        break;
			case DESC90_RATE24M:  ret_rate = MGN_24M;        break;
			case DESC90_RATE36M:  ret_rate = MGN_36M;        break;
			case DESC90_RATE48M:  ret_rate = MGN_48M;        break;
			case DESC90_RATE54M:  ret_rate = MGN_54M;        break;

			default:
					      RT_TRACE(COMP_RECV, "HwRateToMRate90(): Non supported Rate [%x], bIsHT = %d!!!\n", rate, bIsHT);
					      break;
		}

	} else {
		switch(rate) {
			case DESC90_RATEMCS0:   ret_rate = MGN_MCS0;    break;
			case DESC90_RATEMCS1:   ret_rate = MGN_MCS1;    break;
			case DESC90_RATEMCS2:   ret_rate = MGN_MCS2;    break;
			case DESC90_RATEMCS3:   ret_rate = MGN_MCS3;    break;
			case DESC90_RATEMCS4:   ret_rate = MGN_MCS4;    break;
			case DESC90_RATEMCS5:   ret_rate = MGN_MCS5;    break;
			case DESC90_RATEMCS6:   ret_rate = MGN_MCS6;    break;
			case DESC90_RATEMCS7:   ret_rate = MGN_MCS7;    break;
			case DESC90_RATEMCS8:   ret_rate = MGN_MCS8;    break;
			case DESC90_RATEMCS9:   ret_rate = MGN_MCS9;    break;
			case DESC90_RATEMCS10:  ret_rate = MGN_MCS10;   break;
			case DESC90_RATEMCS11:  ret_rate = MGN_MCS11;   break;
			case DESC90_RATEMCS12:  ret_rate = MGN_MCS12;   break;
			case DESC90_RATEMCS13:  ret_rate = MGN_MCS13;   break;
			case DESC90_RATEMCS14:  ret_rate = MGN_MCS14;   break;
			case DESC90_RATEMCS15:  ret_rate = MGN_MCS15;   break;
			case DESC90_RATEMCS32:  ret_rate = (0x80|0x20); break;

			default:
						RT_TRACE(COMP_RECV, "HwRateToMRate90(): Non supported Rate [%x], bIsHT = %d!!!\n",rate, bIsHT);
						break;
		}
	}

	return ret_rate;
}

bool rtl8192_rx_query_status_desc(struct net_device* dev, struct rtllib_rx_stats*  stats,
		rx_desc *pdesc, struct sk_buff* skb)
{
	struct r8192_priv *priv = rtllib_priv(dev);

	stats->bICV = pdesc->ICV;
	stats->bCRC = pdesc->CRC32;
	stats->bHwError = pdesc->CRC32 | pdesc->ICV;

	stats->Length = pdesc->Length;
	if(stats->Length < 24)
		stats->bHwError |= 1;

	if (stats->bHwError) {
		stats->bShift = false;

		if (pdesc->CRC32) {
			if (pdesc->Length <500)
				priv->stats.rxcrcerrmin++;
			else if (pdesc->Length >1000)
				priv->stats.rxcrcerrmax++;
			else
				priv->stats.rxcrcerrmid++;
		}
		return false;
	} else {
		prx_fwinfo pDrvInfo = NULL;
		stats->RxDrvInfoSize = pdesc->RxDrvInfoSize;
		stats->RxBufShift = ((pdesc->Shift)&0x03);
		stats->Decrypted = !pdesc->SWDec;

		pDrvInfo = (rx_fwinfo *)(skb->data + stats->RxBufShift);

		stats->rate = HwRateToMRate90((bool)pDrvInfo->RxHT, (u8)pDrvInfo->RxRate);
		stats->bShortPreamble = pDrvInfo->SPLCP;

		// it is debug only. It should be disabled in released driver.
		// 2007.1.11 by Emily
		//
		UpdateReceivedRateHistogramStatistics8190(dev, stats);

		stats->bIsAMPDU = (pDrvInfo->PartAggr==1);
		stats->bFirstMPDU = (pDrvInfo->PartAggr==1) && (pDrvInfo->FirstAGGR==1);

		stats->TimeStampLow = pDrvInfo->TSFL;
		stats->TimeStampHigh = read_nic_dword(dev, TSFR+4);

		UpdateRxPktTimeStamp8190(dev, stats);

		//
		// Get Total offset of MPDU Frame Body
		//
		if((stats->RxBufShift + stats->RxDrvInfoSize) > 0)
			stats->bShift = 1;

		stats->RxIs40MHzPacket = pDrvInfo->BW;

		TranslateRxSignalStuff819xpci(dev,skb, stats, pdesc, pDrvInfo);

		// Rx A-MPDU */
		if(pDrvInfo->FirstAGGR==1 || pDrvInfo->PartAggr == 1)
			RT_TRACE(COMP_RXDESC, "pDrvInfo->FirstAGGR = %d, pDrvInfo->PartAggr = %d\n",
					pDrvInfo->FirstAGGR, pDrvInfo->PartAggr);
		skb_trim(skb, skb->len - 4/*sCrcLng*/);


		stats->packetlength = stats->Length-4;
		stats->fraglength = stats->packetlength;
		stats->fragoffset = 0;
		stats->ntotalfrag = 1;
		return true;
	}
}

void rtl8192_halt_adapter(struct net_device *dev, bool reset)
{
	u8 cmd;
	struct r8192_priv *priv = rtllib_priv(dev);
	int i;

	cmd=read_nic_byte(dev,CMDR);
	write_nic_byte(dev, CMDR, cmd &~ \
			(CR_TE|CR_RE));
	mdelay(30);

	for(i = 0; i < MAX_QUEUE_SIZE; i++) {
		skb_queue_purge(&priv->rtllib->skb_waitQ [i]);
	}
	for(i = 0; i < MAX_QUEUE_SIZE; i++) {
		skb_queue_purge(&priv->rtllib->skb_aggQ [i]);
	}
	skb_queue_purge(&priv->skb_queue);
	return;
}

//updateRATRTabel for MCS only. Basic rate is not implement.
void rtl8192_update_ratr_table(struct net_device* dev)
{
	struct r8192_priv* priv = rtllib_priv(dev);
	struct rtllib_device* ieee = priv->rtllib;
	u8* pMcsRate = ieee->dot11HTOperationalRateSet;
	//struct rtllib_network *net = &ieee->current_network;
	u32 ratr_value = 0;
	u8 rate_index = 0;

	rtl8192_config_rate(dev, (u16*)(&ratr_value));
	ratr_value |= (*(u16*)(pMcsRate)) << 12;
	switch (ieee->mode)
	{
		case IEEE_A:
			ratr_value &= 0x00000FF0;
			break;
		case IEEE_B:
			ratr_value &= 0x0000000F;
			break;
		case IEEE_G:
			ratr_value &= 0x00000FF7;
			break;
		case IEEE_N_24G:
		case IEEE_N_5G:
			if (ieee->pHTInfo->PeerMimoPs == 0) //MIMO_PS_STATIC
				ratr_value &= 0x0007F007;
			else{
				if (priv->rf_type == RF_1T2R)
					ratr_value &= 0x000FF007;
				else
					ratr_value &= 0x0F81F007;
			}
			break;
		default:
			break;
	}
	ratr_value &= 0x0FFFFFFF;
	if(ieee->pHTInfo->bCurTxBW40MHz && ieee->pHTInfo->bCurShortGI40MHz){
		ratr_value |= 0x80000000;
	}else if(!ieee->pHTInfo->bCurTxBW40MHz && ieee->pHTInfo->bCurShortGI20MHz){
		ratr_value |= 0x80000000;
	}
	write_nic_dword(dev, RATR0+rate_index*4, ratr_value);
	write_nic_byte(dev, UFWP, 1);
}
