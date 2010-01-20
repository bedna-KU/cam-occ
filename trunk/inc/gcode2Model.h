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
#ifndef GCODE2MODEL_H
#define GCODE2MODEL_H

#include <limits.h>
#include <vector>

#include <QAction>
#include <QMenuBar>
#include <QObject>
#include <QProcess>

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shape.hxx>
#include "uiStuff.h"

class gcode2Model : public uiStuff
{
	Q_OBJECT

public:
	gcode2Model();
private slots:
	void myMenuItem();
	void myNextMenuItem();

private:
	typedef enum { TRAVERSE, FEED } MOTION_TYPE;
	typedef enum { ARC, HELIX, LINE } SHAPE_TYPE;
	typedef struct {
		gp_Pnt start,end;
		TopoDS_Edge e;
		MOTION_TYPE motion;
		SHAPE_TYPE shape;
		//int N,S,F;
		//saving vector directions would make the solid easier / faster to make maybe... how to calc?
	} myEdgeType;
	typedef struct {
		double startGap, endGap;
		gp_Pnt realStart, realEnd;
	} chkEdgeStruct;
	gp_Pnt last;	//this holds the coordinates of the end of the last move
	bool firstPoint;
	typedef enum {CANON_PLANE_XY, CANON_PLANE_YZ, CANON_PLANE_XZ} CANONPLANE;   //for arcs
	CANONPLANE CANON_PLANE;

	TopoDS_Wire thePath;
	std::vector<myEdgeType> traverseEdges;
	std::vector<myEdgeType> feedEdges;
	//bool waitRead(QProcess &canon);
	void sleepSecond();
	//std::vector<TopoDS_Shape> 
	TopoDS_Shape traverseSweeps;
	//std::vector<TopoDS_Shape> 
	TopoDS_Shape feedSweeps;
	
	void readLines ( QString filename );
	bool processCanonLine ( QString canon_line );
	void sweep();
	void showWire();
	bool drawSome(int i);
	gp_Pnt readXYZ ( QString canon_line );
	Standard_Real readOne ( QString canon_line, uint n );
	TopoDS_Wire create2dTool(Standard_Real diam, Standard_Real shape);
	//TopoDS_Edge arc ( gp_Pnt a, gp_Pnt b, gp_Pnt c );
	chkEdgeStruct checkEdge( std::vector<myEdgeType> edges, uint n );
	TopoDS_Edge arc ( gp_Pnt a, gp_Vec V, gp_Pnt c );
	TopoDS_Edge helix(gp_Pnt start, gp_Pnt end, gp_Pnt c, gp_Dir dir, int rot);

	QMenu *myMenu;
	QAction *myAction;
	QAction *nextAction;
	bool hasProcessedNgc;
	bool interpret(QString file);
	void cleanUp();
};
#endif //GCODE2MODEL_H
