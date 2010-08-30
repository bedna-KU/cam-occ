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
#ifndef MACHINESTATUS_HH
#define MACHINESTATUS_HH

//#include <map>

#include "tool.hh"
#include "canon.hh"

typedef enum {CANON_PLANE_XY, CANON_PLANE_YZ, CANON_PLANE_XZ} CANON_PLANE;
typedef struct {bool flood; bool mist; bool spindle;} coolantStruct;
typedef enum {OFF,CW,CCW,BRAKE} SPINDLE_STATUS;

/**
\class machineStatus
\brief This class contains the machine's state for one canonical command.
The information stored includes the coolant state, spindle speed and direction, feedrate, start and end pose, tool in use
*/
class machineStatus: protected canon {
  protected:
    gp_Ax1 startPose, endPose;
    double F,S;  //feedrate, spindle speed
    SPINDLE_STATUS spindleStat;
    coolantStruct coolant;
    static millTool* theTool;
//    toolNumber myTool;
/*    / ** \var toolTable
      for now, it is for millTool objs only
    */
    //static std::map<toolNumber, millTool, std::less<toolNumber> > toolTable;
    CANON_PLANE plane;
  public:
    machineStatus(machineStatus const& oldStatus);
    machineStatus(gp_Ax1 initial);
    void setPrevStatus(const machineStatus &oldStatus);
    void setEndPose(gp_Ax1 newPose) {endPose = newPose;};
    void setEndPose(gp_Pnt p);
    void setFeed(const double f) {F=f;};
    void setSpindleSpeed(const double s) {S=s;};
    void setSpindleStatus(SPINDLE_STATUS s);
    void setCoolant(coolantStruct c) {coolant = c;};
    //void setTool(toolNumber n); //n is the tool to be used
    void setTool(uint n); //n is the size of the tool to be used. FIXME: implement tool table stuff
    void setPlane(CANON_PLANE p) {plane = p;};
    double getFeed() const {return F;};
    double getSpindleSpeed() const {return S;};
    SPINDLE_STATUS getSpindleStatus() const {return spindleStat;};
    const coolantStruct getCoolant() {return coolant;};
    const gp_Ax1 getStartPose() {return startPose;};
    const gp_Ax1 getEndPose() {return endPose;};
    CANON_PLANE getPlane() const {return plane;};
    void clearAll(void);
    millTool* getTool() {return theTool;};
    //const millTool& getTool() {return toolTable.find(myTool)->second;};
  private:
    machineStatus();  //prevent use of this ctor by making it private
};
#endif //MACHINESTATUS_HH
