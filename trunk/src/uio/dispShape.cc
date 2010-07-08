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

#include "dispShape.hh"
#include "uio.hh"

#include <limits.h>

#include <AIS_Shape.hxx>
#include <AIS_DisplayMode.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <TopoDS_Shape.hxx>

dispShape::dispShape( const TopoDS_Shape& shape,
                      const int lineNbr,
                      const Graphic3d_NameOfMaterial matl,
                      const AIS_DisplayMode mode):
                   NoM(matl), dMode(mode) {
  s = new AIS_Shape(shape);
  canLine = lineNbr;
}

void dispShape::display() {
  uio::context()->SetMaterial ( s, NoM, Standard_True );
  uio::context()->SetDisplayMode ( s,Standard_Integer(dMode),Standard_False );
  uio::context()->Display ( s );
}


/* values for DisplayMode and NameOfMaterial:
enum      AIS_DisplayMode { AIS_WireFrame, AIS_Shaded, AIS_QuickHLR, AIS_ExactHLR  }

enum      Graphic3d_NameOfMaterial {
  Graphic3d_NOM_BRASS, Graphic3d_NOM_BRONZE, Graphic3d_NOM_COPPER, Graphic3d_NOM_GOLD,
  Graphic3d_NOM_PEWTER, Graphic3d_NOM_PLASTER, Graphic3d_NOM_PLASTIC, Graphic3d_NOM_SILVER,
  Graphic3d_NOM_STEEL, Graphic3d_NOM_STONE, Graphic3d_NOM_SHINY_PLASTIC, Graphic3d_NOM_SATIN,
  Graphic3d_NOM_METALIZED, Graphic3d_NOM_NEON_GNC, Graphic3d_NOM_CHROME, Graphic3d_NOM_ALUMINIUM,
  Graphic3d_NOM_OBSIDIAN, Graphic3d_NOM_NEON_PHC, Graphic3d_NOM_JADE, Graphic3d_NOM_DEFAULT,
  Graphic3d_NOM_UserDefined
}
*/
