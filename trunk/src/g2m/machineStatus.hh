typedef enum {CANON_PLANE_XY, CANON_PLANE_YZ, CANON_PLANE_XZ} CANON_PLANE;
class machineStatus {
  public:
    machineStatus(machineStatus oldStatus);
    void setEndPose(gp_Ax1 newPose);
    void setFeed(const double f);
    void setSpindle(const double s); //0=off
    void setCoolant(const char[2] c); //first char for flood, second for mist
    void setTool(millTool *t); //t is the tool to be used
    void setPlane(CANON_PLANE p) {plane = p;};
    const double getFeed() {return F;};
    const double getSpindle() {return S;};
    const char[2] getCoolant() {return coolant;};
    const gp_Ax1 getStartPose() {return startPose;};
    const gp_Ax1 getEndPose() {return endPose;};
    const CANON_PLANE getPlane() {return plane;};
  protected:
    gp_Ax1 startPose, endPose;
    double F,S;
    char[2] coolant;
    CANON_PLANE plane;
}