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


/****************************************************************************
aSample - a fairly simple class to make it easier to modify/experiment with
camocc.

If you wish to copy this class, there are four lines in cam.cpp that must
be copied, along with this file and ../inc/aSample.h.  Of course, you
will need to change the class name and #include "aSample.h".

This class inherits from uiStuff, so those functions can be used. If you 
need to do something when the app starts up, put it at the bottom of the 
init() function.

For ideas, take a look at the OCC samples, including the java app.
Where Context or myContext is used, you will need to use 
theWindow->getContext()-> instead. Replace View or myView with 
theWindow->getView()->

theWindow->getContext() is a pointer to an AIS_InteractiveContext.  If you
downloaded the Doxygen documentation for OCC, this is documented at
ReferenceDocumentation/Visualization/html/classAIS__InteractiveContext.html

Another useful resource is Google's Code Search. For example,
http://google.com/codesearch?q=BRepAlgoAPI_Section&hl=en&btnG=Search+Code
****************************************************************************/


#include "aSample.h"	//note - this is in ../inc/
#include <ostream>

#include <QKeySequence>
#include <QtGui>
#include <AIS_InteractiveContext.hxx>

//You may not need the following headers - depends on what you are doing.
#include <AIS_Shape.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS_Shape.hxx>



aSample::aSample(){
	theWindow = 0;
}

void aSample::init(QoccHarnessWindow* window) {
	theWindow = window;

	myMenu = new QMenu("aSample");
	theWindow->menuBar()->insertMenu(theWindow->getHelpMenu(),myMenu);

	//these five lines set up a menu item.  Uncomment the second one to have a shortcut.
	myAction = new QAction("aSample", this);
	//myAction->setShortcut(QString("Ctrl+A"));
	myAction->setStatusTip("aSample");
	connect(myAction, SIGNAL(triggered()), this, SLOT(myMenuItem()));
	myMenu->addAction( myAction );
	
};

void aSample::myMenuItem(){
	Standard_Real aXmin = 0, aYmin = 0, aZmin = 0, aXmax = 20, aYmax = 30, aZmax = 10;
	gp_Pnt corner1 = gp_Pnt(aXmin, aYmin, aZmin);
	gp_Pnt corner2 = gp_Pnt(aXmax, aYmax, aZmax);

	slotNeutralSelection();		//Close any local contexts.  Any objects created while a local context is open will dissappear when that context is closed and they won't be shaded.

	infoMsg("Ready to create a box between " + toString(corner1) + " and " + toString(corner2) + ".");

	//create a shape
	TopoDS_Shape Shape = BRepPrimAPI_MakeBox(corner1,corner2).Shape();

	//convert it to an AIS_Shape and display it
	Handle_AIS_Shape AisShape = new AIS_Shape (Shape);
	theWindow->getContext()->SetMaterial(AisShape,Graphic3d_NOM_PLASTIC);  //Supposed to look like plastic.  Try GOLD or PLASTER or ...
	theWindow->getContext()->SetDisplayMode(AisShape,1,Standard_False);  //shaded
	theWindow->getContext()->Display(AisShape);
	//redraw();	//not necessary, Display() automatically redraws the screen.
}

