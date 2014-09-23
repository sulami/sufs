#!/bin/sh

# Create image and mountpoint
if [ ! -e image ]
then
    dd if=/dev/zero of=image bs=1M count=128
fi
if [ ! -d mnt ]
then
    mkdir mnt
fi

# Create new sufs
echo "Creating test image"
./mkfs-sufs image

# Load kernel module and mount the test image
echo "Loading kernel module"
insmod sufs.ko
echo "Mounting test image"
mount -o loop -t sufs image mnt

# Run tests
echo "Starting tests"
echo "Tests finished"

# Unmount the test image and unload the kernel module
echo "Unmounting test image"
umount mnt
echo "Unloading kernel module"
rmmod sufs

