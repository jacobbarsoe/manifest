#!/bin/sh
SDA=sda
BOOTCMD2='extlinux --once="bzImage root=/dev/sda3 rw panic=1" /tmp/g'
BOOTCMD3='extlinux --once="bzImage root=/dev/sda2 rw panic=1" /tmp/g'
set +x
mkdir -p /tmp/g
#ensure we do not override the default 
cat /proc/cmdline | grep panic && exit 

#update correct path
cat /proc/cmdline | grep ${SDA}3 && mount /dev/${SDA}2 /tmp/g
cat /proc/cmdline | grep ${SDA}2 && mount /dev/${SDA}3 /tmp/g
rm -rf /tmp/g/*
unzip -p -q $1 *rootfs.tar | tar -x -C /tmp/g
umount /tmp/g
mount /dev/${SDA}1 /tmp/g
cat /proc/cmdline | grep ${SDA}2 && $BOOTCMD2
cat /proc/cmdline | grep ${SDA}3 && $BOOTCMD3
umount /tmp/g
reboot 
