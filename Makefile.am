## Process this file with automake to produce Makefile.in
## For top-level sear directory

SUBDIRS = data scripts interfaces common loaders renderers environment gui src 

## Files that aren't included automatically (e.g. html, docs, etc.)
EXTRA_DIST =  \
	autogen.sh sear.spec

##manual copy game data to target dir
##and spit out some infos
install-data-local:
	mkdir -p $(DESTDIR)$(datadir)/sear
	mkdir -p $(DESTDIR)$(datadir)/sear/data
	mkdir -p $(DESTDIR)$(datadir)/sear/scripts
	cp -f $(top_srcdir)/data/*.cfg $(DESTDIR)$(datadir)/sear/data
	cp -f $(top_srcdir)/scripts/*.script $(DESTDIR)$(datadir)/sear/scripts

	@echo 
	@echo "Sear is now installed in "$(prefix)
	@echo
	@echo "Using:"
	@echo "----------------------------"
	@echo "$> "$(prefix)"/bin/sear"
	@echo

uninstall-local:
	rm -f $(DESTDIR)$(datadir)/sear/data/*.cfg
	rm -f $(DESTDIR)$(datadir)/sear/scripts/*.script

# Rule to build tar-gzipped distribution package
$(PACKAGE)-$(VERSION).tar.gz: dist
