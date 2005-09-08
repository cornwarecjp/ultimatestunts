bin_PROGRAMS = ustuntstrackedit
ustuntstrackedit_SOURCES = edittrack.cpp main.cpp tecamera.cpp temanager.cpp terenderer.cpp
ustuntstrackedit_LDADD   = ../graphics/libgraphics.a ../simulation/libsimulation.a ../shared/libshared.a @sdllibs@ @graphlibs@ @LIBINTL@


EXTRA_DIST = edittrack.cpp edittrack.h main.cpp tecamera.cpp tecamera.h temanager.cpp temanager.h terenderer.cpp terenderer.h

# set the include path found by configure
INCLUDES= $(all_includes) -I../intl -I../shared -I../simulation -I../graphics

# the library search path.
ustuntstrackedit_LDFLAGS = $(all_libraries) 
