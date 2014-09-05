#!/usr/bin/wish
#
#  TCL P3 "Net View Portable" UI
#
#  Copyright 2009-2014
#  	Vocord, Inc			<info@vocord.com>
#	Alexander Rostovtsev		<alexr@vocord.com>
#	Alexander Nikiforov		<alexander.nikiforov@vocord.com>
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

package provide app-netview 1.0

package require udp
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

proc udp_listen {port} {
    set srv [udp_open $port]
#      set group 224.0.1.20
#      fconfigure $srv -mcastadd $group -remote [list $group $port]
    fconfigure $srv -buffering none -translation binary
    fileevent $srv readable [list ::ACA_state_handler $srv]
    puts "Listening on udp port: [fconfigure $srv -myport]"
    return $srv
}

proc ACA_state_handler {sock} {
    set pkt [read $sock]
    set peer [fconfigure $sock -peer]
#    puts "$peer: [string length $pkt]"

    set ip [lindex [split $peer] 0]

    global dev_array

    abi_parse_packet $pkt
    global abi_tree
   # set msg_part $abi_tree(msg_part:)
   # puts $msg_part
    if [ catch {set host_id $abi_tree(host_id:)} ] {
	set host_id "  unknown_dev_id"
    }

    if [ catch {set dev_name $abi_tree(device_name:)} ] {
	set dev_name "unnamed"
    }

    if [ catch {set dev_class $abi_tree(device_class:)} ] {
	set dev_class "   unclassified"
    }

    set tm [clock clicks -millisecond]
	
    if [ catch {set res $dev_array($host_id)}] {
		puts "New: $host_id"
		array set dev_array [list $host_id [list $ip $dev_name $dev_class $tm $tm "1New"]]
    } else {
		set srv $dev_array($host_id)
        set srv [lreplace $srv 4 4 $tm]
        array set dev_array [list $host_id $srv]
    }

    

    global sort_idx
    dev_list_update $sort_idx

#    binary scan $pkt "H*" pkth
#    puts "{$pkth}"

    return
}

array set dev_array { }
#	ip	{ { } { } { } { } }


proc dev_list_update {sidx} {
    global sort_idx
    global dev_array
    global lst_servers
    global lst_servers_nf
    global f_type f_name
    global w

    set lst_servers_nf ""
    set sort_idx $sidx
	
	
    foreach host_id [array names dev_array] {
		set dev_info $dev_array($host_id)
		set ip    [lindex $dev_info 0]
		set name  [lindex $dev_info 1]
		set class [lindex $dev_info 2]
		
		lappend lst_servers_nf [format "%10s %18s \"%-33s\" %s" $class $host_id $name $ip]
    }
    
	if {$sidx == 4} {
		set lst_servers_nf [lsort -command sort_by_4 $lst_servers_nf] 
	} else {
		set lst_servers_nf [lsort -index $sort_idx -dictionary $lst_servers_nf]
	}
	set cur_time [clock clicks -millisecond]
	set lst_servers ""
	foreach srv $lst_servers_nf {
		set type [lindex $srv 0]
		set name [lindex $srv 2]
		if [regexp -nocase $f_type $type matchresult] {
			if [regexp -nocase $f_name $name matchresult] {
				lappend lst_servers $srv
			}
		}
	}
	for {set x 0} {$x<[llength $lst_servers]} {incr x} {
		set srv [lindex $lst_servers $x]
		set host_id [lindex $srv 1]
		if [ catch {set dev_info $dev_array($host_id)} ] {
#		    puts "No devinfo for $srv"
		    continue
		}
		set first_time [lindex $dev_info 3]
		set last_time [lindex $dev_info 4]
		
		if {$cur_time - $first_time < 10000} {
			set dev_info [lreplace $dev_info 5 5 "1New"]
			$w.frame.servers itemconfigure $x -foreground red
		} elseif {$cur_time - $last_time > 30000} {
			set dev_info [lreplace $dev_info 5 5 "3Old"]
			$w.frame.servers itemconfigure $x -foreground gray
		} else {
			set dev_info [lreplace $dev_info 5 5 "2Usual"]
			$w.frame.servers itemconfigure $x -foreground black
		}
		array set dev_array [list $host_id $dev_info]
	}
	
    
    return
}

proc sort_by_4 {op1 op2} {
	global dev_array
	#puts $op1
	set id1 [lindex $op1 1]	
	set id2 [lindex $op2 1]
	set dev_info1 $dev_array($id1)
	set dev_info2 $dev_array($id2)
	set state1 [lindex $dev_info1 5]
	set state2 [lindex $dev_info2 5]
	if {$state1 < $state2} {
		return -1
	} elseif {$state1 > $state2} {
		return 1
	} else {
		return 0
	}
}

proc set_clear {} {
	global dev_array
	foreach id [array names dev_array] {
		set dev_info $dev_array($id)
		set state    [lindex $dev_info 5]
		if {$state == "3Old"} {
			unset dev_array($id)
		}
    }
}

proc run_rawclient {} {
	global cur_srv
	set url "[lindex $cur_srv 0]"
	set my_ip [ip_address]
	exec "./rawstream.sh" $my_ip $url &
}

proc run_flash {} {
	global cur_srv
	set url "[lindex $cur_srv 0]"
	exec "./flash.tcl" -ip $url -ro 0 & 
}

proc run_uart {} {
	global cur_srv
	set url "[lindex $cur_srv 0]"
	exec "./uart.tcl" -ip $url &
}

