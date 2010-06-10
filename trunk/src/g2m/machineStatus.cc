#include "machineStatus.hh"

//implement machineStatus class
//GPL

machineStatus::machineStatus(machineStatus const& oldStatus) {
	spindleStat = oldStatus.spindleStat;
    F = oldStatus.F;
    S = oldStatus.S;
    coolant = oldStatus.coolant;
    plane = oldStatus.plane;
    endPose = startPose = oldStatus.endPose;
}

/**
\fn machineStatus(gp_Ax1 start)
This constructor is only to be used when initializing the simulation; it would not be useful elsewhere.
\param start is the initial pose of the machine, as determined by the interp from the variable file.
*/
machineStatus::machineStatus(gp_Ax1& start) {
  clearAll();
  startPose = start;
}

void machineStatus::clearAll() {
  F=S=0.0;
  plane = CANON_PLANE_XY;
  coolant.flood = false;
  coolant.mist = false;
  coolant.spindle = false;
  endPose = startPose = gp_Ax1(gp_Pnt(0,0,0),gp_Dir(0,0,0));
  spindleStat = OFF;
}

void machineStatus::setEndPose(gp_Pnt p) {
  endPose = gp_Ax1( p, gp_Dir(0,0,1) );
}

