#package ifneeded hist   1.0 [list source [file join $dir libxv/libhist.so]]
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

set hist_name [file join $dir libxv/libhist.so]
if { ![file exists $hist_name] } {
    set hist_name [file join $dir libxv/libhist.dll]
}
puts "$hist_name"
package ifneeded hist 1.0 [list load $hist_name]

package ifneeded abi-cm     1.0 [list source [file join $dir abi/abi-cm.tcl]]
package ifneeded abi-nodes  1.0 [list source [file join $dir abi/abi-nodes.tcl]]
package ifneeded abi-parser 1.0 [list source [file join $dir abi/abi-parser.tcl]]

