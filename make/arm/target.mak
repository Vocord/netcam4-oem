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

INSTALL_PATH ?= /home/nfs/r/armel/opt
ifeq "$(origin INSTALL_PATH)" "environment"
TARGET_ROOT	?= $(INSTALL_PATH)
else
TARGET_ROOT	?= /home/nfs/r/armel
endif

LIB_DB=-ldb-4
LIBS+=-L/home/nfs1/i686/targets/arm-softfloat-linux-gnueabi/lib

DEF_TPREFIX=arm-softfloat-linux-gnueabi-
#ifeq "$(shell which $(DEF_TPREFIX)gcc >/dev/null; echo $$?)" "0"
#CROSS_COMPILE 	?= $(shell dirname `which $(DEF_TPREFIX)gcc`)/$(DEF_TPREFIX)
#else
CROSS_COMPILE 	?= /home/nfs1/i686/bin/$(DEF_TPREFIX)
#endif

TARGET_ENDIAN?=little

ifeq "$(TARGET_ENDIAN)" "big"
CFLAGS+=-mbig-endian
ASMFLAGS+=-mbig-endian
LDFLAGS+=-EB
else
CFLAGS+=-mlittle-endian
ASMFLAGS+=-mlittle-endian
LDFLAGS+=-EL
endif

LDFLAGS+=-rpath=/lib -rpath=/usr/lib -rpath=/opt/lib
LIBS_ARM+=-lgcc_s
TARGET_CPU?=arm926ej-s
CFLAGS+= -mcpu=$(TARGET_CPU) -msoft-float

CFLAGS_SO+= $(CFLAGS)
CFLAGS_SO+= -fpic -fPIC
