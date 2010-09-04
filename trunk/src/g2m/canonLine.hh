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
#ifndef CANONLINE_HH
#define CANONLINE_HH

#include <string>
#include <vector>

#include <cmath>
#include <limits.h>

#include "canon.hh"
#include "machineStatus.hh"
#include "tool.hh"
#include "dispShape.hh"

//TODO: add methods to set and get display options for the obj - i.e. color, shading, ...
//so objs with trouble can be highlighted. same for start and end move, rapid, canned cycles(?), ...

enum DISPLAY_MODE { NO_DISP,           //this object is not displayed
                    THIN_MOTION,   //show myUnSolid for canonMotion, and nothing else
                    THIN,               //show myUnSolid for all
                    ONLY_MOTION,        //show myShape for canonMotion, or myUnSolid if solid fails
                    BEST,               //show solid where possible, myUnSolid otherwise
                    ALL                 //show both myUnSolid and myShape
                  };

/**
\class canonLine
\brief A canonLine object represents one canonical command.
Each gcode line produces one or more canonical commands. It can either be a motion command (one of LINEAR_TRAVERSE LINEAR_FEED ARC_FEED), or a motionless command (anything else)
You cannot create objects of this class - instead, create an object of a class that inherits from this class via canonLineFactory()
*/
class canonLine: protected canon {
  public:
    const std::string getLine() {return myLine;};
    const gp_Ax1 getStart() {return status.getStartPose();};
    const gp_Ax1 getEnd() {return status.getEndPose();};
    int getN(); //returns the number after N on the line, -1 if none
    int getLineNum(); //returns the canon line number
    const machineStatus* getStatus(); //returns the machine's status after execution of this canon line
    virtual bool isThisMotion() = 0;
    static canonLine* canonLineFactory (std::string l, machineStatus s);
    //static void setToolVecPtr(std::vector<tool> *t);
    const std::string getCanonType();
    const TopoDS_Shape& getUnSolid() {return myUnSolid;}; //FIXME: use dispShape instead? one obj for both solid and unsolid? throw in highlighting as well?
    virtual const TopoDS_Shape& getShape()=0;
    bool checkErrors() {return errors;};
    void display();
    void setDispMode(DISPLAY_MODE m) {dispMode = m;};
  protected:
    canonLine(std::string canonL, machineStatus prevStatus);
    std::string myLine;
    machineStatus status; //the machine's status *after* execution of this canon line
    std::vector<std::string> canonTokens;
    double tok2d(uint n);
    inline int tok2i(uint n,uint offset=0);
    void tokenize(std::string str, std::vector<std::string>& tokenV,
		  const std::string& delimiters = "(), ");
    inline void tokenize();
    const std::string getCanonicalCommand();
    bool errors;
    DISPLAY_MODE dispMode;

    ///return true if the canonical command for this line matches 'm'
    inline bool cmdMatch(std::string m) {
      if (canonTokens.size() < 3) return false;
        return (m.compare(canonTokens[2]) == 0); //compare returns zero for a match
    };

    /** \var myUnSolid
    Use to store a 2d shape for non-motion commands, or to store the tool path for motion.
    */
    TopoDS_Shape myUnSolid;
};

#endif //CANONLINE_HH
