/*
 * Host AP crypt: host-based WEP encryption implementation for Host AP driver
 *
 * Copyright (c) 2002-2004, Jouni Malinen <jkmaline@cc.hut.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 */

#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/random.h>
#include <linux/skbuff.h>
#include <asm/string.h>
#include <linux/interrupt.h>
#include "rtllib.h"

#include <linux/crypto.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 3, 0))
#include <crypto/skcipher.h>
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 20)
    #include <asm/scatterlist.h>
#else
    #include <linux/scatterlist.h>
#endif
#include <linux/crc32.h>

struct prism2_wep_data {
	u32 iv;
#define WEP_KEY_LEN 13
	u8 key[WEP_KEY_LEN + 1];
	u8 key_len;
	u8 key_idx;
	#if ((LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 21)) && (!OPENSUSE_SLED))
	struct crypto_tfm *tfm;
	#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 20, 0))
	struct crypto_sync_skcipher *tx_tfm;
	struct crypto_sync_skcipher *rx_tfm;
	#else
        struct crypto_blkcipher *tx_tfm;
        struct crypto_blkcipher *rx_tfm;
        #endif
};


static void * prism2_wep_init(int keyidx)
{
	struct prism2_wep_data *priv;

	priv = kzalloc(sizeof(*priv), GFP_ATOMIC);
	if (priv == NULL)
		goto fail;
	priv->key_idx = keyidx;

	#if ((LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 21)) && (!OPENSUSE_SLED))
	priv->tfm = crypto_alloc_tfm("arc4", 0);
	if (priv->tfm == NULL) {
		printk(KERN_DEBUG "rtllib_crypt_wep: could not allocate "
		       "crypto API arc4\n");
		goto fail;
	}
	#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 20, 0))
	priv->tx_tfm = crypto_alloc_sync_skcipher("ecb(arc4)", 0, CRYPTO_ALG_ASYNC);
        if (IS_ERR(priv->tx_tfm)) {
                printk(KERN_DEBUG "rtllib_crypt_wep: could not allocate "
                       "crypto API arc4\n");
                priv->tx_tfm = NULL;
                goto fail;
        }
        priv->rx_tfm = crypto_alloc_sync_skcipher("ecb(arc4)", 0, CRYPTO_ALG_ASYNC);
        if (IS_ERR(priv->rx_tfm)) {
                printk(KERN_DEBUG "rtllib_crypt_wep: could not allocate "
                       "crypto API arc4\n");
                priv->rx_tfm = NULL;
                goto fail;
        }
	#else
	priv->tx_tfm = crypto_alloc_blkcipher("ecb(arc4)", 0, CRYPTO_ALG_ASYNC);
        if (IS_ERR(priv->tx_tfm)) {
                printk(KERN_DEBUG "rtllib_crypt_wep: could not allocate "
                       "crypto API arc4\n");
                priv->tx_tfm = NULL;
                goto fail;
        }
        priv->rx_tfm = crypto_alloc_blkcipher("ecb(arc4)", 0, CRYPTO_ALG_ASYNC);
        if (IS_ERR(priv->rx_tfm)) {
                printk(KERN_DEBUG "rtllib_crypt_wep: could not allocate "
                       "crypto API arc4\n");
                priv->rx_tfm = NULL;
                goto fail;
        }
        #endif

	/* start WEP IV from a random value */
	get_random_bytes(&priv->iv, 4);

	return priv;

fail:
	#if ((LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 21)) && (!OPENSUSE_SLED))
	if (priv) {
		if (priv->tfm)
			crypto_free_tfm(priv->tfm);
		kfree(priv);
	}
	#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 20, 0))
	if (priv) {
                if (priv->tx_tfm)
                        crypto_free_sync_skcipher(priv->tx_tfm);
                if (priv->rx_tfm)
                        crypto_free_sync_skcipher(priv->rx_tfm);
                kfree(priv);
        }
	#else
	if (priv) {
                if (priv->tx_tfm)
                        crypto_free_blkcipher(priv->tx_tfm);
                if (priv->rx_tfm)
                        crypto_free_blkcipher(priv->rx_tfm);
                kfree(priv);
        }
        #endif
	return NULL;
}


static void prism2_wep_deinit(void *priv)
{
	struct prism2_wep_data *_priv = priv;
	#if ((LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 21)) && (!OPENSUSE_SLED))
	if (_priv && _priv->tfm)
		crypto_free_tfm(_priv->tfm);
	#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 20, 0))
	if (_priv) {
                if (_priv->tx_tfm)
                        crypto_free_sync_skcipher(_priv->tx_tfm);
                if (_priv->rx_tfm)
                        crypto_free_sync_skcipher(_priv->rx_tfm);
        }
	#else
	if (_priv) {
                if (_priv->tx_tfm)
                        crypto_free_blkcipher(_priv->tx_tfm);
                if (_priv->rx_tfm)
                        crypto_free_blkcipher(_priv->rx_tfm);
        }
        #endif
	kfree(priv);
}

