#!/usr/bin/tclsh
lappend auto_path .

package provide app-udpt 1.0

package require udp

package require abi-nodes
package require abi-parser

#
#  TCL P3 CLI for ACA statistic interface
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

    proc lremove {listVariable value} {
        upvar 1 $listVariable var
        set idx [lsearch -exact $var $value]
        set var [lreplace $var $idx $idx]
    }

     # Send data to a remote UDP socket
    proc udp_puts {host port} {
        set s [udp_open]
        fconfigure $s -remote [list $host $port]
        puts $s "Hello, World"
        close $f
    }


    # A simple UDP server
    proc udpEventHandler {sock} {
        set pkt [read $sock]
        set peer [fconfigure $sock -peer]
        global argv

        if {$argv != "" && [lsearch -exact $argv [lindex $peer 0]] == -1} {
            return
        }

        puts "$peer: [string length $pkt]"

#       binary scan $pkt "H*" pkth
#       puts "{$pkth}"

	parse_abi_packet $pkt

        return
    }

    proc udp_listen {port} {
        set srv [udp_open $port]

        global argv
        if {$argv != "" && [lsearch -exact $argv "-m"] != -1} {
            set group 224.0.1.20
            fconfigure $srv -mcastadd $group -remote [list $group $port]

	    lremove argv "-m"
        }

        fconfigure $srv -buffering none -translation binary
        fileevent $srv readable [list ::udpEventHandler $srv]
        puts "Listening on udp port: [fconfigure $srv -myport]"
        return $srv
    }

    if { ($argv == "--help") || ($argv == "-h") } {
         puts "usage:\n\t./udpt.tcl \[-m\] \[ip1\] \[ip2\] ..."
         exit
    }

    global abi_parse_quiet
    set abi_parse_quiet "0"

    set sock [udp_listen 4521]
    vwait forever
    close $sock


#    # A multicast demo.
#    proc udpEvent {chan} {
#        set data [read $chan]
#        set peer [fconfigure $chan -peer]
#        puts "$peer [string length $data] '$data'"
#        if {[string match "QUIT*" $data]} {
#            close $chan
#            set ::forever 1
#        }
#        return
#    }

#    set group 224.5.1.21
#    set port  7771
#    set s [udp_open $port]
#    fconfigure $s -buffering none -blocking 0
#    fconfigure $s -mcastadd $group -remote [list $group $port]
#    fileevent $s readable [list udpEvent $s]
#    puts -nonewline $s "hello, world"
#    set ::forever 0
#    vwait ::forever
#    exit

