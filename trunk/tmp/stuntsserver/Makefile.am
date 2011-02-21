bin_PROGRAMS = ustuntsserver
ustuntsserver_SOURCES = consolethread.cpp gamecorethread.cpp main.cpp networkthread.cpp uploadmanager.cpp
ustuntsserver_LDADD = $(top_builddir)/simulation/libsimulation.a \
	$(top_builddir)/shared/libshared.a @sdllibs@ @netlibs@ @LIBINTL@


EXTRA_DIST = consolethread.cpp consolethread.h gamecorethread.cpp gamecorethread.h main.cpp networkthread.cpp networkthread.h uploadmanager.cpp uploadmanager.h

# set the include path found by configure
INCLUDES = -I$(top_srcdir)/intl -I$(top_srcdir)/shared \
	-I$(top_srcdir)/simulation $(all_includes)

# the library search path.
ustuntsserver_LDFLAGS = $(all_libraries) 
