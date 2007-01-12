# Process this file with autoconf to produce a configure script.
AC_INIT
AM_INIT_AUTOMAKE(dudlc,0.1.10)
AM_CONFIG_HEADER(lib/dudlc/config.h)

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([netdb.h stdlib.h string.h sys/socket.h sys/time.h unistd.h])

dnl TODO: check for libncc, readline, curses, lirc

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
