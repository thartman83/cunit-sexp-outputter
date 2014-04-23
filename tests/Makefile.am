AM_CFLAGS = -I$(top_builddir)/include -I$(top_srcdir)/include -g
AM_LDFLAGS = -L$(top_builddir)/src/cunitsexpoutputter/
bin_PROGRAMS = test_cunit_sexp_outputter

test_cunit_sexp_outputter_SOURCES = main.c
test_cunit_sexp_outputter_LDADD = -lcunit -lcunitsexpoutputter
