NIC_SELECT = RTL8190P

CC = gcc
KVER  := $(shell uname -r)
MODDESTDIR := /lib/modules/$(KVER)/kernel/drivers/net/wireless/

EXTRA_CFLAGS += -DRTL8190P -DHAVE_NET_DEVICE_OPS
EXTRA_CFLAGS+=-DEEPROM_OLD_FORMAT_SUPPORT=1
EXTRA_CFLAGS += -DUSE_FW_SOURCE_IMG_FILE
#it will fail to compile in suse linux enterprise 10 sp2. This flag is to solve this problem.
ifeq ($(shell uname -r | cut -d. -f1,2,3,4), 2.6.16.60-0)
	EXTRA_CFLAGS += -DOPENSUSE_SLED=1
else
	EXTRA_CFLAGS += -DOPENSUSE_SLED=0
endif
EXTRA_CFLAGS += -I$(TOPDIR)/drivers/net/wireless
EXTRA_CFLAGS += -std=gnu89
EXTRA_CFLAGS += -O2
EXTRA_CFLAGS += -mhard-float -DCONFIG_FORCE_HARD_FLOAT=y
EXTRA_CFLAGS += -DTHOMAS_TURBO
EXTRA_CFLAGS += -DENABLE_DOT11D
#flag for single module compile
EXTRA_CFLAGS += -DBUILT_IN_RTLLIB

r8190_pci-objs := rtl_core.o		\
		  rtl8192e.o		\
		  rtl_eeprom.o		\
		  rtl_ps.o		\
		  rtl_wx.o		\
		  rtl_cam.o		\
		  r8190_rtl8256.o	\
		  r819xE_phy.o		\
		  r819xE_firmware.o	\
		  r819xE_cmdpkt.o	\
		  rtl_dm.o		\
		  rtl_pm.o		\
		  r8190P_hwimg.o	\
		  rtl_ethtool.o		\
		  rtllib_rx.o                \
		  rtllib_softmac.o           \
		  rtllib_softmac_wx.o	\
		  rtllib_tx.o                \
		  rtllib_wx.o                \
		  rtllib_module.o            \
		  rtl819x_HTProc.o              \
		  rtl819x_TSProc.o              \
		  rtl819x_BAProc.o              \
		  dot11d.o                      \
		  rtllib_crypt.o             \
		  rtllib_crypt_tkip.o        \
		  rtllib_crypt_ccmp.o        \
		  rtllib_crypt_wep.o

obj-m := r8190_pci.o

KSRC := /lib/modules/$(KVER)/build
INSTALL_PREFIX :=
IMG_DIR := /lib/firmware/
all: modules

modules:
	$(MAKE) -C $(KSRC) M=$(PWD) CC=$(CC) modules
install: modules
	install -p -m 644 r8190_pci.ko $(MODDESTDIR)

	depmod -a
uninstall:
	$(shell [ -d $(MODDESTDIR) ] && rm -f $(MODDESTDIR)/r8190_pci.ko)
	depmod -a

.PHONY:clean
clean:
	rm -fr *.mod.c *.mod *.o .*.cmd *.ko *~
	rm -fr .tmp_versions
	rm -fr Modules.symvers
	rm -fr Module.symvers
	rm -fr Module.markers
	rm -fr modules.order
	rm -fr tags
