bin_PROGRAMS = hwp-thumbnailer

hwp_thumbnailer_SOURCES = hwp-thumbnailer.c
hwp_thumbnailer_CFLAGS = $(THUMBNAILER_CFLAGS)
hwp_thumbnailer_LDADD = $(THUMBNAILER_LIBS)

thumbnailerdir = $(datadir)/thumbnailers
thumbnailer_DATA = hwp-thumbnailer.thumbnailer

schemadir = $(GCONF_SCHEMA_FILE_DIR)
schema_DATA = hwp-thumbnailer.schemas

install-data-local:
	if test -z "$(DESTDIR)" ; then \
	for p in $(schema_DATA) ; do \
	GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) $(GCONFTOOL) --makefile-install-rule $(top_builddir)/thumbnailer/$$p >&1 > /dev/null; \
	done \
	fi

EXTRA_DIST =         	\
	$(thumbnailer_DATA) \
	$(schema_DATA)
