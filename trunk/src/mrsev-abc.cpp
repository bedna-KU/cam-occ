
#include mrsev-abc.h

mrsev::mrsev() {
  /*
  ** tool = new APTtool;
  ** state = new machineState;
  ** canonAux = new canonAux;
  */
  goodEdge = false;
  goodSolid = false;
  start.Nullify();
  end.Nullify();
  edge.Nullify();
  solid.Nullify(); 
}

mrsev::~mrsev() {
}

bool mrsev::computeEdge() {
}

TopoDS_Edge mrsev::getEdge() {
}

gp_Pnt mrsev::getStart() {
}

gp_Pnt mrsev::getEnd() {
}

gp_Vec mrsev::getStartVec() {
}

gp_Vec mrsev::getEndVec() {
}

bool mrsev::computeSolid() {
}

TopoDS_Solid getSolid() {
}