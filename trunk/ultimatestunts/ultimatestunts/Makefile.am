bin_PROGRAMS = ultimatestunts
ultimatestunts_SOURCES = main.cpp humanplayer.cpp
ultimatestunts_LDADD   = ../graphics/libgraphics.a ../filetransfer/libfiletransfer.a ../simulation/libsimulation.a ../shared/libshared.a @graphlibs@ @soundlibs@ @netlibs@


EXTRA_DIST = main.cpp humanplayer.cpp humanplayer.h

# set the include path found by configure
INCLUDES= $(all_includes) -I../shared -I../simulation -I../filetransfer -I../graphics

# the library search path.
ultimatestunts_LDFLAGS = $(all_libraries) 
