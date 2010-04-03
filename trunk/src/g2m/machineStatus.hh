typedef enum {CANON_PLANE_XY, CANON_PLANE_YZ, CANON_PLANE_XZ} CANON_PLANE;
typedef struct {bool flood; bool mist; bool spindle} coolantStruct;
//typedef enum {CW,CCW,OFF,BRAKE} SPINDLE_STATUS;
class machineStatus: protected canon {
  public:
    machineStatus(machineStatus oldStatus);
    void setEndPose(gp_Ax1 newPose);
    void setFeed(const double f);
    void setSpindle(const double s); //0=off
    void setCoolant(coolantStruct c) {coolant = c;}; 
    void setTool(millTool *t); //t is the tool to be used
    void setPlane(CANON_PLANE p) {plane = p;};
    const double getFeed() {return F;};
    const double getSpindle() {return S;};
    const coolantStruct getCoolant() {return coolant;};
    const gp_Ax1 getStartPose() {return startPose;};
    const gp_Ax1 getEndPose() {return endPose;};
    const CANON_PLANE getPlane() {return plane;};
  protected:
    gp_Ax1 startPose, endPose;
    double F,S;
    coolantStruct coolant;
    CANON_PLANE plane;
};
