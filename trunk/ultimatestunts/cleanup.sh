#!/bin/sh

#Shell script for cleaning up the tree before
#committing it to CVS. Makes it easier to check
#the CVS output for errors

#Author: CJP

make clean
rm -f Makefile */Makefile
rm -rf */.deps

rm -f *.stdout
rm -f *.stderr
rm -f *debuglog*
rm -f config.log
rm -f config.status

rm -rf autom4te.cache

