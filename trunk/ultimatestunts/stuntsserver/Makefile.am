stuntsdir=@stuntsdir@

stunts_PROGRAMS = stuntsserver
stuntsserver_SOURCES = gamecorethread.cpp main.cpp networkthread.cpp remoteplayer.cpp
stuntsserver_LDADD   = ../simulation/libsimulation.a ../shared/libshared.a @sdllibs@ @netlibs@ @odelibs@


EXTRA_DIST = gamecorethread.cpp gamecorethread.h main.cpp networkthread.cpp networkthread.h remoteplayer.cpp remoteplayer.h

# set the include path found by configure
INCLUDES= $(all_includes) -I../shared -I../simulation

# the library search path.
stuntsserver_LDFLAGS = $(all_libraries) 
