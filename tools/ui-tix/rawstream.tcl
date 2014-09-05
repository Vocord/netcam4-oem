#!/usr/bin/wish
#  TCL P3 DEV interface HELPER
#
#  Copyright 2009-2014
#  	Vocord, Inc			<info@vocord.com>
#	Alexander Rostovtsev		<alexr@vocord.com>
#	Sergey Zorkin
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
lappend auto_path .

package provide app-rawstream 1.0
package require Tk
package require cmdline

package require abi-cm
package require abi-nodes
package require abi-parser


proc ip_address {} {
        # find out localhost's IP address
        # courtesy David Gravereaux, Heribert Dahms
        set TheServer [socket -server none -myaddr [info hostname] 0]
        set MyIP [lindex [fconfigure $TheServer -sockname] 0]
        close $TheServer
        return $MyIP
}

proc start { } {
	global ip
	global flow

	set abi_start [binary format "I" 0x51]
	set abi_id [binary format "I" 0x0]
	set dst_flows "fTX"
	set conn_ipv4 [binary format "I" 0xc]
	set abi_request [binary format "I" 0xa3]
	set max_buff [binary format "I" 0x1c]
	
	set my_ip [ip_address]
	set my_ip_bin [binary format c4 [split $my_ip "."]]	
	append my_ip_bin "\x00\x00\x00\x00"

	set id "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	append id [binary format "c" $flow]
	binary scan $id "H*" idh
	puts "Start: $ip\[$idh\] "

	set message [abi_node $abi_start							\
		[abi_append_val									\
			[abi_async_context 0x0]							\
			[abi_node $abi_id $id ]							\
			[abi_node $dst_flows   [abi_node $conn_ipv4 $my_ip_bin]]		\
			[abi_node $abi_request [abi_node $max_buff [binary format "I" 0x0]]]	\
		]										\
	]
	set request [abi_create_stream_packet $message]
	abi_single_request_no_resp $ip 10001 $request
}

proc stop { } {
	global ip
	global flow

	set abi_stop [binary format "I" 0x52]
	set abi_id [binary format "I" 0x0]

	set id "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	append id [binary format "c" $flow]
	binary scan $id "H*" idh
	puts " Stop: $ip\[$idh\] "

	set message [abi_node $abi_stop			\
		[abi_append_val				\
			[abi_async_context 0x0]		\
			[abi_node $abi_id $id ]		\
		]					\
	]
	set request [abi_create_stream_packet $message]
	abi_single_request_no_resp $ip 10001 $request
}

global ip
set ip "192.168.5.16"

set parameters {
    {ip.arg ""   ""}
}
array set arg [cmdline::getoptions argv $parameters]
if {$arg(ip) != ""} {
	set ip $arg(ip)
}

wm withdraw .
set wind .app
set flow 1
toplevel $wind

entry  $wind.ip -textvariable ip
entry  $wind.flow -textvariable flow
button $wind.start -text "Start" -command start
button $wind.stop -text "Stop" -command stop
button $wind.bye -text "Bye" -command {destroy .}

grid $wind.ip	 -row 0 -column 0 -columnspan 2
grid $wind.flow	 -row 0 -column 2
grid $wind.start -row 1 -column 0
grid $wind.stop	 -row 1 -column 1
grid $wind.bye	 -row 1 -column 2


bind $wind <Destroy> {
	exit
}
