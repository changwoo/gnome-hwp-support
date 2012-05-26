INCLUDES=					\
	-DGNOMELOCALEDIR=\"$(datadir)/locale\"	\
	-I$(top_srcdir)				\
	-I$(top_builddir)			\
	$(PROPERTIES_CFLAGS)			\
	$(DISABLE_DEPRECATED)			\
	$(WARN_CFLAGS)

noinst_LTLIBRARIES = libhwpproperties.la

libhwpproperties_la_SOURCES=			\
	props-data.c				\
	props-data.h				\
	hwp-properties-view.c			\
	hwp-properties-view.h

nautilusextensiondir=$(libdir)/$(NAUTILUS_EXTENSION_DIR)
nautilusextension_LTLIBRARIES = libhwp-properties-page.la

libhwp_properties_page_la_CFLAGS = -I$(top_srcdir)
libhwp_properties_page_la_SOURCES = 	\
	hwp-properties-main.c

libhwp_properties_page_la_LIBADD = 		\
	libhwpproperties.la			\
	$(PROPERTIES_LIBS)

libhwp_properties_page_la_LDFLAGS = -module -avoid-version -no-undefined

noinst_PROGRAMS = test
test_SOURCES = test.c
test_LDADD = libhwpproperties.la $(PROPERTIES_LIBS)

-include $(top_srcdir)/git.mk
