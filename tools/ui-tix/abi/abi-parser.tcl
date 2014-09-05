package provide abi-parser 1.0
#
#  TCL P3 ABI helpers for ABI Reader & Parser
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

### ABI Reader & Parser
proc abi_add_hex_val { key val level name } {
            set res [list $name [lindex $val 2]]

	    show_string $res $level
	    return $res
}

proc abi_add_blob_val { key val level name } {
            set res [list $name [lindex $val 1]]

	    show_string $res $level
	    return $res
}

#####################################
proc abi_cm_get_stub {key val level} {
    show_level $level

    puts "K:[lindex $key 1] [lindex $key 2] V:\"$val\""
}

proc abi_init_cm_update_cb { } {
    global abi_cm_update_cba

    array set abi_cm_update_cba {
	""	{abi_cm_get_stub $key [lindex $val 2] $level}
    }
}

proc abi_add_cm_update_cbh {val handler} {
    global abi_cm_update_cba

    if [ catch { set res $abi_cm_update_cba($val) } ] {
	array set abi_cm_update_cba [list $val $handler]
	puts "Added \"$val:[binary format "H*" $val]\" :$handler"
	return
    }

    puts "Handler for \"$val\" already registered."
    puts "Current:  \"$res\""
    puts "Requested:\"$handler\""
}

proc abi_add_cm_update_cba {val handler} {
    global abi_cm_update_cba
    binary scan $val "H*" valh

    if [ catch { set res $abi_cm_update_cba($valh) } ] {
	array set abi_cm_update_cba [list $valh $handler]
	puts "Added \"$valh:$val\" :$handler"
#	puts "val:\"[array get abi_cm_update_cba]\""
	return
    }

    puts "Handler for \"$valh:$val\" already registered."
    puts "Current:  \"$res\""
    puts "Requested:\"$handler\""
}

proc abi_apply_cm_update_cb_ {key val level} {
    global abi_cm_update_cba
    if [ catch { set res $abi_cm_update_cba([lindex $key 2]) } ] {
    	return 0
    }

#    puts "$res"
    eval $res

    return 1
}

proc abi_apply_cm_update_cb {key val level} {
    global abi_cm_update_cba
    if [ catch { set res $abi_cm_update_cba([lindex $key 2]) } ] {
	puts "handler for \"$val\" not found"

    	return 0
    }

    puts "HANDLE:  \"$res\""
    eval $res

    return 1
}
#####################################

proc abi_p3_component {type} {
    array set abi_p3_component_type {
        00000000  "MDM(data)"
        00000001  "MDM(cmd)"
        00000002  "MDM"
        00000003  "DEV(data)"
        00000004  "DEV(cmd)"
        00000005  "DEV"
        00000006  "CM"
        00000007  "DEV HTTP"
        00000008  "DEV RAW IP"
    }

    if [ catch { set res $abi_p3_component_type($type) } ] {
    	return "Unknown component: $type"
    }

    return "$res"
}

proc abi_p3_device_type {type} {
    set descr [abi_p3_device_type_get $type]
    set cmd   [lindex $descr 1]
    if { $cmd != "" } {
    	set descr [lindex $descr 0]

	return [eval $cmd ]
    }
    return $descr
}

proc abi_p3_device_type_get {type} {
    array set abi_p3_device_aa {
	00000000	{ "VBxx/VMxx DVR" }
	00000004	{ "VBxx/VMxx PLR" }
	00000005	{ "VERELAY      " }
	00000006	{ "NETCAM4      " }
	00000007	{ "NANO P3SS    " }
	00000400	{ "WIN P3 Proxy0" }
	50525859	{ "WIN P3 Proxy1" }
	41524348	{ "WIN P3   ARCH" }
	xx17	{ { "test "} { show_mdm_stat $offset $avl $used } }
    }

    if [ catch { set res $abi_p3_device_aa($type) } ] {
    	return "\{$type\}";
    }

    return $res
}

