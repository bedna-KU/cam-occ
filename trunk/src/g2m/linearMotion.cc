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

#include <string>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include "linearMotion.hh"
#include "machineStatus.hh"
#include "canonMotion.hh"
#include "canonLine.hh"

linearMotion::linearMotion(std::string canonL, machineStatus prevStatus): canonMotion(canonL,prevStatus) {
  gp_Pnt a,b;
  a = status.getStartPose().Location();
  b = status.getEndPose().Location();
  //TODO: add support for 5 or 6 axis motion

  myUnSolid = BRepBuilderAPI_MakeEdge(a,b);

  //check if the swept shape will be accurate
  //use the ratio of rise or fall to distance
  double deltaZ = abs(a.Z() - b.Z());
  double dist = a.Distance(b);
  if ( (deltaZ/dist) > 0.001) {
    sweepIsSuspect = true;
  }

  BRepOffsetAPI_MakePipeShell pipe(myUnSolid);
    pipe.Add(status.getToolProfile(),false,true);
    //pipe.Add(tool2d,false,true);
    pipe.SetTransitionMode(BRepBuilderAPI_RoundCorner); //there shouldn't be any discontinuities, but we'll set this anyway
    pipe.SetMode(gp_Dir(0,0,1)); //binormal to vector 0,0,1 - profile can only rotate about Z now
//    infoMsg("added tool\n");
    if ( pipe.IsReady() ) {
        pipe.Build();
        BRepBuilderAPI_PipeError error = pipe.GetStatus();
        switch (error) {
          case BRepBuilderAPI_PipeNotDone:
            infoMsg("Pipe not done");
            errors = true;
            break;
          case BRepBuilderAPI_PlaneNotIntersectGuide:
            infoMsg("Pipe not intersect guide");
            errors = true;
            break;
          case BRepBuilderAPI_ImpossibleContact:
            infoMsg("Pipe impossible contact");
            errors = true;
            break;
          case BRepBuilderAPI_PipeDone:
            //ready = true;
            break;
         default:
            infoMsg("Pipe switch default?!");
            errors = true;
        }
    }
    if (!errors) {
        mySolid = pipe.Shape();
    }else {
        //infoMsg("pipe not ready!");
        mySolid = tool2d;
    }

}

//need to return RAPID for rapids...
MOTION_TYPE linearMotion::getMotionType() {
  static bool rapid = clMatch("STRAIGHT_TRAVERSE");
  if (rapid) {
    return RAPID;
  } else if clMatch("STRAIGHT_FEED") {
    return LINEAR;
  } else {
    string err = "linearMotion::getMotionType failed on canonLine:\n" + myLine + "\n\nExiting.";
    infoMsg(err);
    exit(1);
  }
}
