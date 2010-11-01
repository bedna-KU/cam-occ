/***************************************************************************
 *   Copyright (C) 2010 by Mark Pictor                                     *
 *   mpictor@gmail.com                                                     *
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

#ifndef SILHOUETTE_HH
#define SILHOUETTE_HH

#include <limits.h>

#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Solid.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <gp_Dir.hxx>

/** Create the silhouette (outline) of a tool
*/
class silhouette {
  public:
    silhouette(TopoDS_Solid tool, gp_Dir normal, double radius, double height);
    void init(TopoDS_Solid tool, gp_Dir normal, double radius, double height);
    void newAngle(gp_Dir normal);
    bool done() {return myDone;};
    TopoDS_Wire result() {return myOutline;}
  protected: //data
    TopoDS_Wire myOutline;
    gp_Dir myNormal;
    bool myDone;
    TopTools_IndexedDataMapOfShapeListOfShape hlrMap;
    //TopoDS_ListOfShape myEdges;
    TopoDS_Solid myTool;
    double myRadius,myAngle,myHeight;

  protected: //functions
    void createHlrLines(TopoDS_Shape t, gp_Dir dir);
    bool removeFromList(TopoDS_Edge e);
    bool prune();
    bool reduceMultis();
    double getRemovableDist(TopoDS_Edge e);

};


#endif //SILHOUETTE_HH
