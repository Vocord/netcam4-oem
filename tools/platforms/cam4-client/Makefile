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
vpath   %.c      $(ROOT)/lib/abi
vpath   %.c      $(ROOT)/lib/avi
vpath   %.c      $(ROOT)/lib/ui
vpath   %.c      $(ROOT)/lib/mcast
vpath   %.c      $(ROOT)/lib/platforms/cam4
vpath   %.h      $(ROOT)/include
vpath   %.c      $(ROOT)/tools/mcastd
vpath   %.h      $(ROOT)/tools/mcastd
vpath   %.h      $(ROOT)/tools/platforms/cam4
vpath	%.c	 $(ROOT)/lib/gis
vpath	%.h	 $(ROOT)/lib/gis

include $(ROOT)/make/host.mak
LIBS=
include $(ROOT)/make/target.mak

INSTALL_PATH?=/home/nfs/r

revision := $(shell git log -n 1 --pretty=oneline | cut -d " " -f 1)

ifeq "$(ARCH)" "ppc"
DEFINES+= -DTARGET_CAM4=1
ESUFFIX=
else
DEFINES+= -UTARGET_CAM4 -D__LINUX__
ESUFFIX=
endif

INCLUDES+= -I$(ROOT)/include -I. -I$(ROOT)/tools/mcastd -I$(ROOT)/tools/platforms/cam4 -I$(ROOT)/tools/CamCtrl

LIBS+= -pthread -lpthread -L/usr/X11R6/lib -lm
#LIBS+= -pthread -lpthread -lX11 -lXext -lXv -lXt -L/usr/X11R6/lib -lm

TARG_PS= \
	cam4_ps$(ESUFFIX)

TARG_XCLIENT= \
	cam4_ps_Xclient$(ESUFFIX)       \
	cam4-jpeg-data-cl$(ESUFFIX)

TARG_LIB= \
	cam4_ps_lib.a \
	cam4_ps_lib

ifneq "$(ARCH)" "arm"
TARG_BIN=$(TARG_PS) $(TARG_XCLIENT)
else
TARG_BIN=$(TARG_PS)
endif

ifneq "$(MODE)" "lib"
TARG=$(TARG_BIN) $(TARG_DEBUG)
else
TARG=$(TARG_LIB)
DEFINES+= -DCAM4_PS_LIB=1
endif

#CFLAGS += -g -O0 -DREVI="$(revision)" -Wno-conversion
CFLAGS += -g -O3 -DREVI="$(revision)" -Wno-conversion -pthread
CFLAGS_SSE =  -O3 -msse2 -flax-vector-conversions -mincoming-stack-boundary=4
# -std=gnu99

ifeq "$(ARCH)" "i686"
OBJS_DEB= debayer_sse.o
else
OBJS_DEB=debayer_stub.o
endif

OBJS_COMMON =           \
        signals.o       \

OBJS_CAMCTRL = \
        CCSD.o		\
        DebugOut.o	\
        BuildHist.o	\
        StatData.o \
        Conv10Bit.o \
        Sun.o \
        solar-position.o \
        calibtxt.o

OBJS_GFX = \
        osd-yuyv.o	\
        osd-common.o	\

OBJS_ABI = \
        abi-helpers.o   \
        abi-reader.o    \
        abi-writer.o    \
        mcast-cl.o      \
        net-sockets.o	\
        cam4-cmd-cl.o   \

OBJS= \
        $(OBJS_COMMON)  	\
        $(OBJS_GFX)     	\
        $(OBJS_ABI)     	\
	$(OBJS_CAMCTRL1)	\
	$(OBJS_DEB)		\
        avi-file-writer.o	\
        cam4_ps-lut.o       	\
        cam4_ps.o       	\
        cam4_ps_Xclient.o	\
        cam4-jpeg-data-cl.o	\
	cam4_ps_lib.o

all: depend $(TARG)

install: depend install_bin

include $(ROOT)/make/common.mak
clean: clean_common

.$(ARCH)/cam4_ps_lib.a: cam4_ps.o $(OBJS_COMMON) $(OBJS_ABI) $(OBJS_DEB) $(OBJS_CAMCTRL1) avi-file-writer.o
	$(AR) rcs $@ $^

.$(ARCH)/cam4_ps_lib: cam4_ps_lib.o cam4_ps_lib.a

#object deps
.$(ARCH)/debayer_sse.o:			 CFLAGS+=$(CFLAGS_SSE)
.$(ARCH)/cam4_ps$(ESUFFIX):              cam4_ps.o cam4_ps-lut.o $(OBJS_COMMON) $(OBJS_ABI) $(OBJS_DEB) $(OBJS_CAMCTRL1) avi-file-writer.o 
.$(ARCH)/cam4-jpeg-data-cl$(ESUFFIX):    cam4-jpeg-data-cl.o $(OBJS_COMMON) $(OBJS_ABI)
.$(ARCH)/cam4_ps_Xclient$(ESUFFIX):      cam4_ps_Xclient.o $(OBJS_COMMON) $(OBJS_GFX) -lX11 -lXt -lXv -lXext
