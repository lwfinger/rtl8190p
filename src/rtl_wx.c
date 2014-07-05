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

#include <linux/string.h>
#include <linux/interrupt.h>
#include "rtl_core.h"
#include "dot11d.h"

#ifdef CONFIG_MP
#include "r8192S_mp.h"
#endif

#define RATE_COUNT 12
u32 rtl8192_rates[] = {1000000, 2000000, 5500000, 11000000,
	6000000, 9000000, 12000000, 18000000, 24000000, 36000000, 48000000, 54000000};


#ifndef ENETDOWN
#define ENETDOWN 1
#endif
extern int  hwwep;

static int r8192_wx_get_freq(struct net_device *dev,
			     struct iw_request_info *a,
			     union iwreq_data *wrqu, char *b)
{
	struct r8192_priv *priv = rtllib_priv(dev);

	return rtllib_wx_get_freq(priv->rtllib, a, wrqu, b);
}

static int r8192_wx_get_mode(struct net_device *dev, struct iw_request_info *a,
			     union iwreq_data *wrqu, char *b)
{
	struct r8192_priv *priv = rtllib_priv(dev);

	return rtllib_wx_get_mode(priv->rtllib, a, wrqu, b);
}

static int r8192_wx_get_rate(struct net_device *dev,
			     struct iw_request_info *info,
			     union iwreq_data *wrqu, char *extra)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	return rtllib_wx_get_rate(priv->rtllib, info, wrqu, extra);
}



static int r8192_wx_set_rate(struct net_device *dev,
			     struct iw_request_info *info,
			     union iwreq_data *wrqu, char *extra)
{
	int ret;
	struct r8192_priv *priv = rtllib_priv(dev);

	if (priv->bHwRadioOff == true)
		return 0;

	down(&priv->wx_sem);

	ret = rtllib_wx_set_rate(priv->rtllib, info, wrqu, extra);

	up(&priv->wx_sem);

	return ret;
}


static int r8192_wx_set_rts(struct net_device *dev,
			     struct iw_request_info *info,
			     union iwreq_data *wrqu, char *extra)
{
	int ret;
	struct r8192_priv *priv = rtllib_priv(dev);

	if (priv->bHwRadioOff == true)
		return 0;

	down(&priv->wx_sem);

	ret = rtllib_wx_set_rts(priv->rtllib, info, wrqu, extra);

	up(&priv->wx_sem);

	return ret;
}

static int r8192_wx_get_rts(struct net_device *dev,
			     struct iw_request_info *info,
			     union iwreq_data *wrqu, char *extra)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	return rtllib_wx_get_rts(priv->rtllib, info, wrqu, extra);
}

static int r8192_wx_set_power(struct net_device *dev,
			     struct iw_request_info *info,
			     union iwreq_data *wrqu, char *extra)
{
	int ret;
	struct r8192_priv *priv = rtllib_priv(dev);

	if (priv->bHwRadioOff == true){
		RT_TRACE(COMP_ERR,"%s():Hw is Radio Off, we can't set Power, return\n", __FUNCTION__);
		return 0;
	}
	down(&priv->wx_sem);

	ret = rtllib_wx_set_power(priv->rtllib, info, wrqu, extra);

	up(&priv->wx_sem);

	return ret;
}

static int r8192_wx_get_power(struct net_device *dev,
			     struct iw_request_info *info,
			     union iwreq_data *wrqu, char *extra)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	return rtllib_wx_get_power(priv->rtllib, info, wrqu, extra);
}

static int r8192_wx_set_rawtx(struct net_device *dev,
			       struct iw_request_info *info,
			       union iwreq_data *wrqu, char *extra)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	int ret;

	if (priv->bHwRadioOff == true)
		return 0;

	down(&priv->wx_sem);

	ret = rtllib_wx_set_rawtx(priv->rtllib, info, wrqu, extra);

	up(&priv->wx_sem);

	return ret;

}

static int r8192_wx_force_reset(struct net_device *dev,
		struct iw_request_info *info,
		union iwreq_data *wrqu, char *extra)
{
	struct r8192_priv *priv = rtllib_priv(dev);

	down(&priv->wx_sem);

	printk("%s(): force reset ! extra is %d\n", __FUNCTION__, *extra);
	priv->force_reset = *extra;
	up(&priv->wx_sem);
	return 0;

}

static int r8192_wx_force_mic_error(struct net_device *dev,
		struct iw_request_info *info,
		union iwreq_data *wrqu, char *extra)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	struct rtllib_device* ieee = priv->rtllib;

	down(&priv->wx_sem);

	printk("%s(): force mic error ! \n", __FUNCTION__);
	ieee->force_mic_error = true;
	up(&priv->wx_sem);
	return 0;

}

#define MAX_ADHOC_PEER_NUM 64 //YJ, modified, 090304
typedef struct
{
	unsigned char MacAddr[ETH_ALEN];
	unsigned char WirelessMode;
	unsigned char bCurTxBW40MHz;
} adhoc_peer_entry_t, *p_adhoc_peer_entry_t;
typedef struct
{
	adhoc_peer_entry_t Entry[MAX_ADHOC_PEER_NUM];
	unsigned char num;
} adhoc_peers_info_t, *p_adhoc_peers_info_t;
int r8192_wx_get_adhoc_peers(struct net_device *dev,
			       struct iw_request_info *info,
			       union iwreq_data *wrqu, char *extra)
{
	return 0;
}


static int r8191se_wx_get_firm_version(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_param *wrqu, char *extra)
{
	return 0;
}

