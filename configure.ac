# Process this file with autoconf to produce a configure script.
AC_INIT
AM_INIT_AUTOMAKE(dudlc,0.1.15)
AM_CONFIG_HEADER(lib/dudlc/config.h)

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([netdb.h stdlib.h string.h sys/socket.h sys/time.h unistd.h])

dnl TODO: enhance checks for libncc, readline, curses, lirc
AC_CHECK_LIB([ncc], [pidfile_ftake])
AC_CHECK_LIB([readline], [readline])
AC_CHECK_LIB([curses], [initscr])
AC_CHECK_LIB([lirc_client], [lirc_init])


PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.12.4])
AC_SUBST(GLIB_LIBS)
AC_SUBST(GLIB_CFLAGS)

PKG_CHECK_MODULES([GTK], [gtk+-2.0 >= 2.8.20 ])
AC_SUBST(GTK_LIBS)
AC_SUBST(GTK_CFLAGS)


# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_MALLOC
AC_TYPE_SIGNAL
AC_FUNC_STRFTIME
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([gethostbyname memmove memset select socket strcasecmp strcspn strdup strncasecmp strspn strtol])

AM_MAINTAINER_MODE

AC_CONFIG_FILES([Makefile lib/Makefile lib/dudlc/Makefile
	cmd/Makefile cli/Makefile ir/Makefile gtk/Makefile])
AC_OUTPUT
