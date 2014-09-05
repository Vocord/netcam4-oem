
#
#  TCL P3 ABI helpers for MDM statistic interface
#
#  Copyright 2009-2014
#  	Vocord, Inc			<info@vocord.com>
#	Alexander Rostovtsev		<alexr@vocord.com>
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

proc get_timeline {server time0 time1} {
    set t0 [ expr {116444736000000000 + wide($time0) * 10000000} ]
    set t1 [ expr {116444736000000000 + wide($time1) * 10000000} ]

    puts "Server: $server"
    puts "Time0: $time0"
    puts "Time1: $time1"
    puts "T0: $t0"
    puts "T1: $t1"

    set mdm_io [socket [lindex $server 0] 10003]
    set dev_id [lindex $server 1]

    fconfigure $mdm_io -translation binary
# -encoding binary
    set request [abi_create_stream_packet [abi_make_stat_request $dev_id 1 $t0 $t1]]

    puts $mdm_io $request
    flush $mdm_io
#    gets $sockChan line

    if {![binary scan [read $mdm_io 4] I len]} {
	error "missing length"
    }

    puts "len: $len"
    set mdm_responce [read $mdm_io $len]

    close $mdm_io
#   puts "The connection to $server is closed\n"

   binary scan $mdm_responce H* data
   puts "RAW: $data\n"

   set key [read_val $mdm_responce {"0" "" "" "" ""} ]
   show_val $key 0

   switch -regexp [lindex $key 2] {
       0*1006 {
           set mdm_responce_data [on_timeline_data_msg $mdm_responce $key]
       }

       default {
           set val [read_val $mdm_responce key]
           puts "Unknown Node:"
   	   show_val $key 1
       }
   }

   puts [abi_node2a $request]
}

