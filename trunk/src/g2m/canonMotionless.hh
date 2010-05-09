#ifndef CANONMOTIONLESS_HH
#define CANONMOTIONLESS_HH

#include <string>
#include <vector>

#include <cmath>
#include <limits.h>

//for anything that doesn't cause axis motion - i.e. changes in feedrate, spindle speed, tool, coolant, etc
class canonMotionless: protected canonLine {
  public:
    canonMotionless(std::string canonL, machineStatus prevStatus);
    //TODO: remember to set myStart,myEnd - or overload the func's
};

#endif //CANONMOTIONLESS_HH