// check ac/dc status with the help of user space application */
static int r8192_wx_adapter_power_status(struct net_device *dev,
		struct iw_request_info *info,
		union iwreq_data *wrqu, char *extra)
{
	struct r8192_priv *priv = rtllib_priv(dev);
#ifdef ENABLE_LPS
	PRT_POWER_SAVE_CONTROL pPSC = (PRT_POWER_SAVE_CONTROL)(&(priv->rtllib->PowerSaveControl));
	struct rtllib_device* ieee = priv->rtllib;
#endif
	down(&priv->wx_sem);

#ifdef ENABLE_LPS
	RT_TRACE(COMP_POWER, "%s(): %s\n", __FUNCTION__, (*extra ==  6)?"DC power":"AC power");
    // ieee->ps shall not be set under DC mode, otherwise it conflict
    // with Leisure power save mode setting.
    //
	if (*extra || priv->force_lps) {
		priv->ps_force = false;
		pPSC->bLeisurePs = true;
	} else {
		priv->ps_force = true;
		pPSC->bLeisurePs = false;
		ieee->ps = *extra;
	}

#endif
	up(&priv->wx_sem);
	return 0;

}

static int r8192se_wx_set_radio(struct net_device *dev,
        struct iw_request_info *info,
        union iwreq_data *wrqu, char *extra)
{
    struct r8192_priv *priv = rtllib_priv(dev);

    down(&priv->wx_sem);

    printk("%s(): set radio ! extra is %d\n", __FUNCTION__, *extra);
    if ((*extra != 0) && (*extra != 1))
    {
        RT_TRACE(COMP_ERR, "%s(): set radio an err value, must 0(radio off) or 1(radio on)\n", __FUNCTION__);
        return -1;
    }
    priv->sw_radio_on = *extra;
    up(&priv->wx_sem);
    return 0;

}

static int r8192se_wx_set_lps_awake_interval(struct net_device *dev,
        struct iw_request_info *info,
        union iwreq_data *wrqu, char *extra)
{
    struct r8192_priv *priv = rtllib_priv(dev);
    PRT_POWER_SAVE_CONTROL	pPSC = (PRT_POWER_SAVE_CONTROL)(&(priv->rtllib->PowerSaveControl));

    down(&priv->wx_sem);

    printk("%s(): set lps awake interval ! extra is %d\n", __FUNCTION__, *extra);

    pPSC->RegMaxLPSAwakeIntvl = *extra;
    up(&priv->wx_sem);
    return 0;

}

static int r8192se_wx_set_force_lps(struct net_device *dev,
		struct iw_request_info *info,
		union iwreq_data *wrqu, char *extra)
{
	struct r8192_priv *priv = rtllib_priv(dev);

	down(&priv->wx_sem);

	printk("%s(): force LPS ! extra is %d (1 is open 0 is close)\n", __FUNCTION__, *extra);
	priv->force_lps = *extra;
	up(&priv->wx_sem);
	return 0;

}

static int r8192_wx_set_debugflag(struct net_device *dev,
			       struct iw_request_info *info,
			       union iwreq_data *wrqu, char *extra)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	u8 c = *extra;

	if (priv->bHwRadioOff == true)
		return 0;

	printk("=====>%s(), *extra:%x, debugflag:%x\n", __FUNCTION__, *extra, rt_global_debug_component);
	if (c > 0)  {
		rt_global_debug_component |= (1<<c);
	} else {
		rt_global_debug_component &= BIT31; //use zero to clear debug flag except for ERR.
	}
	return 0;
}

static int r8192_wx_set_mode(struct net_device *dev, struct iw_request_info *a,
			     union iwreq_data *wrqu, char *b)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	RT_RF_POWER_STATE	rtState;
	int ret;

	if (priv->bHwRadioOff == true)
		return 0;
	rtState = priv->rtllib->eRFPowerState;
	down(&priv->wx_sem);
#ifdef ENABLE_IPS
	if (wrqu->mode == IW_MODE_ADHOC){
		if (priv->rtllib->PowerSaveControl.bInactivePs){
			if (rtState == eRfOff){
				if (priv->rtllib->RfOffReason > RF_CHANGE_BY_IPS)
				{
					RT_TRACE(COMP_ERR, "%s(): RF is OFF.\n", __FUNCTION__);
					up(&priv->wx_sem);
					return -1;
				} else {
					printk("=========>%s(): IPSLeave\n", __FUNCTION__);
					down(&priv->rtllib->ips_sem);
					IPSLeave(dev);
					up(&priv->rtllib->ips_sem);
				}
			}
		}
	}
#endif
	ret = rtllib_wx_set_mode(priv->rtllib, a, wrqu, b);

	up(&priv->wx_sem);
	return ret;
}

struct  iw_range_with_scan_capa
{
        /* Informative stuff (to choose between different interface) */
        __u32           throughput;     /* To give an idea... */
        /* In theory this value should be the maximum benchmarked
         * TCP/IP throughput, because with most of these devices the
         * bit rate is meaningless (overhead an co) to estimate how
         * fast the connection will go and pick the fastest one.
         * I suggest people to play with Netperf or any benchmark...
         */

        /* NWID (or domain id) */
        __u32           min_nwid;       /* Minimal NWID we are able to set */
        __u32           max_nwid;       /* Maximal NWID we are able to set */

        /* Old Frequency (backward compat - moved lower ) */
        __u16           old_num_channels;
        __u8            old_num_frequency;

        /* Scan capabilities */
        __u8            scan_capa;
};

static int rtl8192_wx_get_range(struct net_device *dev,
				struct iw_request_info *info,
				union iwreq_data *wrqu, char *extra)
{
	struct iw_range *range = (struct iw_range *)extra;
//	struct iw_range_with_scan_capa* tmp = (struct iw_range_with_scan_capa*)range;
	struct r8192_priv *priv = rtllib_priv(dev);
	u16 val;
	int i;

	wrqu->data.length = sizeof(*range);
	memset(range, 0, sizeof(*range));

	/* ~130 Mb/s real (802.11n) */
	range->throughput = 130 * 1000 * 1000;

	if (priv->rf_set_sens != NULL)
		range->sensitivity = priv->max_sens;	/* signal level threshold range */

	range->max_qual.qual = 100;
	// TODO: Find real max RSSI and stick here */
	range->max_qual.level = 0;
	range->max_qual.noise = 0;
	range->max_qual.updated = 7; /* Updated all three */

	range->avg_qual.qual = 70; /* > 8% missed beacons is 'bad' */
	// TODO: Find real 'good' to 'bad' threshol value for RSSI */
	range->avg_qual.level = 0;
	range->avg_qual.noise = 0;
	range->avg_qual.updated = 7; /* Updated all three */

