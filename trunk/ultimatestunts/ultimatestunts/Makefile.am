bin_PROGRAMS = ultimatestunts
ultimatestunts_SOURCES = main.cpp gamecamera.cpp gamerenderer.cpp humanplayer.cpp sndsample.cpp sound.cpp soundobj.cpp
ultimatestunts_LDADD   = ../graphics/libgraphics.a ../filetransfer/libfiletransfer.a ../simulation/libsimulation.a ../shared/libshared.a @sdllibs@ @graphlibs@ @soundlibs@ @netlibs@


EXTRA_DIST = main.cpp gamecamera.cpp gamecamera.h gamerenderer.cpp gamerenderer.h humanplayer.cpp humanplayer.h sndsample.cpp sndsample.h sound.cpp sound.h soundobj.cpp soundobj.h

# set the include path found by configure
INCLUDES= $(all_includes) -I../shared -I../simulation -I../filetransfer -I../graphics

# the library search path.
ultimatestunts_LDFLAGS = $(all_libraries) 
