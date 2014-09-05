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

#
#  Description: Document suffix conversion Makefile include
#  		for automatic document production.
#
.PHONY: eps pdf clean all tar
ifeq ($(SCP),)
SCP=/bin/true
CVS=/bin/true
$(warning SCP Value not specified)
$(warning Using $(SCP) for SCP)
endif

ifeq ($(CVS_REV),)
$(warning CVS_REV Value not specified)
CVS_REV=1.1
$(warning Using $(CVS_REV) for CVS_REV)
endif

PATH=/usr/local/opt/gtk+-dep/2.5/bin:/bin:/usr/bin:/usr/X11R6/bin:/usr/games:/usr/local/bin:/sbin:/usr/sbin

LATEX2HTML_FLAGS=-split 2 -link 2 -show_section_numbers -html_version 4.0,table,i18n -scalable_fonts
	
#all: $(TARGET_EPS) $(TARGET) $(TARGET_DB)
#	scp -r D16_phobos_2 $(REMOTE_LOC)
############## XML transforms ##############
%.tex: %.xml
	$(DBLATEX) $(DBLATEXFLAGS) -t tex -o $@.tmp $<
	sed -e "s/\.png}/}/g" < $@.tmp > $@
	rm $@.tmp
$(TARGETS_DB_DVI):%.dvi: %.xml
	$(DBLATEX) $(DBLATEXFLAGS) -t dvi -o $@ $<
$(TARGETS_DB_PS):%.ps: %.xml
	$(DBLATEX) $(DBLATEXFLAGS) -t ps  -o $@ $<
#$(TARGETS_DB_PDF):%.pdf: %.xml
#	$(DBLATEX) $(DBLATEXFLAGS) -t pdf -o $@ $<
############################################
eps:	$(TARGET_EPS)
	@echo eps done

#pdf: eps D16_phobos_2.lyx
#	lyx -e latex D16_phobos_2.lyx
#	#patch < do_tex.diff

$(TARGET_DVI):%.dvi: %.tex
	latex $<

$(TARGETS_DVI_PS):%.ps: %.dvi
	dvips $< -o $@
%.dvi: %.tex
	latex $<

$(TARGET_PDF_LATEX):%.pdf: %.tex
	pdflatex --interaction=batchmode $< || /bin/true
	latex $(LATEX_FLAGS) $< || /bin/true
	dvipdf $(patsubst %.tex,%.dvi,$<)
	#$(SCP) $@ $(REMOTE_LOC)

$(TARGETS_DB_PDF):%.pdf: %.tex
	pdflatex --interaction=batchmode $< || /bin/true
	latex $(LATEX_FLAGS) $< || /bin/true
	dvipdf $(patsubst %.tex,%.dvi,$<)
	#$(SCP) $@ $(REMOTE_LOC)


$(patsubst %.pdf,%.dvi, $(TARGET_PDF_LATEX)):%.dvi: %.pdf

%.pdf: %.dvi
	dvipdf $<
	#$(SCP) $@ $(REMOTE_LOC)

%.pdf1: %.lyx
	tex2pdf $<

$(patsubst %.pdf,%.sgml, $(TARGET_PDF_LATEX)):%.sgml: %.lyx
	lyx -e docbook $<

%.tex: %.lyx
	lyx -e latex $<
	$(CVS) log -r$(CVS_REV): $< > $<.log
	$(SCP) $<.log $(REMOTE_LOC)

$(TARGET_DBS):%.sgml: %.lyx
	egrep -v "{graphics}|{multicol}|{pscyr}" $< | \
       	sed  \
	-e "s/^.textclass book/\\\\textclass docbook/" \
       	-e "s/^.textclass report/\\\\textclass docbook/" \
       	-e "s/^.textclass article/\\\\textclass docbook/" \
       	> temp.lyx
	lyx -e docbook temp.lyx
	mv temp.sgml $@


%.xml: %.sgml
	$(DBLATEX) $(DBLATEXFLAGS) -t xml $<
############################ HTML TARGETS ###################################
%.html: %.tex
	latex2html $(LATEX2HTML_FLAGS) $<
$(TARGET_HTML): %: %.tex
	rm -rf $@
	latex2html $(LATEX2HTML_FLAGS) $<
	rm -f $@/index.html $@/labels.pl $@/WARNINGS
	$(SCP) -r $@ $(REMOTE_LOC)
#############################################################################
$(TARGET_DB):%: %.sgml
	rm -rf $@
	db2html $< ||/bin/true
	scp -r $@ $(REMOTE_LOC)

%.eps: %.fig
	fig2dev -L eps $< > $@
#	fig2dev -L eps -n "Memory Map" $< > $@

%.svg: %.fig
	fig2dev -L svg -j $< > $@

$(SVG_BASE)/%.svg: %.fig
	fig2dev -L svg -j $< > $@

$(TARGET_SVG11):$(SVG_BASE)/%.svg: %.fig
	fig2dev -L svg -j $< > $@.tmp
	sed -e "s/\/\/W3C\/\/DTD SVG 1.1\/\/EN/\/\/W3C\/\/DTD SVG 1.0\/\/EN/" \
	-e "s/http:\/\/www.w3.org\/Graphics\/SVG\/1.1\/DTD\/svg11.dtd/http:\/\/www.w3.org\/TR\/2001\/REC-SVG-20010904\/DTD\/svg10.dtd/" \
       	< $@.tmp > $@
	rm $@.tmp

