stuntsdir=@stuntsdir@

stunts_PROGRAMS = trackedit
trackedit_SOURCES = main.cpp
trackedit_LDADD   = ../shared/libshared.a ../graphics/libgraphics.a @sdllibs@ @graphlibs@ @LIBINTL@


EXTRA_DIST = main.cpp 

# set the include path found by configure
INCLUDES= $(all_includes) -I../intl -I../shared -I../graphics

# the library search path.
trackedit_LDFLAGS = $(all_libraries) 
