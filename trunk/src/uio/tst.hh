#ifndef TST_HH
#define TST_HH

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
#include <limits.h> //included to fix errors compiling MOC stuff

#include <QObject>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Dir.hxx>

class nearestEdges {
  public:
    nearestEdges();
    int n;
  //TopTools_ListOfShape edges;
    TopoDS_Edge a,b;
    gp_Pnt c,d;
    bool e;
    std::string meminfo();
};

struct twopnts {
  gp_Pnt a,b;
  bool e;
};


class tst: public QObject {
  Q_OBJECT;
  public:
    tst();
  public slots:
    void slotTest1();
  protected:
    TopoDS_Wire halfProf();
    TopoDS_Shape ballnose(double len, double dia);
    TopoDS_Face silhouette(TopoDS_Shape t, gp_Dir dir = gp_Dir(0,1,0));
    TopoDS_Compound hlrLines(TopoDS_Shape t, gp_Dir dir);
    TopoDS_Wire outermost(TopoDS_Compound h);
    nearestEdges findNearestEdges(TopoDS_Shape s, TopoDS_Shape t);
    double mass(TopoDS_Shape m);
    bool samepnt(gp_Pnt a, gp_Pnt b);
    twopnts getEnds(TopoDS_Edge e);
    bool cmpPntPnts(gp_Pnt c,gp_Pnt p1,gp_Pnt p2);
    bool cmpPntPnts(gp_Pnt c,twopnts tp);



};
#endif //TST_HH
