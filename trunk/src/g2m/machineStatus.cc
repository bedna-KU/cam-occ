/***************************************************************************
 *   Copyright (C) 2010 by Mark Pictor                                     *
 *   mpictor@gmail.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "machineStatus.hh"
millTool* machineStatus::theTool = 0;

machineStatus::machineStatus(machineStatus const& oldStatus) {
    //FIXME: segfault on next line when modelling a second file?!
	spindleStat = oldStatus.spindleStat;
    F = oldStatus.F;
    S = oldStatus.S;
    coolant = oldStatus.coolant;
    plane = oldStatus.plane;
    endPose = startPose = oldStatus.endPose;
    theTool = oldStatus.theTool;
    endDir = gp_Dir(0,0,-1);
    prevEndDir = oldStatus.endDir;
}

/**
This constructor is only to be used when initializing the simulation; it would not be useful elsewhere.
\param initial is the initial pose of the machine, as determined by the interp from the variable file.
\sa machineStatus(machineStatus const& oldStatus)
*/
machineStatus::machineStatus(gp_Ax1 initial) {
  clearAll();
  theTool = new ballnoseTool(0.0625,0.3125); //1/16" tool. TODO: use EMC's tool table for tool sizes
  startPose = endPose = initial;
}

void machineStatus::clearAll() {
  F=S=0.0;
  plane = CANON_PLANE_XY;
  coolant.flood = false;
  coolant.mist = false;
  coolant.spindle = false;
  endPose = startPose = gp_Ax1(gp_Pnt(0,0,0),gp_Dir(0,0,1));
  endDir = prevEndDir = gp_Dir(0,0,-1);
  spindleStat = OFF;
  //theTool = -1;
  if (theTool != 0) {
    delete theTool;
    theTool = 0;
  }
}

void machineStatus::setEndPose(gp_Pnt p) {
  endPose = gp_Ax1( p, gp_Dir(0,0,1) );
}

/*
void machineStatus::setTool(toolNumber n) {
  myTool = n;
  //check if the tool exists
  //if not, load it
}
*/
void machineStatus::setTool(uint n) {
  if (theTool != 0)
    delete theTool;
  double d = double(n)/16.0; //for testing, n is diameter in 16ths, and length is 5*diameter. FIXME
  theTool = new ballnoseTool(d,d*5.0); //TODO: use EMC's tool table for tool sizes
}
