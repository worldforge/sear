## Process this file with automake to produce Makefile.in
## For top-level sear directory

SUBDIRS = data scripts interfaces common loaders renderers environment gui src 

## Files that aren't included automatically (e.g. html, docs, etc.)
EXTRA_DIST =  \
	autogen.sh sear.spec sear.spec.in

# Rule to build tar-gzipped distribution package
$(PACKAGE)-$(VERSION).tar.gz: dist


appdir = $(datadir)/applications/
app_DATA = sear.desktop
