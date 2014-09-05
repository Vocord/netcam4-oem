#!/bin/bash
if [ -z $1 ] || [ -z $2 ]
then
	echo Usage: $0 IP_NETCAM4  IP_LOCAL
	exit 0
fi

sudo killall cam4_ps
sudo killall cam4_ps_Xclient

sudo killall -9 cam4_ps
sudo killall -9 cam4_ps_Xclient

sudo ipcrm  -M 0x00001831
sudo ipcrm  -M 0x00001826
sudo ipcrm  -M 0x0000303c

sudo .i686/cam4_ps -m 2 -v $1 -d $2
