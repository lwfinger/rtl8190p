LINUX_KSRC_MODULE = /lib/modules/$(shell uname -r)/kernel/drivers/net/wireless/
RTL819x_DIR = $(shell pwd)
KVER  = $(shell uname -r)
KSRC = /lib/modules/$(KVER)/build
RTL819x_FIRM_DIR = $(RTL819x_DIR)/firmware
MODULE_FILE = $(RTL819x_DIR)/rtllib/Module.symvers

export LINUX_KSRC_MODULE_DIR RTL819x_FIRM_DIR

all: 
	@make -C $(KSRC) SUBDIRS=$(RTL819x_DIR)/src modules 
	cp ./HAL/rtl8192/r8190_pci.ko .
install: all
	@cp $(RTL819x_DIR)/RadioPower.sh /etc/acpi/events/
	@cp $(RTL819x_DIR)/wireless-rtl-ac-dc-power.sh /etc/acpi/
	@make -C HAL/$(HAL_SUB_DIR)/ install
uninstall:
	@make -C HAL/$(HAL_SUB_DIR)/ uninstall
	@rm -f /etc/acpi/wireless-rtl-ac-dc-power.sh
clean:
	@make -C HAL/$(HAL_SUB_DIR)/ clean
	@make -C rtllib/ clean
	@test -r mshclass/ && make -C mshclass/ clean|| echo > /dev/null 
	@rm -fr Modules.symvers
	@rm -fr Module.symvers
	@rm -fr Module.markers
	@rm -fr modules.order

