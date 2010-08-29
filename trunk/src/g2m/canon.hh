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
#ifndef CANON_HH
#define CANON_HH

#include <string>
#include <iostream>

#include <cmath>
#include <limits.h>

#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax1.hxx>
#include <TopoDS_Shape.hxx>

typedef double degrees; //angle in degrees
typedef int toolNumber;

/**
\class canon
\brief Class canon was created so canonLine and machineStatus could easily share some things.
The only shared items are the unimplemented function abc2dir, and the types degrees and toolNumber. At this time, the list of tools will be handled within the machineStatus class.
*/

class canon {
  public:
    canon();
  protected:
    static const gp_Dir abc2dir(double a, double b, double c);
    void infoMsg(std::string s) {cout << s << endl;};


};


#endif //CANON_HH



 //create tool obj
// toolTable[t] = tool(t);

//access tool obj
///need to check if the tool exists before accessing it!!!
// status.myTool = toolTable[t].getShape();
