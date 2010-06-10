#ifndef CANONMOTION_HH
#define CANONMOTION_HH

#include <string>
#include <vector>
#include <cmath>
#include <limits.h>

#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>

#include "canonLine.hh"

//for LINEAR_TRAVERSE, LINEAR_FEED, ARC_FEED
typedef enum { HELICAL, LINEAR, RAPID } MOTION_TYPE;
/**
\class canonMotion
\brief Class canonMotion is for the three canonical motion commands
canonMotion is for the canonical commands LINEAR_TRAVERSE, LINEAR_FEED, and ARC_FEED
canonMotion can only be instantiated via canonLine::canonLineFactory().
canonLine::canonLineFactory() creates linearMotion objects for LINEAR_TRAVERSE and LINEAR_FEED commands, and helicalMotion objects for ARC_FEED commands
*/
class canonMotion: protected canonLine {
  public:
    virtual MOTION_TYPE getMotionType() {};
    bool thisIsTraverse() {return isTraverse;};
    virtual const TopoDS_Solid getSolid() {};
    bool isThisMotion() {return true;};
  protected:
    //bool isTraverse;
    canonMotion(std::string canonL, machineStatus prevStatus);
    TopoDS_Shape solid;
    TopoDS_Edge edge;
};

#endif //CANONMOTION_HH
