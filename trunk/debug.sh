#!/bin/sh
if [ -z "$MMGT_CLEAR" ];
then . /opt/occ63/env.sh;
fi
#export MMGT_OPT=0
#progdir=/home/mark/projects/sw-dev/cam/cam-occ/trunk
progdir=`dirname $0`
LD_LIBRARY_PATH=$progdir/bin:$LD_LIBRARY_PATH
gdb --args $progdir/bin/camocc2 $1 $2 $3 $4