proc abi_p3_rc2a {rc} {
    array set abi_p3_rc_aa {
	0	{ "clients count" }
	1	{ "transport rx" }
	2	{ "transport tx" }
	3	{ "cpu load" }
	4	{ "storage rx" }
	5	{ "storage tx" }
	6	{ "Vins mask" }
	7	{ "Ains mask" }
	8	{ "Dins mask" }
	9	{ "Vins {offset,count,mask}" }
	10	{ "Vins Video Present mask" }
	11	{ "Vins Data Present mask" }
	12	{ "Alarm Probes {offs,seq,mask}" }
	13	{ "MOD   {offs,seq,mask}" }
	14	{ "Mode/Stage Info" }
	15	{ "Probes count{?, count, mask}" }
	16	{ "MOD {vofs,count,mask}" }
	17	{ { "MDM stat { fsnum, avl MiB, used MiB} "} { show_mdm_stat $offset $avl $used } }
	18	{ "LOD   {offs,seq,mask}" }
	19	{ "LOD {vofs,count,mask}" }
	20	{ "Vins RAW mask" }
	21	{ "Vins RAW Objects mask" }
	22	{ "Vins DEV Objects mask" }
	23	{ "AUDIO 8kHz mask" }
	24	{ "LINE DC Levels mask" }
	25	{ "RAW TDM data mask" }
    }

    if [ catch { set res $abi_p3_rc_aa($rc) } ] {
    	return "\{$rc\}";
    }

    return $res
}

proc show_mdm_stat {offs avl used} {
    return [format "%s: fs:@%08x total: %10u MiB used: %10u MiB avl: %10u MiB" "MDM stat" $offs $avl $used [expr $avl - $used] ]
}

proc abi_p3_ns_info {data} {
    binary scan $data "IIII" offset type avl used
    set descr [abi_p3_rc2a $type]
    set cmd   [lindex $descr 1]
    if { $cmd != "" } {
    	set descr [lindex $descr 0]

	return [eval $cmd ]
    }
    return [format "%32s: @%08x avl:%08x used:%08x" $descr $offset $avl $used]
}

proc abi_p3_load2a {data} {
    binary scan $data "IIIIII" type max cur stat(0)  stat(1)  stat(2)
    return "[abi_p3_rc2a $type]: max: $max cur: $cur stat: \{$stat(0), $stat(1), $stat(2)\}"
}

proc str_limits {str} {
    global debug_str_limit
    if [catch { set limit $debug_str_limit}] {
	set limit 200
    }

    set length [string length $str]
    if { $length <= $limit } {
	return $str
    }

    set a1 [expr $limit / 2]
    set b0 [expr $length - $a1]

    return "[string range $str 0 $a1] ... [string range $str $b0 end]"
}

proc make_level { level } {
    for {set i 0} {$i< $level} {incr i} {
	append l "    "
    }
    return $l
}

proc show_level { level } {
    for {set i 0} {$i< $level} {incr i} {
	puts -nonewline "    "
    }
}

proc show_string { str level } {
    global abi_parse_quiet
    if { $abi_parse_quiet != "0" } {
	return
    }

    set l [make_level $level]
    set lst [list "\n" "\n$l"]
    set str [string map $lst $str]
    puts "$l$str"
}

proc show_node_ { key val level } {
    show_level $level
    puts "\"[lindex $key 2]\" next @[lindex $val 0]: \[[lindex $val 3]\]\"[str_limits [lindex $val 2]]\""
}

proc show_node { key val level } {
    show_string "\"[lindex $key 2]\" next @[lindex $val 0]: \[[lindex $val 3]\]\"[str_limits [lindex $val 2]]\"" $level
}

proc show_res { key res level } {
    show_string "\"[lindex $key 2]\" [lindex $res 0]: \"[lindex $res 1]\"" $level
}

proc show_val { val level } {
    show_level $level
    puts "next @[lindex $val 0]: \[[lindex $val 3]\]\"[str_limits [lindex $val 2]]\""
}

proc p3ss_abi_get_branch { val level } {
    set res ""
    set level1	[expr $level +1]

    set limit [lindex $val 3]
    set data  [lindex $val 1]

    global cm_key

    set val   "0"

    while { [lindex $val 0] < $limit } {
        set key [read_val $data $val]
        set val [read_val $data $key]

#   catch {
#       if { $cm_key != "" } {
#           show_node_ $key $val $level1
#       }
#   }
	set lres [p3ss_abi_fill $key $val $level1] 
	if { [llength $lres] == 2 } {
		abi_parser_add_list $lres
	}
	lappend res $lres
#   catch {
#       if { $cm_key != "" } {
#           for {set i 0} {$i< $level} {incr i} {
#       	puts -nonewline "    "
#           }
#           puts "$lres"
#       }
#   }
    }

    return $res
}


