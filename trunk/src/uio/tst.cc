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
#include <limits.h>

#include "tst.hh"
#include "dispShape.hh"
#include "uio.hh"

#include <QMenu>
#include <QAction>

#include <Handle_Geom_TrimmedCurve.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
//#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <Handle_HLRBRep_Algo.hxx>
#include <HLRBRep_Algo.hxx>
#include <HLRBRep_HLRToShape.hxx>
#include <HLRAlgo_Projector.hxx>
#include <gp_Pln.hxx>
#include <TopoDS_Edge.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>


tst::tst() {
  QMenu* myMenu = new QMenu("test");
  uio::mb()->insertMenu(uio::hm(),myMenu);
  QAction* myAction = new QAction ( "test1", this );
  myAction->setShortcut(QString("Ctrl+L"));
  //myAction->setStatusTip ( "" );
  connect(myAction,SIGNAL(triggered()),this,SLOT(slotTest1()));
  myMenu->addAction( myAction );

}

void tst::slotTest1() {
  uio::hideGrid();
  //uio::axoView();

  TopoDS_Shape b,p;
  b = ballnose(3.0,.5);
  p = getProj(b,gp_Dir(0,sqrt(2),sqrt(2)));

  //std::string s = "Mass: ";
  //s += uio::stringify(mass(b));
  //uio::infoMsg(s);
  dispShape ds(p);
  ds.display();
  uio::axoView();
  uio::fitAll();
  uio::sleep(1);
  dispShape dt(b);
  dt.display();
  uio::fitAll();
}

///Create a solid tool, with tip at (0,0,0)
TopoDS_Shape tst::ballnose(double len, double dia) {
  double r = dia / 2.0;
  TopoDS_Solid s,c;
  TopoDS_Shape tool;
  gp_Ax2 axis(gp_Pnt(0,0,r),gp::DZ()); //for cylinder. 0,0,r is the center of the bottom face
  s = TopoDS::Solid(BRepPrimAPI_MakeSphere(gp_Pnt(0,0,r),r).Solid());
  c = TopoDS::Solid(BRepPrimAPI_MakeCylinder(axis,r,len-r).Solid());
  tool = BRepAlgoAPI_Fuse(s,c);
  return tool;
}

/** Compute the silhouette of a tool
FIXME: incomplete
\param t the shape
\param dir the direction. <b>Must</b> be normal to the x-axis.
\return the silhouette
*/
TopoDS_Shape tst::getProj(TopoDS_Shape t, gp_Dir dir) {
  gp_Pnt viewpnt(0,0,1);  //probably doesn't matter what this point is
  gp_Dir xDir(1,0,0);
  gp_Trsf pTrsf;
  pTrsf.SetTransformation(gp_Ax3(viewpnt,dir,xDir));
  Handle(HLRBRep_Algo) myAlgo = new HLRBRep_Algo();
  myAlgo->Add(t);
  myAlgo->Projector(HLRAlgo_Projector (pTrsf,false,0));
  myAlgo->Update();
  HLRBRep_HLRToShape aHLRToShape(myAlgo);

  //TODO: join the correct compounds together, remove all internal edges, and transform the result to be perpendicular to dir
  //looks like we only need OutlineVCompound and VCompound but there are many others

  //TopoDS_Shape Proj = aHLRToShape.VCompound();
  //TopoDS_Shape Proj = BRepAlgoAPI_Fuse( aHLRToShape.OutLineVCompound(),
  //                                      aHLRToShape.VCompound());
  //                                      aHLRToShape.OutLineHCompound());
  dispShape ds(aHLRToShape.OutLineVCompound());
  ds.display();
  uio::fitAll();
  uio::axoView();
  uio::sleep(1,true);
  return aHLRToShape.VCompound();
  //return aHLRToShape.OutLineVCompound();
}
double tst::mass(TopoDS_Shape s) {
  double m;
  GProp_GProps System;
  BRepGProp::VolumeProperties ( s,System );
  m = System.Mass();
  //cout << "Mass " << m << endl;
  return m;
}

TopoDS_Wire tst::halfProf() {
  double r = .25;
  double len = 3;

  //build half
  Handle(Geom_TrimmedCurve) Tc;
  double rsqrt = r / sqrt(2.0); //for midpoint of circle
  Tc = GC_MakeArcOfCircle (gp_Pnt(r,0,r), gp_Pnt(rsqrt,0,r-rsqrt), gp_Pnt(0,0,0));
  TopoDS_Edge Ec = BRepBuilderAPI_MakeEdge(Tc);
  TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(gp_Pnt(r,0,r), gp_Pnt(r,0,len));
  TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(gp_Pnt(0,0,0), gp_Pnt(0,0,len));
  TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(gp_Pnt(0,0,len), gp_Pnt(r,0,len));
  BRepBuilderAPI_MakeWire wm(Ec,E1,E2,E3);
  return wm.Wire();
}

/*
timespec ts;
clock_gettime(CLOCK_REALTIME, &ts); // Works on Linux
*/

/*

  BRepBuilderAPI_MakeFace mkF(gp_Pln(gp::XOY()),wm.Wire());
  BRepPrimAPI_MakeRevol rev(mkF.Face(),gp::OZ(),M_PI*2.0,true);

  TopoDS_Solid t,c,s;
  t = TopoDS::Solid(rev.Shape());
  //s = TopoDS::Solid(BRepPrimAPI_MakeBox(gp_Pnt(-1,-1,-1),gp_Pnt(1,1,1)).Solid());
  s = TopoDS::Solid(BRepPrimAPI_MakeSphere(gp_Pnt(0,0,r),r).Solid());
  //c = TopoDS::Solid(BRepAlgoAPI_Cut(s,t));

*/