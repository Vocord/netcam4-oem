package provide abi-nodes 1.0
#
#  TCL P3 ABI helpers for ABI Writer
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

### ABI WRITER
#  Node format:
#  {$bval $len}
#  binary value
#  binary value len

#         key: {bval}
#returns list: {blen}
#  {binary value len} container size in octets (1, 2 , 4)
proc abi_get_blen { key } {
    set len [string length $key]
    set	  ll 1

    if {$len > 0x007f} {
	set ll 2
    }

    if {$len > 0x3fff} {
	set ll 4
    }

    switch $ll {
	1 {
	    set blen [binary format c $len]
	}

	2 {
	    set blen [binary format S [expr $len+0x8000]]
	}

	4 {
	    set blen [binary format I [expr $len+0xc0000000]]
	}
    }
    return $blen
}

# Create node from KEY and VAL
#
# key: {bval}
# val: {bval}
#
# returns string: {{klen}{bkey}{vlen}{bval}}
proc abi_node { key val } {
    set    klen [abi_get_blen $key]
    set    vlen [abi_get_blen $val]

    append klen $key $vlen $val

    return $klen
}

# Append nodes together
#
# returns string: "$arg0$arg1....$argN"
proc abi_append_val {args} {
    set node ""

    foreach el [lrange $args 0 end] {
        append node $el
    }

    return $node
}

proc abi_create_stream_packet {nodes} {
    set len [binary format I [string length $nodes]]

    append len $nodes

    return $len
}

### P3SS ABI WRITER HELPERS
proc abi_async_context {args} {
    if { [lindex $args 0] == "" } {
	return "" 
    }
    # P3_ABI_ASYNC_CONTEXT_T
    set abi_key  [binary format "I" 0x0002]
    set abi_context [binary format "I" [lindex $args 0]] 

    return [abi_node $abi_key $abi_context]
}

proc abi_set_id {} {
}

proc abi_set_t1 {t} {
    set k [binary format "I" 0x0018]
    set v [binary format "W" $t]

    return [abi_node $k $v]
}

proc abi_set_t0 {t} {
    set k [binary format "I" 0x0017]
    set v [binary format "W" $t]

    return [abi_node $k $v]
}

proc abi_set_maxsize {size} {
    set k [binary format "I" 0x001c]
    set v [binary format "I" $size]

    return [abi_node $k $v]
}

proc abi_set_granularity {t} {
    set k [binary format "I" 0x0108]
    set v [binary format "W" $t]

    return [abi_node $k $v]
}

proc abi_set_req_mode {mode} {
    set k [binary format "I" 0x0109]
    set v [binary format "I" $mode]

    return [abi_node $k $v]
}

proc abi_set_mdm_request {t0 t1} {
    set k [binary format "I" 0x00A3]

    set vt0 [abi_set_t0 $t0]
    set vt1 [abi_set_t1 $t1]

    set v [abi_append_val [abi_set_t0 $t0]	\
	      [abi_set_t1 $t1]			\
	      [abi_set_maxsize     200000]	\
	      [abi_set_granularity 20000000]	\
	  ]

    return [abi_node $k $v]
}

proc abi_set_bbox_gid {dev_id ch} {
    set k [binary format "I" 0x0029]
    set v [binary format "IH*III" 0 $dev_id 0 0 $ch]

    return [abi_node $k $v]
}

proc abi_make_stat_request {dev_id ch t0 t1} {
    set k [binary format "I" 0x1005]

    set v [abi_append_val \
	      [abi_set_bbox_gid $dev_id $ch]	\
	      [abi_set_mdm_request $t0 $t1]	\
	      [abi_set_req_mode 2]		\
	  ]

    return [abi_node $k $v]
}

proc abi_node2a {node} {
    binary scan $node "H*" node_a
    return $node_a
}

proc abi_writer_test {} {
    set k0 "key0"
    set v0 "val0"
    set k1 "key1"
    set v1 "val1"


    set res0 [abi_node $k0 $v0]
    set res1 [abi_node $k1 $v1]
    set res3 [abi_append_val $res0 $res1]
#puts -nonewline $res
#puts "bin: [lindex $res 0] len: [lindex $res 1]"
    puts -nonewline [string length $res3]
    puts -nonewline $res3
}

### ABI READER
proc read_key { offs data } {
    binary scan $data [format "@%uc" $offs ] len
    set ll [expr (($len & 0xc0) >> 6)]

    switch {$len>>6 } {
        2 {
         # Correct Comment Placement
	    binary scan $data [format "@%uS" $offs ] len

	    set len [expr $len & 0x3fff]
	    binary scan $data [format "@%ux2a" $offs ] val
	    set ll 2
        }

        3 {
	    binary scan $data [format "@%uI" $offs ] len
	    binary scan $data $fmt len

	    set len [expr $len & 0x3fffffff]
	    binary scan $data [format "@%ux4a" $offs ] val
	    set ll 4
        }
	
	default {
	    binary scan $data [format "@%ux1a" $offs ] val
	    set ll 1
	}
    }

    set offs [expr $offs + $ll]

    binary scan $data [format "@%uH%u" $offs [expr 2*$len]] val

#    puts "\[\[ll:$ll\]$len\] val: $val\n"

    set len [expr $len + $ll]
    lappend res $len $val

    return $res
}

#  Node format:
#  {$offs $bval $cval $len $ll}
#  offset
#  binary value
#  binary value hex representation
#  binary value len
#  {binary value len} container size in octets (1, 2 , 4)
proc read_val { data key } {
    set offs [lindex $key 0]
#"@"$offs"c"
    binary scan $data [format "@%uc" $offs ] len
    binary scan $data [format "@%uH*" $offs ] datas

    set ll [expr (($len & 0xc0) >> 6)]

    switch $ll {
        2 {
         # Correct Comment Placement
	    binary scan $data [format "@%uS" $offs ] len

	    set len [expr $len & 0x3fff]
        }

        3 {
	    binary scan $data [format "@%uI" $offs ] len

	    set len [expr $len & 0x3fffffff]
	    set ll 4
        }
	
	default {
	    set ll 1
	}
    }

    #puts "@:$offs ll: $ll len: $len D: $datas"
    set offs [expr $offs + $ll]

    if { $len == 0 } {
    	lappend res $offs "" "" $len $ll
    	return $res
    }

    binary scan $data [format "@%ua%u" $offs $len] bval
    binary scan $bval "H*" cval

#   puts "val: $cval"
#   puts "new offs: $offs"

    set offs [expr $offs + $len]
    lappend res $offs $bval $cval $len $ll

    return $res
}

