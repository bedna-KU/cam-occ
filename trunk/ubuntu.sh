#!/bin/sh

# This script is for ubuntu (or other distros) with
# opencascade binaries installed in standard locations

#the first arg after g2m can be a file name (arg #2 is the only one that's checked for a file name)
#use debug as an arg for more output, i.e.
# ./ubuntu.sh ngc-in/face-sc.canon debug
#g2m works with files of canonical commands, and with g-code.
#EMC2's stand-alone interpreter is required for g-code.

LD_LIBRARY_PATH=./bin bin/camocc2 g2m $1 $2 $3
