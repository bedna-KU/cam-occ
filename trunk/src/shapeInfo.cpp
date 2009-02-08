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




shapeInfo::shapeInfo()
{
	theWindow = 0;

	//tolerances for grouping (binning) arcs
	pointTol = Precision::Confusion();
	radiusTol = Precision::Confusion();
	ax1AngTol = Precision::Angular();
	ax1LinTol = Precision::Confusion();
}

void shapeInfo::init ( QoccHarnessWindow* window )
{

	theWindow = window;

	//set up menus, signals
	myMenu = new QMenu ( "Shape" );
	theWindow->menuBar()->insertMenu ( theWindow->getHelpMenu(),myMenu );

	infoAction = new QAction ( "Shape &Info", this );
	infoAction->setShortcut ( QString ( "Ctrl+I" ) );
	infoAction->setStatusTip ( "Shape Info" );
	connect ( infoAction, SIGNAL ( triggered() ), this, SLOT ( infoButton() ) );
	myMenu->addAction ( infoAction );

	countAction = new QAction ( "Arc Count", this );
	//countAction->setShortcut(QString("Ctrl+I"));
	countAction->setStatusTip ( "Counts the number of arcs in a solid, groups them based on radius and centerpoint" );
	connect ( countAction, SIGNAL ( triggered() ), this, SLOT ( countArcs() ) );
	myMenu->addAction ( countAction );

	canAction = new QAction ( "Canned cycle from hole face", this );
	//countAction->setShortcut(QString("Ctrl+I"));
	countAction->setStatusTip ( "Generates a line of g-code for a canned cycle for a hole,\nfrom a selected cylindrical face" );
	connect ( canAction, SIGNAL ( triggered() ), this, SLOT ( canFace() ) );
	myMenu->addAction ( canAction );

};

void shapeInfo::canFace()
{
	TopExp_Explorer Ex;
	BRepAdaptor_Curve adaptor;
	TopoDS_Edge e;
	gp_Pnt p1, p2, c;
	gp_Ax1 axis;
	Standard_Real r;
	std::vector<anArc> arcsV;

	getSelection();
	for ( uint i=0;i < uiStuff::selectedShapes.size();i++ )
	{
		TopoDS_Shape S = uiStuff::selectedShapes[i];
		TopoDS_Face cylFace = TopoDS::Face ( S );
		for ( Ex.Init ( cylFace, TopAbs_EDGE ) ; Ex.More() ; Ex.Next() )
		{
			e = TopoDS::Edge(Ex.Current());
			adaptor.Initialize ( e );
			if ( adaptor.GetType() ==GeomAbs_Circle )
			{
				anArc thisArc;
				gp_Circ circ = adaptor.Circle();
				thisArc.c = circ.Location();
				thisArc.r = circ.Radius();
				thisArc.ax = circ.Axis();
				arcsV.push_back(thisArc);
			}
		}

		//now compare arcs, must have at least 2 arcs
		bool foundMatch = multipleMatch = false;
		anArc a,b;
		for ( uint j = 0; j < arcsV.size()-1; j++)
		{
			anArc arc1 = arcsV[j];
			for ( uint k = j+1; k < arcsV.size(); k++)
			{
				anArc arc2 = arcsV[k];
				if (	//(arc1.c.IsEqual(arc2.c, pointTol)) && //wtf don't compare this!!! no matches!
					(arc1.r == arc2.r) && 
					(arc1.ax.IsCoaxial(arc2.ax, angTol, linTol)) )
				{
					if ( foundMatch ) 
					{
						multipleMatch || infoMsg("Warning - more than 2 arcs are coaxial."); //tell user once
						multipleMatch = true;
					} else {
						foundMatch = true;
						a = arc1; b = arc2;
					}
				}
				
			}
		}
		if (foundMatch) 
		{
			//a, b contain the data
			//G81 X- Y- Z- A- B- C- R- L-
			//G90 G81 G98 X4 Y5 Z1.5 R2.8
			//http://www.linuxcnc.org/docs/html/gcode_main.html#sub:G81:-Drilling-Cycle
			QString block, comment;
			gp_Pnt top, bottom;
			if (a.c.Z() > b.c.Z()) {
				top = a.c;	//top of the hole
				bottom = b.c;
			} else {
				top = b.c;
				bottom = a.c;
			}
			block = "G90 G81 G98 ";
			block += toNC(bottom); //adds XYZ
			if ( !a.ax.IsCoaxial( gp_Dir(0,0,1), ax1AngTol, ax1LinTol ) )
			{
				//TODO: figure out ABC and print them
				comment += "Angles are in radians --INCOMPLETE!!!--";
			} else comment += "Omitting ABC: hole is vertical";
			block += toNC("R",top.Z());
			infoMsg("Canned cycle G81\n"+block+" ("+comment+")");
		} else {
			infoMsg("Must select a face that is a section of a\n" +
				"cylinder.  Could not find a pair of arcs that\n" +
				"were coaxial and had the same radius.");
		}


/*
		arcCount(cylFace);  
		//arcsByAxis should now contain one bin with two arcs
		for( int n = 0; n < arcsByAxis.size(); n++ )
		{
			if (arcsByAxis[n].i == 2)
			{
				arcsByAxis[n].a.Location()
				arcsByAxis[n].a.Direction()
			}
		}
*/
	}

}

