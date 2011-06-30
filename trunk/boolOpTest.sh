#!/bin/sh

rm output/Dump*.brep

DUMP=-1 ./camocc.sh g2m ngc-in/face-sc.canon debug

#what is the order in which the dumped files are created?