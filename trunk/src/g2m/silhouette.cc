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
/*list of functions that may be helpful for silhouette

 BRepTools::Map3DEdges  (   const TopoDS_Shape &     S,        TopTools_IndexedMapOfShape & M)   [static]

TopTools_IndexedMapOfShape aMapOfVertex;
Draft_VertexInfo
Draft_DataMapOfVertexVertexInfo
ChFiDS_Map

TopTools_IndexedDataMapOfShapeListOfShape

***** TopExp::MapShapesAndAncestors

*use a compound to hold discarded pieces, dump to file
*/

#include "silhouette.hh"
#include <gp_Pnt.hxx>
#include <gp_Ax3.hxx>
#include <Handle_HLRBRep_Algo.hxx>
#include <HLRBRep_Algo.hxx>
#include <HLRAlgo_Projector.hxx>
#include <HLRBRep_HLRToShape.hxx>
#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>
#include <TopExp.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <gp_Lin.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <TopTools_MapIteratorOfMapOfOrientedShape.hxx>
#include <TopTools_MapOfOrientedShape.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <Handle_TopTools_HSequenceOfShape.hxx>
#include <BRepTools_ReShape.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepTools.hxx>
#include <TopOpeBRep_EdgesIntersector.hxx>
#include <TopOpeBRep_Point2d.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <ShapeFix_Edge.hxx>
#include <Geom_Plane.hxx>
#include <TopLoc_Location.hxx>

/*
#include <.hxx>
#include <.hxx>
#include <.hxx>
#include <.hxx>
#include <.hxx>
#include <.hxx>
#include <.hxx>
#include <.hxx>
#include <.hxx>
#include <.hxx>
#include <.hxx>
*/



silhouette::silhouette(TopoDS_Shape tool, gp_Dir normal, double radius, double height):
        myTool(tool), myRadius(radius), myHeight(height) {

    newAngle(normal);
}

void silhouette::init(TopoDS_Shape tool, gp_Dir normal, double radius, double height) {
  myTool = tool;
  myRadius = radius;
  myHeight = height;

  newAngle(normal);
}


void silhouette::newAngle(gp_Dir normal) {
    myNormal = normal;
    myOutline.Nullify();
    myDone = false;
    //myEdges.Clear();

    createHlrLines(myTool,myNormal);

    // remove zero-length edges?
    dumpMap();
    bool r,p;
    int iter=0;
    do {
      iter++;
        p = prune();
        r = reduceMultis();
    } while (r || p);

    //create a wire from the remaining edges
    createWireFromEdgeSeq(getRemainingEdges());
}

void silhouette::dumpMap() {
  hlrMap.Statistics(std::cout);
}

///find all remaining edges in hlrMap and return them as an HSequence, filtering out duplicates
Handle_TopTools_HSequenceOfShape silhouette::getRemainingEdges() {
  TopTools_MapOfOrientedShape mos;  //use a map because the hashing will remove duplicates
  TopTools_ListOfShape los;
  int n = hlrMap.Extent();
  for (int i = 1; i <= n; i++) {
    los = hlrMap.FindFromIndex(i);
    int l = los.Extent();  //should always be 2 or 0 - otherwise the map wasn't cleaned up correctly
    if (l == 2) {
      mos.Add(los.First());
      mos.Add(los.Last());
      std::cout << "adding edges to mos" << std::endl;
    } else if (l != 0) {
      std::cout << "error! los.extent = " << l << " at i=" << i << endl;
    }
    los.Clear();
  }
  //copy from map to hsequence, for createWireFromEdgeList()
  TopTools_MapIteratorOfMapOfOrientedShape mimos;
  Handle_TopTools_HSequenceOfShape hsos = new TopTools_HSequenceOfShape;
  mimos.Initialize(mos);
  for (;mimos.More();mimos.Next()) {
    hsos->Append(mimos.Key());
  }
  return hsos;
}

///create a wire from an unordered sequence of edges that has no duplicates
void silhouette::createWireFromEdgeSeq ( Handle_TopTools_HSequenceOfShape edges ) {
  Handle_TopTools_HSequenceOfShape NewWires = new TopTools_HSequenceOfShape();
  double tol = Precision::Confusion();
  ShapeAnalysis_FreeBounds::ConnectEdgesToWires(edges,tol,Standard_False,NewWires);

  int l = NewWires->Length();
  if ( l >= 1) {
    myOutline = TopoDS::Wire(NewWires->Value(1));
  }
  if ( l != 1) {
    std::cout << "error, " << l << " wires from ShapeAnalysis_FreeBounds::ConnectEdgesToWires!" << std::endl <<
    "started with " << edges->Length() << " edges" << std::endl;
  } else {
    myDone = true;
  }
}

