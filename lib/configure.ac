# Process this file with autoconf to produce a configure script.
AC_INIT(libdudlc,0.0.1)
AM_INIT_AUTOMAKE(libdudlc,0.0.1)
#AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([netdb.h stdlib.h string.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([gethostbyname memmove socket strcspn strdup strspn strtol])

AM_MAINTAINER_MODE

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
