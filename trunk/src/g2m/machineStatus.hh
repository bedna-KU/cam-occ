class machineStatus {
  public:
    machineStatus(machineStatus oldStatus);
    void setPose(gp_Ax1 newPose);
    void setFeed(const double f);
    void setSpindle(const double s); //0=off
    void setCoolant(const char[2] c); //first char for flood, second for mist
    void setTool(millTool *t); //t is the tool to be used
    const double getFeed() {return F;};
    const double getSpindle() {return S;};
    const char[2] getCoolant() {return coolant};
    const gp_Ax1 getPose() {return pose;};
  protected:
    gp_Ax1 pose;
    double F,S;
    char[2] coolant;
}