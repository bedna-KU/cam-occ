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
#include <assert.h>

#include <QKeySequence>
#include <QtGui>
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
#include <Geom_CylindricalSurface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>
#include <gp_Ax1.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <Handle_Geom2d_TrimmedCurve.hxx>
#include <Handle_Geom_TrimmedCurve.hxx>
#include <Handle_Geom_CylindricalSurface.hxx>
#include <GC_MakeArcOfCircle.hxx>



gcode2Model::gcode2Model()
{

	myMenu = new QMenu("gcode2Model");
	theWindow->menuBar()->insertMenu(theWindow->getHelpMenu(),myMenu);

	//these five lines set up a menu item.  Uncomment the second one to have a shortcut.
	myAction = new QAction ( "gcode2Model", this );
	//myAction->setShortcut(QString("Ctrl+A"));
	myAction->setStatusTip ( "gcode2Model" );
	connect ( myAction, SIGNAL ( triggered() ), this, SLOT ( myMenuItem() ) );
	myMenu->addAction( myAction );
};

void gcode2Model::myMenuItem()
{
	slotNeutralSelection();
	
	QString file = QFileDialog::getOpenFileName ( theWindow, "Choose .ngc file", ".", "*.ngc" );
	if (file == "") return;  //because it's a Really Bad Thing to call the script with no file. Gobbles CPU and RAM like craaaazy.  Found out the hard way.
	QString script = "./bin/filter_canon";
	//cout << "Command line --> " << script.constData() << " <--" << endl;
	//system((const char *)f.constData()); //script that runs rs274 interp and filters some stuff out
	QProcess toCanon;
	toCanon.start(script,QStringList(file));
	cout << "Starting..." << endl;
	if (!toCanon.waitForStarted())
		return;
	cout << "Finishing..." << endl;
	if (!toCanon.waitForFinished())
		return;

	qint64 lineLength;
	char line[260];
	do {
		lineLength = toCanon.readLine(line, sizeof(line));
		if (lineLength != -1) {
			processCanonLine(line);
		}
	} while (toCanon.canReadLine());//lineLength != -1);


	//readLines ( "output.canon" ); //stuff vectors with points
	cout << "sweeping..." << endl;
	sweepEm();
	feedEdges.clear(); ///this erases it so when you load a new file, the old data gets erased.
	/**
	//TopoDS_Shape Shape;
	Handle_AIS_Shape feedAis = new AIS_Shape ( feedSweeps );
	theWindow->getContext()->SetMaterial ( feedAis,Graphic3d_NOM_PLASTIC );  //Supposed to look like plastic.  Try GOLD or PLASTER or ...
	theWindow->getContext()->SetDisplayMode ( feedAis,1,Standard_False );  //shaded
	theWindow->getContext()->Display ( feedAis );
	
	Handle_AIS_Shape traverseAis = new AIS_Shape ( traverseSweeps );
	theWindow->getContext()->SetMaterial ( traverseAis,Graphic3d_NOM_PLASTIC );  //Supposed to look like plastic.  Try GOLD or PLASTER or ...
	theWindow->getContext()->SetDisplayMode ( traverseAis,1,Standard_False );  //shaded
	theWindow->getContext()->Display ( traverseAis );
	*/
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

	
//	TopoDS_Shape solidTool = BRepSweep_Revol(toolwire,zAxis,M_PI).Shape();
	//TopoDS_Shape solidTool = BRepPrimAPI_MakeRevol(tool2d,zAxis,M_PI).Shape(); //half circle - result is tool2d plus cylindrical shell
	//TopoDS_Solid solidTool = TopoDS::Solid(BRepPrimAPI_MakeRevol(tool2d,zAxis).Shape()); //FIXME: hollow cylinder
	feedSweeps.Nullify();
	traverseSweeps.Nullify();

	BRepBuilderAPI_MakeWire makeW;

	for ( uint i=0;i < feedEdges.size();i++ )
	{
/// uint i =0;
		//find vector direction of start of the line
/**		GeomAdaptor_Curve curve = BRepAdaptor_Curve( feedEdges[i].e ).Curve();
		curve.D1(0,p,V);
		dir = gp_Dir(V);	//convert vector to direction

		//Rotate the face to be perpendicular to beginning of the path.
		gp_Trsf faceTransform;
		faceTransform.SetRotation(zAxis,
					  -atan(dir.X()/dir.Y()));	//rotate about Z, angle is -atan(x/y)
		TopoDS_Shape orientedFace = BRepBuilderAPI_Transform(tool2d,faceTransform).Shape();
*/		//FIXME: Standard_TypeMismatch
		//FIXME: Standard_Failure
		//cout << "start " << toString(feedEdges[i].start).toStdString() << " end " << toString(feedEdges[i].end).toStdString() <<endl;
		
		makeW.Add(feedEdges[i].e);
		assert(makeW.IsDone());
//		TopoDS_Wire W = makeW->Wire();
		//TopoDS_Wire W = BRepBuilderAPI_MakeWire(feedEdges[i].e);
		
		///----------------------------------------------------------------
		
	
///		feedSweeps = BRepAlgoAPI_Fuse(feedSweeps,W);
		///traverseSweeps = BRepAlgoAPI_Fuse( traverseSweeps, orientedFace);
		
		
		//BRepOffsetAPI_MakePipe *pipe = new BRepOffsetAPI_MakePipe( W , orientedFace );
		//TopoDS_Shape S = pipe->Shape();
		
		//solids
/**		gp_Trsf t;
		t.SetTranslation(gp_Pnt(0,0,0),feedEdges[i].start);
		TopoDS_Shape T1 = BRepBuilderAPI_Transform(solidTool,t).Shape();
		t.SetTranslation(gp_Pnt(0,0,0),feedEdges[i].end);
		TopoDS_Shape T2 = BRepBuilderAPI_Transform(solidTool,t).Shape();
		T1 = BRepAlgoAPI_Fuse(T1,T2);
		S = BRepAlgoAPI_Fuse(T1,S);
		feedSweeps = BRepAlgoAPI_Fuse(feedSweeps,S);
*/
	}
	if (makeW.IsDone()) {
		Handle_AIS_Shape feedAis = new AIS_Shape ( makeW.Wire() );
		theWindow->getContext()->SetMaterial ( feedAis,Graphic3d_NOM_PLASTIC );  //Supposed to look like plastic.  Try GOLD or PLASTER or ...
		theWindow->getContext()->SetDisplayMode ( feedAis,1,Standard_False );  //shaded
		theWindow->getContext()->Display ( feedAis );
	}
// 	Handle_AIS_Shape traverseAis = new AIS_Shape ( solidTool );//orientedFace );
// 	theWindow->getContext()->SetMaterial ( traverseAis,Graphic3d_NOM_PLASTIC );  //Supposed to look like plastic.  Try GOLD or PLASTER or ...
// 	theWindow->getContext()->SetDisplayMode ( traverseAis,1,Standard_False );  //shaded
// 	theWindow->getContext()->Display ( traverseAis );

/**	
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
		traverseSweeps = BRepAlgoAPI_Fuse( traverseSweeps, S);
	}
*/

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


/**
void gcode2Model::readLines ( QString filename )
{
	QFile file(filename);
	if (file.open(QFile::ReadOnly)) {
		cout << "Reading file." << endl;
		char line[260];
		qint64 lineLength;
		do {
			lineLength = file.readLine(line, sizeof(line));
			if (lineLength != -1) {
				processCanonLine(line);
			}
		} while (lineLength != -1);
	} else cout << "Cannot read file." << endl;
}
*/


void gcode2Model::processCanonLine ( QString canon_line )
{
	//bool match = true;
	static gp_Pnt last;	//this holds the coordinates of the end of the last move, no matter what that move was (STRAIGHT_FEED,STRAIGHT_TRAVERSE,ARC_FEED)
	static bool firstPoint = true;
	typedef enum {CANON_PLANE_XY, CANON_PLANE_YZ, CANON_PLANE_XZ} CANONPLANE;   //for arcs
	static CANONPLANE CANON_PLANE = CANON_PLANE_XY;	   //initialize to "normal"
	
	if (canon_line.startsWith( "STRAIGHT_" )) {
		gp_Pnt p = readXYZ(canon_line);
		///if (canon_line.startsWith( "STRAIGHT_FEED(" )) {
			//cout << "straight feed..." << endl;
			//sscanf((char *)canon_line.data(),"STRAIGHT_FEED(%f, %f, %f",&x,&y,&z);
			if (firstPoint) {
				last = p;//.SetCoord(x,y,z);
				firstPoint = false;
			} else if (last.IsEqual(p,Precision::Confusion()*100)) {
				//do nothing
			} else {
				myEdgeType edge;
				edge.e = BRepBuilderAPI_MakeEdge( last, p );//gp_Pnt(x,y,z) );
				edge.start = last;
				edge.end = last = p;
				feedEdges.push_back( edge );
			}
/**
		} else if (canon_line.startsWith( "STRAIGHT_TRAVERSE(" )) {
			//puts( "straight traverse..." );
			//puts ((char *)canon_line.data());
			//sscanf((char *)canon_line.data(),"STRAIGHT_TRAVERSE(%f, %f, %f",&x,&y,&z);
			if (firstPoint) {
				last = p;
						//.SetCoord(x,y,z);
				firstPoint = false;
			} else if (last.IsEqual(p,Precision::Confusion())) {
				//do nothing
			} else {
				myEdgeType edge;
				//FIXME: SIGABRT here!
				edge.e = BRepBuilderAPI_MakeEdge( last, p );
				edge.start = last;
				edge.end = last = p;
				traverseEdges.push_back( edge );
			}
		//} //else 
			//cout << "STRAIGHT_PROBE encountered. It is not handled." <<endl;
			//how WOULD it be handled? If probe is used to affect the program, the model may not be predictable!
*/
	} else if (canon_line.startsWith( "ARC_FEED(" )) {
		gp_Dir arcDir;
//		gp_Pnt start, end,
		gp_Pnt c;
		myEdgeType edge;
		float x,y,z,ep,a1,a2,e1,e2;
		int rot=0;
		x=y=z=ep=a1=a2=e1=e2=0;
		
		edge.start = last;
		c = gp_Pnt(a1,a2,edge.start.Z());
		
		///canon_pre.cc:473: first_end, second_end
		e1  = readOne(canon_line, 0);
		e2  = readOne(canon_line, 1);
		///canon_pre.cc:473: first_axis, second_axis
		a1 = readOne(canon_line, 2);
		a2 = readOne(canon_line, 3);
		///canon_pre.cc:473: rotation, axis_end_point
		rot  = readOne(canon_line, 4);
		ep = readOne(canon_line, 5);
		
		switch (CANON_PLANE) {
			case CANON_PLANE_XZ:
				edge.end = gp_Pnt(e2,ep,e1);
				arcDir = gp_Dir(0,1,0);
				break;
			case CANON_PLANE_YZ:
				edge.end = gp_Pnt(ep,e1,e2);
				arcDir = gp_Dir(1,0,0);
				break;
			case CANON_PLANE_XY:
			default:
				edge.end = gp_Pnt(e1,e2,ep);
				arcDir = gp_Dir(0,0,1);
		}
		last = edge.end;
		
		edge.e = helix(edge.start, edge.end, c, arcDir,rot);
		edge.start = last;
		feedEdges.push_back( edge );
		cout << "Arc " << canon_line.toStdString() << "params:" << endl;
		cout << "e1:"<< e1 <<" e2:" << e2 <<" a1:"<< a1 <<" a2:"<< a2 <<" rot:" << rot <<" ep:" << ep << endl;
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

		//helixes - how??!
		//possible solution - create all arcs as lines on a cylindrical face
		//that is probably expensive though...
//Create an arc or helix.  axis MUST be parallel to X, Y, or Z.
TopoDS_Edge gcode2Model::helix(gp_Pnt start, gp_Pnt end, gp_Pnt c, gp_Dir dir, int rot)
{
	Standard_Real pU,pV, radius = start.Distance(c);
	gp_Pnt2d p1,p2;
	Handle(Geom_CylindricalSurface) cyl = new Geom_CylindricalSurface(gp_Ax2(c,dir) , radius);
	GeomAPI_ProjectPointOnSurf proj;
	TopoDS_Edge h;
	
	h.Nullify();
	cout << "Radius " << radius << "   Rot has the value " << rot << " but is NOT USED." << endl;
	proj.Init(start,cyl);
	if(proj.NbPoints() > 0)
	{
		proj.LowerDistanceParameters(pU, pV);
		if(proj.LowerDistance() > Precision::Confusion() )
		{
			cout << "Arc point fitting distance " << float(proj.LowerDistance()) << endl;
		}
	} else return h;
	p1 = gp_Pnt2d(pU,pV);
	
	proj.Init(end,cyl);
	if(proj.NbPoints() > 0)
	{
		proj.LowerDistanceParameters(pU, pV);
		if(proj.LowerDistance() > Precision::Confusion() )
		{
			cout << "Arc point fitting distance " << float(proj.LowerDistance()) << endl;
		}
	} else return h;
	p2 = gp_Pnt2d(pU,pV);
	
	Handle(Geom2d_TrimmedCurve) segment = GCE2d_MakeSegment(p1 , p2);
	h = BRepBuilderAPI_MakeEdge(segment , cyl);
	
	return h;
}

//read first three numbers from canon_line
gp_Pnt gcode2Model::readXYZ ( QString canon_line )
{
	gp_Pnt p;
	QString t = canon_line.section( '(',1,1 ).section( ')',0,0 );
	//t = t.section( ')',0,0 );
	p.SetCoord( t.section(',',0,0).toFloat(),
	            t.section(',',1,1).toFloat(),
	            t.section(',',2,2).toFloat() );
	return p;
}

//read nth number from canon_line
//when n = 0, reads number before first comma
Standard_Real gcode2Model::readOne ( QString canon_line, uint n )
{
	QString t = canon_line.section( '(',1,1 ).section( ')',0,0 );
	return t.section(',',n,n).toFloat();
}


/*
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

//begin, end, center
TopoDS_Edge gcode2Model::arc(gp_Pnt a, gp_Pnt b, gp_XYZ c) {
 //gp_Circ &Circ, const gp_Pnt &P1, const gp_Pnt &P2, const Standard_Boolean Sense
//   gp_Circ (const gp_Ax2 &A2, const Standard_Real Radius)
//get plane for a,b,c
//get dist a,c - that's radius
	gp_Circ circle = gp_Circ();
	Handle(Geom_TrimmedCurve) Tc = GC_MakeArcOfCircle (circle,a,b,true); //last param affects the arc, but not sure how - experiment w it
	return BRepBuilderAPI_MakeEdge(Tc);
}
*/
