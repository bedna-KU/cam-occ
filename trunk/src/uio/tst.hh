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
#include <TopoDS_Solid.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Dir.hxx>



class tst: public QObject {
  Q_OBJECT;
  public:
    tst();
  public slots:
    void slotTest1();
  protected:
    TopoDS_Wire halfProf();
    TopoDS_Shape ballnose(double len, double dia);
    TopoDS_Shape getProj(TopoDS_Shape t, gp_Dir dir = gp_Dir(0,1,0));
    double mass(TopoDS_Shape m);

};
#endif //TST_HH
