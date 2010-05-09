#ifndef HELICALMOTION_HH
#define HELICALMOTION_HH

#include <string>
#include <vector>
#include <cmath>
#include <limits.h>

#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>

#include "canonMotion.hh"
#include "canonLine.hh"
#include "machineStatus.hh"

/*/for ARC_FEED with motion in only two of XYZ
class arcMotion: protected canonMotion {
  public:
    arcMotion(std::string canonL, machineStatus prevStatus): canonMotion(canonL,prevStatus);
}*/

//for ARC_FEED with motion in all 3 of XYZ (helical move)
class helicalMotion: protected canonMotion {
  public:
    helicalMotion(std::string canonL, machineStatus prevStatus);
  private:
    void helix(gp_Pnt start, gp_Pnt end, gp_Pnt c, gp_Dir dir, int rot);
    //void arc(status.getStartPose().Location(), startVec, status.endPose)
    //void arc(gp_Pnt start, gp_Pnt end, gp_Pnt c, gp_Dir dir, int rot);
    void arc(gp_Pnt start, gp_Vec startVec, gp_Pnt end);
};

#endif //HELICALMOTION_HH