///creates the hlr lines. NOTE: they are in the XY plane!
void silhouette::createHlrLines(TopoDS_Shape t, gp_Dir dir) {
    if (!BRepCheck_Analyzer(t).IsValid() ) {
      std::cout << "bca: tool bad" << std::endl;
    }

    gp_Pnt viewpnt(0,0,1);  //probably doesn't matter what this point is
    gp_Dir xDir(-1,0,0);
    gp_Trsf pTrsf;
    pTrsf.SetTransformation(gp_Ax3(viewpnt,dir,xDir));
    Handle(HLRBRep_Algo) myAlgo = new HLRBRep_Algo();
    myAlgo->Add(t);
    myAlgo->Projector(HLRAlgo_Projector (pTrsf,false,0));
    myAlgo->Update();
    myAlgo->Hide(); //from pdf
    //myAlgo->Update(); //just in case

    HLRBRep_HLRToShape aHLRToShape(myAlgo);

    TopoDS_Compound comp;
    BRep_Builder builder;
    builder.MakeCompound( comp );

    TopoDS_Shape VCompound = aHLRToShape.VCompound();
    if (!VCompound.IsNull())
        builder.Add(comp, VCompound);
    TopoDS_Shape Rg1LineVCompound = aHLRToShape.Rg1LineVCompound();
    if (!Rg1LineVCompound.IsNull())
        builder.Add(comp, Rg1LineVCompound);
    TopoDS_Shape RgNLineVCompound = aHLRToShape.RgNLineVCompound();
    if (!RgNLineVCompound.IsNull())
        builder.Add(comp, RgNLineVCompound);
    TopoDS_Shape OutLineVCompound = aHLRToShape.OutLineVCompound();
    if (!OutLineVCompound.IsNull())
        builder.Add(comp, OutLineVCompound);
    TopoDS_Shape IsoLineVCompound = aHLRToShape.IsoLineVCompound();
    if (!IsoLineVCompound.IsNull())
        builder.Add(comp, IsoLineVCompound);
    TopoDS_Shape HCompound = aHLRToShape.HCompound();
    if (!HCompound.IsNull())
        builder.Add(comp, HCompound);
    TopoDS_Shape Rg1LineHCompound = aHLRToShape.Rg1LineHCompound();
    if (!Rg1LineHCompound.IsNull())
        builder.Add(comp, Rg1LineHCompound);
    TopoDS_Shape RgNLineHCompound = aHLRToShape.RgNLineHCompound();
    if (!RgNLineHCompound.IsNull())
        builder.Add(comp, RgNLineHCompound);
    TopoDS_Shape OutLineHCompound = aHLRToShape.OutLineHCompound();
    if (!OutLineHCompound.IsNull())
        builder.Add(comp, OutLineHCompound);
    TopoDS_Shape IsoLineHCompound = aHLRToShape.IsoLineHCompound();
    if (!IsoLineHCompound.IsNull())
        builder.Add(comp, IsoLineHCompound);
    /*
    each edge has two vertices that it does not share with the other edges
    fix this before putting edges in map!
    */
    if (BRepCheck_Analyzer(comp).IsValid() ) {
      std::cout << "bca: hlr good" << std::endl;
    } else {
      std::cout << "bca: hlr bad" << std::endl;
    }

    fixCommonVertices(comp);
    hlrMap.Clear();
    TopExp::MapShapesAndAncestors(comp,TopAbs_VERTEX,TopAbs_EDGE,hlrMap);

}

///find and check subshapes, ensuring that they aren't null (i.e. handle address = 0xfefdfefdfefd0000)
int silhouette::checkSubShapes(TopoDS_Shape const & c) {
  std::cout << "checkSS" << std::endl;

  int inval = 0;
  if (c.IsNull()) //IsNull compares to UndefinedHandleAddress ( which is 0xfefd0000 or 0xfefdfefdfefd0000 )
    return 1;
  //TopAbs_ShapeEnum contains 8 useable types. use i to iterate over them. they are in order from most to least inclusive
  TopAbs_ShapeEnum se;
  for (se = TopAbs_ShapeEnum(c.ShapeType()+1); se<9 ; se=TopAbs_ShapeEnum(se+1)) {
    TopExp_Explorer ex(c,se);
    int cnt = 0;
    for (; ex.More(); ex.Next() ) {
      std::cout << "checkSS: " << se << " num: " << cnt++ << std::endl;
      if (ex.Current().IsNull()) {
        inval++;
      } else if (ex.Current().ShapeType() != TopAbs_VERTEX) { //vertex will have no sub shapes
        inval += checkSubShapes(ex.Current());
      }
    }
  }
  return inval;
}

