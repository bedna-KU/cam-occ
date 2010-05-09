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
typedef enum { ARC, HELIX, LINE } MOTION_TYPE;

class canonMotion: protected canonLine {
  public:
    MOTION_TYPE getMotionType() {return mtype;};
    bool thisIsTraverse() {return isTraverse;};
    virtual const TopoDS_Solid getSolid();
  protected:
    bool isTraverse;
    canonMotion(std::string canonL, machineStatus prevStatus);
    MOTION_TYPE mtype;
    TopoDS_Shape solid;
    TopoDS_Edge edge;
};

#endif //CANONMOTION_HH
