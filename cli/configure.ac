# Process this file with autoconf to produce a configure script.
AC_INIT
AM_INIT_AUTOMAKE(dudlc,0.0.1)
#AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_STRUCT_TM

# Checks for library functions.
AC_TYPE_SIGNAL
AC_FUNC_STRFTIME
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([memset select strcasecmp strcspn strdup strncasecmp strspn strtol])

AM_MAINTAINER_MODE

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
