#!/bin/sh
if [ -z "$MMGT_CLEAR" ]; then
echo Applying env vars...
. /opt/occ63/env.sh
fi
#export MMGT_OPT=0
progdir=`dirname $0`
LD_LIBRARY_PATH=$progdir/bin:$LD_LIBRARY_PATH
$progdir/bin/camocc2 $1 $2 $3 $4 $5 & echo $!

