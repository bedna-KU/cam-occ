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

#ifndef DISPSHAPE_HH
#define DISPSHAPE_HH

#include <limits.h>

#include <AIS_Shape.hxx>
#include <AIS_DisplayMode.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <Handle_AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>
/**
\class dispShape
\brief Stores shapes to be displayed, as well as data for how to display them.
*/
class dispShape {
  public:
    dispShape (const TopoDS_Shape& s, const int lineNbr=0, const Graphic3d_NameOfMaterial nom = Graphic3d_NOM_PLASTIC, const AIS_DisplayMode mode = AIS_Shaded);
    void display();
    int line() {return canLine;};
  private:
    Handle(AIS_Shape) s;
    Graphic3d_NameOfMaterial NoM;
    AIS_DisplayMode dMode;
    int canLine; //store the line number
};



#endif //DISPSHAPE_HH
