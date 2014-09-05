#!/bin/sh
ARCH=`arch`
[ "$1" != "" ] && ARCH=$1
chown root .$ARCH/cam4_ps
chmod +s   .$ARCH/cam4_ps
