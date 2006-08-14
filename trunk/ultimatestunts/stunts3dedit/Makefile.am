bin_PROGRAMS = ustunts3dedit
ustunts3dedit_SOURCES = 3ds.cpp editcamera.cpp editgraphobj.cpp editrenderer.cpp load3ds.cpp loadglt.cpp loadlwo.cpp loadobj.cpp loadraw.cpp lw.c main.cpp textureloader.cpp
ustunts3dedit_LDADD   = ../graphics/libgraphics.a ../shared/libshared.a @sdllibs@ @graphlibs@ @LIBINTL@


EXTRA_DIST = 3ds.cpp 3ds.h editcamera.cpp editcamera.h editgraphobj.cpp editgraphobj.h editrenderer.cpp editrenderer.h load3ds.cpp load3ds.h loadglt.cpp loadglt.h loadlwo.cpp loadlwo.h loadobj.cpp loadobj.h loadraw.cpp loadraw.h lw.c lw.h main.cpp textureloader.cpp textureloader.h

# set the include path found by configure
INCLUDES= $(all_includes) -I../intl -I../shared -I../graphics

# the library search path.
ustunts3dedit_LDFLAGS = $(all_libraries) 
