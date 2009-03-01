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




#include "gcode2Model.h"
#include <ostream>

#include <QKeySequence>
#include <QtGui>
#include <AIS_InteractiveContext.hxx>

#include <AIS_Shape.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <GeomLProp_CurveTool.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepOffsetAPI_MakePipe.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>
#include <gp_Ax1.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <Handle_Geom_TrimmedCurve.hxx>
#include <GC_MakeArcOfCircle.hxx>



gcode2Model::gcode2Model()
{
// 	theWindow = 0;
// }
//
// void gcode2Model::init ( QoccHarnessWindow* window )
// {
// 	theWindow = window;

///	commented out because this is incomplete.
///	myMenu = new QMenu("gcode2Model");
///	theWindow->menuBar()->insertMenu(theWindow->getHelpMenu(),myMenu);

	//these five lines set up a menu item.  Uncomment the second one to have a shortcut.
	myAction = new QAction ( "gcode2Model", this );
	//myAction->setShortcut(QString("Ctrl+A"));
	myAction->setStatusTip ( "gcode2Model" );
	connect ( myAction, SIGNAL ( triggered() ), this, SLOT ( myMenuItem() ) );
///	myMenu->addAction( myAction );
};

void gcode2Model::myMenuItem()
{
	slotNeutralSelection();
	
	QString file = QFileDialog::getOpenFileName ( theWindow, "Choose .ngc file", ".", "*.ngc" );
	
	system((const char *)QString("bin/filter_canon " + file).constData()); //script that runs rs274 interp and filters some stuff out
	readLines ( "output" ); //stuff vectors with points
	sweepEm();
//sweep 2d wire
//detect non-tangencies and use 3d wire revolved there
//compute solid
//then subtract
//display
	TopoDS_Shape Shape;
	Handle_AIS_Shape AisShape = new AIS_Shape ( Shape );
	theWindow->getContext()->SetMaterial ( AisShape,Graphic3d_NOM_PLASTIC );  //Supposed to look like plastic.  Try GOLD or PLASTER or ...
	theWindow->getContext()->SetDisplayMode ( AisShape,1,Standard_False );  //shaded
	theWindow->getContext()->Display ( AisShape );
	//redraw();	//not necessary, Display() automatically redraws the screen.
}