proc set_fpn_handlers {get_data get_offset get_state get_stage} {
    global fpn_data_handler
    global fpn_offset_handler
    global fpn_state_handler
    global fpn_stage_handler

    set fpn_data_handler   $get_data
    set fpn_offset_handler $get_offset

    set fpn_state_handler  $get_state
    set fpn_stage_handler  $get_stage
}

proc set_uart_handlers { uart_get_pkt get_sens_lut} {
	global uart_get_key_val
	global get_sensor_lut
	set uart_get_key_val $uart_get_pkt
	set get_sensor_lut  $get_sens_lut
}

proc set_eeprom_handlers {get_eeprom} {
	global get_eeprom_handler
	set get_eeprom_handler $get_eeprom
}

proc set_fd_handlers {get_fd} {
	global get_fd_handler
	set get_fd_handler $get_fd
}

proc get_offset {key val level} {
    binary scan $val "I" offset

    global fpn_offset_handler
    append fpn_offset_handler ""

    if { $fpn_offset_handler != "" } {
	catch [$fpn_offset_handler $offset]
    }

    set res [list "offset:" $offset]
    show_res $key $res $level
    return $res
}

proc get_data {key val len level} {
    global data_frame

    set data_frame [list $len $val]

    set res [list "data-octets:" $len ]
    show_res $key $res $level

    global fpn_data_handler
    append fpn_data_handler ""

    if { $fpn_data_handler != "" } {
	catch [$fpn_data_handler $val $len]
    }

    return $res
}

proc get_fpn_params {key val level} {
    binary scan  $val "Scc" \
	max	\
	res 	\
	frm_avg

    set res [list "fpn-params:" [list saturate: $max frm_avg: $frm_avg]]

    show_res $key $res $level
    return $res
}


proc abi_p3_extract_submodel { val } {
    array set abi_p3_model_aa {
	0	 "VB4 AKA VB8.1"
	1	 "VB8 AKA VB8.2"
	3	 "VM4 AKA VM4.1"
    }

    binary scan  [lindex $val 1] "I" model_id

    #Convert to unsigned
    set model_id [expr {$model_id & 0xFFFFFFFF}];

    if [ catch { set model_type $abi_p3_model_aa($model_id) } ] {
    	return [lindex $val 2]
    }

    return [concat $model_type " " [lindex $val 2]]
}

proc abi_p3_extract_firmware { val } {
    array set abi_p3_firmwaree_aa {
	01	 "Filesystem"
	02	 "Kernel    "
	03	 "redboot   "
	04	 "BIOS(mon) "
	05	 "post      "
	06	 "config    "
	ff	 "FACTORY default"
    }

    binary scan  [lindex $val 1] "H2ScW" fw_id fw_major fw_minor fw_build_filetime

    #Convert to unsigned
#    set fw_id    [expr {$fw_id    & 0xFF}];
    set fw_minor [expr {$fw_minor & 0xFF}];
    set fw_major [expr {$fw_major & 0xFFFF}];

    set fw_build_time [expr {($fw_build_filetime-wide(116444736000000000))/wide(10000000)} ]
    set fw_b_tm [clock format $fw_build_time -format "%Y/%m/%d %X"]

    if [ catch { set fw_type $abi_p3_firmwaree_aa($fw_id) } ] {
    	set fw_type "$fw_id      "
    }

    if { $fw_id != "ff" } {
	return [format "%s %5d.%-3d @%s" $fw_type $fw_major $fw_minor $fw_b_tm]
    }

    return "\{ $fw_type \}"
}

proc p3_errnotostr {} {
    global p3_errno
    array set p3_errno {
	    0	"P3_SUCCESS         "
	    1	"P3_HARDWARE_FAULT  "
	    2	"P3_SOFTWARE_FAULT  "
	    3	"P3_MEDIA_DATA_FAULT"
	    4	"P3_MEDIA_FAULT     "
	    5	"P3_STOPPED         "
	    6	"P3_INVALID_FLOW    "
	    7	"P3_INVALID_TIME    "
	    8	"P3_INVALID_QOS     "
	    9	"P3_INVALID_SCALE   "
	    10	"P3_INVALID_ARGUMENT"
	    11	"P3_INVALID_CONTEXT "
	    12	"P3_ALREADY_DONE    "
	    13	"P3_NOT_IMPLEMENTED "
	    14	"P3_NO_RESOURCES    "
	    15	"P3_NO_DATA         "
	    16	"P3_FAILED          "
	    17	"P3_TIME_TO_LOW     "
	    18	"P3_TIME_TO_BIG     "
	    19	"P3_TO_MANY_CLIENTS "
	    20	"P3_TRY_AJUSTED_ARGS"
	    21	"P3_DB_FAILED       "
	    22	"P3_UNKNOWN_COMMAND "
	    23	"P3_MEDIA_FULL      "
    }

    #puts "set errno [array size p3_errno]"
}

