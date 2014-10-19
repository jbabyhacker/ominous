dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(MusicTable, 0.1)

# check for OpenCV
PKG_CHECK_MODULES(OpenCV, [opencv >= 2.4.9])

AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE(-Wall foreign)

AC_PROG_CXX

AC_CONFIG_FILES(Makefile src/Makefile)
AC_OUTPUT

