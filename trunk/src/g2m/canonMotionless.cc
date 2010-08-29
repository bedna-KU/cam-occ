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
#include "canonMotionless.hh"
#include "uio.hh"
#include <string>
#include <BRepBuilderAPI_MakeVertex.hxx>

canonMotionless::canonMotionless(std::string canonL, machineStatus prevStatus):canonLine(canonL, prevStatus) {
  match = true;
  handled = true;
  status.setEndPose(status.getStartPose());
  errors = false;
  myUnSolid = BRepBuilderAPI_MakeVertex(status.getStartPose().Location());

  //match canonical commands. the string MUST be the complete command name
  if (clMatch("COMMENT")) {
    //do nothing
  } else if (clMatch("MESSAGE")) {
    infoMsg("Message: " + canonTokens[3]);
  //} else if (clMatch("SPINDLE")) {
  //} else if (clMatch("MIST")) {
  //} else if (clMatch("ENABLE")) {
  //} else if (clMatch("FLOOD")) {
  } else if (clMatch("DWELL")) {
  //} else if (clMatch("FEEDRATE")) {
  } else if (clMatch("SET_FEED_RATE")) {
    handled = false;
  } else if (clMatch("SET_FEED_REFERENCE")) {
    handled = false;
  } else if (clMatch("SELECT_TOOL")) {
    handled = false;
  } else if (clMatch("CHANGE_TOOL")) {
    //for now, always assume it's ballnose. divide tool number by 16 to get diameter
    //i.e. tool 1 = 1/16" ball nose endmill, tool 24 = 1 1/2" ball nose endmill
    //TODO: implement tool table stuff
    //toolNumber n = tok2i(3);
    status.setTool(tok2i(3));
    handled = false;
  } else if (clMatch("USE_TOOL_LENGTH_OFFSET")) {
    handled = false;
  } else if (clMatch("SET_ORIGIN_OFFSETS")) {

  if (clMatch("(0.0000,")) {
    infoMsg("Warning, input has reduced precision - expected more zeros: \n" + myLine );
  }
    handled = false; //because I still don't know what to do if we have the correct data...
  } else if (clMatch("USE_LENGTH_UNITS")) {
    handled = false;
  } else if (clMatch("SET_MOTION_CONTROL_MODE")) {
    handled = false;
  } else if (clMatch("SET_XY_ROTATION")) {
    handled = false;
  } else if (clMatch("SET_FEED_REFERENCE")) {
    handled = false;
  } else if (clMatch("SET_NAIVECAM_TOLERANCE")) {
    handled = false;
  } else if (clMatch("PROGRAM_END")) {
  } else if (clMatch("PROGRAM_STOP")) {
  } else if (clMatch("SELECT_PLANE(" )) {
    if (clMatch("XZ)")) {
      status.setPlane(CANON_PLANE_XZ);
    } else if (clMatch("YZ)")) {
      status.setPlane(CANON_PLANE_YZ);
    } else {// XY)
      status.setPlane(CANON_PLANE_XY);
    }
  } else match = false;

  if ( !match || !handled ) {
    std::string m;
    if (!handled) {
      m = "Warning, unhandled";
    } else {
      m = "Error, unknown";
      infoMsg(m + " canonical command: " + canonL);
    }
  }
}

 /*
 SET_ORIGIN_OFFSETS(0.0000,  //this is a common canon statement. we are going to hijack it to produce a warning, because
 //the data we're getting was produced with a format of %.4f or so
 infoMsg(QString("Warning, input has reduced precision, expected more zeros: <br>") + canon_line );
 */

 /*
 bool clMatch(string m) {
 return (m.compare(canonTokens[2]) == 0); //compare returns zero for a match
  }
  */
