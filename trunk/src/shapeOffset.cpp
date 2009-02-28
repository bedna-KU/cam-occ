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
#include "shapeOffset.h"
#include <ostream>
#include <AIS_Trihedron.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <V3d_View.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <BRepOffsetAPI_MakeOffsetShape.hxx>

#include <QKeySequence>
#include <QtGui>




shapeOffset::shapeOffset(){
// 	theWindow = 0;
// }
// 
// void shapeOffset::init(QoccHarnessWindow* window) {
// 
// 	theWindow = window;

	//set up menus, signals
	myMenu = new QMenu("Offset");
	theWindow->menuBar()->insertMenu(theWindow->getHelpMenu(),myMenu);

	offsetAction = new QAction("Face Offset", this);
	//offsetAction->setShortcut(QString("Ctrl+I"));
	offsetAction->setStatusTip("Face Offset");
	connect(offsetAction, SIGNAL(triggered()), this, SLOT(offsetButton()));
	myMenu->addAction( offsetAction );
	
};

//this fails silently on at least one solid, an imported STEP file.  Not sure why.
void shapeOffset::offsetButton(){
	getSelection();
	for (uint i=0;i < selectedShapes.size();i++) {
		TopoDS_Shape S = selectedShapes[i];
		switch (S.ShapeType()) {
		case TopAbs_VERTEX :
                    //    vertexInfo(TopoDS::Vertex(S));
			break;
		case TopAbs_FACE :
                        faceOffset(TopoDS::Face(S));
			break;
                case TopAbs_SOLID :
                      //  solidInfo(TopoDS::Solid(S));
			break;
                case TopAbs_EDGE :
                     //   edgeInfo(TopoDS::Edge(S));
			break;
		default:
			debugMsg("Warning - reached default case when testing the shape's type.",__FILE__,__LINE__);
		}
        }
}



void shapeOffset::faceOffset(TopoDS_Face F) {
	TopoDS_Shape S = BRepOffsetAPI_MakeOffsetShape(F,5,0.0001);
	//offset face info

	slotNeutralSelection();
	Handle_AIS_Shape AisShape = new AIS_Shape (S);
	theWindow->getContext()->SetMaterial(AisShape,Graphic3d_NOM_PLASTIC);
	theWindow->getContext()->SetDisplayMode(AisShape,1,Standard_False);
	theWindow->getContext()->Display(AisShape);
}