/* Perform WEP encryption on given skb that has at least 4 bytes of headroom
 * for IV and 4 bytes of tailroom for ICV. Both IV and ICV will be transmitted,
 * so the payload length increases with 8 bytes.
 *
 * WEP frame payload: IV + TX key idx, RC4(data), ICV = RC4(CRC32(data))
 */
static int prism2_wep_encrypt(struct sk_buff *skb, int hdr_len, void *priv)
{
	struct prism2_wep_data *wep = priv;
	u32 klen, len;
	u8 key[WEP_KEY_LEN + 3];
	u8 *pos;
	cb_desc *tcb_desc = (cb_desc *)(skb->cb + MAX_DEV_ADDR_SIZE);
	#if ((LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 21)) && \
	     (LINUX_VERSION_CODE < KERNEL_VERSION(4, 20, 0)))
	struct blkcipher_desc desc = {.tfm = wep->tx_tfm};
	#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 3, 0))
	int err;
#endif
	u32 crc;
	u8 *icv;
	struct scatterlist sg;
	if (skb_headroom(skb) < 4 || skb_tailroom(skb) < 4 ||
	    skb->len < hdr_len){
		printk("Error!!!headroom =%d tailroom =%d skblen =%d hdr_len =%d\n", skb_headroom(skb), skb_tailroom(skb), skb->len, hdr_len);
		return -1;
	}
	len = skb->len - hdr_len;
	pos = skb_push(skb, 4);
	memmove(pos, pos + 4, hdr_len);
	pos += hdr_len;

	klen = 3 + wep->key_len;

	wep->iv++;

	/* Fluhrer, Mantin, and Shamir have reported weaknesses in the key
	 * scheduling algorithm of RC4. At least IVs (KeyByte + 3, 0xff, N)
	 * can be used to speedup attacks, so avoid using them. */
	if ((wep->iv & 0xff00) == 0xff00) {
		u8 B = (wep->iv >> 16) & 0xff;
		if (B >= 3 && B < klen)
			wep->iv += 0x0100;
	}

	/* Prepend 24-bit IV to RC4 key and TX frame */
	*pos++ = key[0] = (wep->iv >> 16) & 0xff;
	*pos++ = key[1] = (wep->iv >> 8) & 0xff;
	*pos++ = key[2] = wep->iv & 0xff;
	*pos++ = wep->key_idx << 6;

	/* Copy rest of the WEP key (the secret part) */
	memcpy(key + 3, wep->key, wep->key_len);

	if (!tcb_desc->bHwSec) {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 20, 0))
		SYNC_SKCIPHER_REQUEST_ON_STACK(req, wep->tx_tfm);
#endif
		/* Append little-endian CRC32 and encrypt it to produce ICV */
	#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 5, 0))
		crc = ~crc32_le(~0, pos, len);
	#else
		crc = ~ether_crc_le(len, pos);
	#endif
		icv = skb_put(skb, 4);
		icv[0] = crc;
		icv[1] = crc >> 8;
		icv[2] = crc >> 16;
		icv[3] = crc >> 24;

	#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24))
		sg.page = virt_to_page(pos);
		sg.offset = offset_in_page(pos);
		sg.length = len + 4;
	#else
		sg_init_one(&sg, pos, len+4);
	#endif
	#if ((LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 21)) && (!OPENSUSE_SLED))
		crypto_cipher_setkey(wep->tfm, key, klen);
		crypto_cipher_encrypt(wep->tfm, &sg, &sg, len + 4);
		return 0;
	#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 20, 0))
		crypto_sync_skcipher_setkey(wep->tx_tfm, key, klen);
		skcipher_request_set_sync_tfm(req, wep->tx_tfm);
		skcipher_request_set_callback(req, 0, NULL, NULL);
		skcipher_request_set_crypt(req, &sg, &sg, len + 4, NULL);
		err = crypto_skcipher_encrypt(req);
		skcipher_request_zero(req);
		return err;
	#else
		crypto_blkcipher_setkey(wep->tx_tfm, key, klen);
		return crypto_blkcipher_encrypt(&desc, &sg, &sg, len + 4);
	#endif
	}

	return 0;
}


/* Perform WEP decryption on given buffer. Buffer includes whole WEP part of
 * the frame: IV (4 bytes), encrypted payload (including SNAP header),
 * ICV (4 bytes). len includes both IV and ICV.
 *
 * Returns 0 if frame was decrypted successfully and ICV was correct and -1 on
 * failure. If frame is OK, IV and ICV will be removed.
 */
