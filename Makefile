.PHONY: all clean
.SILENT: clean

CURRENT = $(shell uname -r)
KCOMPILER = $(shell grep CONFIG_CC_VERSION_TEXT /boot/config-$(CURRENT) | cut -d '"' -f2 | awk '{print $\$1}')
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)

TARGET = mymodule
obj-m += $(TARGET).o
$(TARGET)-objs := mymodule_main.o mymodule_func.o
CC_FLAGS_FPU := -mfpu=vfp
CFLAGS_$(TARGET)-objs += $(CC_FLAGS_FPU)
EXTRA_CFLAGS += -msse -msse2
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	rm -f *.ko *.cmd .*.cmd *.mod .*.mod *.mod.* *.o *.order *.symvers

