#!/bin/bash
#
#  TCL P3 ABI helpers for CM interface
#
#  Copyright 2009-2014
#  	Vocord, Inc			<info@vocord.com>
#	Alexander Rostovtsev		<alexr@vocord.com>
#	Alexander Nikiforov		<alexander.nikiforov@vocord.com>
#

############################################################################
#									   #
#  This program is free software; you can redistribute it and/or modify    #
#  it under the terms of the GNU General Public License as published by    #
#  the Free Software Foundation; either version 2 of the License, or (at   #
#  your option) any later version.					   #
#									   #
#  This program is distributed in the hope that it will be useful, but	   #
#  WITHOUT ANY WARRANTY; without even the implied warranty of		   #
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU	   #
#  General Public License for more details.				   #
#									   #
#  You should have received a copy of the GNU General Public License along #
#  with this program; if not, write to the Free Software Foundation, Inc., #
#  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.		   #
#									   #
############################################################################

killall cam4_ps
killall cam4_ps_Xclient
ipcrm  -M 0x00001831
ipcrm  -M 0x00001826
ipcrm  -M 0x0000303c

ip route get $2 >route.log
grep src route.log | while read a1 a2 a3 a4 SRC
do
	echo ./cam4_ps -M -m 2 -d $SRC -v $2
	./cam4_ps -M -m 2 -d $SRC -v $2 > $2.ccl.log &
done

./cam4_ps_Xclient -g > $2.xcl.log &
