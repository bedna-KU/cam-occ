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
//#include <string>

#include <QKeySequence>
#include <QtGui>
#include <QProcess>

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


using std::cout;
gcode2Model::gcode2Model()
{
	myMenu = new QMenu("gcode2Model");
	theWindow->menuBar()->insertMenu(theWindow->getHelpMenu(),myMenu);

	//these five lines set up a menu item.  Uncomment the second one to have a shortcut.
	myAction = new QAction ( "gcode2Model", this );
	myAction->setShortcut(QString("Ctrl+M"));
	myAction->setStatusTip ( "gcode2Model" );
	connect ( myAction, SIGNAL ( triggered() ), this, SLOT ( myMenuItem() ) );
	myMenu->addAction( myAction );

// do next: show segments one at a time
	nextAction = new QAction ( "Do next", this );
	nextAction->setShortcut(QString("Ctrl+."));
	nextAction->setStatusTip ( "Do next" );
	connect ( nextAction, SIGNAL ( triggered() ), this, SLOT ( myNextMenuItem() ) );
	myMenu->addAction( nextAction );

	hasProcessedNgc = false;
};

void gcode2Model::myMenuItem()
{
	slotNeutralSelection();
	
	QString file = QFileDialog::getOpenFileName ( theWindow, "Choose .ngc file", ".", "*.ngc" );
	if ( ! file.endsWith(".ngc") ) {
		infoMsg("You must select a file ending with .ngc!");
		return;
	}
	QString ipath = "/opt/src/emc2/trunk/";
	QString interp = ipath + "bin/rs274";
	QString iparm = ipath + "configs/sim/sim_mm.var\n";
	QString itool = ipath + "configs/sim/sim_mm.tbl\n";
	QProcess toCanon;
	toCanon.start(interp,QStringList(file));
	/**************************************************
	Apparently, QProcess::setReadChannel screws with waitForReadyRead() and canReadLine()
	So, we just fly blind and assume that there are no errors when we navigate
	the interp's "menu", and that it requires no delays.
	**************************************************/

	//now give the interpreter the data it needs
	toCanon.write("2\n");	//set parameter file
	toCanon.write(iparm.toAscii());
	toCanon.write("3\n");	//set tool table file
	toCanon.write(itool.toAscii());
	//can also use 4 and 5

	toCanon.write("1\n"); //start interpreting
	//cout << "stderr: " << (const char*)toCanon.readAllStandardError() << endl;

	if (!toCanon.waitForReadyRead(1000) ) {
		if ( toCanon.state() == QProcess::NotRunning ){
			infoMsg("Interpreter died.  Bad tool table " + itool + " ?");
		} else  infoMsg("Interpreter timed out for an unknown reason.");
		cout << "stderr: " << (const char*)toCanon.readAllStandardError() << endl;
		cout << "stdout: " << (const char*)toCanon.readAllStandardOutput() << endl;
		toCanon.close();
		return;
	}
	
	//if readLine is used at the wrong time, it is possible to pick up a line fragment! will canReadLine() fix that?
	qint64 lineLength;
	char line[260];
	uint fails = 0;
	do {
		if (toCanon.canReadLine()) {
			lineLength = toCanon.readLine(line, sizeof(line));
			if (lineLength != -1 ) {
				processCanonLine(line);
			} else {	//shouldn't get here!
				fails++;
				sleepSecond();
			}
		} else {
			fails++;
			sleepSecond();
		}
	} while ( (fails < 100) && 
		  ( (toCanon.canReadLine()) || ( toCanon.state() != QProcess::NotRunning ) )  );
	//((lineLength > 0) || 	//loop until interp quits and all lines are read.
	//toCanon.canReadLine() || 
	cout << "sweeping..." << endl;
	sweepEm();
	feedEdges.clear(); //so when user loads a new file, the old data is not prepended.
	hasProcessedNgc = true;
}

// do next: show segments one at a time
void gcode2Model::myNextMenuItem() {
	if (!hasProcessedNgc) return;
	//duplicate sweepEm?
}

void gcode2Model::sleepSecond() {
	//sleep 1s and process events
	//cout << "SLEEP..." << endl;
	QTime dieTime = QTime::currentTime().addSecs(1);
	while( QTime::currentTime() < dieTime )
		QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents, 100);
	return;
}

/*
bool gcode2Model::waitRead(QProcess &canon){
	if ( ! canon.waitForReadyRead(1000) ) {
		infoMsg("Interpreter timed out at startup.");
		canon.close();
		return false;
	} else {
		cout << "stderr: " << (const char*)canon.readAllStandardError() << endl;
		return true;  
	}
}
*/

