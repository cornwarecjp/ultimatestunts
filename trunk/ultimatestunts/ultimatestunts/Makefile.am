bin_PROGRAMS = ultimatestunts
ultimatestunts_SOURCES = main.cpp gamecamera.cpp gamerenderer.cpp humanplayer.cpp music.cpp sndsample.cpp sound.cpp soundobj.cpp soundworld.cpp usserver.cpp
ultimatestunts_LDADD   = ../graphics/libgraphics.a ../simulation/libsimulation.a ../shared/libshared.a @sdllibs@ @graphlibs@ @soundlibs@ @netlibs@ @odelibs@


EXTRA_DIST = main.cpp gamecamera.cpp gamecamera.h gamerenderer.cpp gamerenderer.h humanplayer.cpp humanplayer.h music.cpp music.h sndsample.cpp sndsample.h sound.cpp sound.h soundobj.cpp soundobj.h soundworld.cpp soundworld.h usserver.cpp usserver.h
# set the include path found by configure
INCLUDES= $(all_includes) -I../shared -I../simulation -I../graphics

# the library search path.
ultimatestunts_LDFLAGS = $(all_libraries) 
