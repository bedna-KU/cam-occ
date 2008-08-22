#!/bin/sh
echo If this fails, set CASROOT to the correct location, 
echo usually /opt/OpenCASCADE6.2/ros
export LD_LIBRARY_PATH=/opt/occ62/ros
./cam-occ

