bin_PROGRAMS = stunts3dedit
stunts3dedit_SOURCES = main.cpp
stunts3dedit_LDADD   = ../shared/libshared.a ../graphics/libgraphics.a @graphlibs@


EXTRA_DIST = main.cpp 

# set the include path found by configure
INCLUDES= $(all_includes) -I../shared -I../graphics

# the library search path.
stunts3dedit_LDFLAGS = $(all_libraries) 
