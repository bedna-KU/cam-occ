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

//gcode stuff for gcode2model

#include "gcode2Model.h"
#include <Precision.hxx> 
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <gp_Pnt.hxx>

using std::cout;

bool gcode2Model::interpret ( QString file )
{
	QString ipath = "/opt/src/emc2/trunk/";
	QString interp = ipath + "bin/rs274";
	QString iparm = ipath + "configs/sim/sim_mm.var\n";
	QString itool = ipath + "configs/sim/sim_mm.tbl\n";
	QProcess toCanon;
	bool foundEOF;
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
		return false;
	}
	
	//if readLine is used at the wrong time, it is possible to pick up a line fragment! will canReadLine() fix that?
	qint64 lineLength;
	char line[260];
	uint fails = 0;
	do {
		if (toCanon.canReadLine()) {
			lineLength = toCanon.readLine(line, sizeof(line));
			if (lineLength != -1 ) {
				foundEOF = processCanonLine(line);
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
	return foundEOF;
}

bool gcode2Model::processCanonLine ( QString canon_line )
{
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
			return false;
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
			if (canon_line.contains("FEED"))
			  	edge.motion = FEED;
			else
			  	edge.motion = TRAVERSE;
			edge.shape = LINE;
			feedEdges.push_back( edge );
		}
	} else if (canon_line.startsWith( "ARC_FEED(" )) {
		gp_Dir arcDir;
		gp_Pnt c;
		double x,y,z,ep,a1,a2,e1,e2,zdist;
		int rot=0;
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
		//skip arc if zero length; caught this bug thanks to tort.ngc
		if (edge.start.Distance(edge.end) > Precision::Confusion()) { 
			//center is c; ends are edge.start, edge.last
			if (fabs(zdist) > 0.000001) {
				edge.e = helix(edge.start, edge.end, c, arcDir,rot);
				edge.shape = HELIX;
			} else {
				gp_Vec Vr = gp_Vec(c,edge.start);	//vector from center to start
				gp_Vec Va = gp_Vec(arcDir);		//vector along arc's axis
				gp_Vec startVec = Vr^Va;		//find perpendicular vector using cross product
				if (rot==1) startVec *= -1;
				//cout << "Arc with vector at start: " << toString(startVec).toStdString();
				edge.e = arc(edge.start, startVec, edge.end);
				edge.shape = ARC;
			}
			edge.motion = FEED;
			feedEdges.push_back( edge );
			chkEdgeStruct check = checkEdge ( feedEdges, feedEdges.size() - 1 );
			if (check.startGap != 0.0) {
				exit(-1); //what SHOULD we do here?!
			}
			if (check.endGap != 0.0) {
				last = check.realEnd;
				feedEdges.back().end = last;
				if (check.endGap > 100.0*Precision::Confusion()) {
					cout << " with center " << toString(c).toStdString();
					if (edge.shape == HELIX) cout << " and arcDir " << toString(arcDir).toStdString();
					cout << " from " << toString(edge.start).toStdString() << " to " << toString(edge.end).toStdString() << endl;
					cout << "params:  e1:"<< e1 <<"  e2:" << e2 <<"  a1:"<< a1 <<"  a2:"<< a2 <<"  rot:" << rot <<"  ep:" << ep << endl;
				}
			}
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
	else if (canon_line.startsWith( "SET_ORIGIN_OFFSETS(0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000)" )) {}
	else if (canon_line.startsWith( "SET_ORIGIN_OFFSETS(0.0000" )) {
	  	//this is a common canon statement. we are going to hijack it to produce a warning, because
		//the data we're getting was produced with a format of %.4f or so
		infoMsg(QString("Warning, input has reduced precision, expected more zeros: <br>") + canon_line );
	}
	else if (canon_line.startsWith( "USE_LENGTH_UNITS(CANON_UNITS_MM)" )) {}
	else if (canon_line.startsWith( "USE_LENGTH_UNITS(CANON_UNITS_INCHES)" )) {}
	else if (canon_line.startsWith( "SET_NAIVECAM_TOLERANCE(0.0000)" )) {}
	else if (canon_line.startsWith( "SET_MOTION_CONTROL_MODE(CANON_CONTINUOUS, 0.000000)" )) {}
	else if (canon_line.startsWith( "SET_XY_ROTATION(0.0000)" )) {}
	else if (canon_line.startsWith( "SET_FEED_REFERENCE(CANON_XYZ)" )) {}
	else if (canon_line.startsWith( "SET_XY_ROTATION(0.000000)" )) {}
	else if (canon_line.startsWith( "SET_NAIVECAM_TOLERANCE(0.000000)" )) {}
	else if (canon_line.startsWith( "PROGRAM_END" )) {
	//cout <<"Program ended."<<endl;
	return true; //true => gcode file ends correctly
	}
	//else if (canon_line.startsWith( "" )) {}
	//else if (canon_line.startsWith( "" )) {}
	else {
		cout << "Does not handle " << canon_line.toStdString() << endl;
	}
	//cout << "Done with line" << endl;
	return false;
}


//read first three numbers from canon_line
gp_Pnt gcode2Model::readXYZ ( QString canon_line )
{
	gp_Pnt p;
	QString t = canon_line.section( '(',1,1 ).section( ')',0,0 );
	p.SetCoord( t.section(',',0,0).toDouble(),
	            t.section(',',1,1).toDouble(),
	            t.section(',',2,2).toDouble() );
	return p;
}

//read nth number from canon_line
//when n = 0, reads number before first comma
Standard_Real gcode2Model::readOne ( QString canon_line, uint n )
{
	QString t = canon_line.section( '(',1,1 ).section( ')',0,0 );
	return t.section(',',n,n).toDouble();
}
