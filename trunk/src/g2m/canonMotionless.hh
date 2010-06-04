#ifndef CANONMOTIONLESS_HH
#define CANONMOTIONLESS_HH

#include <string>
#include <vector>

#include <cmath>
#include <limits.h>

/**
\class canonMotionless
\brief A canonical command that (generally) does not cause or alter axis motion
This class is for anything other than LINEAR_FEED, LINEAR_TRAVERSE, and ARC_FEED - including changes in feedrate, spindle speed, tool, coolant, ending the program, etc
*/
class canonMotionless: protected canonLine {
  public:
    canonMotionless(std::string canonL, machineStatus prevStatus);
  protected:
    bool match, handled;
};

#endif //CANONMOTIONLESS_HH