	range->num_bitrates = min(RATE_COUNT, IW_MAX_BITRATES);

	for (i = 0; i < range->num_bitrates; i++) {
		range->bitrate[i] = rtl8192_rates[i];
	}

	range->max_rts = DEFAULT_RTS_THRESHOLD;
	range->min_frag = MIN_FRAG_THRESHOLD;
	range->max_frag = MAX_FRAG_THRESHOLD;

	range->min_pmp = 0;
	range->max_pmp = 5000000;
	range->min_pmt = 0;
	range->max_pmt = 65535*1000;
	range->pmp_flags = IW_POWER_PERIOD;
	range->pmt_flags = IW_POWER_TIMEOUT;
	range->pm_capa = IW_POWER_PERIOD | IW_POWER_TIMEOUT | IW_POWER_ALL_R;
	range->we_version_compiled = WIRELESS_EXT;
	range->we_version_source = 18;

	for (i = 0, val = 0; i < 14; i++) {
		// Include only legal frequencies for some countries
		if ((GET_DOT11D_INFO(priv->rtllib)->channel_map)[i+1]) {
		        range->freq[val].i = i + 1;
			range->freq[val].m = rtllib_wlan_frequencies[i] * 100000;
			range->freq[val].e = 1;
			val++;
		} else {
			// FIXME: do we need to set anything for channels
			// we don't use ?
		}

		if (val == IW_MAX_FREQUENCIES)
		break;
	}
	range->num_frequency = val;
	range->num_channels = val;
#if WIRELESS_EXT > 17
	range->enc_capa = IW_ENC_CAPA_WPA|IW_ENC_CAPA_WPA2|
			  IW_ENC_CAPA_CIPHER_TKIP|IW_ENC_CAPA_CIPHER_CCMP;
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 25)//IW_SCAN_CAPA_ESSID&IW_SCAN_CAPA_TYPE begain from 2.6.25
	{
		struct iw_range_with_scan_capa* tmp = (struct iw_range_with_scan_capa*)range;
		tmp->scan_capa = 0x01;
	}
#else
	range->scan_capa = IW_SCAN_CAPA_ESSID | IW_SCAN_CAPA_TYPE;
#endif
	return 0;
}

static int r8192_wx_set_scan(struct net_device *dev, struct iw_request_info *a,
			     union iwreq_data *wrqu, char *b)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	struct rtllib_device* ieee = priv->rtllib;
	RT_RF_POWER_STATE	rtState;
	int ret;

#ifdef CONFIG_MP
	printk("######################%s(): In MP Test Can not Scan\n", __FUNCTION__);
	return 0;
#endif

	if (priv->bHwRadioOff == true){
		printk("================>%s(): hwradio off\n", __FUNCTION__);
		return 0;
	}
	rtState = priv->rtllib->eRFPowerState;
	if (!priv->up) return -ENETDOWN;
	if (priv->rtllib->LinkDetectInfo.bBusyTraffic == true)
		return -EAGAIN;

#if WIRELESS_EXT > 17
	if (wrqu->data.flags & IW_SCAN_THIS_ESSID)
	{
		struct iw_scan_req* req = (struct iw_scan_req*)b;
		if (req->essid_len)
		{
			//printk("==**&*&*&**===>scan set ssid:%s\n", req->essid);
			ieee->current_network.ssid_len = req->essid_len;
			memcpy(ieee->current_network.ssid, req->essid, req->essid_len);
			//printk("=====>network ssid:%s\n", ieee->current_network.ssid);
		}
	}
#endif
	down(&priv->wx_sem);
#ifdef ENABLE_IPS
	priv->rtllib->actscanning = true;
	if (priv->rtllib->state != RTLLIB_LINKED){
		if (priv->rtllib->PowerSaveControl.bInactivePs){
			if (rtState == eRfOff){
				if (priv->rtllib->RfOffReason > RF_CHANGE_BY_IPS)
				{
					RT_TRACE(COMP_ERR, "%s(): RF is OFF.\n", __FUNCTION__);
					up(&priv->wx_sem);
					return -1;
				}
				else {
					printk("=========>%s(): IPSLeave\n", __FUNCTION__);
					down(&priv->rtllib->ips_sem);
					IPSLeave(dev);
					up(&priv->rtllib->ips_sem);				}
			}
		}
		priv->rtllib->scanning = 0;
		if (priv->rtllib->LedControlHandler)
			priv->rtllib->LedControlHandler(dev, LED_CTL_SITE_SURVEY);
                if (priv->rtllib->eRFPowerState != eRfOff){
			priv->rtllib->sync_scan_hurryup = 0;
			rtllib_softmac_scan_syncro(priv->rtllib);
                }
		ret = 0;
	}
	else
#else

	if (priv->rtllib->state != RTLLIB_LINKED){
		priv->rtllib->scanning = 0;
		if (priv->rtllib->LedControlHandler)
			priv->rtllib->LedControlHandler(dev, LED_CTL_SITE_SURVEY);
		priv->rtllib->sync_scan_hurryup = 0;
		rtllib_softmac_scan_syncro(priv->rtllib);
		ret = 0;
	}
	else
#endif
	ret = rtllib_wx_set_scan(priv->rtllib, a, wrqu, b);

	up(&priv->wx_sem);
	return ret;
}


static int r8192_wx_get_scan(struct net_device *dev, struct iw_request_info *a,
			     union iwreq_data *wrqu, char *b)
{

	int ret;
	struct r8192_priv *priv = rtllib_priv(dev);

	if (!priv->up) return -ENETDOWN;

	if (priv->bHwRadioOff == true)
		return 0;


	down(&priv->wx_sem);

	ret = rtllib_wx_get_scan(priv->rtllib, a, wrqu, b);

	up(&priv->wx_sem);

	return ret;
}

static int r8192_wx_set_essid(struct net_device *dev,
			      struct iw_request_info *a,
			      union iwreq_data *wrqu, char *b)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	int ret;