proc run_fd {} {
	global cur_srv
	set url "[lindex $cur_srv 0]"
	exec "./fd.tcl" -ip $url &
}

proc run_fwconfig {} {
	global cur_srv
	set url "[lindex $cur_srv 0]"
	exec "./fw-config.tcl" -ip $url &
}

proc run_rawstream {} {
	global cur_srv
	set url "[lindex $cur_srv 0]"
	exec "./rawstream.tcl" -ip $url &
}

proc run_browser {} {
	global cur_srv
	if { "[lindex $cur_srv 3]" == "VBxx/VMxx DVR"} {
		set url "http://[lindex $cur_srv 0]:81"
	} else {
		set url "http://[lindex $cur_srv 0]"
	}

	puts "$url"
	exec "./open-device.sh" $url &
}

proc stop_rawclient {} {
	exec echo "STOP:" > /tmp/cam4.fifo
}


set t [clock scan now]
set sort_idx 2
#wm_default::setup
#wm_default::addoptions

wm withdraw .

set w .app

toplevel $w

wm transient $w ""
wm geometry $w 1024x768

bind $w <Destroy> {

  if {"%W" == "$w"} {
    puts "Bye %W"

    exit
  }
}

set font [font create -family Courier -size 11]
#puts "test len: [font measure $font test]"

#set w .apphscale
#catch {destroy $w}
#toplevel $w

wm title $w "Netview by Vocord"
wm iconname $w "hscale"
#positionWindow $w

frame $w.filters
frame $w.buttons
labelframe $w.frame -borderwidth 10 -text "Group"

pack $w.filters -side bottom -fill x -pady 2m -pady 2m
pack $w.buttons -side bottom -fill x -pady 2m -pady 2m

button $w.buttons.dismiss -text "Dismiss" -command "destroy $w"
button $w.buttons.sort2 -text "Sort Names" -command "dev_list_update 2"
button $w.buttons.sort3 -text "Sort IP" -command "dev_list_update 3"
button $w.buttons.sort4 -text "Sort State" -command "dev_list_update 4"
button $w.buttons.clear -text "Clear" -command "set_clear"
button $w.buttons.stop -text "Stop raw" -command "stop_rawclient"

label $w.filters.type_lab -text "Filter by type:"
entry $w.filters.f_type -textvariable f_type
label $w.filters.name_lab -text "Filter by name:"
entry $w.filters.f_name -textvariable f_name

set m [menu .popup_menu]
$m add command -label "rawclient" -command run_rawclient
$m add command -label "eeprom" -command run_flash
$m add command -label "uart" -command run_uart
$m add command -label "fd" -command run_fd
$m add command -label "fw-config" -command run_fwconfig
$m add command -label "rawstream" -command run_rawstream
$m add command -label "browser" -command run_browser

pack $w.filters.type_lab $w.filters.f_type $w.filters.name_lab $w.filters.f_name  -side left -expand 1
pack $w.buttons.dismiss $w.buttons.sort2 $w.buttons.sort3 $w.buttons.sort4 $w.buttons.clear $w.buttons.stop -side left -expand 1
pack $w.frame -side top -fill both -expand yes

dev_list_update $sort_idx
listbox $w.frame.servers -selectmode single -listvariable lst_servers -font $font
#"Fixed"
global abi_parse_quiet_aca
set abi_parse_quiet_aca "1"



bind $w.frame.servers <<ListboxSelect>> {
global cur_srv
global old_sel_id
#    puts "Selection:[%W curselection] | #%# a:%a b:%b c:%c d:%d e:%e f:%f g:%g h:%h i:%i j:%j k:%k l:%l m:%m n:%n o:%o p:%p s:%s t:%t x:%x y:%y w:%w #%# A:%A B:%B C:%C D:%D E:%E F:%F G:%G H:%H I:%I J:%J K:%K L:%L M:%M N:%N O:%O P:%P S:%S T:%T  X:%X Y:%Y W:%W"
    set sel_id  [%W curselection]
    if [ catch { set old_sel_id $old_sel_id } ] {
	set old_sel_id ""
    }

    if [ catch { set cur_ids [%W get $sel_id] } ] {
	puts "No Servers To Select"
    } else {
	set cur_srv [lindex $cur_ids 3]
	lappend cur_srv [lindex $cur_ids 2] [lindex $cur_ids 1] [lindex $cur_ids 0]
	if { $old_sel_id != $sel_id } {
	    puts "Selection\[$sel_id\]: $cur_srv"
	    set old_sel_id $sel_id
	} else {
            #SETUP Model dependend params
	    switch -regexp [lindex $cur_ids 0]  {
		VERELAY {
		    set url "http://[lindex $cur_srv 0]/"
		}

		NETCAM4 {
		    set url "[lindex $cur_srv 0]"
		}

		default {
		    set url "http://[lindex $cur_srv 0]:81/"
		}
	   }
	   set x [winfo pointerx .]
	   set y [winfo pointery .]
	   tk_popup .popup_menu $x $y

	#   puts "exec \"./rawstream.sh\" $my_ip $url"
	  
#	    exec "./rawstream.sh" $my_ip $url &
	}

    }
}

set server [$w.frame.servers get 0]
$w.frame.servers selection set 0
pack  $w.frame.servers -side bottom -fill both -expand yes -anchor nw

set sock [udp_listen 4521]
while {1==1} {
	after 100
	update
	dev_list_update $sort_idx
}
close $sock
