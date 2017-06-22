#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([skey],[0.01], [shakeeb.saleh@stonybrook.edu])
AM_INIT_AUTOMAKE(skey,0.01)
AC_CONFIG_SRCDIR([src/config.h.in])
AC_CONFIG_HEADER([src/config.h])
AC_PROG_RANLIB

# Checks for programs.
AC_PROG_CC
AC_HEADER_STDBOOL
AC_C_BIGENDIAN

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h sgtty.h termio.h shadow.h stdlib.h string.h sys/ioctl.h sys/param.h sys/systeminfo.h sys/time.h termios.h unistd.h crypt.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_TIME
AC_STRUCT_TM

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_FUNC_STRFTIME
AC_CHECK_FUNCS([dup2 gethostname getpass getspnam isascii memset setenv strchr sysinfo])
AC_CHECK_FUNC(crypt, , AC_CHECK_LIB(crypt, crypt, ,
	[echo "cannot find crypt anywhere"; exit 1]))
AC_CONFIG_FILES([Makefile
                 man/Makefile
                 src/Makefile])
AC_OUTPUT
