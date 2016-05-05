AM_CFLAGS = 
AM_LDFLAGS =
AC_PROG_RANLIB = "echo"

noinst_LIBRARIES = libskey.a
libskey_a_SOURCES = skeylogin.c skeysubr.c md4.c put.c

bin_PROGRAMS = key keyinit keysh
key_SOURCES = skey.c
keyinit_SOURCES = skeyinit.c
keysh_SOURCES = skeysh.c
key_LDADD = libskey.a
keyinit_LDADD = libskey.a
keysh_LDADD = libskey.a
