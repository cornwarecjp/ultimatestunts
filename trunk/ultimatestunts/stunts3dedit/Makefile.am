bin_PROGRAMS = stunts3dedit
stunts3dedit_SOURCES = editcamera.cpp editgraphobj.cpp editrenderer.cpp main.cpp textureloader.cpp
stunts3dedit_LDADD   = ../graphics/libgraphics.a ../shared/libshared.a @sdllibs@ @graphlibs@


EXTRA_DIST = editcamera.cpp editcamera.h editgraphobj.cpp editgraphobj.h editrenderer.cpp editrenderer.h main.cpp textureloader.cpp textureloader.h

# set the include path found by configure
INCLUDES= $(all_includes) -I../shared -I../graphics

# the library search path.
stunts3dedit_LDFLAGS = $(all_libraries) 
