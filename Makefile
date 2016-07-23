include .config

# --- Change the settings below to suit your environment. See explanations in
# --- the build documentation for more details. ---

PREFIX ?= /usr/local
MODDIR ?= $(PREFIX)/lib/lua/5.2
DOCDIR ?= $(PREFIX)/share/doc/luahpdf
LUALIB ?= -llua5.2
LUAINC ?= -I/usr/include/lua5.2
HPDFLIB ?= -lhpdf
HPDFINC ?=
LUA ?= lua
LUAC ?= luac
PLATFORM ?= linux
# PLATFORM=macosx

# --- End of user settings, no need to change anything below this line. ---

TARGET=hpdf.so
PACKAGE=luahpdf-1.5
TAR=$(PACKAGE).tar.gz
ZIP=$(PACKAGE).zip
linux_COMPILE=cc -DHPDF_SHARED $(LUAINC) $(HPDFINC) -Wall -O2 -fomit-frame-pointer -shared -fPIC -c -o $@ $<
linux_LINK=cc -shared -fPIC -o $@ $^ $(HPDFLIB) -lz -lpng -lm
linux_REPORT=ldd ./$(TARGET)
macosx_COMPILE=cc -DHPDF_SHARED $(LUAINC) $(HPDFINC) -DLUA_USE_MACOSX -Wall -O2 -fomit-frame-pointer -fPIC -c -o $@ $<
macosx_LINK=cc -bundle -undefined dynamic_lookup -DLUA_USE_MACOSX $(HPDFLIB) -lz -lpng -o hpdf.so hpdf.o
macosx_REPORT=otool -L ./$(TARGET)

PDF = \
	demo/arc_demo.pdf \
	demo/encryption_demo.pdf \
	demo/ext_gstate_demo.pdf \
	demo/font_demo.pdf \
	demo/grid_demo.pdf \
	demo/hello_demo.pdf \
	demo/line_demo.pdf \
	demo/link_demo.pdf \
	demo/logo_demo.pdf \
	demo/outline_demo.pdf \
	demo/permission_demo.pdf \
	demo/slide_show_demo.pdf \
	demo/text_demo.pdf

HTML = \
	doc/html/binding-changes.html \
	doc/html/binding-notes.html \
	doc/html/build.html \
	doc/html/change-log.html \
	doc/html/demos.html \
	doc/html/index.html \
	doc/html/license.html

$(TARGET) : hpdf.o
	$($(PLATFORM)_LINK)
	$($(PLATFORM)_REPORT)

hpdf.o : hpdf.c
	$($(PLATFORM)_COMPILE)

dump :
	cc -E -dM -ansi -DHPDF_SHARED -pedantic -Wall -O2 $(CFLAGS) $(LUAINC) $(HPDFINC) -shared hpdf.c > $@

test : $(TARGET)
	$(LUA) -e "package.path=[[]] package.cpath=[[./?.so;./?.dll]] local hpdf = require [[hpdf]] print(hpdf.VERSION_TEXT)"

all : test

install : $(TARGET) doc
	install -m 0755 -d "$(DESTDIR)$(MODDIR)"
	install -m 0755 $(TARGET) "$(DESTDIR)$(MODDIR)"
	install -m 0755 -d "$(DESTDIR)$(DOCDIR)"
	install -m 0755 -d "$(DESTDIR)$(DOCDIR)/text"
	install -m 0755 -d "$(DESTDIR)$(DOCDIR)/html"
	install -m 0644 README.md doc/*.lua "$(DESTDIR)$(DOCDIR)"
	install -m 0644 doc/text/*.txt "$(DESTDIR)$(DOCDIR)/text"
	install -m 0644 doc/html/*.html doc/html/*.css doc/html/*.png "$(DESTDIR)$(DOCDIR)/html"

doc : $(HTML)

demo : $(PDF)

package : clean doc
	rm -fr $(PACKAGE)
	mkdir -p $(PACKAGE)/doc/text $(PACKAGE)/doc/html $(PACKAGE)/demo $(PACKAGE)/windows
	cp doc/*.lua $(PACKAGE)/doc
	cp doc/text/*.txt $(PACKAGE)/doc/text
	cp doc/html/*.css doc/html/*.png doc/html/*.html $(PACKAGE)/doc/html
	cp demo/* $(PACKAGE)/demo
	cp windows/* $(PACKAGE)/windows
	cp README Makefile hpdf.c $(PACKAGE)
	tar czvf $(TAR) $(PACKAGE)
	zip -r $(ZIP) $(PACKAGE)
	rm -fr $(PACKAGE)

clean :
	rm -f $(PDF) $(HTML) $(TARGET) $(TAR) $(ZIP) *.o dump

.PHONY : main doc demo clean lib package test install

$(HTML): doc/html/%.html: doc/text/%.txt
	$(LUA) doc/txt2html.lua < $< > $@

$(PDF): %.pdf: %.lua $(TARGET)
	@$(LUAC) -l -p $< | grep SETGLOBAL; true
	$(LUA) -e 'package.path="demo/?.lua;./?.lua" package.cpath="./?.so"' $<
