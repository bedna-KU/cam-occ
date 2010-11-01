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
/*
#include <.hxx>
#include <.hxx>
#include <.hxx>
#include <.hxx>
#include <.hxx>
#include <.hxx>
#include <.hxx>
#include <.hxx>
*/


silhouette::silhouette(TopoDS_Solid tool, gp_Dir normal, double radius, double height):
                       myTool(tool), myRadius(radius), myHeight(height) {
  newAngle(normal);
}

void silhouette::init(TopoDS_Solid tool, gp_Dir normal, double radius, double height):
                 myTool(tool), myRadius(radius), myHeight(height) {
  newAngle(normal);
}

void silhouette::newAngle(gp_Dir normal): myNormal(normal) {
  myOutline.Nullify();
  myDone = false;
  //myEdges.Clear();

  createHlrLines(myTool,myNormal);

  // remove zero-length edges?

  bool r;
  do {
    prune();
    r = reduceMultis();
  } while (r);
}


///creates the hlr lines. NOTE: they are in the XY plane!
void silhouette::createHlrLines(TopoDS_Shape t, gp_Dir dir) {

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

  hlrMap.Clear();
  TopExp::MapShapesAndAncestors(comp,TopAbs_VERTEX,TopAbs_EDGE,hlrMap);

}

///find vertices with >2 edges and remove one edge. return false if no changes
bool silhouette::reduceMultis() {
  bool found = false;
  int n = hlrMap.Extent();
  for (int i = 0; i <= n; i++) {
    TopTools_ListOfShape los ( hlrMap.FindFromIndex(i));
    int l = los.Extent();
    if ( l > 2 ) {
      //figure out which edge is unnecessary, and remove it
      TopTools_ListIteratorOfListOfShape lit(los), closest;
      double minDist = DBL_MAX;
      for( ; lit.More() ; lit.Next() ) {
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

///intersect e with gp::OY(). return distance to center, or -1 if no intersection
double silhouette::getRemovableDist(TopoDS_Edge e) {
  //find intersection i of e and gp::OY()
  bool intersect = false;
  //intersection from java app?
  gp_Pnt i;
  i = ...
  if (!intersect) return -1.0;
  //find dist between i and center
  gp_Pnt center(0,myHeight/2,0);
  return center.Distance(i);
}

///remove any edges which have a loose end, return true if map was updated
bool silhouette::prune() {
  bool pruned = false;
  bool mapHasChange = false;

  //wtf is difference between Extent() and NbBuckets() in tcollection_basicmap?!
  do {
    int n = hlrMap.Extent();
    for (int i = 0; i <= n; i++) {
      TopTools_ListOfShape los ( hlrMap.FindFromIndex(i));
      int l = los.Extent();
      if ( l < 2 ) { //one or zero edges connect to the current vertex
        if ( l == 1 ) {
          removeFromList(TopoDS::Edge( los.First() ));
        }
        //remove vertex
        TopoDS_Vertex v;
        v.Nullify();
        los.Clear();
        hlrMap.Substitute(i,v,los);
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
  for (int i = 0; i <= m; i++) {
    TopTools_ListOfShape los ( hlrMap.ChangeFromIndex(i) );
    TopTools_ListIteratorOfListOfShape lit(los);
    for( ; lit.More() ; lit.Next() ) {
      if( compareEdges(e,lit.Value()) ) {
        los.Remove(lit);
        found = true;
      }
    }
  }
  return found;
}

bool silhouette::compareEdges(TopoDS_Edge a, TopoDS_Edge b) {
  return a.IsEqual(b);
}
