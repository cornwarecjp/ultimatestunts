#!/bin/sh

#The user should call this script
#to start Ultimate Stunts when it
#is installed with "make install".
#It should be in the PATH of the user.

oldpwd=$PWD
prefix=/usr/local
cd ${prefix}/games/ultimatestunts
./ultimatestunts
cd $oldpwd
