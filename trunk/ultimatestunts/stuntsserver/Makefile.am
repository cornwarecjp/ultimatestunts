stuntsdir=@stuntsdir@

stunts_PROGRAMS = stuntsserver
stuntsserver_SOURCES = consolethread.cpp gamecorethread.cpp main.cpp networkthread.cpp
stuntsserver_LDADD   = ../simulation/libsimulation.a ../shared/libshared.a @sdllibs@ @netlibs@ @odelibs@ @LIBINTL@


EXTRA_DIST = consolethread.cpp consolethread.h gamecorethread.cpp gamecorethread.h main.cpp networkthread.cpp networkthread.h

# set the include path found by configure
INCLUDES= $(all_includes) -I../intl -I../shared -I../simulation

# the library search path.
stuntsserver_LDFLAGS = $(all_libraries) 
