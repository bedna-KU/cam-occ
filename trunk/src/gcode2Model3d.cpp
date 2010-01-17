/***************************************************************************
 *   Copyright (C) 2009 by Mark Pictor					   *
 *   mpictor@gmail.com							   *
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

//3d stuff for gcode2model

#include "gcode2Model.h"
#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
///#include <BRepPrimAPI_MakeRevol.hxx>
///#include <BRepSweep_Revol.hxx>
#include <GCE2d_MakeSegment.hxx>
#include <GeomLProp_CurveTool.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepOffsetAPI_MakePipe.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRep_Tool.hxx> 
#include <Geom_CylindricalSurface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>
#include <gp_Ax1.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopExp.hxx>
#include <Handle_Geom2d_TrimmedCurve.hxx>
#include <Handle_Geom_TrimmedCurve.hxx>
#include <Handle_Geom_CylindricalSurface.hxx>
#include <GC_MakeArcOfCircle.hxx>

using std::cout;

//to create the cross-section of the tool, take a 3d model of it and project onto a plane normal to the motion vector
//cross section would be useful for linear moves, instantaneous outline computation for helical moves in any plane, and arc moves in xz or yz plane
//for arcs in xy plane, simply sweep the cross-section of the tool.
//despite all the effort put into creating a wire, that really isn't useful - the MRSEVs will have to be calculated individually.
//create a function that will create a 2d outline of an APT tool.

//hard parts: any shape but ball-nose for non-xy-plane cuts
//need to create a cut-segment ABC and subclass it for linear, arc, and helical csegs.

//doesn't really sweep right now, just displays the wire
void gcode2Model::sweep()
{
	drawSome(-1);
	showWire();
}

/*void gcode2Model::showWire()

  //TODO:
  //change this function to display as much as it can, i.e.
  //create wire. if it fails, show the wire up to the failure. if it doesn't fail,
  //create sweep. if sweep fails, show the wire instead
  	gp_Pnt p;
	gp_Vec V;
	gp_Dir dir;
	gp_Ax1 zAxis(gp_Pnt(0,0,0),gp_Dir(0,0,1));
	Standard_Real diam = 1;
	Standard_Real shape = 0;
	TopoDS_Wire toolwire = create2dTool(diam,shape);
	TopoDS_Face tool2d = BRepBuilderAPI_MakeFace(gp_Pln(gp::ZOX()),toolwire);

	feedSweeps.Nullify();
	traverseSweeps.Nullify();

	BRepBuilderAPI_MakeWire makeW;

	for ( uint i=0;i < feedEdges.size();i++ ) {
		checkEdge( feedEdges, i );	//check that edges are connected
		//infoMsg("ready?");
		makeW.Add(feedEdges[i].e);
		//assert(makeW.IsDone());
	}
	if (makeW.IsDone()) {
		Handle_AIS_Shape feedAis = new AIS_Shape ( makeW.Wire() );
		theWindow->getContext()->SetMaterial ( feedAis,Graphic3d_NOM_PLASTIC );  //Supposed to look like plastic.  Try GOLD or PLASTER or ...
		theWindow->getContext()->SetDisplayMode ( feedAis,1,Standard_False );  //shaded
		theWindow->getContext()->Display ( feedAis );
	} else {
	  	cout << "Failed to build wire from " << feedEdges.size() << " segments!" << endl;
	 	//infoMsg( QString("Failed to build wire from ") + int(feedEdges.size()) + " segments!");
	}

}*/
void gcode2Model::showWire() {
	Handle_AIS_Shape feedAis = new AIS_Shape ( thePath );
	theWindow->getContext()->SetMaterial ( feedAis,Graphic3d_NOM_PLASTIC );  //Supposed to look like plastic.  Try GOLD or PLASTER or ...
	theWindow->getContext()->SetDisplayMode ( feedAis,1,Standard_False );  //shaded
	theWindow->getContext()->Display ( feedAis );
}

