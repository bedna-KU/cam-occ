#!/bin/sh
#script to clean up doxygen html dir

#delete old (all) files
#too bad we can't figure out which ones are not obsolete...
cd ~/projects/sw-dev/cam/cam-occ/doc/doxygen/html
rm *

#generate docs
cd ~/projects/sw-dev/cam/cam-occ/trunk
make clean   #this gets rid of moc stuff...
doxygen

#add and remove files
cd ~/projects/sw-dev/cam/cam-occ/doc/doxygen/html
#add files which aren't in svn yet
svn add `svn stat|grep ^\?|sed 's/^\? *//'`

#force remove files that are gone
svn rm `svn stat|grep ^\!|sed 's/^\! *//;'`

#------------------ don't need the following ----------------------------
#svn rm --force fails if the local file does not exist - ?! - don't use --force in that case...
#for i in `svn stat|grep ^\!|sed 's/^\! *//;'`; do
#  touch $i
#  svn rm --force $i
#done
