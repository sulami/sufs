obj-m += sufs.o

ifdef KERNEL_TREE
all:
	make -C $(KERNEL_TREE) M=$(PWD) modules
else
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
endif