bool gcode2Model::drawSome(int j)
{
  	bool success = false;
  	if ( j == -1 ) j += feedEdges.size();
	if (j < (int)feedEdges.size()) {
		BRepBuilderAPI_MakeWire makeW;
		uint i;
		for ( i=0 ; i < j ; i++ ) {
			checkEdge( feedEdges, i );	//check that edges are connected
			makeW.Add(feedEdges[i].e);
		}
		if (makeW.IsDone()) {
			thePath = makeW.Wire();
			success = true;
		} else {
	  		cout << "Failed to build wire of length " << j << endl;
			if ( j > 1 ) {
				cout << "last edge: " << toString(feedEdges[i-1].start).toStdString() << " to ";
				cout << toString(feedEdges[i-1].end).toStdString() << endl;
				cout << "prev edge: " << toString(feedEdges[i-2].start).toStdString() << " to ";
				cout << toString(feedEdges[i-2].end).toStdString() << endl;
				return drawSome (j-1);
			}
		}
	} else {
	  	cout << "past end of feedEdges: index " << j << " is too big for count " << feedEdges.size() << endl;
//	  	infoMsg(s);
	}
	return success;
}

/****************************************************************************
TopoDS_Wire create2dTool(Standard_Real diam, Standard_Real shape)

Returns a wire that is a cross-section of the described tool, for sweeping along toolpath.
diam is the diameter (units ???)
shape controls the tool's tip as such:
  shape==0   ->  ball nose mill
  shape>=PI  ->  endmill (square tip) (PI radians is 180 degrees, think of it as flat)
  0<shape<PI ->  engraving tool, value is the included angle in RADIANS

Wire that is returned MUST be on the XZ plane, symmeteric about Z, center of the tooltip at the origin, rest of the tool above (+Z) the origin
******************************************************************************/
TopoDS_Wire gcode2Model::create2dTool ( Standard_Real diam, Standard_Real shape ) 
{
	TopoDS_Wire tool2d;

	if (shape==0) { //ballnose mill -- need arc, 3 lines
		//puts("ball tool");
		Handle(Geom_TrimmedCurve) Tc = GC_MakeArcOfCircle (gp_Pnt(diam/2,0,diam/2), gp_Pnt(0,0,0), gp_Pnt(-diam/2,0,diam/2));
		TopoDS_Edge Ec = BRepBuilderAPI_MakeEdge(Tc);  //convert the curve from geometry to topology
		TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(gp_Pnt(diam/2,0,diam/2), gp_Pnt(diam/2,0,diam*10));
		TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(gp_Pnt(-diam/2,0,diam/2), gp_Pnt(-diam/2,0,diam*10));
		TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(gp_Pnt(-diam/2,0,diam*10), gp_Pnt(diam/2,0,diam*10));
		tool2d = BRepBuilderAPI_MakeWire(Ec,E1,E2,E3); //.Wire();
	} else if (shape>=PI) { //endmill -- need rectangle
		//puts("endmill");
		TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(gp_Pnt(diam/2,0,0), gp_Pnt(diam/2,0,diam*10));
		TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(gp_Pnt(-diam/2,0,0), gp_Pnt(-diam/2,0,diam*10));
		TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(gp_Pnt(-diam/2,0,0), gp_Pnt(diam/2,0,0));
		TopoDS_Edge E4 = BRepBuilderAPI_MakeEdge(gp_Pnt(diam/2,0,diam*10), gp_Pnt(-diam/2,0,diam*10));
		tool2d = BRepBuilderAPI_MakeWire(E1,E3,E2,E4); //apparently the lines have to be in order or it will silently fail?!

	} else {  //V-tip engraving tool -- need V, 3 more lines.
		//puts("engraving");
		Standard_Real Vz = diam/(2*tan(shape/2)); //Vz is Z height of top of angled part of tip.
		BRepBuilderAPI_MakeWire toolMakeWire;
			//again, wires must be in order, wtf?
		toolMakeWire.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(0,0,0), gp_Pnt(diam/2,0,Vz)));	
		toolMakeWire.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(0,0,0), gp_Pnt(-diam/2,0,Vz)));
		toolMakeWire.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(-diam/2,0,Vz), gp_Pnt(-diam/2,0,diam*10)));
		toolMakeWire.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(-diam/2,0,diam*10), gp_Pnt(diam/2,0,diam*10)));
		toolMakeWire.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(diam/2,0,Vz), gp_Pnt(diam/2,0,diam*10)));
		tool2d = toolMakeWire.Wire();
	}

	return tool2d;
}

//begin, direction at begin, end
TopoDS_Edge gcode2Model::arc ( gp_Pnt a, gp_Vec V, gp_Pnt b )
{
        Handle ( Geom_TrimmedCurve ) Tc = GC_MakeArcOfCircle ( a, V, b );
        return BRepBuilderAPI_MakeEdge ( Tc );
}

