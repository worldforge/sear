SUBDIRS = interfaces script engine src

EXTRA_DIST =  \
	autogen.sh

# Rule to build tar-gzipped distribution package
$(PACKAGE)-$(VERSION).tar.gz: dist

docs: sear.dox
	doxygen sear.dox
