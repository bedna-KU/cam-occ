#ifndef MACHINESTATUS_HH
#define MACHINESTATUS_HH

#include "tool.hh"
#include "canon.hh"

typedef enum {CANON_PLANE_XY, CANON_PLANE_YZ, CANON_PLANE_XZ} CANON_PLANE;
typedef struct {bool flood; bool mist; bool spindle;} coolantStruct;
typedef enum {OFF,CW,CCW,BRAKE} SPINDLE_STATUS;

/**
\class machineStatus
\brief This class contains the machine's state for one canonical command.
Class machineStatus stores the state of the machine - i.e. coolant, spindle speed and direction, feedrate, start and end pose, tool in use
*/
class machineStatus: protected canon {
  protected:
    gp_Ax1 startPose, endPose;
    double F,S;  //feedrate, spindle speed
    SPINDLE_STATUS spindleStat;
    coolantStruct coolant;
    toolNumber myTool;
    CANON_PLANE plane;
  public:
    machineStatus(machineStatus const& oldStatus);
    machineStatus(gp_Ax1& start);
    void setPrevStatus(const machineStatus &oldStatus);
    void setEndPose(gp_Ax1 newPose) {endPose = newPose;};
    void setEndPose(gp_Pnt p);
    void setFeed(const double f) {F=f;};
    void setSpindleSpeed(const double s) {S=s;};
    void setSpindleStatus(SPINDLE_STATUS s);
    void setCoolant(coolantStruct c) {coolant = c;}; 
    void setTool(toolNumber n) {myTool = n}; //n is the tool to be used
    void setPlane(CANON_PLANE p) {plane = p;};
    double getFeed() const {return F;};
    double getSpindleSpeed() const {return S;};
    SPINDLE_STATUS getSpindleStatus() const {return spindleStat;};
    const coolantStruct getCoolant() {return coolant;};
    const gp_Ax1 getStartPose() {return startPose;};
    const gp_Ax1 getEndPose() {return endPose;};
    CANON_PLANE getPlane() const {return plane;};
    void clearAll(void);
    
    //TODO: choose one of these
    //const tool* getTool() {return toolTable[myTool];};
    //const toolNumber getTool() {return myTool;};

};

#endif //MACHINESTATUS_HH
