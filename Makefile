obj-m += sufs.o

ifdef KERNEL_TREE
	TREE=$(KERNEL_TREE)
else
	TREE=/lib/modules/$(shell uname -r)/build
endif

all: ko mkfs-sufs

ko:
	make -C $(TREE) M=$(PWD) modules

mkfs-sufs_SOURCES:
	mkfs-sufs.c mkfs-sufs.h