proc p3ss_abi_fill { key val level} {
    global cm_key
	
    switch [lindex $key 1] {
	"FW-main" {
	    return  [abi_add_blob_val $key $val $level "FW   main:"]
	}

	"FW-rescue" {
	    return  [abi_add_blob_val $key $val $level "FW rescue:"]
	}
    }

    switch [lindex $key 2] {
        00000000 {
	    return  [abi_add_hex_val $key $val $level "abi_id:"]
        }
        00000002 {
	    return  [abi_add_hex_val $key $val $level "context"]
        }
        00000003 {
	    global p3_errno

	    if {[array size p3_errno] == 0} {
		# first time here - init p3 errno array
		#puts "LOL"
		p3_errnotostr
	    }

	    set verr 0
	    binary scan [lindex $val 1] "I" verr
	    set p3_err [array get p3_errno [expr - $verr]]
	    #puts "p3_err:  [lindex $p3_err 1]"
	    
            #set res [list "status" $idx]
            set res [list "status:" [lindex $p3_err 1]]

	    show_string $res $level

	    return $res
        }

	00000007 {
	    return  [abi_add_hex_val $key $val $level "msg_part:"]
	}

        00000008 {
	    return  [abi_add_hex_val $key $val $level "host_id:"]
        }
        00000009 {
	    return  [abi_add_blob_val $key $val $level "host_name:"]
        }
        0000000b {
            set res [list "component:" [abi_p3_component [lindex $val 2]]]

	    show_string $res $level
	    return $res
        }
        0000000c {
	    binary scan [lindex $val 1] "ccccSH2H2" ip0 ip1 ip2 ip3 port tp ap
	    set ip0 [expr $ip0 & 0xff] 
	    set ip1 [expr $ip1 & 0xff] 
	    set ip2 [expr $ip2 & 0xff] 
	    set ip3 [expr $ip3 & 0xff]
	    set port [expr $port & 0xffff]
 	    if [catch {set tp $tp}] {
		return "ERROR"
	    }
            set res [list "conn_ipv4:" "$tp:$ap://$ip0.$ip1.$ip2.$ip3:$port"]

	    show_string $res $level
	    return $res
        }
        0000000e {
            set res [list "server_param:" [abi_p3_load2a [lindex $val 1]]]

	    show_string $res $level
	    return $res
        }
        00000010 {
	    return  [abi_add_hex_val $key $val $level "index"]
        }
        00000011 {
	    binary scan [lindex $val 1] "WII" ts id load

            set res [list "client_id:" "ts: $ts con number: $id server load: $load"]

	    show_string $res $level
	    return $res
        }
        00000013 {
	    return  [abi_add_hex_val $key $val $level "mac:"]
        }
        00000015 {
	    return  [abi_add_blob_val $key $val $level "device_name:"]
        }
        00000017 {
            set res [list "       t0:" [lindex $val 2]]
        }
        00000018 {
            set res [list "       t1:" [lindex $val 2]]
        }
        00000019 {
            set res [list "       te:" [lindex $val 2]]
        }
        00000013 {
	    return  [abi_add_hex_val $key $val $level "host_type:"]
        }
	000000ec {
            set res [list "device_class:" [abi_p3_device_type [lindex $val 2]]]

	    show_string $res $level
	    return $res
        }

        0000002b {
            set res [list "ns_info:" [abi_p3_ns_info [lindex $val 1]]]

	    show_string $res $level
	    return $res
        }
        00000100 {
	    show_stat_data "stat data:" [lindex $val 1] [lindex $val 3] $level
	    return  [abi_add_hex_val $key $val $level "stat data:"]
        }
        00000108 {
            set res [list "       tu:" [lindex $val 2]]
        }
        00000109 {
            set res [list "stat type:" [lindex $val 2]]
        }
        0000010a {
            set res [list "stat size:" [lindex $val 2]]
        }
        00000029 {
            set res [list "      gid:" [lindex $val 2]]
	    show_string $res $level
        }

        000000a3 {
	    # P3_ABI_ITEM_N or P3_ABI_REQUEST_N
            set res [list "request:" [p3ss_abi_get_branch $val $level]]
        }
	
	000000ca {
	    # P3_ABI_KEY_T
            set cm_key [lindex $val 1]
	    return  [abi_add_hex_val $key $val $level "key:"]
        }
        
	000000cd {
	    # P3_ABI_KEY_ATTR_T
	    return  [abi_add_hex_val $key $val $level "key attr:"]
        }
	
	000000ce {
	    # P3_ABI_VAL_T
	    set res [list $cm_key [lindex $val 2]]
	    global uart_get_key_val
	    append uart_get_key_val ""
	    if {$uart_get_key_val != ""} {
			$uart_get_key_val $cm_key $val
	    } else {
		
		#puts "val for: $cm_key"
		#set res [list "/" $cm_key]
		#
		show_res $key $res $level
		set res [p3ss_abi_get_branch $val $level]
	    }
	    return $res
        }

        2f000000 {
            set res [list "interface" ""]
	    show_string "/branch:" $level

	    p3ss_abi_get_branch $val $level
	    return $res
        }
	73656e736f722d6c7574 {
		#lut-sensor
		set res [list $cm_key [lindex $val 2]]
		global get_sensor_lut
	    append get_sensor_lut ""
	    if {$get_sensor_lut != ""} {
			$get_sensor_lut [lindex $val 1]
		}
	}
	
	4669726d77617265526576 {
            set res [list "firmware:" [abi_p3_extract_firmware $val]]

	    show_string $res $level
	    return $res
	}

	5375624d6f64656c {
            set res [list "submodel:" [abi_p3_extract_submodel $val]]

	    show_string $res $level
	    return $res
	}

	744d494e {
	    return  [abi_add_hex_val $key $val $level "     tMIN:"]
	}

	744d4158 {
	    return  [abi_add_hex_val $key $val $level "     tMAX:"]
	}

	49662043617073 {
	    return  [abi_add_hex_val $key $val $level "  If Caps:"]
	}

	77642d6f7574 {
	    return  [abi_add_hex_val $key $val $level "wd-out:"]
	}

	77642d6f75742d7374617465 {
	    return  [abi_add_hex_val $key $val $level "wd-out-state:"]
	}

	636c696d617465 {
	    return  [abi_add_hex_val $key $val $level "climate:"]
	}
	
	636c696d6174652d7374617465 {
	    return  [abi_add_hex_val $key $val $level "climate-state:"]
	}

	7374616765 {
	    #fpn-stage
	    append cm_key ""
	    if { $cm_key != "" } {
	        global fpn_stage_handler
		return [$fpn_stage_handler $key [lindex $val 2] $level]
	    }

	    return  [abi_add_hex_val $key $val $level "stage:"]
	}

	6f666673 {
	    return [get_offset $key [lindex $val 1] $level]
	}

	2f66706e2f636f6e666967 {
		#puts [string length [string range [lindex $val 1] 7 end ] ]
		get_eeprom_data [string range [lindex $val 1] 7 end ]
		return
	}
	
	74662d696e {
		global get_fd_handler
		$get_fd_handler [lindex $key 1] [lindex $val 1]
		return
	}
	74662d6f7574 {
		global get_fd_handler
		$get_fd_handler [lindex $key 1] [lindex $val 1]
		return
	}
	64696d2d6d696e {
		global get_fd_handler
		$get_fd_handler [lindex $key 1] [lindex $val 1]
		return
	}
	64696d2d6d6178 {
		global get_fd_handler
		$get_fd_handler [lindex $key 1] [lindex $val 1]
		return
	}
	7468726573686f6c64 {
		global get_fd_handler
		$get_fd_handler [lindex $key 1] [lindex $val 1]
		return
	}
	66642d63656c6c {
		global get_fd_handler
		$get_fd_handler [lindex $key 1] [lindex $val 1]
		return
	}
	66642d666c616773 {
		global get_fd_handler
		$get_fd_handler [lindex $key 1] [lindex $val 1]
		return
	}
	66642d666163652d616e676c65{
		global get_fd_handler
		$get_fd_handler [lindex $key 1] [lindex $val 1]
		return
	}
	7363616c6531363a3136 {
		global get_fd_handler
		$get_fd_handler [lindex $key 1] [lindex $val 1]
		return
	}
	
	64617461 {
		return [get_data $key [lindex $val 1] [lindex $val 3] $level]
	}

	66706e2d7374617465 {
	    global fpn_state_handler
	    #fpn-state
	    return [$fpn_state_handler $key [lindex $val 1] $level]
	}

	66706e2d706172616d73 {
	    #fpn-param
	    return [get_fpn_params $key [lindex $val 1] $level]
	}
	
        default {
	    if [ abi_apply_cm_update_cb_ $key $val $level ] {
		return [list [lindex $key 2] [lindex $val 2]]
	    }

	    switch -regexp [lindex $key 2]  {
		"(^2f)" {
		    set res [list [lindex $key 2] [binary format "H*" [lindex $key 2]]]
		    show_res $key $res $level

		    return [p3ss_abi_get_branch $val $level]
		}

		"000000a3" {
		    set res [list [lindex $key 2] "/branch:"]
		    show_string $res $level

		    return [p3ss_abi_get_branch $val $level]
		}
	    }
	    
	    set res [list [lindex $key 2] [lindex $val 2]]
	    show_node $key $val $level
	    #puts "ret k:[lindex $key 2] V:[lindex $val 2]"
	    return $res
        }
    }

    return $res
}

