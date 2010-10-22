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
#include <iostream>

#include <Bnd_Box.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgo_Fuse.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <Geom_Curve.hxx>
#include <gp_Circ.hxx>
#include <gp_Pln.hxx>
#include <GProp_GProps.hxx>
#include <Handle_Geom_Curve.hxx>
#include <Handle_Geom_TrimmedCurve.hxx>
#include <Handle_HLRBRep_Algo.hxx>
#include <HLRAlgo_Projector.hxx>
#include <HLRBRep_Algo.hxx>
#include <HLRBRep_HLRToShape.hxx>
#include <HLRTopoBRep_OutLiner.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <BRepTools.hxx>

tst::tst() {
  QMenu* myMenu = new QMenu("test");
  uio::mb()->insertMenu(uio::hm(),myMenu);
  QAction* myAction = new QAction ( "test1", this );
  myAction->setShortcut(QString("Ctrl+L"));
  //myAction->setStatusTip ( "" );
  connect(myAction,SIGNAL(triggered()),this,SLOT(slotTest1()));
  myMenu->addAction( myAction );
  //uio::window()->showNormal();
}

void tst::slotTest1() {
  //uio::hideGrid();
  //uio::axoView();

  TopoDS_Shape b,p;
  b = ballnose(30,5);
  p = silhouette(b,gp_Dir(0,sqrt(2),sqrt(2)));

  //std::string s = "Mass: ";
  //s += uio::toString(mass(b));
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
The tool is assumed to have rotational symmetry.
Unfortunately, OCC's HLR seems to be broken; some HLR shapes, like OutlineVCompound, do not contain all the edges that they should - not even enough to make a closed wire. The only way to get the sillhouette is to take ALL edges available from HLR, then throw away all but the outermost edges.

TODO: for speed, we should cache silhouettes. Map angle to silhouette for each tool?
\param t the tool
\param d the direction of motion
\return the silhouette
*/
TopoDS_Face tst::silhouette(TopoDS_Shape t, gp_Dir d) {
  TopoDS_Face s;
  //find angle between d and Z
  double theta = d.Angle(gp::DZ());
  //create a vector in YZ with same angle
  gp_Dir hDir(0,sin(theta),cos(theta));
  TopoDS_Compound edges = hlrLines(t,hDir); //get all the edges
  BRepTools::Write(edges,"edges.brep");
  //TODO:transform the result to be normal to pose
  TopoDS_Wire w = outermost(edges);
  BRepBuilderAPI_MakeFace mf(w);
  if (mf.IsDone()) {
    s = mf.Face();
  } else {
    s.Nullify();
    uio::infoMsg("error, cannot make face from outermost wire");
  }
  return s;
}

/** Find the outermost edges
FIXME: incomplete
\param e the edges to sort through
\return a closed TopoDS_Wire
*/
TopoDS_Wire tst::outermost(TopoDS_Compound h) {
  //bounding box. shape is laying along the y axis
  Bnd_Box bbox;
  double aXmin,aYmin,aZmin, aXmax,aYmax,aZmax;
  TopoDS_Wire w;  //this is what we return
  w.Nullify();
  BRepBndLib::Add (h, bbox);
  bbox.Get ( aXmin,aYmin,aZmin, aXmax,aYmax,aZmax ); //we only need the Y's

  //points on tool axis
  gp_Pnt pa(0,aYmin-1.0,0),pb(0,aYmax+1.0,0);

  nearestEdges keepA,keepB; //edge(s) to keep at ends A and B
  keepA = findNearestEdges(h,BRepBuilderAPI_MakeVertex(pa));
  keepB = findNearestEdges(h,BRepBuilderAPI_MakeVertex(pb));
  //TODO: abort if we don't find anything at one end (i.e. keepA.n==0)
  TopTools_ListOfShape lsh;

  TopExp_Explorer Ex(h,TopAbs_EDGE);
  while (Ex.More()) { //check end points so we know what to do with the edge
    bool aA,bA,aB,bB;
    twopnts curr;
    curr = getEnds(TopoDS::Edge(Ex.Current()));
    aA = cmpPntPnts(curr.a,keepA.c,keepA.d); //true for match
    bA = cmpPntPnts(curr.b,keepA.c,keepA.d);
    aB = cmpPntPnts(curr.a,keepB.c,keepB.d);
    bB = cmpPntPnts(curr.b,keepB.c,keepB.d);
    if ( (aA && bA) || (aB && bB) ) {
      //both ends of this edge match the points at one end of the tool, so we don't want it
    } else {
      if ( (aA && (aB||bB)) || (bA && (aB||bB)) ){
        //matches both ends of the tool
        lsh.Append(Ex.Current());
      } else {
        //matches zero or one points. discard (for now)
        //TODO: may need to use these edges in some cases
        //warn potential problem
        //FILE_LOG(logWARN) << "__FILE__ __PRETTY_FUNCTION__ __LINE__ Discarding line that matches < 2 points";
      }
    }
    Ex.Next();
  }
  //append edges from the ends
  lsh.Append(keepA.a);
  if (keepA.n == 2)
    lsh.Append(keepA.b);
  lsh.Append(keepB.a);
  if (keepB.n == 2)
    lsh.Append(keepB.b);

  //make closed wire:

  //take first edge from lsh and put in a wire, remove it from lsh
  TopoDS_Edge e = TopoDS::Edge(lsh.First());
  lsh.RemoveFirst();
  twopnts ends = getEnds(e);
  BRepBuilderAPI_MakeWire mw(e);
  TopTools_ListIteratorOfListOfShape iter;
  bool finished = false;

  //go through the rest of the edges, adding them to the wire in order
  //note - a while loop, nested inside a do-while loop
  int dwcnt = 0;
  do {
    int listsize = lsh.Extent();
    iter.Initialize(lsh);
    int whcnt = 0;
    while (iter.More()) {
      whcnt++;
      if (whcnt > 500) break;
      cout << "check an edge..." << endl;
      //add that edge to wire, remove from lsh
      e = TopoDS::Edge(iter.Value());
      twopnts currentEnds = getEnds(e);
      if (!currentEnds.e) { //no error
        //check for a match and update 'ends'
        bool match = false;
        if (cmpPntPnts(currentEnds.a,ends)) { /* currentEnds.a matches, so replace one of
          'ends' with currentEnds.b - but which one? */
          if (samepnt(currentEnds.a,ends.a))
            ends.a = currentEnds.b;
          else
            ends.b = currentEnds.b;
          match = true;
        }
        if (cmpPntPnts(currentEnds.b,ends)) {
          if (samepnt(currentEnds.b,ends.a))
            ends.a = currentEnds.a;
          else
            ends.b = currentEnds.a;
          match = true;
        }
        if (match){
          lsh.Remove(iter);
          mw.Add(e);
          break;
        }
      } else { //error while finding ends
        lsh.Remove(iter);
        break;
      }
    }
    if ( (ends.a.SquareDistance(ends.b) < 1e-10) || //too loose for BRepBuilderAPI_MakeWire?
              (lsh.IsEmpty()) ) {
      finished = true;
    }
    dwcnt++;
    if ( (lsh.Extent() == listsize) && (dwcnt > 50) ) break;
  } while (!finished);

  if (!lsh.IsEmpty()) {
    //FIXME:error
  } else if (!mw.IsDone()) {
    //FIXME:error
  } else if (!finished) {
    //FIXME:error
  } else {
    w = mw.Wire();
  }
  /*if (!w.IsClosed()) {
  //FIXME:error
  }  */
  return w;
}

bool tst::samepnt(gp_Pnt a, gp_Pnt b) {
  if (a.SquareDistance(b) < 1e-10)
    return true;
  else
    return false;
}

/** Get endpoints of a TopoDS_Edge
\param e the edge
\return two endpoints
*/
twopnts tst::getEnds (TopoDS_Edge e) {
  TopExp_Explorer vx(e,TopAbs_VERTEX);
  twopnts tp;
  int v = 0;
  gp_Pnt prev;
  tp.e = true;
  for (;vx.More();vx.Next()) { //compare vertices; if they match, error. also error if >2 vertices
    v++;
    if (v == 1) {
      tp.a = BRep_Tool::Pnt( TopoDS::Vertex(vx.Current()) );
    } else if (v == 2) {
      tp.b = BRep_Tool::Pnt( TopoDS::Vertex(vx.Current()) );
      if ( tp.a.SquareDistance( tp.b ) > 1e-10 ) {
        tp.e = false;  //not the same point, so no error unless there are more vertices
      }
    }
  }
  if (v > 2) {
    tp.e = true;
    cout << "getends - " << v << " vertices." << endl;
    BRepTools::Dump(e,std::cout);
    //cout << "v = " << v << ". point
  }
  return tp;
}

/*
twopnts tst::getEnds(TopoDS_Edge e) {
  double d1=0,d2=0;
  twopnts tp;
  Handle ( Geom_Curve ) C = BRep_Tool::Curve ( e,d1,d2 );
  if ( d1 == d2 ) {
    tp.a=gp::Origin();
    tp.b=gp::Origin();
    //BRepTools::Dump(e,std::cout);
    cout << "failed to find ends" << endl;
    } else {
      cout << "found ends: " << d1 << "," << d2 << endl;
      tp.a=C->Value ( d1 );
      tp.b=C->Value ( d2 );
    }
    return tp;
    }
    */

/** Compare one point to two others
\param c the point to be compared
\param p1,p2 points to compare it to
\return true for match
*/
bool tst::cmpPntPnts(gp_Pnt c,gp_Pnt p1,gp_Pnt p2) {
  if ( (c.SquareDistance(p1) < 1e-10) //square distance is faster. actual distance would be .00001
    || (c.SquareDistance(p2) < 1e-10) )
    return true;
  else
    return false;
}

/** Compare one point to two others in a twopts struct
\param c the point to be compared
\param tp struct of two points to compare it to
\return true for match
*/
inline bool tst::cmpPntPnts(gp_Pnt c,twopnts tp) {
  return cmpPntPnts(c,tp.a,tp.b);
}

/** Find nearest edges
\param s shape containing the edges
\param t the object/point to measure from
\return a struct containing up to two edges
*/
nearestEdges tst::findNearestEdges(TopoDS_Shape s, TopoDS_Shape t) {
  //TopoDS_Compound elements;
  //BRep_Builder bld;
  nearestEdges ne;
  ne.n = 0;
  ne.e = false;
  BRepExtrema_DistShapeShape dss(s,t);
  if (dss.NbSolution() == 1) {        //one edge
    ne.n = 1;
    ne.a = TopoDS::Edge(dss.SupportOnShape1(1)); //starts counting at 0, right?
    if (ne.a.IsNull()) uio::infoMsg("error, null edge in dss - ne.a");
  } else if (dss.NbSolution() == 2) {   //certain angles could cause two edges to be coincident
    uio::infoMsg("Error, can't solve with two edges");
    ne.e = true;
    //FIXME:how the $%#%^%$&^$ do we solve this?!
    //-->if only one is an arc, choose that one; if both are, choose the one with largest radius
    //if one is a non-circular arc, or if neither is an arc then it becomes really difficult...
  } else if (dss.NbSolution() == 3) {   //2 edges, 1 point - engraving tool?
    uio::infoMsg("3 solutions to dss");
    //only want edges
    TopoDS_Edge tmp;
    for(int i=0; i<dss.NbSolution();i++) {
      if (dss.SupportTypeShape1(i) == BRepExtrema_IsOnEdge ) {
        tmp = TopoDS::Edge(dss.SupportOnShape1(i+1));
        if (tmp.IsNull()) uio::infoMsg("error, null edge in dss - tmp");
        if (ne.n == 0) {
          ne.a = tmp;
          ne.n++;
        } else if (ne.n == 1) {
          ne.b = tmp;
          ne.n++;
        } else {
          ne.e = true;
          uio::infoMsg("Error, too many edges - max 2");
        }
      }
    }
  } else if (dss.NbSolution() == 4) {   //combination of #2 and #3 above
    uio::infoMsg("Error, can't solve with four edges");
    ne.e = true;
  } else { //shouldn't get here
    ne.e = true;
    uio::infoMsg("Error! Cannot find nearest elements, given " + uio::toString(dss.NbSolution()) + " solutions.");
  }
  if (!ne.e) {
    //no errors, so figure out the endpoints
    double d1=0,d2=0;
    gp_Pnt p1,p2;
    //Standard_Real first, last;
    Handle ( Geom_Curve ) C = BRep_Tool::Curve ( ne.a,d1,d2 );
    /*
    std::stringstream ss;
    ss << "n: " << ne.n << " e: " << ne.e << " c: " << uio::toString(ne.c) << " d: " << uio::toString(ne.d) << " C: " << &C << std::endl;
    ss << " d1: " << d1 << " &ne: " << &ne << " &ne.n: " << &(ne.n) << &ne.n << " &ne.e: " << &(ne.e) << std::endl;
    ss << ne.meminfo();
    if (ne.a.IsNull()) ss << "ne.a is null" << std::endl;
    else ss << "ne.a not null" << std::endl;
    //uio::infoMsg(ss.str());
    cout << ss.str();
    */
    if (d1 == d2) {
      cout << "params zero - skipping" << endl;
      ne.e = true;
      BRepAdaptor_Curve adaptor = BRepAdaptor_Curve ( ne.a );
      if ( adaptor.GetType() ==GeomAbs_Circle ) {
        cout << "Arc" << endl;
      } else if ( adaptor.GetType() ==GeomAbs_Line ) {
        cout << "Line" << endl;
      } else cout << "Edge" << endl;
      //BRepTools::Dump(ne.a,std::cout);
    } else {
      cout << "ends: " << d1 << "," << d2 << endl;
      p1=C->Value ( d1 );
      p2=C->Value ( d2 );
      if (ne.n == 1) {
        ne.c = p1;
        ne.d = p2;
      } else {
        gp_Pnt p3,p4;
        C = BRep_Tool::Curve ( ne.b,d1,d2 );
        p3 = C->Value(d1);
        p4 = C->Value(d2);
        /*
        compare the points; p1 or p2 should match one of p3, p4.
        *must* have 3 unique points, so find two that match
        and then look for another (problem if found)
        */
        //alternate: use a line between the outlier points and find all edges that intersect it?
        int matches = 0;
        if (p1.SquareDistance(p3) < 1e-10) {
          matches++;
          ne.c = p2;
          ne.d = p4;
        }
        if (p1.SquareDistance(p4) < 1e-10) {
          matches++;
          ne.c = p2;
          ne.d = p3;
        }
        if (p2.SquareDistance(p3) < 1e-10) {
          matches++;
          ne.c = p1;
          ne.d = p4;
        }
        if (p2.SquareDistance(p4) < 1e-10) {
          matches++;
          ne.c = p1;
          ne.d = p3;
        }
        if (matches < 1) {
          uio::infoMsg("Error, no matches");
        }
        if (matches > 1) {
          uio::infoMsg("Error, too many points coincident: " + uio::toString(matches));
        }
      }
    }
  }
  return ne;
}

/** Find all hlr edges for a shape.
OCC's HLR algorithms are used to find these edges. Unfortunately, HLR seems to be broken. The only way to get the outline is to take ALL edges available from HLR (what this function does), and throw away all but the outermost edges (which is what tst::outermost() does).
This function also removes zero-length lines.
\param t the shape
\param dir the direction. <b>Must</b> be normal to the x-axis.
\return the edges, as a TopoDS_Compound.
*/
TopoDS_Compound tst::hlrLines(TopoDS_Shape t, gp_Dir dir) {
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

  //get rid of any edges whose endpoints are coincident
  TopoDS_Compound e;
  builder.MakeCompound(e);
  TopExp_Explorer ex(comp,TopAbs_EDGE);
  TopExp_Explorer vx;
  int ec = 0, vc = 0; //edge and vertex count
  for (;ex.More();ex.Next()) {
    ec++;
    vx.Init(TopoDS::Edge(ex.Current()),TopAbs_VERTEX);
    int v = 0;
    gp_Pnt prev;
    bool discard = true;
    for (;vx.More();vx.Next()) { //compare vertices; if they match, error. also error if >2 vertices
      v++;
      if (v == 1) {
        prev = BRep_Tool::Pnt( TopoDS::Vertex(vx.Current()) );
      } else /*if (v == 2)*/ {
        if ( prev.SquareDistance( BRep_Tool::Pnt( TopoDS::Vertex(vx.Current()) ) ) > 1e-10 ) {
          discard = false;  //not the same point, so we keep this edge
        }
      }
      vc += v; //add current vertex count to total
    }
    if (discard) {
      cout << "skipped zero-length edge" << endl;
    } else {
      builder.Add(e,ex.Current());
    }
  }
  cout << "hlrLines - " << ec << " edges, " << vc << " vertices." << endl;
  return e;
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

nearestEdges::nearestEdges() {
  n=0;
  a.Nullify();
  b.Nullify();
  c=gp::Origin();
  d=gp::Origin();
  e=false;
}

std::string nearestEdges::meminfo() {
  std::stringstream ss;
  ss << "meminfo n: " << n << " e: " << e << " &ne: " << this << " &ne.n: " << &n << " &ne.e: " << &e << std::endl;
  return ss.str();
}