#ifdef CONFIG_MP
	printk("######################%s(): In MP Test Can not Set Essid\n", __FUNCTION__);
	return 0;
#endif
	if (priv->bHwRadioOff == true){
		printk("=========>%s():hw radio off, or Rf state is eRfOff, return\n", __FUNCTION__);
		return 0;
	}
	down(&priv->wx_sem);
	ret = rtllib_wx_set_essid(priv->rtllib, a, wrqu, b);

	up(&priv->wx_sem);

	return ret;
}

static int r8192_wx_get_essid(struct net_device *dev,
			      struct iw_request_info *a,
			      union iwreq_data *wrqu, char *b)
{
	int ret;
	struct r8192_priv *priv = rtllib_priv(dev);

	down(&priv->wx_sem);

	ret = rtllib_wx_get_essid(priv->rtllib, a, wrqu, b);

	up(&priv->wx_sem);

	return ret;
}

static int r8192_wx_set_nick(struct net_device *dev,
			   struct iw_request_info *info,
			   union iwreq_data *wrqu, char *extra)
{
	struct r8192_priv *priv = rtllib_priv(dev);

	if (wrqu->data.length > IW_ESSID_MAX_SIZE)
		return -E2BIG;
	down(&priv->wx_sem);
	wrqu->data.length = min((size_t) wrqu->data.length, sizeof(priv->nick));
	memset(priv->nick, 0, sizeof(priv->nick));
	memcpy(priv->nick, extra, wrqu->data.length);
	up(&priv->wx_sem);
	return 0;

}

static int r8192_wx_get_nick(struct net_device *dev,
			     struct iw_request_info *info,
			     union iwreq_data *wrqu, char *extra)
{
	struct r8192_priv *priv = rtllib_priv(dev);

	down(&priv->wx_sem);
	wrqu->data.length = strlen(priv->nick);
	memcpy(extra, priv->nick, wrqu->data.length);
	wrqu->data.flags = 1;   /* active */
	up(&priv->wx_sem);
	return 0;
}

static int r8192_wx_set_freq(struct net_device *dev, struct iw_request_info *a,
			     union iwreq_data *wrqu, char *b)
{
	int ret;
	struct r8192_priv *priv = rtllib_priv(dev);

	if (priv->bHwRadioOff == true)
		return 0;

	down(&priv->wx_sem);

	ret = rtllib_wx_set_freq(priv->rtllib, a, wrqu, b);

	up(&priv->wx_sem);
	return ret;
}

static int r8192_wx_get_name(struct net_device *dev,
			     struct iw_request_info *info,
			     union iwreq_data *wrqu, char *extra)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	return rtllib_wx_get_name(priv->rtllib, info, wrqu, extra);
}


static int r8192_wx_set_frag(struct net_device *dev,
			     struct iw_request_info *info,
			     union iwreq_data *wrqu, char *extra)
{
	struct r8192_priv *priv = rtllib_priv(dev);

	if (priv->bHwRadioOff == true)
		return 0;

	if (wrqu->frag.disabled)
		priv->rtllib->fts = DEFAULT_FRAG_THRESHOLD;
	else {
		if (wrqu->frag.value < MIN_FRAG_THRESHOLD ||
		    wrqu->frag.value > MAX_FRAG_THRESHOLD)
			return -EINVAL;

		priv->rtllib->fts = wrqu->frag.value & ~0x1;
	}

	return 0;
}


static int r8192_wx_get_frag(struct net_device *dev,
			     struct iw_request_info *info,
			     union iwreq_data *wrqu, char *extra)
{
	struct r8192_priv *priv = rtllib_priv(dev);

	wrqu->frag.value = priv->rtllib->fts;
	wrqu->frag.fixed = 0;	/* no auto select */
	wrqu->frag.disabled = (wrqu->frag.value == DEFAULT_FRAG_THRESHOLD);

	return 0;
}


static int r8192_wx_set_wap(struct net_device *dev,
			 struct iw_request_info *info,
			 union iwreq_data *awrq,
			 char *extra)
{

	int ret;
	struct r8192_priv *priv = rtllib_priv(dev);
//        struct sockaddr *temp = (struct sockaddr *)awrq;

	if (priv->bHwRadioOff == true)
		return 0;

	down(&priv->wx_sem);

	ret = rtllib_wx_set_wap(priv->rtllib, info, awrq, extra);

	up(&priv->wx_sem);

	return ret;

}


static int r8192_wx_get_wap(struct net_device *dev,
			    struct iw_request_info *info,
			    union iwreq_data *wrqu, char *extra)
{
	struct r8192_priv *priv = rtllib_priv(dev);

	return rtllib_wx_get_wap(priv->rtllib, info, wrqu, extra);
}


static int r8192_wx_get_enc(struct net_device *dev,
			    struct iw_request_info *info,
			    union iwreq_data *wrqu, char *key)
{
	struct r8192_priv *priv = rtllib_priv(dev);

	return rtllib_wx_get_encode(priv->rtllib, info, wrqu, key);
}

static int r8192_wx_set_enc(struct net_device *dev,
			    struct iw_request_info *info,
			    union iwreq_data *wrqu, char *key)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	int ret;

	struct rtllib_device *ieee = priv->rtllib;
	//u32 TargetContent;
	u32 hwkey[4]={0, 0, 0, 0};
	u8 mask = 0xff;
	u32 key_idx = 0;
	u8 zero_addr[4][6] ={	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
				{0x00, 0x00, 0x00, 0x00, 0x00, 0x01},
				{0x00, 0x00, 0x00, 0x00, 0x00, 0x02},
				{0x00, 0x00, 0x00, 0x00, 0x00, 0x03} };
	int i;

#ifdef CONFIG_MP
	printk("######################%s(): In MP Test Can not Set Enc\n", __FUNCTION__);
	return 0;
#endif
	if (priv->bHwRadioOff == true)
		return 0;

       if (!priv->up) return -ENETDOWN;

        priv->rtllib->wx_set_enc = 1;
