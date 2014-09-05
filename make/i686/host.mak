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

#Check host
CC_PATH=/usr/local/opt/gcc
CC_TEST := $(shell test -d $(CC_PATH) && echo ok)
LIBS1=-lgcc_s

ifneq ($(CC_TEST), ok)
CC_PATH=/usr/local/opt/gcc-3.3.2-i686
CC_TEST := $(shell test -d $(CC_PATH) && echo ok)
LIBS1=-lgcc_s
CFLAGS_COMMON=--param max-inline-insns-rtl=150

ifneq ($(CC_TEST), ok)
CC_PATH=/usr/local/opt/gcc-3.3.1-i686
CC_TEST := $(shell test -d $(CC_PATH) && echo ok)
LIBS1=-lgcc_s
CFLAGS_COMMON=--param max-inline-insns-rtl=150

ifneq ($(CC_TEST), ok)

CC_PATH=/usr/local/opt/gcc-3.3.3-i686
CC_TEST := $(shell test -d $(CC_PATH) && echo ok)
LIBS1=-lgcc_s
CFLAGS_COMMON=--param max-inline-insns-rtl=150

ifneq ($(CC_TEST), ok)

CC_PATH=/usr/local/opt/gcc-3.3.3
CC_TEST := $(shell test -d $(CC_PATH) && echo ok)
CFLAGS_COMMON=--param max-inline-insns-rtl=150

ifneq ($(CC_TEST), ok)

CC_PATH=/usr/local/opt/gcc-3.2.2-i686
CC_TEST := $(shell test -d $(CC_PATH) && echo ok)
CFLAGS_COMMON=

ifneq ($(CC_TEST), ok)

CC_PATH=/usr/local/opt/gcc-3.2.1
CC_TEST := $(shell test -d $(CC_PATH) && echo ok)
CFLAGS_COMMON=

ifneq ($(CC_TEST), ok)
CC_PATH=/usr
endif

endif
endif
endif
endif
endif
endif

#P4
CFLAGS1=-march=pentium4 $(MTUNE)=pentium4
KCFLAGS1=-march=i686 $(MTUNE)=i686
CFLAGS_TEST := $(shell fgrep Pentium\(R\)\ 4 /proc/cpuinfo >/dev/null && echo ok)

ifneq ($(CFLAGS_TEST), ok)

#P3
CFLAGS1= -march=pentium3 $(MTUNE)=pentium3
KCFLAGS1=-march=pentium3 $(MTUNE)=pentium3
CFLAGS_TEST := $(shell fgrep Pentium\ III\  /proc/cpuinfo >/dev/null && echo ok)

ifneq ($(CFLAGS_TEST), ok)

#P2
CFLAGS1= -march=pentium2 $(MTUNE)=pentium2
KCFLAGS1=-march=pentium2 $(MTUNE)=pentium2
CFLAGS_TEST := $(shell fgrep Pentium\ II\  /proc/cpuinfo >/dev/null && echo ok)

ifneq ($(CFLAGS_TEST), ok)

#P2 Celeron
CFLAGS1=-march=pentium2 $(MTUNE)=pentium2
KCFLAGS1=-march=pentium2 $(MTUNE)=pentium2
CFLAGS_TEST := $(shell fgrep Pentium\ II\  /proc/cpuinfo >/dev/null && echo ok)

ifneq ($(CFLAGS_TEST), ok)

#VIA
CFLAGS1=-march=i486 $(MTUNE)=i486
KCFLAGS1=-march=i486 $(MTUNE)=i486
CFLAGS_TEST := $(shell fgrep Ezra /proc/cpuinfo >/dev/null && echo ok)

ifneq ($(CFLAGS_TEST), ok)

CFLAGS1=-march=i686 $(MTUNE)=i686
KCFLAGS1=-march=i686 $(MTUNE)=i686
endif
endif
endif
endif
endif

##############################
# Test for mmx, sse, sse2, 3Dnow support
CFLAGS_TEST := $(shell fgrep mmx  /proc/cpuinfo >/dev/null && echo ok)
ifeq ($(CFLAGS_TEST), ok)
CFLAGS1+= -mmmx
DEFINES+= -DUSE_MMX_OK=1
endif

CFLAGS_TEST := $(shell fgrep sse /proc/cpuinfo >/dev/null && echo ok)
ifeq ($(CFLAGS_TEST), ok)
DEFINES+= -DUSE_SSE_OK=1 
CFLAGS1+= -msse -mfpmath=sse
endif

CFLAGS_TEST := $(shell fgrep sse2 /proc/cpuinfo >/dev/null && echo ok)
ifeq ($(CFLAGS_TEST), ok)
DEFINES+= -DUSE_SSE2_OK=1 
CFLAGS1+= -msse2
endif

CFLAGS_TEST := $(shell fgrep 3dnow /proc/cpuinfo >/dev/null && echo ok)
ifeq ($(CFLAGS_TEST), ok)
DEFINES+= -DUSE_3DNOW_OK=1 
CFLAGS1+= -m3dnow
endif
