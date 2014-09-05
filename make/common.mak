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
# <http://www.gnu.org/licenses/>.

#
#       Sub-makefile for conform portion of the system.
#

.PHONY:	install_lib install_bin install all depend

OD_FLAGS?=-d

ifneq "$(ARCH)" ""
#GPATH=.$(ARCH)
vpath   %	 .$(ARCH)/
vpath   %.lo	 .$(ARCH)/
vpath   %.ot	 .$(ARCH)/
vpath   %.so	 .$(ARCH)/

OBJ_ARCH+=$(join $(addsuffix .$(ARCH)/, $(dir $(OBJS))), $(notdir $(OBJS)))
#OBJ_ARCH=$(foo:.o=.c)
C_ARCH_O=$(patsubst %.o,%.c,  $(filter %.o, $(OBJS)))
C_ARCH_OT=$(patsubst %.ot,%.c,$(filter %.ot,$(OBJS)))
C_ARCH_KO=$(patsubst %.ko,%.c,$(filter %.ko,$(OBJS)))
C_ARCH_LO=$(patsubst %.lo,%.c,$(filter %.lo,$(OBJS)))

.$(ARCH):
	mkdir -p $@
endif

ifeq "$(DEPRECATED)" "0"
CFLAGS+=-Wno-deprecated-declarations
CFLAGS_SO+=-Wno-deprecated-declarations
endif

# Clear Debug Targs if no debug
ifneq "$(DEBUG)" "1"
TARG_DEBUG=
endif
#OBJS := $(patsubst %.c,%.o,$(wildcard *.c))


#Build C dependency
depend:
	$(MAKE) ARCH=$(ARCH) VM4=$(VM4) DEBUG=$(DEBUG) DEPRECATED=$(DEPRECATED) ESUFFIX=$(ESUFFIX) depend_real

depend_real: .$(ARCH) .$(ARCH)/.ko.depend .$(ARCH)/.lo.depend .$(ARCH)/.ot.depend .$(ARCH)/.o.depend

.$(ARCH)/.ko.depend:  $(C_ARCH_KO)
ifeq ($(C_ARCH_KO),)
	echo -n > $@
else
	$(GCC) $(INCLUDES) $(DEFINES) -MM $^ | sed -e 's=^\([^:]\+\).o:=.$(ARCH)/\1.ko:=g' > $@
endif

.$(ARCH)/.lo.depend:  $(C_ARCH_LO)
ifeq ($(C_ARCH_LO),)
	echo -n > $@
else
	$(GCC) $(INCLUDES) $(DEFINES) -MM $^ | sed -e 's=^\([^:]\+\).o:=.$(ARCH)/\1.lo:=g' > $@
endif

.$(ARCH)/.ot.depend:  $(C_ARCH_OT)
ifeq ($(C_ARCH_OT),)
	echo -n > $@
else
	$(GCC) $(INCLUDES) $(DEFINES) -MM $^ | sed -e 's=^\([^:]\+\).o:=.$(ARCH)/\1.ot:=g' > $@
endif

.$(ARCH)/.o.depend:  $(C_ARCH_O)
ifeq ($(C_ARCH_O),)
	echo -n > $@
else
	$(GCC) $(INCLUDES) $(DEFINES) -MM $^ | sed -e 's=^\([^:]\+\).o:=.$(ARCH)/\1.o:=g' > $@
endif

ifdef EXPORTED_DRIVER
$(OBJB)::%.o:%.oc
	cp $< $@
else
%.oc:%.c
	$(GCC) $(INCLUDES) $(DEFINES) $(CFLAGS) -c $< -o $@
$(OBJB):%.o:%.c
	$(GCC) $(INCLUDES) $(DEFINES) $(CFLAGS) -c $< -o $@
endif

$(filter %.ot,$(OBJ_ARCH)):.$(ARCH)/%.ot:%.c
	$(GCC) $(CFLAGS) $(INCLUDES) -c -o $@ $< -DELEMENTARY_TEST=1

$(filter %.lo,$(OBJ_ARCH)):.$(ARCH)/%.lo:%.c
	$(GCC) $(CFLAGS_SO) $(INCLUDES)  $(DEFINES_SO) -fPIC -DCOMPILE_LO=1 -c -o $@ $<

$(OBJA):%.o: %.S
	$(GCC) $(ASMDEFINES) $(ASMFLAGS) $(INCLUDES) -c $< -o $@

%.srec: %
	$(OBJCOPY) --srec-forceS3 -O srec $< $@

.$(ARCH)/%.img: .$(ARCH)/%
	$(OBJCOPY) $(IMG_FLAGS) -O binary $< $@

%.img: %
	$(OBJCOPY) $(IMG_FLAGS) -O binary $< $@

$(TARG_STATIC_BIN) $(TARG_STATIC_TESTS):%:%.o
	$(GCC) -static -o $@ $(addprefix -Xlinker , $(LDFLAGS)) $(STATIC_LIBS) $^ ${LIBS_TAIL}
	$(STRIP) $@
	chmod a+rx $@

.$(ARCH)/%$(ESUFFIX):.$(ARCH)/%.o
	$(GCC) -o $@ $(LIBS)  $(addprefix -Xlinker , $(LDFLAGS)) $^ ${LIBS_TAIL}

