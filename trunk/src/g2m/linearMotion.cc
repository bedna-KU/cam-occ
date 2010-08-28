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
#include "linearMotion.hh"

#include <string>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgo_Fuse.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepOffsetAPI_MakePipeShell.hxx>
#include <BRepBuilderAPI_PipeError.hxx>
#include <ShapeFix_Shape.hxx>
#include <ShapeFix_Solid.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include "machineStatus.hh"
#include "canonMotion.hh"
#include "canonLine.hh"
#include "uio.hh"

linearMotion::linearMotion(std::string canonL, machineStatus prevStatus): canonMotion(canonL,prevStatus) {
  gp_Pnt a,b;
  a = status.getStartPose().Location();
  b = status.getEndPose().Location();
  //TODO: add support for 5 or 6 axis motion

  TopoDS_Edge e = BRepBuilderAPI_MakeEdge(a,b).Edge();
  myUnSolid = e;

  //check if the swept shape will be accurate
  //use the ratio of rise or fall to distance
  double deltaZ = abs(a.Z() - b.Z());
  double dist = a.Distance(b);
  if ( (deltaZ/dist) > 0.001) {
    sweepIsSuspect = true;
  }

  BRepOffsetAPI_MakePipeShell pipe(BRepBuilderAPI_MakeWire(e).Wire());
  TopoDS_Wire w = TopoDS::Wire(status.getTool()->getProfile());
  if (!w.Closed()) {
    uio::infoMsg("Wire not closed!");
  } else {
    pipe.Add(w,false,true);
    //pipe.Add(tool2d,false,true);
    pipe.SetTransitionMode(BRepBuilderAPI_RoundCorner); //there shouldn't be any discontinuities, but we'll set this anyway
    pipe.SetMode(gp_Dir(0,0,1)); //binormal to vector 0,0,1 - profile can only rotate about Z now
//    infoMsg("added tool\n");
    if ( pipe.IsReady() ) {
        pipe.Build();
        BRepBuilderAPI_PipeError error = pipe.GetStatus();
        switch (error) {
          case BRepBuilderAPI_PipeNotDone:
            uio::infoMsg("Pipe not done");
            errors = true;
            break;
          case BRepBuilderAPI_PlaneNotIntersectGuide:
            uio::infoMsg("Pipe not intersect guide");
            errors = true;
            break;
          case BRepBuilderAPI_ImpossibleContact:
            uio::infoMsg("Pipe impossible contact");
            errors = true;
            break;
          case BRepBuilderAPI_PipeDone:
            //ready = true;
            break;
         default:
            uio::infoMsg("Pipe switch default?!");
            errors = true;
        }
    }
  }
  if (!errors) {
    dispShape ds(pipe.Shape());
    ds.display();
    uio::sleep(5);
    pipe.MakeSolid();

    //FIXME: each canonMotion obj should check if its startpoint is colinear with the endpoint of the previous object. If not, it should add a 3d model of the tool there.

    //for now, we'll add one at each end of every obj.
    //gp_Vec sv = status.getStartVector();
    //gp_Vec ev = prevStatus.getEndVector();
    //BRepBuilderAPI_Transform
    //BRepBuilderAPI_GTransform

    gp_Trsf tr;
    tr.SetTranslation(gp::Origin(),a);
    BRepBuilderAPI_Transform bt1(tr);
    bt1.Perform(status.getTool()->get3d(),true);
    TopoDS_Shape e1 = bt1.Shape(); //this is the tool, translated to the startpoint of the sweep

    tr.SetTranslation(gp::Origin(),b);
    BRepBuilderAPI_Transform bt2(tr);
    bt2.Perform(status.getTool()->get3d(),true);
    TopoDS_Shape e2 = bt2.Shape(); //this is the tool, translated to the endpoint of the sweep

    TopoDS_Solid ps = TopoDS::Solid(pipe.Shape());
    TopoDS_Shape temp = BRepAlgoAPI_Fuse( e1, e2 );
    if (temp.IsNull()) std::cout << "null shape" << endl;
    myShape = BRepAlgoAPI_Fuse( ps, temp );
  } else {
    uio::infoMsg("pipe not ready!");
    myShape = status.getTool()->get3d();
  }
}

//need to return RAPID for rapids...
MOTION_TYPE linearMotion::getMotionType() {
  static bool traverse = clMatch("STRAIGHT_TRAVERSE");
  if (traverse) {
    return TRAVERSE;
  } else if (clMatch("STRAIGHT_FEED")) {
    return LINEAR;
  } else {
    std::string err = "linearMotion::getMotionType failed on canonLine:\n" + myLine + "\n\nExiting.";
    uio::infoMsg(err);
    exit(1);
  }
}

/*
TopTools_ListOfShape oneShell (TopoDS_Solid a, TopoDS_Solid b) {
  TopTools_ListOfShape lsh;
  TopoDS_Shell shp1_shell = BRepTools::OuterShell(a);
  TopoDS_Shell shp2_shell = BRepTools::OuterShell(b);
  TopoDS_Shape fused_shells = BRepAlgoAPI_Fuse(shp1_shell, shp2_shell).Shape();

  TopExp_Explorer Ex;
  for (Ex.Init(fused_shells, TopAbs_ShapeEnum.TopAbs_SHELL); Ex.More(); Ex.Next()) {
    TopoDS_Shell crt_shell = TopoDS::Shell(Ex.Current());
    ShapeFix_Shell FixTopShell(crt_shell);
    FixTopShell.Perform();
    if (FixTopShell.NbShells() > 1) {
      TopoDS_Compound shellComp = OCTopoDS.Compound(FixTopShell.Shape());
      TopExp_Explorer ExShls(shellComp, TopAbs_ShapeEnum.TopAbs_SHELL);
      for (; ExShls.More(); ExShls.Next()) {
        TopoDS_Shell shl1 = TopoDS::Shell(ExShls.Current());
        ShapeFix_Shell FixShl = new ShapeFix_Shell(shl1);
        FixShl.Perform();

        TopoDS_Solid sol_tmp1 = BRepBuilderAPI_MakeSolid(FixShl.Shell()).Solid();
        ShapeFix_Solid FixSld(sol_tmp1);
        FixSld.Perform();
        TopoDS_Solid sol_tmp = TopoDS.Solid(FixSld.Solid());
        // add it to collection....
        lsh.Add(sol_tmp);
        cout << "multiple solids!" << endl;
      }
    } else {
      TopoDS_Shell aShell = FixTopShell.Shell();
      TopoDS_Solid sol = new BRepBuilderAPI_MakeSolid(aShell).Solid();
      // add it to collection....
      lsh.Add(sol);
        cout << "one solid" << endl;
    }
  }
  return lsh;
}
*/
