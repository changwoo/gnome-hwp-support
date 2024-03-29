AC_PREREQ(2.63)
AC_INIT([gnome-hwp-support], [0.2.0], [https://github.com/changwoo/gnome-hwp-support/issues], [gnome-hwp-support])

AC_CONFIG_SRCDIR([thumbnailer])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([1.11 dist-xz no-dist-gzip tar-ustar foreign])
AM_MAINTAINER_MODE([enable])
AM_SILENT_RULES([yes])

LT_PREREQ([2.2.6])
LT_INIT([dlopen disable-static])

AC_PROG_CC
AC_PROG_CC_C99
AM_PROG_CC_C_O
AC_HEADER_STDC

IT_PROG_INTLTOOL([0.35.0])

GETTEXT_PACKAGE=gnome-hwp-support
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],["$GETTEXT_PACKAGE"],[Gettext package])
AM_GLIB_GNU_GETTEXT

PKG_CHECK_MODULES([THUMBNAILER],[libgsf-1 gdk-pixbuf-2.0])
AC_SUBST(THUMBNAILER_CFLAGS)
AC_SUBST(THUMBNAILER_LIBS)

PKG_CHECK_MODULES([PROPERTIES],[libnautilus-extension-4 libgsf-1])
NAUTILUS_EXTENSION_DIR=`$PKG_CONFIG --variable=extensiondir libnautilus-extension-4`
NAUTILUS_EXTENSION_DIR=${NAUTILUS_EXTENSION_DIR#`$PKG_CONFIG --variable=libdir libnautilus-extension-4`}
NAUTILUS_EXTENSION_DIR=${NAUTILUS_EXTENSION_DIR#/}
AC_SUBST(PROPERTIES_CFLAGS)
AC_SUBST(PROPERTIES_LIBS)
AC_SUBST(NAUTILUS_EXTENSION_DIR)

AM_CONDITIONAL([ENABLE_NAUTILUS],[test "$enable_nautilus" = "yes"])

AC_CONFIG_FILES([Makefile
		 thumbnailer/Makefile
		 properties/Makefile
		 po/Makefile.in])

AC_OUTPUT