#ifdef ENABLE_IPS
        down(&priv->rtllib->ips_sem);
        IPSLeave(dev);
        up(&priv->rtllib->ips_sem);
#endif
	down(&priv->wx_sem);

	RT_TRACE(COMP_SEC, "Setting SW wep key");
	ret = rtllib_wx_set_encode(priv->rtllib, info, wrqu, key);
	up(&priv->wx_sem);


	//sometimes, the length is zero while we do not type key value
	if (wrqu->encoding.flags & IW_ENCODE_DISABLED) {
		ieee->pairwise_key_type = ieee->group_key_type = KEY_TYPE_NA;
		CamResetAllEntry(dev);
		memset(priv->rtllib->swcamtable, 0, sizeof(SW_CAM_TABLE)*32);//added by amy for silent reset 090415
		goto end_hw_sec;
	}
	if (wrqu->encoding.length!= 0){

		for (i = 0 ; i<4 ; i++){
			hwkey[i] |=  key[4*i+0]&mask;
			if (i == 1&&(4*i+1) == wrqu->encoding.length) mask = 0x00;
			if (i == 3&&(4*i+1) == wrqu->encoding.length) mask = 0x00;
			hwkey[i] |= (key[4*i+1]&mask)<<8;
			hwkey[i] |= (key[4*i+2]&mask)<<16;
			hwkey[i] |= (key[4*i+3]&mask)<<24;
		}

		#define CONF_WEP40  0x4
		#define CONF_WEP104 0x14

		switch (wrqu->encoding.flags & IW_ENCODE_INDEX){
			case 0: key_idx = ieee->tx_keyidx; break;
			case 1:	key_idx = 0; break;
			case 2:	key_idx = 1; break;
			case 3:	key_idx = 2; break;
			case 4:	key_idx	= 3; break;
			default: break;
		}
		//printk("-------====>length:%d, key_idx:%d, flag:%x\n", wrqu->encoding.length, key_idx, wrqu->encoding.flags);
		if (wrqu->encoding.length == 0x5){
			ieee->pairwise_key_type = KEY_TYPE_WEP40;
			EnableHWSecurityConfig8192(dev);
			setKey( dev,
				key_idx,                //EntryNo
				key_idx,                //KeyIndex
				KEY_TYPE_WEP40,         //KeyType
				zero_addr[key_idx],
				0,                      //DefaultKey
				hwkey);                 //KeyContent

			set_swcam( dev,
				key_idx,                //EntryNo
				key_idx,                //KeyIndex
				KEY_TYPE_WEP40,         //KeyType
				zero_addr[key_idx],
				0,                      //DefaultKey
				hwkey);                 //KeyContent

		} else if (wrqu->encoding.length == 0xd){
			ieee->pairwise_key_type = KEY_TYPE_WEP104;
				EnableHWSecurityConfig8192(dev);
			setKey( dev,
				key_idx,                //EntryNo
				key_idx,                //KeyIndex
				KEY_TYPE_WEP104,        //KeyType
				zero_addr[key_idx],
				0,                      //DefaultKey
				hwkey);                 //KeyContent
			set_swcam( dev,
				key_idx,                //EntryNo
				key_idx,                //KeyIndex
				KEY_TYPE_WEP104,        //KeyType
				zero_addr[key_idx],
				0,                      //DefaultKey
				hwkey);                 //KeyContent
		}
		else printk("wrong type in WEP, not WEP40 and WEP104\n");
	}

end_hw_sec:
	priv->rtllib->wx_set_enc = 0;
	return ret;
}


static int r8192_wx_set_scan_type(struct net_device *dev, struct iw_request_info *aa, union
 iwreq_data *wrqu, char *p){

	struct r8192_priv *priv = rtllib_priv(dev);
	int *parms = (int*)p;
	int mode = parms[0];

	if (priv->bHwRadioOff == true)
		return 0;

	priv->rtllib->active_scan = mode;

	return 1;
}



#define R8192_MAX_RETRY 255
static int r8192_wx_set_retry(struct net_device *dev,
				struct iw_request_info *info,
				union iwreq_data *wrqu, char *extra)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	int err = 0;

	if (priv->bHwRadioOff == true)
		return 0;

	down(&priv->wx_sem);

	if (wrqu->retry.flags & IW_RETRY_LIFETIME ||
	    wrqu->retry.disabled){
		err = -EINVAL;
		goto exit;
	}
	if (!(wrqu->retry.flags & IW_RETRY_LIMIT)){
		err = -EINVAL;
		goto exit;
	}

	if (wrqu->retry.value > R8192_MAX_RETRY){
		err = -EINVAL;
		goto exit;
	}
	if (wrqu->retry.flags & IW_RETRY_MAX) {
		priv->retry_rts = wrqu->retry.value;
		DMESG("Setting retry for RTS/CTS data to %d", wrqu->retry.value);

	} else {
		priv->retry_data = wrqu->retry.value;
		DMESG("Setting retry for non RTS/CTS data to %d", wrqu->retry.value);
	}

	// FIXME !
	// We might try to write directly the TX config register
	// or to restart just the (R)TX process.
	// I'm unsure if whole reset is really needed
	//

	rtl8192_commit(dev);
	/*
	if (priv->up){
		rtl8180_halt_adapter(dev);
		rtl8180_rx_enable(dev);
		rtl8180_tx_enable(dev);

	}
	*/
exit:
	up(&priv->wx_sem);

	return err;
}

static int r8192_wx_get_retry(struct net_device *dev,
				struct iw_request_info *info,
				union iwreq_data *wrqu, char *extra)
{
	struct r8192_priv *priv = rtllib_priv(dev);


	wrqu->retry.disabled = 0; /* can't be disabled */

	if ((wrqu->retry.flags & IW_RETRY_TYPE) ==
	    IW_RETRY_LIFETIME)
		return -EINVAL;

	if (wrqu->retry.flags & IW_RETRY_MAX) {
		wrqu->retry.flags = IW_RETRY_LIMIT & IW_RETRY_MAX;
		wrqu->retry.value = priv->retry_rts;
	} else {
		wrqu->retry.flags = IW_RETRY_LIMIT & IW_RETRY_MIN;
		wrqu->retry.value = priv->retry_data;
	}
	//DMESG("returning %d", wrqu->retry.value);