///replaces close/identical vertices with a single vertex
void silhouette::fixCommonVertices ( TopoDS_Compound& c ) {
  std::cout << "void entities: " << checkSubShapes(c) << " in c" << std::endl;
  //for debugging
  int e=0;
  ShapeAnalysis_Edge sae;
  ShapeFix_Edge sfe;
  TopExp_Explorer ex(c,TopAbs_VERTEX);
  for (; ex.More(); ex.Next())
    e++;
  std::cout << "initial vertex count: " << e << std::endl;
  e = 0;
  Handle(Geom_Surface) xyp = new Geom_Plane(gp::Origin(),gp::DZ());  //xy plane, necessary to add pcurve below
  TopLoc_Location loc;
  loc.Identity();
  BRepTools_ReShape swap;
  ex.Init(c,TopAbs_EDGE);
  for (; ex.More(); ex.Next()) {
    e++;
    TopoDS_Edge edge = TopoDS::Edge(ex.Current());
    if ( !sae.CheckVerticesWithCurve3d(edge) ) {
      //FIXME - check/remove pcurve and curve3d; add pcurve with xy plane as surface; add curve3d
      sfe.FixRemoveCurve3d (edge);
      sfe.FixAddCurve3d (edge);
      sfe.FixAddPCurve (edge,xyp,loc,false);
      swap.Replace(ex.Current(),edge);
      //ex.Current() = edge;

      std::cout << "edge " << e << " bad before subst" << std::endl;
    }
  }
  c=TopoDS::Compound(swap.Apply(c));

  std::cout << "initial edges " << e << std::endl;

  TopTools_IndexedMapOfShape map;
  bool reload = false;
  bool done = false;
  map.Clear();
  TopExp::MapShapes (c, TopAbs_VERTEX, map);
  int x = map.Extent();
  BRepTools_ReShape rsh;
  TopoDS_Vertex vi,vj;
  for (int i = 1; i<=x; i++) {
    vi = TopoDS::Vertex(map.FindKey(i));
    for (int j = i+1; j<=x; j++) {
      vj = TopoDS::Vertex(map.FindKey(j));
      if (BRepTools::Compare(vi,vj)) {
	if (!rsh.IsRecorded(vj)) {
	  rsh.Replace(vj,vi);
	  std::cout << "replacing " << j << " with " << i << std::endl;
	}
      }
    }
  }
  c=TopoDS::Compound(rsh.Apply(c));

  //for debugging
  e=0;
  ex.Init(c,TopAbs_VERTEX);
  for (; ex.More(); ex.Next())
    e++;
  std::cout << "final vertex count: " << e << std::endl;

  e=0;
  ex.Init(c,TopAbs_EDGE);
  for (; ex.More(); ex.Next()) {
    e++;
    //checkSubShapes(ex.Current());
    std::cout << "void entities: " << checkSubShapes(ex.Current()) << " in current" << std::endl;
    if ( !sae.CheckVerticesWithCurve3d(TopoDS::Edge(ex.Current())) ) {
      std::cout << "edge " << e << " bad after subst" << std::endl;
    }
  }
  std::cout << "subst edges " << e << std::endl;

}


///find vertices with >2 edges and remove one edge. return false if no changes
bool silhouette::reduceMultis() {
    bool found = false;
    int n = hlrMap.Extent();
   // TopTools_ListOfShape &los();
    for (int i = 1; i <= n; i++) {
	TopTools_ListOfShape &los = hlrMap.ChangeFromIndex(i);
        int l = los.Extent();
        if ( l > 2 ) {
            //figure out which edge is unnecessary, and remove it
            TopTools_ListIteratorOfListOfShape lit, closest;
	    lit = los;
            double minDist = DBL_MAX;
            for ( ; lit.More() ; lit.Next() ) {
                double d = getRemovableDist(TopoDS::Edge( lit.Value() ));
                if ( (d >= 0) && (d < minDist) ) {
                    minDist = d;
                    closest = lit;
                } else if ( d == minDist ) {
                    cout << "Error! two edges with same distance!" << endl;
                    abort();
                }
            }
            los.Remove(closest);
            found = true;
        }
    }
    return found;
}

