#!/bin/sh
# if [ -z "$MMGT_CLEAR" ]; then
# echo Applying env vars...
# . /opt/occ63/env.sh
# fi
#export MMGT_OPT=0
progdir="`pwd`/`dirname $0`/build/bin"
LD_LIBRARY_PATH="$progdir:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH
$progdir/camocc2 $1 $2 $3 $4 $5 & echo $!

