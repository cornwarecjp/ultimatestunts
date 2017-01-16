sysconf_DATA = ultimatestunts.conf
EXTRA_DIST = m4/ChangeLog  config.rpath ultimatestunts.conf ultimatestunts.kdevprj AUTHORS COPYING ChangeLog INSTALL README TODO

CFLAGS=-Wall

AUTOMAKE_OPTIONS = foreign 1.9

SUBDIRS = po intl data shared simulation graphics stuntsserver stuntsai stunts3dedit trackedit ultimatestunts

ultimatestunts.conf: ultimatestunts.conf.in Makefile
	sed 's,@usdatadir\@,$(usdatadir),;s,@PACKAGE_VERSION\@,$(PACKAGE_VERSION),' < $< > $@


ACLOCAL_AMFLAGS = -I m4
