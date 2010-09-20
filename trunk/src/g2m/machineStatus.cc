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
#include <BRepBndLib.hxx>
#include <Bnd_Box.hxx>


//millTool* machineStatus::theTool = 0;
Bnd_Box machineStatus::traverseBbox;
Bnd_Box machineStatus::feedBbox;

machineStatus::machineStatus(const machineStatus& oldStatus) {
    //FIXME: segfault on next line when modelling a second file?!
	spindleStat = oldStatus.spindleStat;
    F = oldStatus.F;
    S = oldStatus.S;
    coolant = oldStatus.coolant;
    plane = oldStatus.plane;
    endPose = startPose = oldStatus.endPose;
    myTool = oldStatus.myTool;
    endDir = gp_Dir(0,0,-1);
    prevEndDir = oldStatus.endDir;
    first = oldStatus.first;
    motionType = NOT_DEFINED;
}

/**
This constructor is only to be used when initializing the simulation; it would not be useful elsewhere.
\param initial is the initial pose of the machine, as determined by the interp from the variable file.
\sa machineStatus(machineStatus const& oldStatus)
*/
machineStatus::machineStatus(gp_Ax1 initial) {
  clearAll();
  //theTool = new ballnoseTool(0.0625,0.3125); //1/16" tool. TODO: use EMC's tool table for tool sizes
  startPose = endPose = initial;
  first = true;
  setTool(1);
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
  myTool = -1;
}

///sets motion type, and checks whether this is the second (or later) motion command.
void machineStatus::setMotionType(MOTION_TYPE m) {
  motionType = m;
  if (motionType == NOT_DEFINED) {
    infoMsg("mt undef");
  }
  static int count = 0;
  if ((first) && ((m == STRAIGHT_FEED) || (m == TRAVERSE) || (m == HELICAL)) ) {
    count++;
    if (count == 2) {
      first = false;
    }
  }
}


/** \fn setEndPose
Set end points, and call addToBounds to add points to bndbox. For an arc or helix, the edge must be added from its ctor with addArcToBbox.
\sa addArcToBbox(TopoDS_Edge e)
*/
void machineStatus::setEndPose(gp_Pnt p) {
  endPose = gp_Ax1( p, gp_Dir(0,0,1) );
  addToBounds();
}
void machineStatus::setEndPose(gp_Ax1 newPose) {
  endPose = newPose;
  addToBounds();
}

void machineStatus::addToBounds() {
  if (first) {
    return;
  } else if (motionType == NOT_DEFINED) {
    infoMsg("error, mtype not defined");
  } else if (motionType == STRAIGHT_FEED) {
    feedBbox.Add(startPose.Location());
    feedBbox.Add(endPose.Location());
  } else if (motionType == TRAVERSE) {
    traverseBbox.Add(startPose.Location());
    traverseBbox.Add(endPose.Location());
  }
}

void machineStatus::setTool(toolNumber n) {
  infoMsg("adding tool " + n + ".");
  myTool = n;
  canon::addTool(n);
}

/*
void machineStatus::setTool(uint n) {
  if (theTool != 0)
    delete theTool;
  double d = double(n)/16.0; //for testing, n is diameter in 16ths, and length is 5*diameter. FIXME
  theTool = new ballnoseTool(d,d*5.0); //TODO: use EMC's tool table for tool sizes
}
*/

///if not first, add an arc or helix to feedBbox. STRAIGHT_* is added in setEndPose()
void machineStatus::addArcToBbox(TopoDS_Edge e) {
  if (!first) {
    BRepBndLib::Add(e,feedBbox);
  }
}

pntPair machineStatus::getTraverseBounds() {
  double aXmin,aYmin,aZmin, aXmax,aYmax,aZmax;
  traverseBbox.Get (aXmin,aYmin,aZmin, aXmax,aYmax,aZmax );
  pntPair b;
  b.a=gp_Pnt(aXmin,aYmin,aZmin);
  b.b=gp_Pnt(aXmax,aYmax,aZmax);
  return b;
}

pntPair machineStatus::getFeedBounds() {
  double aXmin,aYmin,aZmin, aXmax,aYmax,aZmax;
  feedBbox.Get ( aXmin,aYmin,aZmin, aXmax,aYmax,aZmax );
  pntPair b;
  b.a=gp_Pnt(aXmin,aYmin,aZmin);
  b.b=gp_Pnt(aXmax,aYmax,aZmax);
  return b;
}

