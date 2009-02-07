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
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS_Shape.hxx>
#include <Handle_Geom_TrimmedCurve.hxx>
#include <GC_MakeArcOfCircle.hxx>



gcode2Model::gcode2Model()
{
	theWindow = 0;
}

void gcode2Model::init ( QoccHarnessWindow* window )
{
	theWindow = window;

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
//open file, read, convert to arcs and lines
//sweep 2d wire
//detect non-tangencies and use 3d wire revolved there
//compute solid
//then subtract
//display
	Standard_Real aXmin = 0, aYmin = 0, aZmin = 0, aXmax = 20, aYmax = 30, aZmax = 10;
	gp_Pnt corner1 = gp_Pnt ( aXmin, aYmin, aZmin );
	gp_Pnt corner2 = gp_Pnt ( aXmax, aYmax, aZmax );

	slotNeutralSelection();		//Close any local contexts.  Any objects created while a local context is open will dissappear when that context is closed and they won't be shaded.

	TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge ( gp_Pnt ( 0,0,0 ), gp_Pnt ( 1,1,1 ) );


	//create a shape
	TopoDS_Shape Shape = BRepPrimAPI_MakeBox ( corner1,corner2 ).Shape();

	//convert it to an AIS_Shape and display it
	Handle_AIS_Shape AisShape = new AIS_Shape ( Shape );
	theWindow->getContext()->SetMaterial ( AisShape,Graphic3d_NOM_PLASTIC );  //Supposed to look like plastic.  Try GOLD or PLASTER or ...
	theWindow->getContext()->SetDisplayMode ( AisShape,1,Standard_False );  //shaded
	theWindow->getContext()->Display ( AisShape );
	//redraw();	//not necessary, Display() automatically redraws the screen.
}

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
