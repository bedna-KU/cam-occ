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
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <Handle_Geom_TrimmedCurve.hxx>
#include <Handle_HLRBRep_Algo.hxx>
#include <HLRBRep_Algo.hxx>
#include <HLRBRep_HLRToShape.hxx>
#include <GC_MakeArcOfCircle.hxx>
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
  revol.Nullify();
}

const TopoDS_Solid& millTool::getRevol() {
  if (revol.IsNull()) {
    gp_Ax1 vertical(gp_Pnt(0,0,0),gp_Dir(0,0,1));
    BRepBuilderAPI_MakeFace mkF(profile);
    BRepPrimAPI_MakeRevol rev(mkF.Face(),vertical,M_PI,true);
    validRev = rev.IsDone();
    if (validRev)
      revol = TopoDS::Solid( rev.Shape() );
  }
  return revol;
}


/**
Projects the 3d model of the tool onto a plane normal to XY.
Some code from http://www.opencascade.org/org/forum/thread_16928/
\param deg Projection angle, in degrees. Sign is ignored.
\return the projection as a TopoDS_Face.
\sa getProfile(), getRevol()
*/
//if this doesnt work, try raytracing
// see also OpenCASCADE6.3.0/samples/standard/mfc/12_HLR/src/SelectionDialog.cpp
//Prs3d_Projector (const Standard_Boolean Pers, const Quantity_Length Focus, const Quantity_Length DX, const Quantity_Length DY, const Quantity_Length DZ, const Quantity_Length XAt, const Quantity_Length YAt, const Quantity_Length ZAt, const Quantity_Length XUp, const Quantity_Length YUp, const Quantity_Length ZUp)
const TopoDS_Face& millTool::getProj(degrees deg) {
  double zloc = 5; //zloc is z coordinate of projection
  Handle(HLRBRep_Algo) myAlgo = new HLRBRep_Algo();
  myAlgo->Add(profile);
  Prs3d_Projector myProj(false,0, 0,0,zloc, 0,0,1, 0,0,1); //point
  myAlgo->Projector(myProj.Projector());
  myAlgo->Update();
  HLRBRep_HLRToShape aHLRToShape(myAlgo);
  TopoDS_Shape Proj = aHLRToShape.VCompound();
  return TopoDS::Face(Proj);
}

//aptTool::aptTool() {}

ballnoseTool::ballnoseTool(double diameter, double length) {
  double r = diameter/2.0;
  validProfile = false;
  Handle(Geom_TrimmedCurve) Tc;
  Tc = GC_MakeArcOfCircle (gp_Pnt(r,0,r), gp_Pnt(0,0,0), gp_Pnt(-r,0,r));
  TopoDS_Edge Ec = BRepBuilderAPI_MakeEdge(Tc);
  TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(gp_Pnt(r,0,r), gp_Pnt(r,0,length));
  TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(gp_Pnt(-r,0,r), gp_Pnt(-r,0,length));
  TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(gp_Pnt(-r,0,length), gp_Pnt(r,0,length));
  BRepBuilderAPI_MakeWire wm(Ec,E1,E2,E3);
  if ( wm.IsDone() ) {
    profile = wm.Wire();
    validProfile = true;
    shape = BALLNOSE;
    /*
    TopoDS_Wire w = wm.Wire();
    if ( w.Closed() ) {
      BRepBuilderAPI_MakeFace f(w);
      if (f.IsDone()) {
	profile = f.Face();
	validProfile = true;
	shape = BALLNOSE;
      }
    }
    */
  }
}

latheTool::latheTool() {
 type = TURNING_TOOL;
}