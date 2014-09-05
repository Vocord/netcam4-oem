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

.PHONY: all clean

LIBS	+= \
	-L$(TARGET_ROOT)/opt/lib \
	-L$(TARGET_ROOT)/usr/lib \
	-L$(TARGET_ROOT)/lib	 \
	-L.$(ARCH)

INCLUDES += \
	-I$(TARGET_ROOT)/usr/include \
	-I$(TARGET_ROOT)/include

CROSS_SUFFIX  =
BIN_SUFFIX    =
ESUFFIX       ?=

#Check OS Arch
OSTYPE?=$(shell echo $$OSTYPE)
ifeq ($(OSTYPE), cygwin)
CROSS_SUFFIX  =.exe
BIN_SUFFIX    =.exe
ESUFFIX       =.exe
ARCH?=cygwin
endif

#Check Target Arch
ARCH?=host

ifeq ($(ARCH), host)
ARCH=$(shell uname -m)
TARGET_SELECTED=1
endif

ifeq ($(ARCH), arm)
include $(ROOT)/make/arm/target.mak
TARGET_SELECTED=1
endif

ifeq ($(ARCH), armeb)
include $(ROOT)/make/armeb/target.mak
TARGET_SELECTED=1
endif

ifeq ($(ARCH), ppc)
include $(ROOT)/make/ppc/target.mak
TARGET_SELECTED=1
endif

ifeq ($(ARCH), x86_64)
include $(ROOT)/make/x86_64/target.mak
TARGET_SELECTED=1
endif

ifeq ($(ARCH), i686)
include $(ROOT)/make/i686/target.mak
TARGET_SELECTED=1
endif

CFLAGS+=$(CFLAGS_COMMON)
CFLAGS_SO+=$(CFLAGS_COMMON)

ifeq "$(origin AS)" "default"
AS		= $(CROSS_COMPILE)as$(CROSS_SUFFIX)
endif

ifeq "$(origin AR)" "default"
AR		= $(CROSS_COMPILE)ar$(CROSS_SUFFIX)
endif

ifeq "$(origin CXX)" "default"
CXX		?= $(CROSS_COMPILE)g++$(CROSS_SUFFIX)
endif
GPP		?= $(CROSS_COMPILE)g++$(CROSS_SUFFIX)

GCC		?= $(CROSS_COMPILE)gcc$(CROSS_SUFFIX)
ifeq "$(origin CC)" "default"
CC		?= $(CROSS_COMPILE)gcc$(CROSS_SUFFIX)
endif

ifeq "$(origin CPP)" "default"
CPP		= $(CC) -E
endif

LD		?= $(CROSS_COMPILE)ld$(CROSS_SUFFIX)
NM		?= $(CROSS_COMPILE)nm$(CROSS_SUFFIX)

ifeq "$(origin LD)" "default"
LD		= $(CROSS_COMPILE)ld$(CROSS_SUFFIX)
endif

ifeq "$(origin NM)" "default"
NM		= $(CROSS_COMPILE)nm$(CROSS_SUFFIX)
endif

STRIP		?= $(CROSS_COMPILE)strip$(CROSS_SUFFIX)
OBJCOPY		?= $(CROSS_COMPILE)objcopy$(CROSS_SUFFIX)
OBJDUMP		?= $(CROSS_COMPILE)objdump$(CROSS_SUFFIX)

ifeq "$(origin STRIP)" "default"
STRIP		= $(CROSS_COMPILE)strip$(CROSS_SUFFIX)
endif
ifeq "$(origin OBJCOPY)" "default"
OBJCOPY		= $(CROSS_COMPILE)objcopy$(CROSS_SUFFIX)
endif
ifeq "$(origin OBJDUMP)" "default"
OBJDUMP		= $(CROSS_COMPILE)objdump$(CROSS_SUFFIX)
endif

#ifeq "$(DEVEL)" "0"
#INSTALL_PATH?=/home/nfs/r/NETCAM-DEVEL/
#endif
ifeq "$(DEBUG_LIB)" "1"
INSTALL_PATH_LIB?=$(INSTALL_PATH)/lib/debug
else
INSTALL_PATH_LIB?=$(INSTALL_PATH)/lib
endif
INSTALL_PATH_BIN?=$(INSTALL_PATH)/bin
INSTALL_PATH_SBIN?=$(INSTALL_PATH)/sbin

ifeq "$(DEBUG)" "1"
STRIP = true
endif

ifeq "$(MAKECMDGOALS)" "test"
ELEMENTARY_TEST=1
endif