//doesn't really sweep right now, just displays the wire
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
	  cout << "Failed to build wire!" << endl;
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



void gcode2Model::processCanonLine ( QString canon_line )
{
	static gp_Pnt last;	//this holds the coordinates of the end of the last move, no matter what that move was (STRAIGHT_FEED,STRAIGHT_TRAVERSE,ARC_FEED)
	static bool firstPoint = true;
	typedef enum {CANON_PLANE_XY, CANON_PLANE_YZ, CANON_PLANE_XZ} CANONPLANE;   //for arcs
	static CANONPLANE CANON_PLANE = CANON_PLANE_XY;	   //initialize to "normal"
	//ideally, handle rapids (STRAIGHT_TRAVERSE) separately from STRAIGHT_FEED
	//for now, handle together
	if ( ! canon_line.endsWith(")\n") ) {
	infoMsg("Warning! Line " + canon_line + " appears to be truncated!");
	//cout << "Line " << canon_line.toStdString() << endl;
	}

	//a line looks like "   14 N..... STRAIGHT_FEED(0.0000, -1.0000, 0.0000, 0.0000, 0.0000, 0.0000)"
	//find position of N, save next 5 chars as the ngc line number
	myEdgeType edge;
	int N = canon_line.indexOf("N");
	//edge.N = canon_line.mid(N+1,5).toInt();
	//chop to N+6 (incl the space)
	canon_line.remove(0,N+7);
	//cout << "Line " << canon_line.toStdString() << endl;

	/***************************************************
	CANON lines that can be ignored are those beginning with:

	COMMENT SPINDLE MIST ENABLE FLOOD DWELL FEEDRATE 
	SET_FEED_RATE PROGRAM_STOP

	at some point in the future, it would be nice to 
	associate spindle speed, feedrate, coolant, and tool 
	number with the section of the model where it 
	applies, along with rapid/feed moves.
	****************************************************/
	QStringList canonIgnore;
	QString ign;
	canonIgnore << "COMMENT" << "SPINDLE" << "MIST" << "ENABLE" << "FLOOD" << "DWELL" 
		<< "FEEDRATE" << "SET_FEED_" << "_TOOL" << "PROGRAM_STOP()";
	foreach (ign,canonIgnore) 
		if (canon_line.contains(ign))
			return;
	if (canon_line.startsWith( "STRAIGHT_" )) {
		gp_Pnt p = readXYZ(canon_line);
		if (firstPoint) {
			last = p;
			firstPoint = false;
		} else if (last.IsEqual(p,Precision::Confusion()*100)) {
			//do nothing
		} else {
			edge.e = BRepBuilderAPI_MakeEdge( last, p );
			edge.start = last;
			edge.end = last = p;
			feedEdges.push_back( edge );
		}
	} else if (canon_line.startsWith( "ARC_FEED(" )) {
		gp_Dir arcDir;
		gp_Pnt c;
		float x,y,z,ep,a1,a2,e1,e2,zdist;
		int rot=0;
		//bool isHelix = false;
		x=y=z=ep=a1=a2=e1=e2=0;
		
		edge.start = last;
		
		//canon_pre.cc:473: first_end, second_end (two coords for end)
		e1  = readOne(canon_line, 0);
		e2  = readOne(canon_line, 1);
		//canon_pre.cc:473: first_axis, second_axis (two coords for axis of rotation)
		a1 = readOne(canon_line, 2);
		a2 = readOne(canon_line, 3);
		//canon_pre.cc:473: rotation, axis_end_point (last is third point for end)
		rot  = readOne(canon_line, 4); //if rot is 1, arc is ccw
		ep = readOne(canon_line, 5);
		
		switch (CANON_PLANE) {
		case CANON_PLANE_XZ:
			edge.end = gp_Pnt(e2,ep,e1);	/**** the order for these vars is copied from ****/
			arcDir = gp_Dir(0,1,0);		/****  canon_pre.cc, probably below line 473  ****/
			c = gp_Pnt(a2,edge.start.Y(),a1);
			zdist = ep - edge.start.Y();
			break;
		case CANON_PLANE_YZ:
			edge.end = gp_Pnt(ep,e1,e2);
			arcDir = gp_Dir(1,0,0);
			c = gp_Pnt(edge.start.X(),a1,a2);
			zdist = ep - edge.start.X();
			break;
		case CANON_PLANE_XY:
		default:
			edge.end = gp_Pnt(e1,e2,ep);
			arcDir = gp_Dir(0,0,1);
			c = gp_Pnt(a1,a2,edge.start.Z());
			zdist = ep - edge.start.Z();
		}
		last = edge.end;
		//cout << "Dist " << zdist << endl;
		if (edge.start.Distance(edge.end) > Precision::Confusion()) { //caught this bug thanks to tort.ngc
			//cout << "Create ";
			if (fabs(zdist) > 0.000001) {
				edge.e = helix(edge.start, edge.end, c, arcDir,rot);
				cout << "Helix with center " << toString(c).toStdString() << " and arcDir " << toString(arcDir).toStdString();
			//cout << "helix." << endl;
			} else {
				//arc center is c; ends are edge.start, edge.last
				gp_Vec Vr = gp_Vec(c,edge.start);	//vector from center to start
				gp_Vec Va = gp_Vec(arcDir);		//vector along arc's axis
				gp_Vec startVec = Vr^Va;		//find perpendicular vector using cross product
				if (rot==1) startVec *= -1;
				//cout << "Arc with vector at start: " << toString(startVec).toStdString();
				edge.e = arc(edge.start, startVec, edge.end);
				//cout << "arc." << endl;
			}
			cout << " from " << toString(edge.start).toStdString() << " to " << toString(edge.end).toStdString() << endl;
			cout << "params:  e1:"<< e1 <<"  e2:" << e2 <<"  a1:"<< a1 <<"  a2:"<< a2 <<"  rot:" << rot <<"  ep:" << ep << endl;
			feedEdges.push_back( edge );
		} else cout << "Skipped zero-length arc." << endl;
	} else if (canon_line.startsWith( "SELECT_PLANE(" )) {
		if (canon_line.contains( "XZ)" )) {
			CANON_PLANE=CANON_PLANE_XZ;
		} else if (canon_line.contains( "YZ)" )) {
			CANON_PLANE=CANON_PLANE_YZ;
		} else {// XY)
			CANON_PLANE=CANON_PLANE_XY;
		}
	} else if (canon_line.startsWith( "MESSAGE" )) {
		cout << canon_line.toStdString() << endl;
	}
	//the else if's below are to silently ignore certain canonical commands which I don't know what to do with
	else if (canon_line.startsWith( "SET_ORIGIN_OFFSETS(0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000)" )) {}
	else if (canon_line.startsWith( "USE_LENGTH_UNITS(CANON_UNITS_MM)" )) {}
	else if (canon_line.startsWith( "SET_FEED_REFERENCE(CANON_XYZ)" )) {}
	else if (canon_line.startsWith( "PROGRAM_END" )) {
	cout <<"Program ended."<<endl;
	}
	//else if (canon_line.startsWith( "" )) {}
	//else if (canon_line.startsWith( "" )) {}
	else {
		cout << "Does not handle " << canon_line.toStdString() << endl;
	}
	//cout << "Done with line" << endl;
}