	return 0;
}

static int r8192_wx_get_sens(struct net_device *dev,
				struct iw_request_info *info,
				union iwreq_data *wrqu, char *extra)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	if (priv->rf_set_sens == NULL)
		return -1; /* we have not this support for this radio */
	wrqu->sens.value = priv->sens;
	return 0;
}


static int r8192_wx_set_sens(struct net_device *dev,
				struct iw_request_info *info,
				union iwreq_data *wrqu, char *extra)
{

	struct r8192_priv *priv = rtllib_priv(dev);

	short err = 0;

	if (priv->bHwRadioOff == true)
		return 0;

	down(&priv->wx_sem);
	//DMESG("attempt to set sensivity to %ddb", wrqu->sens.value);
	if (priv->rf_set_sens == NULL) {
		err = -1; /* we have not this support for this radio */
		goto exit;
	}
	if (priv->rf_set_sens(dev, wrqu->sens.value) == 0)
		priv->sens = wrqu->sens.value;
	else
		err = -EINVAL;

exit:
	up(&priv->wx_sem);

	return err;
}

#if (WIRELESS_EXT >= 18)

static int r8192_wx_set_enc_ext(struct net_device *dev,
                                        struct iw_request_info *info,
                                        union iwreq_data *wrqu, char *extra)
{
	int ret = 0;

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 5, 0)
	struct r8192_priv *priv = rtllib_priv(dev);
	struct rtllib_device* ieee = priv->rtllib;

	if (priv->bHwRadioOff == true)
		return 0;

	down(&priv->wx_sem);

        priv->rtllib->wx_set_enc = 1;
#ifdef ENABLE_IPS
        down(&priv->rtllib->ips_sem);
        IPSLeave(dev);
        up(&priv->rtllib->ips_sem);
#endif

	ret = rtllib_wx_set_encode_ext(ieee, info, wrqu, extra);

	{
		u8 broadcast_addr[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
		u8 zero[6] = {0};
		u32 key[4] = {0};
		struct iw_encode_ext *ext = (struct iw_encode_ext *)extra;
		struct iw_point *encoding = &wrqu->encoding;
		u8 idx = 0, alg = 0, group = 0;
		if ((encoding->flags & IW_ENCODE_DISABLED) ||
		ext->alg == IW_ENCODE_ALG_NONE) //none is not allowed to use hwsec WB 2008.07.01
		{
			ieee->pairwise_key_type = ieee->group_key_type = KEY_TYPE_NA;
			CamResetAllEntry(dev);
			memset(priv->rtllib->swcamtable, 0, sizeof(SW_CAM_TABLE)*32);//added by amy for silent reset 090415
			goto end_hw_sec;
		}
		alg =  (ext->alg == IW_ENCODE_ALG_CCMP)?KEY_TYPE_CCMP:ext->alg; // as IW_ENCODE_ALG_CCMP is defined to be 3 and KEY_TYPE_CCMP is defined to 4;
		idx = encoding->flags & IW_ENCODE_INDEX;
		if (idx)
			idx --;
		group = ext->ext_flags & IW_ENCODE_EXT_GROUP_KEY;

		if ((!group) || (IW_MODE_ADHOC == ieee->iw_mode) || (alg ==  KEY_TYPE_WEP40))
		{
			if ((ext->key_len == 13) && (alg == KEY_TYPE_WEP40) )
				alg = KEY_TYPE_WEP104;
			ieee->pairwise_key_type = alg;
			EnableHWSecurityConfig8192(dev);
		}
		memcpy((u8*)key, ext->key, 16); //we only get 16 bytes key.why? WB 2008.7.1

		if ((alg & KEY_TYPE_WEP40) && (ieee->auth_mode != 2) )
		{
			if (ext->key_len == 13)
				ieee->pairwise_key_type = alg = KEY_TYPE_WEP104;
			setKey( dev,
					idx,//EntryNo
					idx, //KeyIndex
					alg,  //KeyType
					zero, //MacAddr
					0,              //DefaultKey
					key);           //KeyContent
			set_swcam( dev,
					idx,//EntryNo
					idx, //KeyIndex
					alg,  //KeyType
					zero, //MacAddr
					0,              //DefaultKey
					key);           //KeyContent
		}
		else if (group)
		{
			ieee->group_key_type = alg;
			setKey( dev,
					idx,//EntryNo
					idx, //KeyIndex
					alg,  //KeyType
					broadcast_addr, //MacAddr
					0,              //DefaultKey
					key);           //KeyContent
			set_swcam( dev,
					idx,//EntryNo
					idx, //KeyIndex
					alg,  //KeyType
					broadcast_addr, //MacAddr
					0,              //DefaultKey
					key);           //KeyContent
		}
		else //pairwise key
		{
			setKey( dev,
					4,//EntryNo
					idx, //KeyIndex
					alg,  //KeyType
					(u8*)ieee->ap_mac_addr, //MacAddr
					0,              //DefaultKey
					key);           //KeyContent
			set_swcam( dev,
					4,//EntryNo
					idx, //KeyIndex
					alg,  //KeyType
					(u8*)ieee->ap_mac_addr, //MacAddr
					0,              //DefaultKey
					key);           //KeyContent
		}


	}

end_hw_sec:
        priv->rtllib->wx_set_enc = 0;
	up(&priv->wx_sem);
#endif
	return ret;

}
static int r8192_wx_set_auth(struct net_device *dev,
                                        struct iw_request_info *info,
                                        union iwreq_data *data, char *extra)
{
	int ret = 0;

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 5, 0)
	//printk("====>%s()\n", __FUNCTION__);
	struct r8192_priv *priv = rtllib_priv(dev);

	if (priv->bHwRadioOff == true)
		return 0;

	down(&priv->wx_sem);
	ret = rtllib_wx_set_auth(priv->rtllib, info, &(data->param), extra);
	up(&priv->wx_sem);
#endif
	return ret;
}

