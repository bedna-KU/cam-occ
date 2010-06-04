#include "machineStatus.hh"

//implement machineStatus class
//GPL

machineStatus::machineStatus(const machineStatus &oldStatus) {
	//TODO
	#warning incomplete
	spindleStat = oldStatus.spindleStat;
    F = oldStatus.F;
    S = oldStatus.S;
    coolant = oldStatus.coolant;
    plane = oldStatus.plane;
    endPose = startPose = oldStatus.endPose;
}

machineStatus::clearAll() {
  F=S=0.0;
  plane = CANON_PLANE_XY;
  coolant.flood = false;
  coolant.mist = false;
  coolant.spindle = false;
  endPose = startPose = gp_Ax1(0,0,0,0,0,0);
  spindleStat = OFF;
}

void machineStatus::setEndPose(gp_Ax1 newPose) {
  endPose = newPose;
}

void machineStatus::setEndPose(gp_Pnt p) {
  endPose = gp_Ax1( p, gp_Dir(0,0,1) );
}
