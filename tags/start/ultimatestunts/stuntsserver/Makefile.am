bin_PROGRAMS = stuntsserver
stuntsserver_SOURCES = main.cpp
stuntsserver_LDADD   = ../shared/libshared.a ../simulation/libsimulation.a ../filetransfer/libfiletransfer.a @netlibs@


EXTRA_DIST = main.cpp 

# set the include path found by configure
INCLUDES= $(all_includes) -I../shared -I../simulation -I../filetransfer

# the library search path.
stuntsserver_LDFLAGS = $(all_libraries) 
