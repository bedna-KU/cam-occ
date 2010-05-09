#ifndef MACHINESTATUS_HH
#define MACHINESTATUS_HH

#include "tool.hh"
#include "canon.hh"

typedef enum {CANON_PLANE_XY, CANON_PLANE_YZ, CANON_PLANE_XZ} CANON_PLANE;
typedef struct {bool flood; bool mist; bool spindle;} coolantStruct;
//typedef enum {CW,CCW,OFF,BRAKE} SPINDLE_STATUS;

//class canon;
class machineStatus: protected canon {
  public:
    machineStatus(const machineStatus &oldStatus);
    machineStatus();
    void setPrevStatus(const machineStatus &oldStatus);
    
    void setEndPose(gp_Ax1 newPose);
    void setEndPose(gp_Pnt p);
    void setFeed(const double f);
    void setSpindle(const double s); //0=off
    void setCoolant(coolantStruct c) {coolant = c;}; 
    void setTool(millTool *t); //t is the tool to be used
    void setPlane(CANON_PLANE p) {plane = p;};
    double getFeed() const {return F;};
    double getSpindle() const {return S;};
    const coolantStruct getCoolant() {return coolant;};
    const gp_Ax1 getStartPose() {return startPose;};
    const gp_Ax1 getEndPose() {return endPose;};
    CANON_PLANE getPlane() const {return plane;};
  protected:
    gp_Ax1 startPose, endPose;
    //gp_Pnt startPose, endPose;
    double F,S;  //feedrate, spindle speed
    coolantStruct coolant;
    CANON_PLANE plane;
};

#endif //MACHINESTATUS_HH
