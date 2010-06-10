#ifndef CANON_HH
#define CANON_HH

#include <string>
#include <map>

#include <cmath>
#include <limits.h>

#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax1.hxx>
#include <TopoDS_Shape.hxx>

typedef double degrees; //angle in degrees
typedef int toolNumber;

/**
\class canon
\brief Class canon was created so canonLine and machineStatus could easily share some things.
This class was created so that canonLine and machineStatus could easily share tool data and any functions that they have in common (at the moment, the only function is the unimplemented function abc2dir)
Tool data could be considered a sparse array, so a std::map<> is used to store it. Tools should be loaded from file the first time they are needed, so that the toolTable does not take up more memory than necessary.
*/

class canon {
  public:
    canon();
  protected:
    static const gp_Dir abc2dir(double a, double b, double c);
    //std::vector<tool> toolVec; /* don't need a map<>  */ //yes we do 
    static std::map<toolNumber, tool, std::less<toolNumber> > toolTable;


};


#endif //CANON_HH


 
 //create tool obj
// toolTable[t] = tool(t);
 
//access tool obj
///need to check if the tool exists before accessing it!!!
// status.myTool = toolTable[t].getShape();
