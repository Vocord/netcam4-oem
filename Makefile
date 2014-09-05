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

.PHONY: all clean install tags todo cscope
ROOT=.

EXPORT_DIR1=p3ss-export
EXPORT_DIR=../$(EXPORT_DIR1)

all: cscope tags
	@for i in `find . -mindepth 1 -maxdepth 1 -type d` ; \
        do \
       	   $(MAKE) -C $$i DEBUG=$(DEBUG) ESUFFIX=$(ESUFFIX) ; \
	done

install:

install_test:

tags:
	ctags -R --exclude={pap,chap}-secrets --exclude=gui/bbox/{bbox,verelay,cam4} --exclude=.git --C-kinds=+lpx --C++-kinds=+lpx

cscope:
	@find -P -O3 `pwd` \! -wholename */.git/* -a -type f -a \( -name '*.[ch]' -o -name '*.cpp' -o -name '*.cxx' -o -name '*.cc' \) >cscope.files
	@cscope -b

export:
	rm -rf $(EXPORT_DIR)

update:
	git submodule update

clean:
	rm -f *~ TAGS tags cscope.* 
	@for i in `find . -mindepth 1 -maxdepth 1 -type d` ; \
        do \
       	   $(MAKE) -C $$i DEBUG=$(DEBUG) ESUFFIX=$(ESUFFIX) clean ; \
	done
