## Process this file with automake to produce Makefile.in
## For top-level sear directory

SUBDIRS = common loaders renderers terrain sky gui src tools

## Files that aren't included automatically (e.g. html, docs, etc.)
EXTRA_DIST =  \
	sear.spec

## ensure, that all  grafx get into the dist
dist-hook:
	mkdir -p $(distdir)/data
	mkdir -p $(distdir)/scripts
	cp -f data/*.bmp $(distdir)/data
	cp -f data/*.cal $(distdir)/data
	cp -f data/*.cfg.in $(distdir)/data
	cp -f data/*.png $(distdir)/data
	cp -f scripts/*.script.in $(distdir)/scripts

##manual copy game data to target dir
##and spit out some infos
install-data-local:
	mkdir -p $(DESTDIR)$(datadir)/sear
	mkdir -p $(DESTDIR)$(datadir)/sear/data
	mkdir -p $(DESTDIR)$(datadir)/sear/scripts
	cp -f data/*.bmp $(DESTDIR)$(datadir)/sear/data
	cp -f data/*.png $(DESTDIR)$(datadir)/sear/data
	cp -f data/*.cal $(DESTDIR)$(datadir)/sear/data
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

# Rule to build RPM distribution package
rpm: $(PACKAGE)-$(VERSION).tar.gz
	rpm -ta $(PACKAGE)-$(VERSION).tar.gz
