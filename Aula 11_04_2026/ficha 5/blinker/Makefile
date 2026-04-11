obj-m += blinker.o

#KERNEL_DIR = /usr/src/linux-headers-6.12.75+rpt-rpi-v8
KERNEL_DIR = /lib/modules/$(shell uname -r)/build
# Arguments for cross-compiling to the Aarch64 architecture
#CCC_ARGS = ARCH=arm64 CROSS_COMPILE=/home/daniel/arm-gnu-toolchain-15.2.rel1-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-

all:
# 	make -C $(KERNEL_DIR) M=$(PWD) modules $(CCC_ARGS)
	make -C $(KERNEL_DIR) M=$(PWD) 

clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean