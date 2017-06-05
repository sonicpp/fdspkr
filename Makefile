ARCH=arm
COMPILE=arm-none-eabi-
KERNEL=~/fdspeaker/linux/build/src/linux-31e73f03aa61482b89067ea3ea14670acd319e2e/
MODULE=fdspkr.ko

$(MODULE): fdspkr.c
	make ARCH=$(ARCH) CROSS_COMPILE=$(COMPILE) -C $(KERNEL) modules M=`pwd`

clean:
	rm -f *.mod.* *.o *.ko
