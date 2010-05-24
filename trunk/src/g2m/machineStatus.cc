#include "machineStatus.hh"

//implement machineStatus class
//GPL

machineStatus::machineStatus(const machineStatus &oldStatus) {
	//TODO
	#warning unimplemented
}

void machineStatus::setEndPose(gp_Ax1 newPose) {
  endPose = newPose;
}

void machineStatus::setEndPose(gp_Pnt p) {
  endPose = gp_Ax1( p, gp_Dir(0,0,1) );
}
