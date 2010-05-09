#ifndef CANON_HH
#define CANON_HH

#include <string>
#include <vector>

#include <cmath>
#include <limits.h>

#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax1.hxx>
#include <TopoDS_Shape.hxx>
typedef double degrees; //angle in degrees

class canon {
  public:
    static const gp_Dir abc2dir(double a, double b, double c);
};


#endif //CANON_HH
