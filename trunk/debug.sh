#!/bin/sh
if [ -z "MMGT_CLEAR" ];
then . /opt/occ63/env.sh;
fi
progdir=`dirname $0`
LD_LIBRARY_PATH=$progdir/bin:$LD_LIBRARY_PATH
gdb $progdir/bin/camocc2