//formats X,Y,Z for a RS274NGC block
QString shapeInfo::toNC(gp_Pnt p)
{
	QString s;
	s = toNC("X",p.X()) + toNC("Y",p.Y()) + toNC("Z",p.Z());
	//s.sprintf("X%#.7f Y%#.7f Z%#.7f",p.X(),p.Y(),p.Z());
	return s;
}

//Formats number for part of a RS274NGC block, preceding it with letter.
//if number == last, don't print anything
QString shapeInfo::toNC(char letter, Standard_Real number, Standard_Real last = NaN)
{
	QString s = "";
	(number == last) || s.sprintf("%c%#.7f",letter, number);
	return s;

}

void shapeInfo::countArcs()
{
	getSelection();  //puts selection into the vector uiStuff::selectedShapes
	for ( uint i=0;i < uiStuff::selectedShapes.size();i++ )
	{
//		infoMsg("solid");
		TopoDS_Shape S = uiStuff::selectedShapes[i];
//		checkShapeType(S);
		arcCount ( TopoDS::Solid ( S ) );
	}
	printBinningResults(); //message user or save text file? either?
}


//use "bins" to group arcs
//when comparing values to determine the bin, use Precision::Confusion or other value, so that these truly are the bins used in sadistics.
//  OCC Doxygen: ReferenceDocumentation/FoundationClasses/html/classPrecision.html
void shapeInfo::arcCount ( TopoDS_Solid theShape )
{
	TopExp_Explorer Ex;
	BRepAdaptor_Curve adaptor;
	TopoDS_Edge e;
	gp_Pnt p1, p2;
	gp_Ax1 axis;
	Standard_Real r;




	for ( Ex.Init ( theShape, TopAbs_EDGE ) ; Ex.More() ; Ex.Next() )
	{
		e = TopoDS::Edge(Ex.Current());  //FIXME: correct?
		adaptor.Initialize ( e );
		if ( adaptor.GetType() ==GeomAbs_Circle )
		{
			gp_Circ circ = adaptor.Circle();
//	                c = circ.Position();
			r = circ.Radius();
			axis = circ.Axis();
			//p1 = adaptor.Value ( adaptor.FirstParameter() );
			//p2 = adaptor.Value ( adaptor.LastParameter() );
//			storeArcByPosition(c);
			storeArcByRadius ( r );
			storeArcByAxis ( axis );
		}
	}
}

//sorts an arc into a bin, based on its radius and radiusTol
void shapeInfo::storeArcByRadius ( Standard_Real r )
{
	bool notFound = true;
	for ( uint n = 0 ; notFound && n < arcsByRadius.size() ; n++ )
	{
		if ( abs ( r - arcsByRadius[n].r ) < radiusTol )
		{
			arcsByRadius[n].i++;
			notFound = false;
		}
	}
	if (notFound)
	{
		arcRadiusBin bin;
		bin.r = r;
		bin.i = 1;
		arcsByRadius.push_back(bin);
	}
}

//sort an arc into a bin, based on its axis and ax1*Tol
void shapeInfo::storeArcByAxis ( gp_Ax1 axis )
{
	bool notFound = true;
	for ( uint n = 0 ; notFound && n < arcsByAxis.size() ; n++ )
	{
		if ( axis.IsCoaxial( arcsByAxis[n].a, ax1AngTol, ax1LinTol ) )
		{
			arcsByAxis[n].i++;
			notFound = false;
		}
	}
	if (notFound)
	{
		arcAxisBin bin;
		bin.a = axis;
		bin.i = 1;
		arcsByAxis.push_back(bin);
	}
}

void shapeInfo::printBinningResults()
{
	int rBins = arcsByRadius.size();
	int aBins = arcsByAxis.size();
	QString str = "<H2>Binning results</H2><hl><H3>By Radius:</H3><br>";
	str += QString("%1 bins<br>").arg(rBins);
	for( int n = 0; n < rBins; n++ )
	{
		str += QString("Bin %1: %2 arcs, radius %3<br>").arg(n).arg(arcsByRadius[n].i).arg(arcsByRadius[n].r);
	}
	str += "<br><H3>By Axis:</H3><br>";
	str += QString("%1 bins<br>").arg(aBins);
	for( int n = 0; n < aBins; n++ )
	{
		str += QString("Bin %1: %2 arcs, with axis passing through point ").arg(n).arg(arcsByAxis[n].i);
		str += toString(arcsByAxis[n].a.Location());
		str += " in direction ";
		str += toString(arcsByAxis[n].a.Direction());
		str += ".<br>";
	}
	arcsByRadius.clear();
	arcsByAxis.clear();
	cout << str.toStdString() << endl;
	//longMsg(str);
}