static int prism2_wep_decrypt(struct sk_buff *skb, int hdr_len, void *priv)
{
	struct prism2_wep_data *wep = priv;
	u32  klen, plen;
	u8 key[WEP_KEY_LEN + 3];
	u8 keyidx, *pos;
	cb_desc *tcb_desc = (cb_desc *)(skb->cb + MAX_DEV_ADDR_SIZE);
	#if ((LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 21)) && \
	     (LINUX_VERSION_CODE < KERNEL_VERSION(4, 20, 0)))
	struct blkcipher_desc desc = {.tfm = wep->rx_tfm};
	#endif
	u32 crc;
	u8 icv[4];
	struct scatterlist sg;
	if (skb->len < hdr_len + 8)
		return -1;

	pos = skb->data + hdr_len;
	key[0] = *pos++;
	key[1] = *pos++;
	key[2] = *pos++;
	keyidx = *pos++ >> 6;
	if (keyidx != wep->key_idx)
		return -1;

	klen = 3 + wep->key_len;

	/* Copy rest of the WEP key (the secret part) */
	memcpy(key + 3, wep->key, wep->key_len);

	/* Apply RC4 to data and compute CRC32 over decrypted data */
	plen = skb->len - hdr_len - 8;

	if (!tcb_desc->bHwSec) {
	#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 20, 0))
		int err;
		SYNC_SKCIPHER_REQUEST_ON_STACK(req, wep->rx_tfm);
	#endif
	#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24))
		sg.page = virt_to_page(pos);
		sg.offset = offset_in_page(pos);
		sg.length = plen + 4;
	#else
		sg_init_one(&sg, pos, plen+4);
	#endif
	#if ((LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 21)) && (!OPENSUSE_SLED))
		crypto_cipher_setkey(wep->tfm, key, klen);
		crypto_cipher_decrypt(wep->tfm, &sg, &sg, plen + 4);
	#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 20, 0))
		crypto_sync_skcipher_setkey(wep->rx_tfm, key, klen);
		skcipher_request_set_sync_tfm(req, wep->rx_tfm);
		skcipher_request_set_callback(req, 0, NULL, NULL);
		skcipher_request_set_crypt(req, &sg, &sg, plen + 4, NULL);
		err = crypto_skcipher_decrypt(req);
		skcipher_request_zero(req);
		if (err)
			return -7;
	#else
		crypto_blkcipher_setkey(wep->rx_tfm, key, klen);
		if (crypto_blkcipher_decrypt(&desc, &sg, &sg, plen + 4))
			return -7;
	#endif
	#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 5, 0))
		crc = ~crc32_le(~0, pos, plen);
	#else
		crc = ~ether_crc_le(plen, pos);
	#endif
		icv[0] = crc;
		icv[1] = crc >> 8;
		icv[2] = crc >> 16;
		icv[3] = crc >> 24;
		if (memcmp(icv, pos + plen, 4) != 0) {
			/* ICV mismatch - drop frame */
			return -2;
		}
	}
	/* Remove IV and ICV */
	memmove(skb->data + 4, skb->data, hdr_len);
	skb_pull(skb, 4);
	skb_trim(skb, skb->len - 4);

	return 0;
}


static int prism2_wep_set_key(void *key, int len, u8 *seq, void *priv)
{
	struct prism2_wep_data *wep = priv;

	if (len < 0 || len > WEP_KEY_LEN)
		return -1;

	memcpy(wep->key, key, len);
	wep->key_len = len;

	return 0;
}


static int prism2_wep_get_key(void *key, int len, u8 *seq, void *priv)
{
	struct prism2_wep_data *wep = priv;

	if (len < wep->key_len)
		return -1;

	memcpy(key, wep->key, wep->key_len);

	return wep->key_len;
}


static char * prism2_wep_print_stats(char *p, void *priv)
{
	struct prism2_wep_data *wep = priv;
	p += sprintf(p, "key[%d] alg = WEP len =%d\n",
		     wep->key_idx, wep->key_len);
	return p;
}


static struct rtllib_crypto_ops rtllib_crypt_wep = {
	.name			= "WEP",
	.init			= prism2_wep_init,
	.deinit			= prism2_wep_deinit,
	.encrypt_mpdu		= prism2_wep_encrypt,
	.decrypt_mpdu		= prism2_wep_decrypt,
	.encrypt_msdu		= NULL,
	.decrypt_msdu		= NULL,
	.set_key		= prism2_wep_set_key,
	.get_key		= prism2_wep_get_key,
	.print_stats		= prism2_wep_print_stats,
	.extra_prefix_len	= 4, /* IV */
	.extra_postfix_len	= 4, /* ICV */
	.owner			= THIS_MODULE,
};


int __init rtllib_crypto_wep_init(void)
{
	return rtllib_register_crypto_ops(&rtllib_crypt_wep);
}


void __exit rtllib_crypto_wep_exit(void)
{
	rtllib_unregister_crypto_ops(&rtllib_crypt_wep);
}

void rtllib_wep_null(void)
{
        return;
}
