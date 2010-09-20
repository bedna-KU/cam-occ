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
#ifndef TOOL_HH
#define TOOL_HH

#include <assert.h>
#include <limits.h>

#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Solid.hxx>

#include "canon.hh"

/**
\class tool
\brief Base class for all tools, whether they are mill tools, lathe tools, or something else
*/
typedef enum {ROTARY_TOOL,TURNING_TOOL, UNDEFINED} TOOLTYPE;
class tool {
  public:
    const TopoDS_Wire& getProfile() const {assert (validProfile);return profile;};
    TOOLTYPE getType() const {return type;};
    bool isValid() const {return validProfile;};
  protected:
    tool();
    bool validProfile;
    bool bruteForceOnly; //for creating solid. FIXME:would it make more sense in another class?
    /// profile MUST be in XZ plane!
    TopoDS_Wire profile;
    TOOLTYPE type;
};

/**
\class millTool
\brief Rotary tool for the milling machine
It must be possible to create both a 2d and a 3d representation of a millTool
*/
typedef enum { BALLNOSE, CYLINDRICAL, TOROIDAL, ENGRAVING, TAPERED, OTHER, UNDEF } SHAPE_TYPE;
class millTool: public tool {
  public:
    SHAPE_TYPE getShape() const {return shape;};
    virtual const TopoDS_Shape& get3d();
    double Dia() {return dia;};
    double Len() {return len;};
  protected:
    millTool();
    SHAPE_TYPE shape;
    double dia,len; //diameter, length
    bool validSolid; //true if the solid is valid
    TopoDS_Shape myShape; //it's solid, but not always of type TopoDS_Solid
};

/**
\class aptTool
\brief This class is for the generic APT-style milling tool.
FIXME This class is not complete!
*/
class aptTool: public millTool {
  public:
    aptTool(double dd,double aa,double bb,
	    degrees AA,degrees BB, double ll);
  private:
    double a,b; //d and l are in millTool
    degrees A,B;
};

/**
\class ballnoseTool
\brief This class is for the ballnose tool.
It was created first because the ballnose tool is easy.
*/
class ballnoseTool: public millTool {
  public:
    const TopoDS_Shape& get3d();
    ballnoseTool(double diameter, double length);
};
/**
\class cylindricalTool
\brief This class is for the standard endmill.
*/
class cylindricalTool: public millTool {
  public:
    const TopoDS_Shape& get3d();
    cylindricalTool(double diameter, double length);
};

/**
\class latheTool
\brief A 2d tool for the lathe.
This class is incomplete. Furthermore, at this time g2m only supports milling.
*/
class latheTool: public tool {
  public:
    latheTool();
};

#endif //TOOL_HH
