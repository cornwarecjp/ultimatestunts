bin_PROGRAMS = ustunts3dedit
ustunts3dedit_SOURCES = 3ds.cpp editcamera.cpp editgraphobj.cpp editrenderer.cpp main.cpp textureloader.cpp
ustunts3dedit_LDADD   = ../graphics/libgraphics.a ../shared/libshared.a @sdllibs@ @graphlibs@ @LIBINTL@


EXTRA_DIST = 3ds.cpp 3ds.h editcamera.cpp editcamera.h editgraphobj.cpp editgraphobj.h editrenderer.cpp editrenderer.h main.cpp textureloader.cpp textureloader.h

# set the include path found by configure
INCLUDES= $(all_includes) -I../intl -I../shared -I../graphics

# the library search path.
ustunts3dedit_LDFLAGS = $(all_libraries) 
