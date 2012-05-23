bin_PROGRAMS = hwp-thumbnailer

AM_CFLAGS = $(WARN_CFLAGS)

hwp_thumbnailer_SOURCES = hwp-thumbnailer.c
hwp_thumbnailer_CPPFLAGS = $(THUMBNAILER_CFLAGS)
hwp_thumbnailer_LDADD = $(THUMBNAILER_LIBS)

thumbnailerdir = $(datadir)/thumbnailers
thumbnailer_DATA = hwp-thumbnailer.thumbnailer

EXTRA_DIST = $(thumbnailer_DATA)

-include $(top_srcdir)/git.mk
