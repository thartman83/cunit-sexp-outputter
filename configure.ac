dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.26)

m4_define([cunit_sexp_outputter_major_version], [0])
m4_define([cunit_sexp_outputter_minor_version], [1])
m4_define([cunit_sexp_outputter_version], \
          [cunit_sexp_outputter_major_version.cunit_sexp_outputter_minor_version])

AC_INIT([cunit-s-exp-outputter],[1.0])
AC_CONFIG_MACRO_DIR([config])
AM_INIT_AUTOMAKE([1.11 dist-bzip2])

AC_SUBST(CUNIT_SEXP_OUTPUTTER_MAJOR_VERSION, [cunit_sexp_outputter_major_version])
AC_SUBST(CUNIT_SEXP_OUTPUTTER_MINOR_VERSION, [cunit_sexp_outputter_minor_version])
AC_SUBST(CUNIT_SEXP_OUTPUTTER_VERSION, [cunit_sexp_outputter_version])

dnl Check for programs.

AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_PROG_LIBTOOL

AC_HEADER_STDC
PKG_CHECK_MODULES([CUNIT], [cunit])
AC_PROG_LIBTOOL
AC_LTDL_DLLIB

AC_CONFIG_FILES([Makefile] [include/Makefile] \
                           [src/Makefile] \
                           [include/cunitsexpoutputter/Makefile] \
                           [src/cunitsexpoutputter/Makefile] \
                           [tests/Makefile])

AC_OUTPUT