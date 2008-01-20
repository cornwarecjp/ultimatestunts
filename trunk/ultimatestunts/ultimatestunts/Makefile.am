bin_PROGRAMS = ustunts
ustunts_SOURCES = carviewer.cpp gamecamera.cpp gamegui.cpp gamerenderer.cpp \
	gamewinsystem.cpp humanplayer.cpp main.cpp music.cpp sndsample.cpp sound.cpp soundobj.cpp \
	soundworld.cpp uscore.cpp usserver.cpp
ustunts_LDADD = $(top_builddir)/trackedit/libtrackedit.a \
	$(top_builddir)/graphics/libgraphics.a $(top_builddir)/simulation/libsimulation.a \
	$(top_builddir)/shared/libshared.a @sdllibs@ @graphlibs@ @soundlibs@ @netlibs@ @LIBINTL@

EXTRA_DIST = carviewer.cpp carviewer.h gamecamera.cpp gamecamera.h gamegui.cpp gamegui.h gamerenderer.cpp gamerenderer.h gamewinsystem.cpp gamewinsystem.h humanplayer.cpp humanplayer.h main.cpp music.cpp music.h sndsample.cpp sndsample.h sound.cpp sound.h soundobj.cpp soundobj.h soundworld.cpp soundworld.h uscore.cpp uscore.h usserver.cpp usserver.h
# set the include path found by configure
INCLUDES = -I$(top_srcdir)/graphics -I$(top_srcdir)/intl -I$(top_srcdir)/shared \
	-I$(top_srcdir)/simulation -I$(top_srcdir)/trackedit $(all_includes)

# the library search path.
ustunts_LDFLAGS = $(all_libraries) 

