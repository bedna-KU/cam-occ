#!/bin/sh
if [ -z "MMGT_CLEAR" ];
then . /opt/occ63/env.sh;
fi
progdir=/home/mark/projects/sw-dev/cam/cam-occ/trunk
LD_LIBRARY_PATH=$progdir/bin:/opt/occ63/debug/lib
gdb --args $progdir/bin/camocc2 $1 $2 $3 $4
