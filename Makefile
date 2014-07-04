LINUX_KSRC_MODULE = /lib/modules/$(shell uname -r)/kernel/drivers/net/wireless/
RTL819x_DIR = $(shell pwd)
KVER  = $(shell uname -r)
MODDESTDIR = /lib/modules/$(KVER)/kernel/drivers/net/wireless/
KSRC = /lib/modules/$(KVER)/build
RTL819x_FIRM_DIR = $(RTL819x_DIR)/firmware
MODULE_FILE = $(RTL819x_DIR)/rtllib/Module.symvers

export LINUX_KSRC_MODULE_DIR RTL819x_FIRM_DIR

all: 
	@make -C $(KSRC) SUBDIRS=$(RTL819x_DIR)/src modules 
install: all
	install -p -m 644 src/r8190_pci.ko $(MODDESTDIR)
	depmod -a
	cp -p firmware/*.img /lib/firmware/rtlwifi/.
uninstall:
	$(shell [ -d $(MODDESTDIR) ] && rm -f $(MODDESTDIR)/r8190_pci.ko)
	depmod -a
clean:
	@make -C src/ clean
	@rm -fr Modules.symvers
	@rm -fr Module.symvers
	@rm -fr Module.markers
	@rm -fr modules.order