#elementary tests obj -> executable binary RULE
.$(ARCH)/%.test%$(ESUFFIX) .$(ARCH)/%$(ESUFFIX):
	$(GCC) -o $@ $^ $(LIBS) $(LIBS_OT) $(addprefix -Xlinker , $(LDFLAGS)) ${LIBS_TAIL}
	@echo PATH=$(PATH)

.$(ARCH)/lib%.so:%.lo
	$(GCC) -shared -o $@ $^ ${LIBS} ${LIBS_SO} $(addprefix -Xlinker , $(LDFLAGS_SO)) ${LIBS_TAIL}
#	$(LD) -shared -o $@ $^ ${LIBS_SO} $(LDFLAGS_SO)

.$(ARCH)/%.so:%.lo
	$(GCC) -shared -o $@ $^ ${LIBS} ${LIBS_SO} $(addprefix -Xlinker , $(LDFLAGS_SO)) ${LIBS_TAIL}

$(filter %.o,$(OBJ_ARCH)):.$(ARCH)/%.o:%.c
	$(GCC) $(INCLUDES) $(DEFINES) $(CFLAGS) -c $< -o $@

$(filter %.o,$(OBJS1)):%.o:%.c
	$(GCC) $(INCLUDES) $(DEFINES) $(CFLAGS) -c $< -o $@

$(filter %.ko,$(OBJS)):%.ko:%.c
	$(GCC) $(INCLUDES) $(KDEFINES) $(KCFLAGS) -c $< -o $@
	
$(filter %.ko,$(OBJS1)):%.ko:%.c
	$(GCC) $(INCLUDES) $(KDEFINES) $(KCFLAGS) -c $< -o $@

%.ko.dis: %.ko
	$(OBJDUMP) $(OD_FLAGS) -S $< > $@

.$(ARCH)/%.lo.dis: .$(ARCH)/%.lo
	$(OBJDUMP) $(OD_FLAGS) -S $< > $@

.$(ARCH)/%.ot.dis: .$(ARCH)/%.ot
	$(OBJDUMP) $(OD_FLAGS) -S $< > $@

.$(ARCH)/%.o.dis: .$(ARCH)/%.o
	$(OBJDUMP) $(OD_FLAGS) -S $< > $@

%.dis: %
	$(OBJDUMP) $(OD_FLAGS) -S $< > $@

#$(filter %.dis,$(TARG_DEBUG)):.$(ARCH)/%$(ESUFFIX).dis: .$(ARCH)/%$(ESUFFIX)
#	$(OBJDUMP) $(OD_FLAGS) -S $< > $@

$(filter %.map,$(TARG_DEBUG)):%.map: %
	$(OBJDUMP) -t $< > $@

install_local: $(TARG)
	$(STRIP) $^
	cp $^ $(INSTALL_PATH_LOCAL)

install_tests: $(filter %, $(TARG_TESTS) $(TARG_STATIC_TESTS))
	for i in $(notdir $^); \
	do \
		echo Install $$i && \
		$(CP) .$(ARCH)/$$i $(INSTALL_PATH_BIN)/$$i && \
		$(STRIP) $(INSTALL_PATH_BIN)/$$i ; \
	done

install_bin: $(filter %, $(TARG_BIN) $(TARG_STATIC_BIN))
	for i in $(notdir $^); \
	do \
		echo Install $$i && \
		$(CP) .$(ARCH)/$$i $(INSTALL_PATH_BIN)/$$i && \
		$(STRIP) $(INSTALL_PATH_BIN)/$$i ; \
	done

install_lib: $(filter %.so, $(TARG_LIB) $(TARG_STATIC_LIB))
	for i in $(notdir $^); \
       	do \
       		echo Install $$i && \
		$(CP)  .$(ARCH)/$$i $(INSTALL_PATH_LIB)/$$i$ && \
		$(STRIP) $(INSTALL_PATH_LIB)/$$i ; \
	done

install_debug: $(TARG_DEBUG)
	for i in $^ ; \
	do \
		echo Install $$i$ && \
		$(RM) $(INSTALL_PATH_DEBUG)/$$i ; \
		$(CP) $$i $(INSTALL_PATH_DEBUG)/$$i ; \
	done

clean_common:
	$(RM) *.o *.ot *.lo *.out .*~ *~ .depend* $(TARG_BIN) $(TARG_STATIC_BIN) $(TARG_LIB) $(TARG_CLEAN)
	$(RM) -r .$(ARCH)

ifeq "$(shell test -e .$(ARCH)/.ko.depend; echo $$?)" "0"
$(info 'Include Dependencies' )
include .$(ARCH)/.ko.depend
endif

ifeq "$(shell test -e .$(ARCH)/.lo.depend; echo $$?)" "0"
$(info 'Include Dependencies' )
include .$(ARCH)/.lo.depend
endif

ifeq "$(shell test -e .$(ARCH)/.ot.depend; echo $$?)" "0"
$(info 'Include Dependencies' )
include .$(ARCH)/.ot.depend
endif

ifeq "$(shell test -e .$(ARCH)/.o.depend; echo $$?)" "0"
$(info 'Include Dependencies' )
include .$(ARCH)/.o.depend
endif
