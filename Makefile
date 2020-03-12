LINUX_KSRC_MODULE = /lib/modules/$(shell uname -r)/kernel/drivers/net/wireless/
RTL819x_DIR = $(shell pwd)
KVER  = $(shell uname -r)
MODDESTDIR = /lib/modules/$(KVER)/kernel/drivers/net/wireless/
KSRC = /lib/modules/$(KVER)/build
RTL819x_FIRM_DIR = $(RTL819x_DIR)/firmware
MODULE_FILE = $(RTL819x_DIR)/rtllib/Module.symvers
EXTRA_CFLAGS += -DHAVE_NET_DEVICE_OPS
EXTRA_CFLAGS+=-DEEPROM_OLD_FORMAT_SUPPORT=1
EXTRA_CFLAGS += -DUSE_FW_SOURCE_IMG_FILE
#it will fail to compile in suse linux enterprise 10 sp2. This flag is to solve this problem.
EXTRA_CFLAGS += -I$(TOPDIR)/drivers/net/wireless
EXTRA_CFLAGS += -std=gnu89
EXTRA_CFLAGS += -O2
EXTRA_CFLAGS += -mhard-float -DCONFIG_FORCE_HARD_FLOAT=y

SUBARCH := $(shell uname -m | sed -e "s/i.86/i386/; s/ppc.*/powerpc/; s/armv.l/arm/; s/aarch64/arm64/;")

ARCH ?= $(SUBARCH)
CROSS_COMPILE ?=
KVER  := $(shell uname -r)
KSRC ?= /lib/modules/$(KVER)/build
MODDESTDIR := /lib/modules/$(KVER)/kernel/drivers/net/wireless
INSTALL_PREFIX :=

ifneq ($(KERNELRELEASE),)

FILES := 	src/dot11d.o	\
		src/r819xE_firmware.o	\
		src/rtl819x_HTProc.o	\
		src/rtl_debug.o		\
		src/rtllib_crypt.o	\
		src/rtllib_module.o	\
		src/rtllib_tx.o		\
		src/rtl_wx.o		\
		src/r8190P_hwimg.o	\
		src/r819xE_phy.o	\
		src/rtl819x_TSProc.o	\
		src/rtl_dm.o		\
		src/rtllib_crypt_ccmp.o	\
		src/rtllib_rx.o		\
		src/rtllib_wx.o		\
		src/r8190_rtl8256.o	\
		src/rtl8192e.o		\
		src/rtl_cam.o		\
		src/rtl_eeprom.o	\
		src/rtllib_crypt_tkip.o	\
		src/rtllib_softmac.o	\
		src/rtl_pm.o		\
		src/r819xE_cmdpkt.o	\
		src/rtl819x_BAProc.o	\
		src/rtl_core.o		\
		src/rtl_ethtool.o	\
		src/rtllib_crypt_wep.o	\
		src/rtllib_softmac_wx.o	\
		src/rtl_ps.o		\

RTL8190P = rtl8190p

8190p-y += $(FILES)

obj-m := 8190.o

export CONFIG_RTL8190P = m

obj-$(CONFIG_RTL8190P) := 8190p.o

endif

all:	modules

modules:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KSRC) M=$(shell pwd)  modules

install:
	install -p -m 644 src/r8190_pci.ko $(MODDESTDIR)
	depmod -a
	cp -p firmware/*.img /lib/firmware/rtlwifi/.
uninstall:
	$(shell [ -d $(MODDESTDIR) ] && rm -f $(MODDESTDIR)/r8190_pci.ko)
	depmod -a
clean:
	@rm -f rc/.mod.c src/*.mod *.o src/.*.cmd src/*.ko *~
	@rm -fr src/.tmp_versions
	@rm -fr src/Modules.symvers
	@rm -fr src/Module.markers
	@rm -fr src/modules.order
	@rm -fr src/tags
	@rm -fr Modules.symvers
	@rm -fr Module.symvers
	@rm -fr Module.markers
	@rm -fr modules.order

