## Process this file with automake to produce Makefile.in
## For top-level sear directory

SUBDIRS = data scripts interfaces common loaders renderers terrain sky  src # gui tools

## Files that aren't included automatically (e.g. html, docs, etc.)
EXTRA_DIST =  \
	autogen.sh sear.spec

##manual copy game data to target dir
##and spit out some infos
install-data-local:
	mkdir -p $(DESTDIR)$(datadir)/sear
	mkdir -p $(DESTDIR)$(datadir)/sear/data
	mkdir -p $(DESTDIR)$(datadir)/sear/scripts
	cp -f data/*.png $(DESTDIR)$(datadir)/sear/data
	cp -f data/*.cfg $(DESTDIR)$(datadir)/sear/data
	cp -f scripts/*.script $(DESTDIR)$(datadir)/sear/scripts

	@echo 
	@echo "Sear is now installed in "$(prefix)
	@echo
	@echo "Using:"
	@echo "----------------------------"
	@echo "$> "$(prefix)"/bin/sear"
	@echo


# Rule to build tar-gzipped distribution package
$(PACKAGE)-$(VERSION).tar.gz: dist