proc get_eeprom_data {data} {
	global get_eeprom_handler
	$get_eeprom_handler $data
}

proc cm_data_msg { data key } {
    set val [read_val $data $key ]
    #puts "P3_CM_GET_VALS_CMD:"
    #show_node $key $val 1
    
    set limit [lindex $val 3]
    set data  [lindex $val 1]
    set val   "0"

    while { [lindex $val 0] < $limit } {
        set key [read_val $data $val]
        #show_val $key 2

        set val [read_val $data $key]
        #show_node $key $val 1

   	lappend res [p3ss_abi_fill $key $val 1]
#        break
    }

#    puts "cm_data_msg: $res"

    return $res
}

proc on_timeline_data_msg { data key } {
    set val [read_val $data $key ]
    puts "BIT Index Response:"
    show_node $key $val 0

    set limit [lindex $val 3]
    set data  [lindex $val 1]

    set val   "0"

    while { [lindex $val 0] < $limit } {
        set key [read_val $data $val]
#        show_val $key 2

        set val [read_val $data $key]
#        show_node $key $val 1

   	lappend res [p3ss_abi_fill $key $val 1]

#        break
    }

    return res
}

proc on_msg_aca_responce { data key } {
    set val [read_val $data $key ]
    puts "ACA Response:"
#    show_node $key $val 0

    set limit [lindex $val 3]
    set data  [lindex $val 1]

    set val   "0"

    while { [lindex $val 0] < $limit } {
        set key [read_val $data $val]
        set val [read_val $data $key]

   	lappend res [p3ss_abi_fill $key $val 1]
    }

    return res
}

