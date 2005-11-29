bin_PROGRAMS = ustuntsserver
ustuntsserver_SOURCES = consolethread.cpp gamecorethread.cpp main.cpp networkthread.cpp uploadmanager.cpp
ustuntsserver_LDADD   = ../simulation/libsimulation.a ../shared/libshared.a @sdllibs@ @netlibs@ @LIBINTL@


EXTRA_DIST = consolethread.cpp consolethread.h gamecorethread.cpp gamecorethread.h main.cpp networkthread.cpp networkthread.h uploadmanager.cpp uploadmanager.h

# set the include path found by configure
INCLUDES= $(all_includes) -I../intl -I../shared -I../simulation

# the library search path.
ustuntsserver_LDFLAGS = $(all_libraries) 
