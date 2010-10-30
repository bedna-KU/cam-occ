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

silhouette::silhouette(TopoDS_Solid tool, gp_Dir normal, double radius):
                       myTool=tool,myRadius=radius {

  newAngle(normal);
}

silhouette::init(TopoDS_Solid tool, gp_Dir normal, double radius):
                 myTool=tool,myRadius=radius {
  newAngle(normal);
}

silhouette::newAngle(gp_Dir normal):myNormal = normal {
  myOutline.Nullify();
  myDone = false;
  myEdges.Clear();

  findEdges();
  removeShortEdges();

}


void silhouette::hlrLines(TopoDS_Shape t, gp_Dir dir) {

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

///remove any edges which have a loose end, return true if one was removed (so prune can be ran again)
bool silhouette::prune() {
  bool mapHasChange = false;
  //wtf is difference between extent() and NbBuckets() in tcollection_basicmap?!
  int n = hlrMap.Extent();
  for (int i = 0; i <= n; i++) {
    TopTools_ListOfShape los ( hlrMap.FindFromIndex(i));
    int l = los.Extent();
    if ( l < 2 ) { //one or zero edges connect to it
      if ( l == 1 ) {
        removeFromList(TopoDS::Edge( los.First() ));
      }
      //remove vertex
      hlrMap.Substitute(
      mapHasChange = true;
    }


  }
  return mapHasChange;
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
void silhouette::removeFromList(TopoDS_Edge e) {

}
