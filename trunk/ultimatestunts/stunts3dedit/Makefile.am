bin_PROGRAMS = stunts3dedit
stunts3dedit_SOURCES = editcamera.cpp editrenderer.cpp main.cpp
stunts3dedit_LDADD   = ../graphics/libgraphics.a ../shared/libshared.a @sdllibs@ @graphlibs@


EXTRA_DIST = editcamera.cpp editcamera.h editrenderer.cpp editrenderer.h main.cpp

# set the include path found by configure
INCLUDES= $(all_includes) -I../shared -I../graphics

# the library search path.
stunts3dedit_LDFLAGS = $(all_libraries) 
