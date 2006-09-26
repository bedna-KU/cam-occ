#!/bin/sh
echo If this fails, set CASROOT to the correct location, 
echo usually /opt/OpenCASCADE6.1/ros
export LD_LIBRARY_PATH=$CASROOT/lib
./cam-occ