static int r8192_wx_set_mlme(struct net_device *dev,
                                        struct iw_request_info *info,
                                        union iwreq_data *wrqu, char *extra)
{
	//printk("====>%s()\n", __FUNCTION__);

	int ret = 0;

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 5, 0)
	struct r8192_priv *priv = rtllib_priv(dev);

	if (priv->bHwRadioOff == true)
		return 0;

	down(&priv->wx_sem);
	ret = rtllib_wx_set_mlme(priv->rtllib, info, wrqu, extra);
	up(&priv->wx_sem);
#endif
	return ret;
}
#endif

static int r8192_wx_set_gen_ie(struct net_device *dev,
                                        struct iw_request_info *info,
                                        union iwreq_data *data, char *extra)
{
	int ret = 0;

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 5, 0)
        struct r8192_priv *priv = rtllib_priv(dev);

	if (priv->bHwRadioOff == true)
		return 0;

        down(&priv->wx_sem);
        ret = rtllib_wx_set_gen_ie(priv->rtllib, extra, data->data.length);
        up(&priv->wx_sem);
#endif
        return ret;
}

static int r8192_wx_get_gen_ie(struct net_device *dev,
                               struct iw_request_info *info,
			       union iwreq_data *data, char *extra)
{
	int ret = 0;
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 5, 0)
        struct r8192_priv *priv = rtllib_priv(dev);
	struct rtllib_device* ieee = priv->rtllib;

	if (ieee->wpa_ie_len == 0 || ieee->wpa_ie == NULL) {
		data->data.length = 0;
		return 0;
	}

	if (data->data.length < ieee->wpa_ie_len) {
		return -E2BIG;
	}

	data->data.length = ieee->wpa_ie_len;
	memcpy(extra, ieee->wpa_ie, ieee->wpa_ie_len);
#endif
        return ret;
}

#define IW_IOCTL(x) [(x)-SIOCSIWCOMMIT]
static iw_handler r8192_wx_handlers[] =
{
        IW_IOCTL(SIOCGIWNAME) = r8192_wx_get_name,
        IW_IOCTL(SIOCSIWFREQ) = r8192_wx_set_freq,
        IW_IOCTL(SIOCGIWFREQ) = r8192_wx_get_freq,
        IW_IOCTL(SIOCSIWMODE) = r8192_wx_set_mode,
        IW_IOCTL(SIOCGIWMODE) = r8192_wx_get_mode,
        IW_IOCTL(SIOCSIWSENS) = r8192_wx_set_sens,
        IW_IOCTL(SIOCGIWSENS) = r8192_wx_get_sens,
        IW_IOCTL(SIOCGIWRANGE) = rtl8192_wx_get_range,
        IW_IOCTL(SIOCSIWAP) = r8192_wx_set_wap,
        IW_IOCTL(SIOCGIWAP) = r8192_wx_get_wap,
        IW_IOCTL(SIOCSIWSCAN) = r8192_wx_set_scan,
        IW_IOCTL(SIOCGIWSCAN) = r8192_wx_get_scan,
        IW_IOCTL(SIOCSIWESSID) = r8192_wx_set_essid,
        IW_IOCTL(SIOCGIWESSID) = r8192_wx_get_essid,
        IW_IOCTL(SIOCSIWNICKN) = r8192_wx_set_nick,
		IW_IOCTL(SIOCGIWNICKN) = r8192_wx_get_nick,
        IW_IOCTL(SIOCSIWRATE) = r8192_wx_set_rate,
        IW_IOCTL(SIOCGIWRATE) = r8192_wx_get_rate,
        IW_IOCTL(SIOCSIWRTS) = r8192_wx_set_rts,
        IW_IOCTL(SIOCGIWRTS) = r8192_wx_get_rts,
        IW_IOCTL(SIOCSIWFRAG) = r8192_wx_set_frag,
        IW_IOCTL(SIOCGIWFRAG) = r8192_wx_get_frag,
//		IW_IOCTL(SIOCSIWTXPOW) = r8192_wx_set_txpow,
//		IW_IOCTL(SIOCGIWTXPOW) = r8192_wx_get_txpow,
        IW_IOCTL(SIOCSIWRETRY) = r8192_wx_set_retry,
        IW_IOCTL(SIOCGIWRETRY) = r8192_wx_get_retry,
        IW_IOCTL(SIOCSIWENCODE) = r8192_wx_set_enc,
        IW_IOCTL(SIOCGIWENCODE) = r8192_wx_get_enc,
        IW_IOCTL(SIOCSIWPOWER) = r8192_wx_set_power,
        IW_IOCTL(SIOCGIWPOWER) = r8192_wx_get_power,
//      IW_IOCTL(SIOCSIWSPY) = iw_handler_set_spy,
//      IW_IOCTL(SIOCGIWSPY) = iw_handler_get_spy,
//      IW_IOCTL(SIOCSIWTHRSPY) = iw_handler_set_thrspy,
//		IW_IOCTL(SIOCGIWTHRSPY) = iw_handler_get_thrspy,
#if (WIRELESS_EXT >= 18)
		IW_IOCTL(SIOCSIWGENIE) = r8192_wx_set_gen_ie,
		IW_IOCTL(SIOCGIWGENIE) = r8192_wx_get_gen_ie,
		IW_IOCTL(SIOCSIWMLME) = r8192_wx_set_mlme,
		IW_IOCTL(SIOCSIWAUTH) = r8192_wx_set_auth,
//		IW_IOCTL(SIOCGIWAUTH) = r8192_wx_get_auth,
		IW_IOCTL(SIOCSIWENCODEEXT) = r8192_wx_set_enc_ext,
//		IW_IOCTL(SIOCGIWENCODEEXT) = r8192_wx_get_encodeext,
#endif
};

/*
 * the following rule need to be follwing,
 * Odd : get (world access),
 * even : set (root access)
 * */