proc trace_abi_packet { data key name } {
    set val [read_val $data $key]
    puts "$name:"

    return [p3ss_abi_get_branch $val 0]
}

proc abi_trace_abi_packet { data key } {
    set val [read_val $data $key]

    return [p3ss_abi_get_branch $val 0]
}

proc parse_abi_packet {pkt} {
   set key [read_val $pkt {"0" "" "" "" ""} ]

   switch -regexp [lindex $key 2] {
       0*0 {
           set result [trace_abi_packet $pkt $key "ACA Request:"]
       }
          0*1 {
#           set aca_responce_d [on_msg_aca_responce $pkt $key]
           set result [trace_abi_packet $pkt $key "ACA Response:"]
       }

       0*1006 {
           set mdm_responce_data [on_timeline_data_msg $pkt $key]
       }

       default {
	   binary scan $pkt "H*" pkth
           puts "Unknown Node: RAW: {$pkth}"

           show_val $key 0
           set val [read_val $pkt $key]
           puts "Unknown Node:"
   	   show_val $val 1
           set result [trace_abi_packet $pkt $key "Unknown Node:"]
       }
   }
}

set abi_parse_quiet "1"
set abi_parse_quiet_aca "0"
set is_eeprom_data "0"


proc abi_parser_clear {} {
   global abi_tree
   global cm_key

   set cm_key ""
   array unset abi_tree
}

