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
#include "tool.hh"
#include "uio.hh"

#ifdef MULTITHREADED
#include <QMutex>
#endif //MULTITHREADED

#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <Handle_Geom_TrimmedCurve.hxx>
#include <Handle_HLRBRep_Algo.hxx>
#include <HLRBRep_Algo.hxx>
#include <HLRBRep_HLRToShape.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <gp_Pln.hxx>
#include <Prs3d_Projector.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>

//tool.cc - functions in classes tool, millTool, etc

tool::tool()/*: profile()*/ {
  validProfile = false;
  profile.Nullify();
  type = UNDEFINED;
}

millTool::millTool() {
  shape = UNDEF;
  type = ROTARY_TOOL;
  dia=0.0;
  len=0.0;
  myShape.Nullify();
}

const TopoDS_Shape& millTool::get3d() {
  if (myShape.IsNull()) {
    BRepBuilderAPI_MakeFace mkF(gp_Pln(gp::XOY()),profile);
    BRepPrimAPI_MakeRevol rev(mkF.Face(),gp::OZ(),M_PI,true);
    validSolid = rev.IsDone();
    if (validSolid)
      myShape = rev.Shape();
  }
  return myShape;
}

const TopoDS_Shape& ballnoseTool::get3d() {
  #ifdef MULTITHREADED
  QMutex get3dMutex;
  QMutexLocker g3ml(&get3dMutex);  //locks mutex on create, unlocks on destroy
  #endif //MULTITHREADED
  if (!myShape.IsNull()) {
    return myShape;
  } else {
    double r = dia / 2.0;
    TopoDS_Solid s,c;
    gp_Ax2 axis(gp_Pnt(0,0,r),gp::DZ()); //for cylinder. 0,0,r is the center of the bottom face
    s = TopoDS::Solid(BRepPrimAPI_MakeSphere(gp_Pnt(0,0,r),r).Solid());
    c = TopoDS::Solid(BRepPrimAPI_MakeCylinder(axis,r,len-r).Solid());
    BRepAlgoAPI_Fuse f(s,c);
    validSolid = f.IsDone();
    if (validSolid) {
      myShape = f.Shape();
    }
  }
  return myShape;
}

ballnoseTool::ballnoseTool(double diameter, double length) {
  dia = diameter;
  len = length;
  if (len < dia) len = dia;

  double r = dia/2.0;
  validProfile = false;
  Handle(Geom_TrimmedCurve) Tc;
  Tc = GC_MakeArcOfCircle (gp_Pnt(r,0,r), gp::Origin(), gp_Pnt(-r,0,r));
  TopoDS_Edge Ec = BRepBuilderAPI_MakeEdge(Tc);
  TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(gp_Pnt(r,0,r), gp_Pnt(r,0,length));
  TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(gp_Pnt(-r,0,r), gp_Pnt(-r,0,length));
  TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(gp_Pnt(-r,0,length), gp_Pnt(r,0,length));
  BRepBuilderAPI_MakeWire wm(Ec,E1,E2,E3);
  if ( wm.IsDone() ) {
    profile = wm.Wire();
    if ( profile.Closed() ) {
      validProfile = true;
      shape = BALLNOSE;
    }
  }
  if (!validProfile) {
    uio::infoMsg("Error, invalid ball tool profile len=" + uio::toString(len) + " dia=" + uio::toString(dia));
  }
}

const TopoDS_Shape& cylindricalTool::get3d() {
  #ifdef MULTITHREADED
  QMutex get3dMutex;
  QMutexLocker g3ml(&get3dMutex);  //locks mutex on create, unlocks on destroy
  #endif //MULTITHREADED
  if (!myShape.IsNull()) {
    return myShape;
  } else {
    double r = dia / 2.0;
    gp_Ax2 axis(gp_Pnt(0,0,0),gp::DZ()); //for cylinder. 0,0,0 is the center of the bottom face
    myShape = TopoDS::Solid(BRepPrimAPI_MakeCylinder(axis,r,len).Solid());
  }
  return myShape;
}

cylindricalTool::cylindricalTool(double diameter, double length) {
  dia = diameter;
  len = length;
  double r = dia/2.0;
  validProfile = false;
  TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(gp_Pnt(r,0,0), gp_Pnt(r,0,length));
  TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(gp_Pnt(-r,0,0), gp_Pnt(-r,0,length));
  TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(gp_Pnt(-r,0,length), gp_Pnt(r,0,length));
  TopoDS_Edge E4 = BRepBuilderAPI_MakeEdge(gp_Pnt(r,0,0),gp_Pnt(-r,0,0));
  BRepBuilderAPI_MakeWire wm(E4,E1,E2,E3);
  if ( wm.IsDone() ) {
    profile = wm.Wire();
    if ( profile.Closed() ) {
      validProfile = true;
      shape = CYLINDRICAL;
    }
  }
  if (!validProfile) {
    uio::infoMsg("Error, invalid cylindrical tool profile len=" + uio::toString(len) + " dia=" + uio::toString(dia));
  }
}

latheTool::latheTool() {
 type = TURNING_TOOL;
}