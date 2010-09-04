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
//for LINEAR_TRAVERSE, LINEAR_FEED, ARC_FEED
#include <string>
#include <limits.h>

#include <Precision.hxx>
#include <BRepOffsetAPI_MakePipeShell.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_PipeError.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <TopoDS.hxx>
#include <gp_Circ.hxx>
#include <ShapeFix_Solid.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepTools.hxx>

#include "canonMotion.hh"
#include "canonLine.hh"
#include "uio.hh"

canonMotion::canonMotion(std::string canonL, machineStatus prevStatus): canonLine(canonL,prevStatus) {
  status.setEndPose(getPoseFromCmd());
}

///for STRAIGHT_* and ARC_FEED, first 3 are always xyz and last 3 always abc
gp_Ax1 canonMotion::getPoseFromCmd() {
  double x,y,z;

  //need 3,4,5,and -3,-2,-1
  x = tok2d(3);
  y = tok2d(4);
  z = tok2d(5);
  gp_Pnt p(x,y,z);

/* FIXME
  double a,b,c;
  uint s = canonTokens.size(); //a,b,c are last 3 numbers
  c = tok2d(s-1);
  b = tok2d(s-2);
  a = tok2d(s-3);
  assert (a+b+c < 3.0 * Precision::Confusion());
  //now how to convert those angles to a unit vector (i.e. gp_Dir)?
*/
  //for now we take the easy way out
  gp_Dir d(0,0,1); //vertical
  return gp_Ax1(p,d);
}

/// Sweep tool outline along myUnSolid, "cap" it with 3d tools, and put result in myShape
void canonMotion::sweepSolid() {
  Standard_Real angTol = 0.000175;  //approx .01 degrees
  TopoDS_Solid solid;
  gp_Pnt a,b;
  a = status.getStartPose().Location();
  b = status.getEndPose().Location();
  gp_Vec d(a,b);

  gp_Trsf oa,ob;
  oa.SetTranslation(gp::Origin(),a);
  BRepBuilderAPI_Transform toa(oa);
  ob.SetTranslation(gp::Origin(),b);
  BRepBuilderAPI_Transform tob(ob);

  //check if the sweep will be vertical. if so, we can't use the tool's profile
  TopoDS_Wire w;
  bool vert = false;
  if ( d.IsParallel( gp::DZ(), angTol )) {
    vert = true;
    gp_Circ c(gp::XOY(),status.getTool()->Dia()/2.0);
    w = BRepBuilderAPI_MakeWire(BRepBuilderAPI_MakeEdge(c));
    tob.Perform(w,true);  //toa will always be used, whether vertical or not
  } else {
    w = TopoDS::Wire(status.getTool()->getProfile());
  }
  toa.Perform(w,true);
  BRepOffsetAPI_MakePipeShell pipe(BRepBuilderAPI_MakeWire(TopoDS::Edge(myUnSolid)).Wire());
  if (!w.Closed()) {
    infoMsg("Wire not closed!");
  } else {
      pipe.Add(toa.Shape(),false,true);  //transform the sweep outline, so that the pipe will be located correctly
    if (vert) {
      pipe.Add(tob.Shape(),false,true);
    } else {
      /*
      FIXME: setmode (or build, if setmode is commented out) causes a crash at
      segf.ngc:N070 - crashes because the edge is too far from horizontal?
      */
      pipe.SetMode(gp_Dir(0,0,1)); //binormal mode: can only rotate about Z
    }
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
  }
  if (!errors) {
    bool mserr = false;
    TopoDS_Shape t = pipe.Shape();
    try {
    pipe.MakeSolid();
    } catch (Standard_ConstructionError) {
      infoMsg("can't make solid - a:" + uio::toString(a) + " b:" + uio::toString(b) +" line: "+ myLine);
      mserr = true;
      solid.Nullify();
    }
    if (mserr) {
      dispShape p(t,getN(),Graphic3d_NOM_NEON_PHC,AIS_Shaded);
      p.display();
    } else {
      ShapeFix_Solid fs(TopoDS::Solid(pipe.Shape()));
      fs.Perform();
      solid = TopoDS::Solid(fs.Solid());
    }
  } else {
    solid.Nullify();
  }
  if (!solid.IsNull()) {
    if (vert) {
      //raise the sweep up by 1 radius
      gp_Pnt v(0,0,status.getTool()->Dia()/2.0);
      gp_Trsf ov;
      ov.SetTranslation(gp::Origin(),v);
      solid = TopoDS::Solid(BRepBuilderAPI_Transform(solid, ov, true).Shape());
    }

/*    BRepCheck_Analyzer bca(solid);
    if (!bca.IsValid()) BRepTools::Dump(solid,std::cout);
    cout << "line N" << getN() << ": solid's mass - " << uio::mass(solid) << endl;
*/

    //is there a sharp corner between the last line and this one?
    if (!((vert) || (!status.getPrevEndDir().IsParallel(status.getStartDir(),angTol)))) {
      myShape = solid; //smooth transition, so we don't need to do anything extra
    } else {
      //translate the tool to the startpoint of the sweep, then fuse it with the sweep
      TopoDS_Shape t;
      toa.Perform(status.getTool()->get3d(),true);
      if (vert) { //add in another tool shape
        tob.Perform(status.getTool()->get3d(),true);
        t = BRepAlgoAPI_Fuse(toa.Shape(),tob.Shape());
      } else {
        t = toa.Shape();
      }
      try {
        myShape = BRepAlgoAPI_Fuse( solid, t );
      } catch (...){
        dispShape s(solid,getN(),Graphic3d_NOM_NEON_PHC,AIS_Shaded);
        s.display();
        infoMsg("Problematic Fuse operation: " + myLine);
        //uio::sleep(1);
      }
    }
  } else {
    infoMsg("pipe not ready!");
    myShape = status.getTool()->get3d();
  }
}
