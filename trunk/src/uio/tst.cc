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
#include <BRepAlgo_Fuse.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Face.hxx>
#include <Handle_HLRBRep_Algo.hxx>
#include <HLRBRep_Algo.hxx>
#include <HLRBRep_HLRToShape.hxx>
#include <HLRAlgo_Projector.hxx>
#include <gp_Pln.hxx>
#include <TopoDS_Edge.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <HLRTopoBRep_OutLiner.hxx>

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
  b = ballnose(30,5);
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
The tool is assumed to have rotational symmetry.
Unfortunately, OCC's HLR seems to be broken; some HLR shapes, like OutlineVCompound, do not contain all the edges that they should - not even enough to make a closed wire. The only way to get the sillhouette is to take ALL edges available from HLR, then throw away all but the outermost edges.
\param t the tool
\param pose the direction of motion
\return the silhouette
*/
TopoDS_Face tst::silhouette(TopoDS_Shape t, gp_Ax1 pose) {
  gp_Dir hDir = ... //need to rotate pose.Direction() to be normal to x for HlrLines(...)
  TopoDS_Compound edges = hlrLines(t,hDir); //get all the edges
  //TODO:remove all internal edges, and transform the result to be normal to pose
  TopoDS_Wire outer = outermost(edges);

}

/** Find the outermost edges
\param e the edges to sort through
\return a closed TopoDS_Wire
*/
TopoDS_Wire tst::outermost(TopoDS_Compound h) {
  //bounding box. shape is laying along the y axis
  Bnd_Box bbox;
  double aXmin,aYmin,aZmin, aXmax,aYmax,aZmax;
  BRepBndLib::Add (h, bbox);
  bbox.Get ( aXmin,aYmin,aZmin, aXmax,aYmax,aZmax ); //we only need the Y's

  //points on tool axis
  gp_Pnt pa(0,aYmin-1.0,0),pb(0,aYmax+1.0,0);
  TopoDS_Vertex a(pa), b(pb); //probably need BRepPrim_Builder::MakeVertex (a,pa);
  TopoDS_Edge edgeA,edgeB;
  //find edges closest to those points and save them
/*
  TopExp_Explorer ex;
  for(ex.Init(h,TopAbs_Edge),ex.More(),ex.Next()) {
    ex.Current()
  }
*/
  //throw away other edges that touch the axis and/or have the same endpoints
  //make closed wire
}
TopoDS_Compound tst::findNearestElements(TopoDS_Shape s, gp_Pnt p) {
BRepExtrema_DistShapeShape dss(h,a);
    if (dss.NbSolution() != 1) {
    //should always be exactly 1
    //TODO: determine if that is the case - if not, need to find another way to determine which edge is the right one to use
    //engraving tool comes to mind... nbsolutions would probably = 3: 2 edges, 1 point
    //also other tools, when at certain angles that would cause two edges to be coincident at Y
  } else {

  }

}

/** Find all hlr edges for a shape.
OCC's HLR algorithms are used to find these edges. Unfortunately, HLR seems to be broken. The only way to get the outline is to take ALL edges available from HLR (what this function does), and throw away all but the outermost edges (which is what tst::outermost() does).
\param t the shape
\param dir the direction. <b>Must</b> be normal to the x-axis.
\return the edges, as a TopoDS_Compound.
*/
TopoDS_Compound& tst::hlrLines(TopoDS_Shape t, gp_Dir dir) {
  gp_Pnt viewpnt(0,0,1);  //probably doesn't matter what this point is
  gp_Dir xDir(-1,0,0);
  gp_Trsf pTrsf;
  pTrsf.SetTransformation(gp_Ax3(viewpnt,dir,xDir));
  Handle(HLRBRep_Algo) myAlgo = new HLRBRep_Algo();
  myAlgo->Add(t);
  myAlgo->Projector(HLRAlgo_Projector (pTrsf,false,0));
  myAlgo->Update();
  myAlgo->Hide(); //from pdf
  myAlgo->Update(); //just in case

  HLRBRep_HLRToShape aHLRToShape(myAlgo);

  TopoDS_Compound comp;
  BRep_Builder builder;
  builder.MakeCompound( comp );
  TopoDS_Shape VCompound = aHLRToShape.VCompound();
  if(!VCompound.IsNull())
    builder.Add(comp, VCompound);
  TopoDS_Shape Rg1LineVCompound = aHLRToShape.Rg1LineVCompound();
  if(!Rg1LineVCompound.IsNull())
    builder.Add(comp, Rg1LineVCompound);
  TopoDS_Shape RgNLineVCompound = aHLRToShape.RgNLineVCompound();
  if(!RgNLineVCompound.IsNull())
    builder.Add(comp, RgNLineVCompound);
  TopoDS_Shape OutLineVCompound = aHLRToShape.OutLineVCompound();
  if(!OutLineVCompound.IsNull())
    builder.Add(comp, OutLineVCompound);
  TopoDS_Shape IsoLineVCompound = aHLRToShape.IsoLineVCompound();
  if(!IsoLineVCompound.IsNull())
    builder.Add(comp, IsoLineVCompound);
  TopoDS_Shape HCompound = aHLRToShape.HCompound();
  if(!HCompound.IsNull())
    builder.Add(comp, HCompound);
  TopoDS_Shape Rg1LineHCompound = aHLRToShape.Rg1LineHCompound();
  if(!Rg1LineHCompound.IsNull())
    builder.Add(comp, Rg1LineHCompound);
  TopoDS_Shape RgNLineHCompound = aHLRToShape.RgNLineHCompound();
  if(!RgNLineHCompound.IsNull())
    builder.Add(comp, RgNLineHCompound);
  TopoDS_Shape OutLineHCompound = aHLRToShape.OutLineHCompound();
  if(!OutLineHCompound.IsNull())
    builder.Add(comp, OutLineHCompound);
  TopoDS_Shape IsoLineHCompound = aHLRToShape.IsoLineHCompound();
  if(!IsoLineHCompound.IsNull())
    builder.Add(comp, IsoLineHCompound);

  return comp;
}

