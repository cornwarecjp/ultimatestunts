stuntsdir=@stuntsdir@

stunts_PROGRAMS = stuntsai
stuntsai_SOURCES = main.cpp
stuntsai_LDADD   = ../shared/libshared.a ../simulation/libsimulation.a @sdllibs@ @netlibs@ @odelibs@


EXTRA_DIST = main.cpp 

# set the include path found by configure
INCLUDES= $(all_includes) -I../shared -I../simulation

# the library search path.
stuntsai_LDFLAGS = $(all_libraries) 