/** intersect e with gp::OY()
find distance from intersection to geometric center of tool
return distance to center, or -1 if no intersection
*/
double silhouette::getRemovableDist(TopoDS_Edge e) {
    ShapeAnalysis_Edge sae;
      if ( !sae.CheckVerticesWithCurve3d(e) ) {
        abort();
      }
    bool found = false;
    TopoDS_Edge v = BRepBuilderAPI_MakeEdge(gp_Lin(gp::OY()));
    gp_Pnt i;
    if (e.IsNull()) abort();
    //BRepTools::Dump(e,std::cout);
    //BRepTools::Dump(v,std::cout);
    BRepTools::Write(e,"edge.brep");
    BRepExtrema_DistShapeShape dss(e,v);
    dss.Perform();
    if ( dss.IsDone() && (dss.Value() < 1.0e-5) ) {
      i = dss.PointOnShape1(1);
      found = true;
    } else {
      std::cout << "BRepExtrema_DistShapeShape failed" << std::endl;
    }
    /*
    TopOpeBRep_EdgesIntersector ei;
    ei.Perform(e,v);
    if (ei.NbPoints() == 1) {
      i = ei.Point(1).Value();
      found = true;
    } else {
      std::cout << "TopOpeBRep_EdgesIntersector failed, trying BRepExtrema_DistShapeShape" << std::endl;
      BRepExtrema_DistShapeShape dss(e,v);
      dss.Perform();
      if ( dss.IsDone() && (dss.Value() < 1.0e-5) ) {
	i = dss.PointOnShape1(1);
	found = true;
      } else {
        //warning
        std::cout << "BRepExtrema_DistShapeShape failed" << std::endl;
	abort();
      }
    }
    */
    if (!found) return -1.0;
    //find dist between i and center
    gp_Pnt center(0,myHeight/2,0); //remember, silhouette is in XY plane, symmetric about Y
    return center.Distance(i);
}

///remove any edges which have a loose end, return true if map was updated
bool silhouette::prune() {
    bool pruned = false;
    bool mapHasChange = false;

    //wtf is difference between Extent() and NbBuckets() in tcollection_basicmap?!
    do {
        int n = hlrMap.Extent();
        for (int i = 1; i <= n; i++) {
            TopTools_ListOfShape los;
	    los = hlrMap.FindFromIndex(i);
            int l = los.Extent();
            //if ( l < 2 ) { //one or zero edges connect to the current vertex
                if ( l == 1 ) {
                    removeFromList(TopoDS::Edge( los.First() ));
              //  }
                /*
		//remove vertex
                TopoDS_Vertex v;
                v.Nullify();
                los.Clear();
                hlrMap.Substitute(i,v,los);
		*/
		//clear list but don't nullify vertex
		hlrMap.ChangeFromIndex(i).Clear();
                mapHasChange = true;
                pruned = true;
            } else mapHasChange = false;
        }
    } while (mapHasChange);
    return pruned;
}

/*
    TopExp::MapShapesAndAncestors( theShell, TopAbs_VERTEX, TopAbs_EDGE, veMap );
    gp_Vec dir001 = gp::DZ();
    gp_Pnt p000 = BRep_Tool::Pnt( TopoDS::Vertex( V000 ));
    double maxVal = -DBL_MAX;
    TopTools_ListIteratorOfListOfShape eIt ( veMap.FindFromKey( V000 ));
    for (  ; eIt.More(); eIt.Next() ) {
      const TopoDS_Edge& e = TopoDS::Edge( eIt.Value() );
      TopoDS_Vertex v = TopExp::FirstVertex( e );
      if ( v.IsSame( V000 ))
        v = TopExp::LastVertex( e );
      val = dir001 * gp_Vec( p000, BRep_Tool::Pnt( v )).Normalized();
      if ( val > maxVal ) {
        V001 = v;
        maxVal = val;
      }
    }
*/

///find all instances of e in the lists in hlrMap, and remove them
///return true if hlrMap was modified
bool silhouette::removeFromList(TopoDS_Edge e) {
    bool found = false;
    int m = hlrMap.Extent();
    for (int i = 1; i <= m; i++) {
        TopTools_ListOfShape los;
	los = hlrMap.ChangeFromIndex(i);
        TopTools_ListIteratorOfListOfShape lit(los);
        for ( ; lit.More() ; lit.Next() ) {
            if ( compareEdges(e,TopoDS::Edge(lit.Value())) ) {
                los.Remove(lit);
                found = true;
		break;
            }
        }
    }
    return found;
}

bool silhouette::compareEdges(TopoDS_Edge a, TopoDS_Edge b) {
    return a.IsEqual(b);
}
