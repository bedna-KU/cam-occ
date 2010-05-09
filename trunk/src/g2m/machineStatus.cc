#include "machineStatus.hh"

//implement machineStatus class
//GPL

machineStatus::machineStatus(const machineStatus &oldStatus) {
	//TODO
	#warning unimplemented
}

void machineStatus::setEndPose(gp_Ax1 newPose) {
  startPose = newPose;
}

void machineStatus::setEndPose(gp_Pnt p) {
  startPose = gp_Ax1( p, gp_Dir(0,0,1) );
}
