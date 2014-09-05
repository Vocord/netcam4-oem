package provide abi-cm 1.0

#
#  TCL P3SS ABI helpers for CM interface
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

proc abi_cm_get {key val args} {
    set kcm_get  [binary format "I" 0x1503]
    set abi_item [binary format "I" 0x00a3]

    set abi_key  [binary format "I" 0x00ca]
    set abi_val  [binary format "I" 0x00ce]

    set vreq [abi_append_val		\
	[abi_node $abi_key $key]	\
	[abi_node $abi_val $val]	\
    ]

    return [abi_node $kcm_get [abi_append_val	\
	[abi_async_context [lindex $args 0]]	\
	[abi_node $abi_item $vreq]		\
    ]]
}

proc abi_cm_update {key val args} {
    set kcm_upd  [binary format "I" 0x1505]
    set abi_item [binary format "I" 0x00a3]

    set abi_key  [binary format "I" 0x00ca]
    set abi_val  [binary format "I" 0x00ce]

    set vreq [abi_append_val		\
	[abi_node $abi_key $key]	\
	[abi_node $abi_val $val]	\
    ]

    return [abi_node $kcm_upd [abi_append_val	\
	[abi_async_context [lindex $args 0]]	\
	[abi_node $abi_item $vreq]		\
    ]]
}

proc abi_cm_append {key val args} {
    set kcm_app  [binary format "I" 0x1506]
    set abi_item [binary format "I" 0x00a3]

    set abi_key  [binary format "I" 0x00ca]
    set abi_val  [binary format "I" 0x00ce]

    set vreq [abi_append_val		\
	[abi_node $abi_key $key]	\
	[abi_node $abi_val $val]	\
    ]

    return [abi_node $kcm_app [abi_append_val	\
	[abi_async_context [lindex $args 0]]	\
	[abi_node $abi_item $vreq]		\
    ]]
}

proc abi_cm_tunnel_start {key val args} {
    set kcm_tst  "CM Tunnel Start"
    set abi_item [binary format "I" 0x00a3]

    set abi_key  [binary format "I" 0x00ca]
    set abi_val  [binary format "I" 0x00ce]

    set vreq [abi_append_val		\
	[abi_node $abi_key $key]	\
	[abi_node $abi_val $val]	\
    ]

    return [abi_node $kcm_tst [abi_append_val	\
	[abi_async_context [lindex $args 0]]	\
	[abi_node $abi_item $vreq]		\
    ]]
}

proc abi_cm_tunnel_stop {key val args} {
    set kcm_tsp  "CM Tunnel Stop"
    set abi_item [binary format "I" 0x00a3]

    set abi_key  [binary format "I" 0x00ca]
    set abi_val  [binary format "I" 0x00ce]

    set vreq [abi_append_val		\
	[abi_node $abi_key $key]	\
	[abi_node $abi_val $val]	\
    ]

    return [abi_node $kcm_tsp [abi_append_val	\
	[abi_async_context [lindex $args 0]]	\
	[abi_node $abi_item $vreq]		\
    ]]
}