//begin, direction at begin, end
TopoDS_Edge gcode2Model::arc ( gp_Pnt a, gp_Vec V, gp_Pnt b )
{
        Handle ( Geom_TrimmedCurve ) Tc = GC_MakeArcOfCircle ( a, V, b );
        return BRepBuilderAPI_MakeEdge ( Tc );
}



//Create a helix.  Axis MUST be parallel to X, Y, or Z. Create as lines on a cylindrical face (like the makebottle demo)
TopoDS_Edge gcode2Model::helix(gp_Pnt start, gp_Pnt end, gp_Pnt c, gp_Dir dir, int rot)
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
			//cout << "Point fitting distance " << float(proj.LowerDistance()) << endl;
		}
		success++;
		p1 = gp_Pnt2d(pU,pV);
	}
	
	proj.Init(end,cyl);
	if(proj.NbPoints() > 0) {
		proj.LowerDistanceParameters(pU, pV);
		if(proj.LowerDistance() > 1.0e-6 ) {
			//cout << "Point fitting distance " << float(proj.LowerDistance()) << endl;
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
//	if (rot==1) p2.SetX(p2.X()-2*M_PI); //only works for simple cases, should always work for G02/G03 because they don't go around more than once
	
	Handle(Geom2d_TrimmedCurve) segment = GCE2d_MakeSegment(p1 , p2);
	h = BRepBuilderAPI_MakeEdge(segment , cyl);
	
	return h;
}

//read first three numbers from canon_line
gp_Pnt gcode2Model::readXYZ ( QString canon_line )
{
	gp_Pnt p;
	QString t = canon_line.section( '(',1,1 ).section( ')',0,0 );
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

void gcode2Model::checkEdge( std::vector<myEdgeType> edges, int n )
{
  if (n < 2) return;
  float d = 0;
  d = edges[n].start.Distance(edges[n-1].end);
  if (d > Precision::Confusion())
    cout << "Found gap of " << d << " before edge " << n << endl;
}
