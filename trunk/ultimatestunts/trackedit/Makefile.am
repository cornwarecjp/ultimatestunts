bin_PROGRAMS = ustuntstrackedit
ustuntstrackedit_SOURCES = main.cpp
ustuntstrackedit_LDADD = $(top_builddir)/trackedit/libtrackedit.a \
	$(top_builddir)/graphics/libgraphics.a $(top_builddir)/simulation/libsimulation.a \
	$(top_builddir)/shared/libshared.a @sdllibs@ @graphlibs@ @LIBINTL@


EXTRA_DIST = action.h edittrack.cpp edittrack.h main.cpp mainpage.cpp mainpage.h replaceaction.cpp replaceaction.h tecamera.cpp tecamera.h tegui.cpp tegui.h temanager.cpp temanager.h terenderer.cpp terenderer.h trackdocument.cpp trackdocument.h trkfile.cpp trkfile.h

# set the include path found by configure
INCLUDES = -I$(top_srcdir)/graphics -I$(top_srcdir)/intl -I$(top_srcdir)/shared \
	-I$(top_srcdir)/simulation $(all_includes)

# the library search path.
ustuntstrackedit_LDFLAGS = $(all_libraries) 
noinst_HEADERS = tileselect.h actionwidget.h replaceactionwidget.h compass.h
noinst_LIBRARIES = libtrackedit.a
libtrackedit_a_SOURCES = actionwidget.cpp edittrack.cpp \
		mainpage.cpp	replaceaction.cpp tecamera.cpp tegui.cpp temanager.cpp terenderer.cpp	tileselect.cpp \
		trackdocument.cpp trkfile.cpp replaceactionwidget.cpp compass.cpp