//this fails silently on at least one solid, an imported STEP file.  Not sure why. Don't get the warning, so ?!
void shapeInfo::infoButton()
{
	getSelection();
	for ( uint i=0;i < uiStuff::selectedShapes.size();i++ )
	{
		TopoDS_Shape S = uiStuff::selectedShapes[i];
		switch ( S.ShapeType() )
		{
			case TopAbs_VERTEX :
				vertexInfo ( TopoDS::Vertex ( S ) );
				break;
			case TopAbs_EDGE :
				edgeInfo ( TopoDS::Edge ( S ) );
				break;
			case TopAbs_FACE :
				faceInfo ( TopoDS::Face ( S ) );
				break;
			case TopAbs_SOLID :
				solidInfo ( TopoDS::Solid ( S ) );
				break;
			default:
				debugMsg ( "Warning - reached default case when testing the shape's type.",__FILE__,__LINE__ );
		}
	}
}


void shapeInfo::vertexInfo ( TopoDS_Vertex V )
{
	QString s = "Vertex XYZ: " + toString ( BRep_Tool::Pnt ( V ) );
	infoMsg ( s );
}

/*
**
**  Tell the user about the solid that is selected.
**  TODO: more info (bbox coords & volume, number of faces, number of edges, ...)
**
*/
void shapeInfo::solidInfo ( TopoDS_Solid S )
{
	GProp_GProps System;
	BRepGProp::VolumeProperties ( S,System );
	gp_Pnt G = System.CentreOfMass ();
	QString s = QString ( "Solid selected. Center of mass: %1\nVolume: %2"
	                    ).arg ( toString ( G ) ).arg ( System.Mass() );
	infoMsg ( s );
}

/*
**
**  Tell the user about the face that is selected.
**  TODO: number of edges, area, test if it is a section of a
**  cylinder, plane, or sphere (i.e. easily machined with proper mill bit)
**  examine neighbors
**
*/
void shapeInfo::faceInfo ( TopoDS_Face F )
{

	/*
	**  from forum: "Developing the idea of Rob, I suggest that you compute
	**  principal axes of inertia of your shape using BRepGProp functions,
	**  and transform the shape from that system before computing bnd box."
	*/

	Bnd_Box aBox;
	Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
	BRepAdaptor_Surface aSurf ( F );
	BndLib_AddSurface::Add ( aSurf, ( Standard_Real ) 0.0,aBox );
	aBox.Get ( aXmin,aYmin,aZmin, aXmax,aYmax,aZmax );
	QString s = QString ( "Selected a face. X extents %1, %2.\n" ).arg ( aXmin ).arg ( aXmax );
	s += QString ( "Y extents %1, %2. " ).arg ( aYmin ).arg ( aYmax );
	s += QString ( "Z extents %1, %2." ).arg ( aZmin ).arg ( aZmax );
	infoMsg ( s );
}

/*
**  copied from cam-occ/qt3
**  There are two ways to go from  a TopoDS entity to a form
**  where basics like center of a circle can be extracted.
**  We mix them since BRepTool does not test shape type and
**  the entity types returned by BRepAdaptor do not provide
**  a method to turn parameters into points.
*/
void shapeInfo::edgeInfo ( TopoDS_Edge E )
{
	Standard_Real first, last;
	gp_Pnt p1,p2,c;

//we don't (yet) change the position of the part in camocc2, so skip the transform...
//        BRepBuilderAPI_Transform trsf(E,Part.GetAxis());
//        TopoDS_Edge Et=TopoDS::Edge(trsf.Shape());

	Handle ( Geom_Curve ) C = BRep_Tool::Curve ( E,first,last );
	BRepAdaptor_Curve adaptor = BRepAdaptor_Curve ( E );
	QString s;
	if ( adaptor.GetType() ==GeomAbs_Circle )
	{
		gp_Circ circ = adaptor.Circle();
		c = circ.Location();
		Standard_Real R = circ.Radius();
		s = QString ( "Center of that arc: %1; Radius: %2" ).arg ( toString ( c ) ).arg ( R );
	}
	else if ( adaptor.GetType() ==GeomAbs_Line )
	{
		///gp_Lin line = adaptor.Line();
		s = "Line";
	}
	else s = "Edge";
	p1=C->Value ( first );
	p2=C->Value ( last );
	s += QString ( "\nFirst point %1\nLast point %2" ).arg ( toString ( p1 ) ).arg ( toString ( p2 ) );
	infoMsg ( s );
}
