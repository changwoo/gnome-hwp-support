AM_CPPFLAGS =						\
	-DGETTEXT_PACKAGE=\"$(GETTEXT_PACKAGE)\"	\
	-DLOCALEDIR=\"$(datadir)/locale\"		\
	-I$(top_srcdir)					\
	-I$(top_builddir)				\
	$(PROPERTIES_CFLAGS)				\
	$(DISABLE_DEPRECATED)				\
	$(WARN_CFLAGS)

nautilusextensiondir=$(libdir)/$(NAUTILUS_EXTENSION_DIR)
nautilusextension_LTLIBRARIES = libnautilus-hwp-properties.la

libnautilus_hwp_properties_la_CFLAGS = -I$(top_srcdir)
libnautilus_hwp_properties_la_SOURCES =		\
	props-data.c				\
	props-data.h				\
	hwp-properties-model-provider.c		\
	hwp-properties-model.c			\
	hwp-properties-model.h			\
	hwp-properties-module.c

libnautilus_hwp_properties_la_LIBADD =		\
	$(PROPERTIES_LIBS)

libnautilus_hwp_properties_la_LDFLAGS = -module -avoid-version -no-undefined

noinst_PROGRAMS = test
test_SOURCES = \
	props-data.c				\
	props-data.h				\
	test.c
test_LDADD = \
	$(PROPERTIES_LIBS)

-include $(top_srcdir)/git.mk