proc abi_parser_add {key val} {
   global abi_tree
   set node [list key val]
   array set abi_tree $node
}

proc abi_parser_add_list {node} {
   global abi_tree

   if [ catch {array set abi_tree $node} ] {
       puts "error: $node"
   }
}

proc abi_parse_packet {pkt} {
   abi_parser_clear
   global abi_parse_quiet
   global abi_parse_quiet_aca

   set key [read_val $pkt {"0" "" "" "" ""} ]
   switch -regexp [lindex $key 2] {
       0*0$ {
	   set old $abi_parse_quiet
	   set abi_parse_quiet $abi_parse_quiet_aca
	   abi_parser_add "cmd" "ACA Request"
           set response [abi_trace_abi_packet $pkt $key]
	   set abi_parse_quiet $old
       }

       0*0001$ {
	   set old $abi_parse_quiet
	   set abi_parse_quiet $abi_parse_quiet_aca
	   abi_parser_add "cmd" "ACA Response"
           set response [abi_trace_abi_packet $pkt $key]
	   set abi_parse_quiet $old
       }
       
       0*1502$ {
	   # P3_CM_GET_VALS_CMD
	   abi_parser_add "cmd" "P3_CM_GET_VALS_CMD"
           
	   puts "P3_CM_GET_VALS_CMD:"
	   set response [cm_data_msg $pkt $key]
       }
       
       0*1505$ {
	   # P3_CM_UPDATE_VAL_CMD
	   abi_parser_add "cmd" "P3_CM_UPDATE_VAL_CMD"
	   
           #puts "P3_CM_UPDATE_VAL_CMD:"
	   
	   set response [cm_data_msg $pkt $key]
       }

		0*1506$ {
	   # P3_CM_APPEND_VAL_CMD
	   abi_parser_add "cmd" "P3_CM_APPEND_VAL_CMD"   
       #puts "P3_CM_APPEND_VAL_CMD:"
	   set response [cm_data_msg $pkt $key]
       }

       0*1006$ {
	   abi_parser_add "cmd" "MDM Response Data"
           set response [on_timeline_data_msg $pkt $key]
       }

       default {
	   abi_parser_add "cmd" "Unknown"
	   binary scan $pkt "H*" pkth
           puts "Unknown Node: RAW: {$pkth}"

           show_val $key 0
           set val [read_val $pkt $key]
           puts "Unknown Node:"
   	   show_val $val 1
           set response [trace_abi_packet $pkt $key "Unknown Node:"]
       }
   }

   return $response
}

proc abi_reopen_dst {server port flag} {
    global req_io
    global req_dst
	
    append req_dst ""
    set new_dst "$server : $port"
  
    if {$flag != "1"  && $req_dst == $new_dst} then {
	return f
    }

    if [catch {set req_io [socket [lindex $server 0] $port] } err_conn] then {
	puts "Cannot connect to: $server:$port errno: $err_conn"
	return t
    }

    fconfigure $req_io -translation binary
    set req_dst $new_dst

    return f
}

proc abi_single_request {server port request args} {
    global req_io
    global req_dst

    if [abi_reopen_dst $server $port 0] {
	return f
    }

#   Send request
    puts -nonewline $req_io $request
    if [catch { flush $req_io } err_flush] {
	puts "I/O error: $err_flush Reconnecting."

	if [abi_reopen_dst $server $port 1] {
	    return f
	}

	puts -nonewline $req_io $request
	flush $req_io
    }

    set blen [read $req_io 4] 
    if {![binary scan $blen I len]} {
	puts "missing length"

	close $req_io
        set req_dst ""

	return
    }
#    puts "len: $len"

    if {[llength $args] >= 1} {
	update
    }
    set response [read $req_io $len]

#    binary scan $response "H*" pkth
#    puts "ans {$pkth}"

    abi_parse_packet $response
}



proc abi_single_request_no_resp {server port request args} {
    global req_io
    global req_dst
	
    abi_reopen_dst $server $port 0

#   Send request
    puts -nonewline $req_io $request
    if [catch { flush $req_io } err_flush] {
	puts "I/O error: $err_flush Reconnecting."

	abi_reopen_dst $server $port 1

	puts -nonewline $req_io $request
	flush $req_io
    }
#    binary scan $response "H*" pkth
#    puts "ans {$pkth}"
}
