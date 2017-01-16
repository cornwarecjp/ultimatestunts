bin_PROGRAMS = ustuntsai
ustuntsai_SOURCES = main.cpp
ustuntsai_LDADD = $(top_builddir)/simulation/libsimulation.a \
	$(top_builddir)/shared/libshared.a @sdllibs@ @netlibs@ @LIBINTL@


EXTRA_DIST = main.cpp 

# set the include path found by configure
AM_CPPFLAGS = -I$(top_srcdir)/intl -I$(top_srcdir)/shared \
	-I$(top_srcdir)/simulation $(all_includes)

# the library search path.
ustuntsai_LDFLAGS = $(all_libraries) 