/* things I've tried for silhouette that failed
////////////////////////////////////////////////////////////////////////////

  //form a wire for the face:
  create a horiz line.
  find which lines are parallel to it - those are the sides
  lines that intersect it are the ends, find the two which are farthest apart


//  dispShape c(comp);
//  c.display();
//  TopoDS_Face silhouette;
//  silhouette = BRepBuilderAPI_MakeFace(TopoDS::Wire(HLRTopoBRep_OutLiner(comp).OutLinedShape()));

////////////////////////////////////////////////////////////////////////////
  gp_Trsf tr;
  tr.SetTranslation(gp_Vec(6,0,0));
  dispShape vc(BRepBuilderAPI_Transform(aHLRToShape.VCompound(),tr).Shape());
  vc.display();
  tr.SetTranslation(gp_Vec(12,0,0));
  dispShape ovc(BRepBuilderAPI_Transform(aHLRToShape.OutLineVCompound(),tr).Shape());
  ovc.display();
  tr.SetTranslation(gp_Vec(18,0,0));
  dispShape rgn(BRepBuilderAPI_Transform(aHLRToShape.RgNLineVCompound(),tr).Shape());
  rgn.display();
  tr.SetTranslation(gp_Vec(24,0,0));
  dispShape hc(BRepBuilderAPI_Transform(aHLRToShape.HCompound(),tr).Shape());
  hc.display();
  tr.SetTranslation(gp_Vec(30,0,0));
  dispShape rgoh(BRepBuilderAPI_Transform(aHLRToShape.Rg1LineHCompound(),tr).Shape());
  rgoh.display();
  tr.SetTranslation(gp_Vec(36,0,0));
  dispShape rgnh(BRepBuilderAPI_Transform(aHLRToShape.RgNLineHCompound(),tr).Shape());
  rgnh.display();
  tr.SetTranslation(gp_Vec(42,0,0));
  dispShape ilh(BRepBuilderAPI_Transform(aHLRToShape.IsoLineHCompound(),tr).Shape());
  ilh.display();
  tr.SetTranslation(gp_Vec(48,0,0));
  dispShape rgo(BRepBuilderAPI_Transform(aHLRToShape.Rg1LineVCompound(),tr).Shape());
  rgo.display();
  tr.SetTranslation(gp_Vec(54,0,0));
  dispShape ohc(BRepBuilderAPI_Transform(aHLRToShape.OutLineHCompound(),tr).Shape());
  ohc.display();
  tr.SetTranslation(gp_Vec(60,0,0));
  dispShape ilv(BRepBuilderAPI_Transform(aHLRToShape.IsoLineVCompound(),tr).Shape());
  ilv.display();
////////////////////////////////////////////////////////////////////////////

  probably don't need either of these methods but leave them here for now...
  //TopoDS_Compound pllels, intrct;
  //getParallels(comp, &pllels); //find all edges in comp that are not arcs, but are parallel to y
  //getIntersects(comp,&intrct); //find all edges that intersect X=0;
  /// http://www.opencascade.org/org/forum/thread_11613/
  //outermost(intrct)); //find the outermost edges

////////////////////////////////////////////////////////////////////////////
  //discard all edges which aren't connected
  //compute the bounding box, and create two points on the axis, one beyond either end of the box
  //for each point, find the closest edge
  //discard all other edges which cross the axis
  //create a wire with the remaining edges

////////////////////////////////////////////////////////////////////////////

from docs: "Tries to build wires of maximum length. Building a wire is stopped when no edges can be connected to it at its head or at its tail. "
ConnectEdgesToWires (Handle(TopTools_HSequenceOfShape)&edges, const Standard_Real toler, const Standard_Boolean shared, Handle(TopTools_HSequenceOfShape)&wires)
////////////////////////////////////////////////////////////////////////////

 this does not work - uses the wrong edges, then gives up
  Handle(TopTools_HSequenceOfShape) Edges = new TopTools_HSequenceOfShape();
  for (TopExp_Explorer Ex(comp,TopAbs_EDGE); Ex.More(); Ex.Next())
    Edges->Append(TopoDS::Edge(Ex.Current()));

  Handle(TopTools_HSequenceOfShape) Wires = new TopTools_HSequenceOfShape(); //Will hold the wires found
  ShapeAnalysis_FreeBounds::ConnectEdgesToWires(Edges,Precision::Confusion(),Standard_False,Wires);
  if (Wires->Length() == 1) {
    //FIXME: will still be in XY plane... need it to be perpendicular to dir
    silhouette = BRepBuilderAPI_MakeFace(TopoDS::Wire(Wires->Value(1)));
  } else {
    std::string s = "Error, wrong number of wires: ";
    s += uio::stringify(Wires->Length());
    uio::infoMsg(s);
    silhouette.Nullify();
  }
////////////////////////////////////////////////////////////////////////////


// this does not work
  dispShape outl(HLRTopoBRep_OutLiner(comp).OutLinedShape());
  outl.display();
*/

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