//Create a helix.  Axis MUST be parallel to X, Y, or Z. Create as lines on a cylindrical face (like the makebottle demo)
TopoDS_Edge gcode2Model::helix ( gp_Pnt start, gp_Pnt end, gp_Pnt c, gp_Dir dir, int rot )
{
	Standard_Real pU,pV;
	Standard_Real radius = start.Distance(c);
	gp_Pnt2d p1,p2;
	Handle(Geom_CylindricalSurface) cyl = new Geom_CylindricalSurface(gp_Ax2(c,dir) , radius);
	GeomAPI_ProjectPointOnSurf proj;
	TopoDS_Edge h;
	int success = 0;
	
	h.Nullify();
	//cout << "Radius " << radius << "   Rot has the value " << rot << endl;
	proj.Init(start,cyl);
	if(proj.NbPoints() > 0) {
		proj.LowerDistanceParameters(pU, pV);
		if(proj.LowerDistance() > 1.0e-6 ) {
			cout << "Point fitting distance " << double(proj.LowerDistance()) << endl;
		}
		success++;
		p1 = gp_Pnt2d(pU,pV);
	}
	
	proj.Init(end,cyl);
	if(proj.NbPoints() > 0) {
		proj.LowerDistanceParameters(pU, pV);
		if(proj.LowerDistance() > 1.0e-6 ) {
			cout << "Point fitting distance " << double(proj.LowerDistance()) << endl;
		}
		success++;
		p2 = gp_Pnt2d(pU,pV);
	}
	
	if (success != 2) {
	  cout << "Couldn't create a helix from " << toString(start).toStdString() << " to " << toString(end).toStdString() << ". Replacing with a line." <<endl;
	  h = BRepBuilderAPI_MakeEdge( start, end );
	  return h;
	}

	//for the 2d points, x axis is about the circumference.  Units are radians.
	//change direction if rot = 1, not if rot = -1
	//if (rot==1) p2.SetX((p1.X()-p2.X())-2*M_PI); << this is wrong!
	cout << "p1x " << p1.X() << ", p2x " << p2.X() << endl;

	//switch direction if necessary, only works for simple cases
	//should always work for G02/G03 because they are less than 1 rotation
	if (rot==1) {
	  p2.SetX(p2.X()-2*M_PI);
	  cout << "p2x now " << p2.X() << endl;
	}
	Handle(Geom2d_TrimmedCurve) segment = GCE2d_MakeSegment(p1 , p2);
	h = BRepBuilderAPI_MakeEdge(segment , cyl);
	
	return h;
}

void gcode2Model::checkEdge( std::vector<myEdgeType> edges, int n )
{
  if (n < 2) return;
  double d = 0;
  gp_Pnt p;
  bool nogap = true;
  d = edges[n].start.Distance(edges[n-1].end);
  if (d > Precision::Confusion()) {
    cout << "Found gap of " << d << " before edge " << n << endl;
    nogap = false;
  }
  
  p = BRep_Tool::Pnt(TopExp::FirstVertex(edges[n].e));
  d = edges[n].start.Distance(p);
  if (d > Precision::Confusion()) {
    cout << "Start data differs by " << d << " before edge " << n << " - expected: ";
    cout << toString(feedEdges[n].start).toStdString();
    cout << " - actual: " << toString(p).toStdString() << endl;
    nogap = false;
  }
  
  p = BRep_Tool::Pnt(TopExp::LastVertex(edges[n].e));
  d = edges[n].end.Distance(p);
  if (d > Precision::Confusion()) {
    cout << "End data differs by " << d << " after edge " << n << " - expected: ";
    cout << toString(feedEdges[n].end).toStdString();
    cout << " - actual: " << toString(p).toStdString() << endl;
    nogap = false;
  }
  
  if (!nogap) {
    if (edges[n].shape == HELIX) {
	cout << "Failure is on a helical move" << endl;
    } else if (edges[n].shape == ARC) {
	cout << "Failure is on an arc move" << endl;
    } else if (edges[n].shape == LINE) {
      if (edges[n].motion == TRAVERSE)
	cout << "Failure is on a rapid traverse" << endl;
      else
	cout << "Failure is on a linear move" << endl;
    } else cout << "Failure is on a move of undefined type!" << endl; //should never get here
  }
}
