# Copyright (C) 2004-2005 VOCORD, Inc. <info@vocord.com>
# This file is part of the P3SS API/ABI/VERIFICATION system.

# The P3SS API/ABI/VERIFICATION system is free software; you can
# redistribute it and/or modify it under the terms of the
# GNU Lesser General Public License
# as published by the Free Software Foundation; either
# version 3 of the License, or (at your option) any later version.

# The part of the P3SS API/ABI/VERIFICATION system
# is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.

# You should have received a copy of the GNU Lesser General Public
# License along with the part of the P3SS API/ABI/VERIFICATION system;
# if not, see <http://www.gnu.org/licenses/>.

#
#       Sub-makefile for conform portion of the system.
#

ifeq "$(origin INSTALL_PATH)" "default"
TARGET_ROOT	?= /home/nfs/r
else
TARGET_ROOT	?= $(INSTALL_PATH)
endif

LIB_DB=-ldb-4

ifeq ($(OSTYPE), cygwin)
CROSS_COMPILE 	?= powerpc-eabi-
else
CROSS_COMPILE   ?= /home/nfs1/i686/bin/powerpc-unknown-linux-gnu-
endif

#CFLAGS+= -ffixed-r2 -msdata
CFLAGS+= -mtune=405 -mcpu=405 -msoft-float -mmultiple -mstring -mupdate

CFLAGS_SO+= -ffixed-r2
CFLAGS_SO+= -mtune=405 -mcpu=405 -msoft-float -mmultiple -mstring -mupdate
CFLAGS_SO+=-mno-sdata

ifeq "$(DEVEL)" "1"
INSTALL_PATH?=/home/nfs/r/NETCAM-DEVEL/VM4
else
INSTALL_PATH?=/home/nfs/r/VM4
endif
