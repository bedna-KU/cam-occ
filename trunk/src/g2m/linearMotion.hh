#ifndef LINEARMOTION_HH
#define LINEARMOTION_HH

#include <string>
#include <vector>

#include <cmath>
#include <limits.h>
#include "canonMotion.hh"
#include "machineStatus.hh"

//for LINEAR_TRAVERSE, LINEAR_FEED
class linearMotion: protected canonMotion {
  public:
    linearMotion(std::string canonL, machineStatus prevStatus);
    ///MOTION_TYPE getMotionType() {return LINEAR;};
    //need to return RAPID for rapids...
};

#endif //LINEARMOTION_HH
