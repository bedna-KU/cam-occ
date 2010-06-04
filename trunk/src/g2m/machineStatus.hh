#ifndef MACHINESTATUS_HH
#define MACHINESTATUS_HH

#include "tool.hh"
#include "canon.hh"

typedef enum {CANON_PLANE_XY, CANON_PLANE_YZ, CANON_PLANE_XZ} CANON_PLANE;
typedef struct {bool flood; bool mist; bool spindle;} coolantStruct;
typedef enum {CW,CCW,OFF,BRAKE} SPINDLE_STATUS;

/**
\class machineStatus
\brief This class contains the machine's state for one canonical command.
Class machineStatus stores the state of the machine - i.e. coolant, spindle speed and direction, feed speed, start and end pose, 
*/
class machineStatus: protected canon {
  public:
    machineStatus(const machineStatus &oldStatus);
    machineStatus();
    void setPrevStatus(const machineStatus &oldStatus);
    
    void setEndPose(gp_Ax1 newPose) {endPose = newPose;};
    void setEndPose(gp_Pnt p);
    void setFeed(const unsigned double f) {F=f;};
    void setSpindleSpeed(const unsigned double s) {S=s;};
    void setSpindleStatus(SPINDLE_STATUS s);
    void setCoolant(coolantStruct c) {coolant = c;}; 
    void setTool(millTool *t); //t is the tool to be used
    void setPlane(CANON_PLANE p) {plane = p;};
    unsigned double getFeed() const {return F;};
    unsigned double getSpindleSpeed() const {return S;};
    SPINDLE_STATUS getSpindleStatus() const {return spindleStat;};
    inline bool spindleIsOn() {return spindleOn};
    const coolantStruct getCoolant() {return coolant;};
    const gp_Ax1 getStartPose() {return startPose;};
    const gp_Ax1 getEndPose() {return endPose;};
    CANON_PLANE getPlane() const {return plane;};
  protected:
    gp_Ax1 startPose, endPose;
    //gp_Pnt startPose, endPose;
    unsigned double F,S;  //feedrate, spindle speed
    SPINDLE_STATUS spindleStat;
    coolantStruct coolant;
    CANON_PLANE plane;
};

#endif //MACHINESTATUS_HH
