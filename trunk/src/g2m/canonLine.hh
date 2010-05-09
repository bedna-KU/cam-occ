#ifndef CANONLINE_HH
#define CANONLINE_HH

#include <string>
#include <vector>

#include <cmath>
#include <limits.h>

#include "canon.hh"
#include "machineStatus.hh"
#include "tool.hh"

class canonLine: protected canon {
  public:
    const std::string getLine() {return myLine;};
    const gp_Ax1 getStart() {return status.getStartPose();};
    const gp_Ax1 getEnd() {return status.getEndPose();};
    int getN(); //returns the number after N on the line, -1 if none
    int getLineNum(); //returns the canon line number
    const machineStatus* getStatus(); //returns the machine's status after execution of this canon line
    bool isThisMotion() {return isMotion;};
    static canonLine* canonLineFactory (std::string l, machineStatus s);
    static void setToolVecPtr(std::vector<tool> *t);
    const std::string getCanonType();
    const TopoDS_Shape getUnSolid();
    bool checkErrors() {return errors;};
  protected:
    canonLine(std::string canonL, machineStatus prevStatus);
    std::string myLine;
    gp_Ax1 myStart,myEnd;
    machineStatus status; //the machine's status *after* execution of this canon line
    bool isMotion;
    static std::vector<tool> *toolVec;
    std::vector<std::string> canonTokens;
    inline double tok2d(uint n);
    inline int tok2i(uint n,uint offset=0);
    void tokenize(std::string str, std::vector<std::string>& tokenV,
		  const std::string& delimiters = "(), ");
    inline void tokenize();
    gp_Ax1 getPose();
    const std::string getCanonicalCommand();
    bool errors;

};

#endif //CANONLINE_HH