static const struct iw_priv_args r8192_private_args[] = {
	{
		SIOCIWFIRSTPRIV + 0x0,
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "set_debugflag"
	},
	{
		SIOCIWFIRSTPRIV + 0x1,
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "activescan"
	},
	{
		SIOCIWFIRSTPRIV + 0x2,
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "rawtx"
	}
	,
	{
		SIOCIWFIRSTPRIV + 0x3,
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "forcereset"
	}
	,
	{
		SIOCIWFIRSTPRIV + 0x4,
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "force_mic_error"
	}
	,
	{
		SIOCIWFIRSTPRIV + 0x5,
		IW_PRIV_TYPE_NONE, IW_PRIV_TYPE_INT|IW_PRIV_SIZE_FIXED|1,
		"firm_ver"
	}
	,
	{
		SIOCIWFIRSTPRIV + 0x6,
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED|1, IW_PRIV_TYPE_NONE,
		"set_power"
	}
        ,
	{
		SIOCIWFIRSTPRIV + 0x9,
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED|1, IW_PRIV_TYPE_NONE,
		"radio"
	}
        ,
	{
		SIOCIWFIRSTPRIV + 0xa,
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED|1, IW_PRIV_TYPE_NONE,
		"lps_interv"
	}
        ,
	{
		SIOCIWFIRSTPRIV + 0xb,
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED|1, IW_PRIV_TYPE_NONE,
		"lps_force"
	}
	,
	{
		SIOCIWFIRSTPRIV + 0xc,
		0, IW_PRIV_TYPE_CHAR|2047, "adhoc_peer_list"
	}
#ifdef CONFIG_MP
	,
	{
		SIOCIWFIRSTPRIV + 0xe,
                IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "SetChan"
	}
	,
	{
		SIOCIWFIRSTPRIV + 0xf,
                IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "SetRate"
	}
	,
	{
		SIOCIWFIRSTPRIV + 0x10,
                IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "SetTxPower"
	}
	,
	{
		SIOCIWFIRSTPRIV + 0x11,
                IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "SetBW"
	}
        ,
        {
                SIOCIWFIRSTPRIV + 0x12,
                IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "TxStart"
        }
	,
	{
		SIOCIWFIRSTPRIV + 0x13,
                IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "SetSingleCarrier"
	}
        ,
        {
                SIOCIWFIRSTPRIV + 0x14,
                IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 3, 0, "WriteRF"
        }
        ,
        {
                SIOCIWFIRSTPRIV + 0x15,
                IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 3, 0, "WriteMAC"
        }
#endif
};

static iw_handler r8192_private_handler[] = {
	(iw_handler)r8192_wx_set_debugflag,   /*SIOCIWSECONDPRIV*/
	(iw_handler)r8192_wx_set_scan_type,
	(iw_handler)r8192_wx_set_rawtx,
	(iw_handler)r8192_wx_force_reset,
	(iw_handler)r8192_wx_force_mic_error,
	(iw_handler)r8191se_wx_get_firm_version,
	(iw_handler)r8192_wx_adapter_power_status,
	//must here or else handler will wrong
	(iw_handler)NULL,
	(iw_handler)NULL,
	(iw_handler)r8192se_wx_set_radio,
	(iw_handler)r8192se_wx_set_lps_awake_interval,
	(iw_handler)r8192se_wx_set_force_lps,
	(iw_handler)r8192_wx_get_adhoc_peers,
	(iw_handler)NULL,
#ifdef CONFIG_MP
	(iw_handler)r8192_wx_mp_set_chan,
	(iw_handler)r8192_wx_mp_set_txrate,
	(iw_handler)r8192_wx_mp_set_txpower,
	(iw_handler)r8192_wx_mp_set_bw,
        (iw_handler)r8192_wx_mp_set_txstart,
        (iw_handler)r8192_wx_mp_set_singlecarrier,
        (iw_handler)r8192_wx_mp_write_rf,
	(iw_handler)r8192_wx_mp_write_mac,
#else
	(iw_handler)NULL,
	(iw_handler)NULL,
	(iw_handler)NULL,
	(iw_handler)NULL,
	(iw_handler)NULL,
	(iw_handler)NULL,
	(iw_handler)NULL,
	(iw_handler)NULL,
#endif
};

//#if WIRELESS_EXT >= 17
struct iw_statistics *r8192_get_wireless_stats(struct net_device *dev)
{
       struct r8192_priv *priv = rtllib_priv(dev);
	struct rtllib_device* ieee = priv->rtllib;
	struct iw_statistics* wstats = &priv->wstats;
	int tmp_level = 0;
	int tmp_qual = 0;
	int tmp_noise = 0;
	if (ieee->state < RTLLIB_LINKED)
	{
		wstats->qual.qual = 10;
		wstats->qual.level = 0;
		wstats->qual.noise = -100;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 14))
		wstats->qual.updated = IW_QUAL_ALL_UPDATED | IW_QUAL_DBM;
#else
		wstats->qual.updated = 0x0f;
#endif
		return wstats;
	}

       tmp_level = (&ieee->current_network)->stats.rssi;
	tmp_qual = (&ieee->current_network)->stats.signal;
	tmp_noise = (&ieee->current_network)->stats.noise;
	//printk("level:%d, qual:%d, noise:%d\n", tmp_level, tmp_qual, tmp_noise);

	wstats->qual.level = tmp_level;
	wstats->qual.qual = tmp_qual;
	wstats->qual.noise = tmp_noise;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 14))
	wstats->qual.updated = IW_QUAL_ALL_UPDATED | IW_QUAL_DBM;
#else
	wstats->qual.updated = 0x0f;
#endif
	return wstats;
}
//#endif

struct iw_handler_def  r8192_wx_handlers_def ={
	.standard = r8192_wx_handlers,
	.num_standard = sizeof(r8192_wx_handlers) / sizeof(iw_handler),
	.private = r8192_private_handler,
	.num_private = sizeof(r8192_private_handler) / sizeof(iw_handler),
	.num_private_args = sizeof(r8192_private_args) / sizeof(struct iw_priv_args),
#if WIRELESS_EXT >= 17
	.get_wireless_stats = r8192_get_wireless_stats,
#endif
	.private_args = (struct iw_priv_args *)r8192_private_args,
};
