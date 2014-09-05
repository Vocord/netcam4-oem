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

#check for binutils path
BU_PATH=/usr/local/opt/binutils/bin
BU_TEST := $(shell test -d $(BU_PATH) && echo ok)

#check for compiler path
CC_PATH=/usr

GIT_REVISION := $(shell git log -n 1 --pretty=oneline | cut -d " " -f 1)

#check for CFLAGS
ifeq ($(ARCH), i686)
include $(ROOT)/make/i686/host.mak
endif

# What to use -mtune= or -mcpu= ?
TUNE_TEST := $(shell $(CC) --target-help 2>&1|grep mcpu 1>/dev/null && echo ok)
ifeq ($(TUNE_TEST), ok)
MTUNE=-mcpu
else
MTUNE=-mtune
endif

#Setup PATH
ifneq ($(OSTYPE), cygwin)
ifeq ($(BU_TEST), ok)
PATH=$(CC_PATH)/bin:$(BU_PATH):$(PATH_COMMON)
else
PATH=$(CC_PATH)/bin:$(PATH_COMMON)
endif
endif

#INCLUDES += -I../common

INSTALL   ?= install -D -g 501
RM	  ?= rm
LN	  ?= ln
CP	  ?= cp -f
MV	  ?= mv
CHMOD	  ?= chmod
CHOWN	  ?= chown
##############################
CFLAGS_COMMON+= -pipe -Wall -Wstrict-prototypes -Wcast-qual -Wcast-align
CFLAGS_COMMON+= -fschedule-insns2 -Wbad-function-cast -Wconversion

KCFLAGS+=-Wall -Wstrict-prototypes -Wcast-qual -fschedule-insns2 -Wbad-function-cast -Wcast-align -Wconversion
# -Wpacked
ifeq "$(SRC)" "kernel"
CFLAGS_COMMON+= $(KCFLAGS1)
CFLAGS_COMMON+= -D__KERNEL__ -I/usr/src/linux/include -Wno-trigraphs
CFLAGS_COMMON+= -fno-strict-aliasing -fno-common -fomit-frame-pointer -mpreferred-stack-boundary=2
CFLAGS_COMMON+= -nostdinc -iwithprefix include -O2
else
CFLAGS_COMMON+= $(CFLAGS1)
endif

#CFLAGS+=$(CFLAGS_COMMON)
PATH_COMMON=/usr/bin:/bin:/usr/X11R6/bin:/usr/local/bin

vpath %.h	../common

.PHONY: all clean depend
