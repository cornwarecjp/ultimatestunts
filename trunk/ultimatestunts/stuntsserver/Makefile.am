bin_PROGRAMS = stuntsserver
stuntsserver_SOURCES = main.cpp
stuntsserver_LDADD   = ../simulation/libsimulation.a ../shared/libshared.a @sdllibs@ @netlibs@


EXTRA_DIST = main.cpp 

# set the include path found by configure
INCLUDES= $(all_includes) -I../shared -I../simulation

# the library search path.
stuntsserver_LDFLAGS = $(all_libraries) 
