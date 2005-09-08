bin_PROGRAMS = ustuntsai
ustuntsai_SOURCES = main.cpp
ustuntsai_LDADD   = ../shared/libshared.a ../simulation/libsimulation.a @sdllibs@ @netlibs@ @LIBINTL@


EXTRA_DIST = main.cpp 

# set the include path found by configure
INCLUDES= $(all_includes) -I../intl -I../shared -I../simulation

# the library search path.
ustuntsai_LDFLAGS = $(all_libraries) 