void gcode2Model::sweepEm()
{
	gp_Pnt p;
	gp_Vec V;
	gp_Dir dir;
	gp_Ax1 zAxis(gp_Pnt(0,0,0),gp_Dir(0,0,1));
	Standard_Real diam = 1;
	Standard_Real shape = 0;
	TopoDS_Wire toolwire = create2dTool(diam,shape);
	TopoDS_Face tool2d = BRepBuilderAPI_MakeFace(gp_Pln(gp::ZOX()),toolwire);
	TopoDS_Solid solidTool = TopoDS::Solid(BRepPrimAPI_MakeRevol(tool2d,zAxis).Shape());
	
	for ( uint i=0;i < feedEdges.size();i++ )
	{
		//find vector direction of start of 'curve'
		GeomAdaptor_Curve curve = BRepAdaptor_Curve( feedEdges[i].e ).Curve();
		curve.D1(0,p,V);
		//GeomLProp_CurveTool::D1( curve, 0, p, V );
		
		 dir = gp_Dir(V);	//convert vector to direction

		//Rotate the face to be perpendicular to beginning of the path.
		gp_Trsf faceTransform;
		faceTransform.SetRotation(zAxis,
					  -atan(dir.X()/dir.Y()));	//rotate about Z, angle is -atan(x/y)
		TopoDS_Shape orientedFace = BRepBuilderAPI_Transform(tool2d,faceTransform).Shape();
		TopoDS_Shape S = BRepOffsetAPI_MakePipe( TopoDS::Wire(feedEdges[i].e) , orientedFace ).Shape();
		
		//solids
		gp_Trsf t;
		t.SetTranslation(gp_Pnt(0,0,0),feedEdges[i].start);
		TopoDS_Shape T1 = BRepBuilderAPI_Transform(solidTool,t).Shape();
		t.SetTranslation(gp_Pnt(0,0,0),feedEdges[i].end);
		TopoDS_Shape T2 = BRepBuilderAPI_Transform(solidTool,t).Shape();
		T1 = BRepAlgoAPI_Fuse(T1,T2);
		S = BRepAlgoAPI_Fuse(T1,S);
		feedSweeps.push_back(S);
	}
	
	for ( uint i=0;i < traverseEdges.size();i++ )
	{
		//find vector direction of start of 'curve'
		GeomAdaptor_Curve curve = BRepAdaptor_Curve( traverseEdges[i].e ).Curve();
		curve.D1(0,p,V);
		//GeomLProp_CurveTool::D1( curve, 0, p, V );
		
		dir = gp_Dir(V);	//convert vector to direction

		//Rotate the face to be perpendicular to beginning of the path.
		gp_Trsf faceTransform;
		faceTransform.SetRotation(zAxis,
					  -atan(dir.X()/dir.Y()));	//rotate about Z, angle is -atan(x/y)
		TopoDS_Shape orientedFace = BRepBuilderAPI_Transform(tool2d,faceTransform).Shape();
		TopoDS_Shape S = BRepOffsetAPI_MakePipe( TopoDS::Wire(traverseEdges[i].e) , orientedFace ).Shape();
		
		//solids
		gp_Trsf t;
		t.SetTranslation(gp_Pnt(0,0,0),traverseEdges[i].start);
		TopoDS_Shape T1 = BRepBuilderAPI_Transform(solidTool,t).Shape();
		t.SetTranslation(gp_Pnt(0,0,0),traverseEdges[i].end);
		TopoDS_Shape T2 = BRepBuilderAPI_Transform(solidTool,t).Shape();
		T1 = BRepAlgoAPI_Fuse(T1,T2);
		S = BRepAlgoAPI_Fuse(T1,S);
		traverseSweeps.push_back(S);
	}

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
TopoDS_Wire gcode2Model::create2dTool(Standard_Real diam, Standard_Real shape) 
{
	TopoDS_Wire tool2d;

	if (shape==0) { //ballnose mill -- need arc, 3 lines
		puts("ball tool");
		Handle(Geom_TrimmedCurve) Tc = GC_MakeArcOfCircle (gp_Pnt(diam/2,0,diam/2), gp_Pnt(0,0,0), gp_Pnt(-diam/2,0,diam/2));
		TopoDS_Edge Ec = BRepBuilderAPI_MakeEdge(Tc);  //convert the curve from geometry to topology
		TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(gp_Pnt(diam/2,0,diam/2), gp_Pnt(diam/2,0,diam*10));
		TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(gp_Pnt(-diam/2,0,diam/2), gp_Pnt(-diam/2,0,diam*10));
		TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(gp_Pnt(-diam/2,0,diam*10), gp_Pnt(diam/2,0,diam*10));
		tool2d = BRepBuilderAPI_MakeWire(Ec,E1,E2,E3); //.Wire();
	} else if (shape>=PI) { //endmill -- need rectangle
		puts("endmill");
		TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(gp_Pnt(diam/2,0,0), gp_Pnt(diam/2,0,diam*10));
		TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(gp_Pnt(-diam/2,0,0), gp_Pnt(-diam/2,0,diam*10));
		TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(gp_Pnt(-diam/2,0,0), gp_Pnt(diam/2,0,0));
		TopoDS_Edge E4 = BRepBuilderAPI_MakeEdge(gp_Pnt(diam/2,0,diam*10), gp_Pnt(-diam/2,0,diam*10));
		tool2d = BRepBuilderAPI_MakeWire(E1,E3,E2,E4); //apparently the lines have to be in order or it will silently fail?!

	} else {  //V-tip engraving tool -- need V, 3 more lines.
		puts("engraving");
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


void gcode2Model::readLines ( QString filename )
{
	QFile file(filename);
	if (file.open(QFile::ReadOnly)) {
		char line[260];
		qint64 lineLength = file.readLine(line, sizeof(line));
		if (lineLength != -1) {
			processCanonLine(line);
		}
	}
}


void gcode2Model::processCanonLine ( QString canon_line )
{
	//bool match = true;
	float x,y,z,ep,a1,a2;
	int r=0;
	x=y=z=ep=a1=a2=0;
	static gp_Pnt last;	//this holds the coordinates of the end of the last move, no matter what that move was (STRAIGHT_FEED,STRAIGHT_TRAVERSE,ARC_FEED)
	static bool firstPoint = true;
	typedef enum {CANON_PLANE_XY, CANON_PLANE_YZ, CANON_PLANE_XZ} CANONPLANE;
	static CANONPLANE CANON_PLANE = CANON_PLANE_XY;	//only for arcs
	
	if (canon_line.startsWith( "STRAIGHT_" )) {
		if (canon_line.startsWith( "STRAIGHT_FEED(" )) {
			sscanf((char *)canon_line.data(),"STRAIGHT_FEED(%f, %f, %f",&x,&y,&z);
			if (firstPoint) {
				last.SetCoord(x,y,z);
				firstPoint = false;
			} else {
				myEdgeType edge;
				edge.e = BRepBuilderAPI_MakeEdge( last, gp_Pnt(x,y,z) );
				edge.start = last;
				last.SetCoord(x,y,z);
				edge.end = last;
				feedEdges.push_back( edge );
			}
		} else if (canon_line.startsWith( "STRAIGHT_TRAVERSE(" )) {
			sscanf((char *)canon_line.data(),"STRAIGHT_TRAVERSE(%f, %f, %f",&x,&y,&z);
			if (firstPoint) {
				last.SetCoord(x,y,z);
				firstPoint = false;
			} else {
				myEdgeType edge;
				edge.e = BRepBuilderAPI_MakeEdge( last, gp_Pnt(x,y,z) );
						edge.start = last;
				last.SetCoord(x,y,z);
				edge.end = last;
				traverseEdges.push_back( edge );
			}
		} //else 
			//cout << "STRAIGHT_PROBE encountered. It is not handled." <<endl;
			//how would it be handled? If probe is used to control the program, the model is not predictable!
	} else if (canon_line.startsWith( "ARC_FEED(" )) {
		sscanf((char *)canon_line.data(),"ARC_FEED(%f, %f, %f, %f, %d, %f",&x,&y,&a1,&a2,&r,&ep);
		//canon_pre.cc:473: first_end, second_end, first_axis, second_axis, rotation, axis_end_point
		//must take into account CANON_PLANE
		//TODO: complete this
		cout << "Does not handle " << canon_line.toStdString() << endl;
	} else if (canon_line.startsWith( "SELECT_PLANE(" )) {
		if (canon_line.contains( "XZ)" )) {
			CANON_PLANE=CANON_PLANE_XZ;
		} else if (canon_line.contains( "YZ)" )) {
			CANON_PLANE=CANON_PLANE_YZ;
		} else {// XY)
			CANON_PLANE=CANON_PLANE_XY;
		}
//	} else if (canon_line.startsWith( "(" )) {
	} else {
		//match = false;
		cout << "Does not handle " << canon_line.toStdString() << endl;
	}
}

/*
//read first three numbers from canon_line
gp_Pnt gcode2Model::readXYZ ( QString canon_line )
{
	gp_Pnt p;
	QString t = canon_line.section( '(',1,1 ).section( ')',0,0 );
	//t = t.section( ')',0,0 );
	p.X() = t.section(',',0,0).toFloat();
	p.Y() = t.section(',',1,1).toFloat();
	p.Z() = t.section(',',2,2).toFloat();
	return p;
}

//read nth number from canon_line
//when n = 0, reads number before first comma
Standard_Real gcode2Model::readOne ( QString canon_line, uint n )
{
	QString t = canon_line.section( '(',1,1 ).section( ')',0,0 );
	return t.section(',',n,n).toFloat();
}
*/

//3 points
TopoDS_Edge gcode2Model::arc ( gp_Pnt a, gp_Pnt b, gp_Pnt c )
{
	Handle ( Geom_TrimmedCurve ) Tc = GC_MakeArcOfCircle ( a, b, c );
	return BRepBuilderAPI_MakeEdge ( Tc );
}

//begin, direction at begin, end
TopoDS_Edge gcode2Model::arc ( gp_Pnt a, gp_Vec V, gp_Pnt b )
{
	Handle ( Geom_TrimmedCurve ) Tc = GC_MakeArcOfCircle ( a, V, b );
	return BRepBuilderAPI_MakeEdge ( Tc );
}

/* don't bother unless its actually necessary
//begin, end, center
TopoDS_Edge gcode2Model::arc(gp_Pnt a, gp_Pnt b, gp_XYZ c) {
 //gp_Circ &Circ, const gp_Pnt &P1, const gp_Pnt &P2, const Standard_Boolean Sense
//   gp_Circ (const gp_Ax2 &A2, const Standard_Real Radius)
//get plane for a,b,c
//get dist a,c - that's radius
	gp_Circ circle = gp_Circ();
	Handle(Geom_TrimmedCurve) Tc = GC_MakeArcOfCircle (circle,a,b,true); //last param affects the arc, but not sure how - experiment w it
	return BRepBuilderAPI_MakeEdge(Tc);
}*/
