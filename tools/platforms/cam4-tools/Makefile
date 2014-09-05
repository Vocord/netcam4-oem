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

SRC_LEVEL=3
ROOT=../../..

vpath   %.c      $(ROOT)/lib/parsers
vpath   %.c      $(ROOT)/lib/os-helpers
vpath   %.h      $(ROOT)/include

include $(ROOT)/make/host.mak
LIBS=
include $(ROOT)/make/target.mak

INSTALL_PATH?=/home/nfs/r

revision := $(shell git log -n 1 --pretty=oneline | cut -d " " -f 1)

INCLUDES+= -I$(ROOT)/include -I.

LIBS+= 

TARG_BIN= \
	cam4-tcpdump$(ESUFFIX)       \

TARG= \
	$(TARG_BIN)

CFLAGS += -g -O3 -DREVI="$(revision)" -Wno-conversion -pthread
# -std=gnu99
OBJS_COMMON =  \
               \

OBJS= \
        $(OBJS_COMMON)  	\
        net-player.o

all: depend $(TARG)

install: depend install_bin

include $(ROOT)/make/common.mak
clean: clean_common

#object deps
.$(ARCH)/cam4-tcpdump$(ESUFFIX):              net-player.o
