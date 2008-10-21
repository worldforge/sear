## Process this file with automake to produce Makefile.in
## For top-level sear directory

SUBDIRS = autopackage data scripts interfaces common Eris loaders renderers environment gui guichan src

## Files that aren't included automatically (e.g. html, docs, etc.)
EXTRA_DIST =  \
	autogen.sh sear.spec sear.spec.in macros/*.m4

# Rule to build tar-gzipped distribution package
$(PACKAGE)-$(VERSION).tar.gz: dist


appdir = $(datadir)/applications/
app_DATA = sear.desktop
