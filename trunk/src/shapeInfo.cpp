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
#include "shapeInfo.h"
#include <ostream>
#include <AIS_Trihedron.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_InteractiveContext.hxx>
#include <V3d_View.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <Handle_Geom_Curve.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRep_Tool.hxx>
#include <gp_Circ.hxx>
#include <gp_Lin.hxx>
#include <TopoDS_Edge.hxx>
#include <Bnd_Box.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BndLib_AddSurface.hxx>


#include <QKeySequence>
#include <QtGui>




shapeInfo::shapeInfo(){
	theWindow = 0;
}

void shapeInfo::init(QoccHarnessWindow* window) {

	theWindow = window;

	//set up menus, signals
	myMenu = new QMenu("Shape");
	theWindow->menuBar()->insertMenu(theWindow->getHelpMenu(),myMenu);

	infoAction = new QAction("Shape &Info", this);
	infoAction->setShortcut(QString("Ctrl+I"));
	infoAction->setStatusTip("Shape Info");
	connect(infoAction, SIGNAL(triggered()), this, SLOT(infoButton()));
	myMenu->addAction( infoAction );
	
};

//this fails silently on at least one solid, an imported STEP file.  Not sure why.
void shapeInfo::infoButton(){
	getSelection();
	for (uint i=0;i < uiStuff::selectedShapes.size();i++) {
		TopoDS_Shape S = uiStuff::selectedShapes[i];
		switch (S.ShapeType()) {
		case TopAbs_VERTEX :
                        vertexInfo(TopoDS::Vertex(S));
			break;
		case TopAbs_FACE :
                        faceInfo(TopoDS::Face(S));
			break;
                case TopAbs_SOLID :
                        solidInfo(TopoDS::Solid(S));
			break;
                case TopAbs_EDGE :
                        edgeInfo(TopoDS::Edge(S));
			break;
		default:
			debugMsg("Warning - reached default case when testing the shape's type.",__FILE__,__LINE__);
		}
        }
}


void shapeInfo::vertexInfo(TopoDS_Vertex V) {
	QString s = "Vertex XYZ: " + toString(BRep_Tool::Pnt(V));
	infoMsg(s);
}

/*
**  
**  Tell the user about the solid that is selected.
**  TODO: more info (bbox coords & volume, number of faces, number of edges, ...)
**  
*/
void shapeInfo::solidInfo(TopoDS_Solid S) {
	GProp_GProps System;
	BRepGProp::VolumeProperties(S,System);
	gp_Pnt G = System.CentreOfMass ();
	QString s = QString( "Solid selected. Center of mass: %1, %2, %3\nVolume: %4"
			).arg(G.X()).arg(G.Y()).arg(G.Z()).arg(System.Mass());
	infoMsg(s);
}

/*
**  
**  Tell the user about the face that is selected.
**  TODO: number of edges, area, test if it is a section of a
**  cylinder, plane, or sphere (i.e. easily machined with proper mill bit)
**  examine neighbors
**  
*/
void shapeInfo::faceInfo(TopoDS_Face F) {

/*
**  from forum: "Developing the idea of Rob, I suggest that you compute 
**  principal axes of inertia of your shape using BRepGProp functions,
**  and transform the shape from that system before computing bnd box."
*/

	Bnd_Box aBox;
	Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
	BRepAdaptor_Surface aSurf(F);
	BndLib_AddSurface::Add(aSurf,(Standard_Real) 0.0,aBox);
	aBox.Get(aXmin,aYmin,aZmin, aXmax,aYmax,aZmax);
	QString s = QString("Selected a face. X extents %1, %2.\n").arg(aXmin).arg(aXmax);
	s += QString("Y extents %1, %2. ").arg(aYmin).arg(aYmax);
	s += QString("Z extents %1, %2.").arg(aZmin).arg(aZmax);
	infoMsg(s);
}

/*
**  copied from cam-occ/qt3
**  There are two ways to go from  a TopoDS entity to a form
**  where basics like center of a circle can be extracted.
**  We mix them since BRepTool does not test shape type and
**  the entity types returned by BRepAdaptor do not provide
**  a method to turn parameters into points.
*/
void shapeInfo::edgeInfo(TopoDS_Edge E) {
        Standard_Real first, last;
        gp_Pnt p1,p2,c;

//we don't (yet) change the position of the part in camocc2, so skip the transform...
//        BRepBuilderAPI_Transform trsf(E,Part.GetAxis());
//        TopoDS_Edge Et=TopoDS::Edge(trsf.Shape());

        Handle(Geom_Curve) C = BRep_Tool::Curve(E,first,last);
        BRepAdaptor_Curve adaptor = BRepAdaptor_Curve(E);
        QString s;
        if (adaptor.GetType()==GeomAbs_Circle) {
                gp_Circ circ = adaptor.Circle();
                c = circ.Location();
                Standard_Real R = circ.Radius();
                s = QString("Center of that arc: %1, %2, %3; Radius: %4").arg(c.X()).arg(c.Y()).arg(c.Z()).arg(R);
        } else if (adaptor.GetType()==GeomAbs_Line) {
                ///gp_Lin line = adaptor.Line();
                s = "Line";
        } else s = "Edge";
        p1=C->Value(first);
        p2=C->Value(last);
        s += QString("\nFirst point %1, %2, %3\nLast point %4, %5, %6"
			).arg(p1.X()).arg(p1.Y()).arg(p1.Z()).arg(p2.X()).arg(p2.Y()).arg(p2.Z());
        infoMsg(s);
}
