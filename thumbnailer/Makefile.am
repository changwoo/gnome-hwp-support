bin_PROGRAMS = hwp-thumbnailer

hwp_thumbnailer_SOURCES = hwp-thumbnailer.c
hwp_thumbnailer_CFLAGS = $(THUMBNAILER_CFLAGS)
hwp_thumbnailer_LDADD = $(THUMBNAILER_LIBS)

thumbnailerdir = $(datadir)/thumbnailers
thumbnailer_DATA = hwp-thumbnailer.thumbnailer

EXTRA_DIST =         	\
	$(thumbnailer_DATA)
