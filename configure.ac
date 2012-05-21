AC_INIT([gnome-hwp-support], [0.1.1], [cwryu@debian.org], [gnome-hwp-support])
AC_CONFIG_SRCDIR([README])
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE([1.11 no-dist-gzip dist-xz tar-ustar foreign])
AC_CONFIG_MACRO_DIR([m4])

AM_PROG_LIBTOOL
AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AM_PROG_CC_C_O
AC_STDC_HEADERS

PKG_CHECK_MODULES([THUMBNAILER],[libgsf-1 gdk-pixbuf-2.0])
AC_SUBST(THUMBNAILER_CFLAGS)
AC_SUBST(THUMBNAILER_LIBS)

# PKG_CHECK_MODULES([nautilus],[libnautilus-extension libgsf-1])
# NAUTILUS_EXTENSION_DIR=`$PKG_CONFIG --variable=extensiondir libnautilus-extension`
# AC_SUBST([nautilusextensiondir],[$NAUTILUS_EXTENSION_DIR])
# AC_SUBST(nautilus_CFLAGS)
# AC_SUBST(nautilus_LIBS)

AC_CONFIG_FILES([Makefile
		 thumbnailer/Makefile])

AC_OUTPUT
