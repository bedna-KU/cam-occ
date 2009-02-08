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
#ifndef SHAPEINFO_H
#define SHAPEINFO_H

#include <limits.h>
#include <vector>

#include <gp_Ax1.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>


#include <QAction>
#include <QMenuBar>
#include <QObject>

#include "uiStuff.h"

class shapeInfo : public uiStuff
{
	Q_OBJECT

public:
	shapeInfo();
	void init(QoccHarnessWindow *window);
private slots:
	void infoButton();
	void countArcs();
	void canFace();

private:
	void edgeInfo(TopoDS_Edge E);
	void faceInfo(TopoDS_Face F);
	void solidInfo(TopoDS_Solid S);
	void vertexInfo(TopoDS_Vertex V);
	void arcCount(TopoDS_Solid S);
	QString toNC(char letter, Standard_Real number, Standard_Real last = NaN);
	QString toNC(gp_Pnt p);


/*********************************************************************
*****  
*****  Data and functions for binning arcs
*****  Note that which arcs are binned together may depend on the
*****  order in which they are analyzed, if the difference between
*****  them is near one of the tolerance values.  this, as well as
*****  binning in general, could cause headaches when using models
*****  that are imprecise or very complex.
*****  
*****  There is a caveat with using gp_Pnt for arcsByCenter: two arcs
*****  may have the same center, but not be in the same plane.  They
*****  would still be binned together.  It is necessary to use 
*****  gp_Ax2.  gp_Ax2 may also have a drawback: two arcs in the
*****  same plane and with the same center might have different
*****  gp_Ax2's, with one being upside down. (need to verify this)
*****  
*********************************************************************/
	void storeArcByRadius(Standard_Real r);
	void storeArcByAxis(gp_Ax1 axis);
	void printBinningResults();


	Standard_Real pointTol;		//tolerance for determining equality of points
	Standard_Real radiusTol;	//tolerance for arc radius
	Standard_Real ax1AngTol;	//angular tolerance for arc gp_Ax1 isCoaxial()
	Standard_Real ax1LinTol;	//linear tolerance for arc gp_Ax1 isCoaxial()

/* Skip this for now, using radius and axis may be enough.  May not need to find arcs w/common center.
** Or maybe use gp_Pnt center and gp_Ax1 axis - easier to compare than gp_Ax2 which contains unnecessary data.

	typedef struct {
		gp_Ax2 l;			//location
		uint i;		//count of arcs in this bin
	} arcCenterBin;
	vector<arcCenterBin> arcsByCenter;
*/

	typedef struct {
		Standard_Real r;		//radius
		uint i;
	} arcRadiusBin;
	std::vector<arcRadiusBin> arcsByRadius;

	typedef struct {
		gp_Ax1 a;			//arc's axis
		uint i;
	} arcAxisBin;
	std::vector<arcAxisBin> arcsByAxis;

	typedef struct {
		gp_Pnt c;
		gp_Ax1 ax;
		Standard_Real r;
	} anArc;


	QMenu *myMenu;
	QAction *infoAction;
	QAction *countAction;
};
#endif //SHAPEINFO_H
