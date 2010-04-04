
#include mrsev-abc.h
canonAux::canonAux(int l=-1, int n=-1, string canonStr = "") {
  L=l;
  N=n;
  str=canonStr;
}

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
  startV.Nullify();
  endV.Nullify();
  edge.Nullify();
  solid.Nullify(); 
}

mrsev::~mrsev() {
}

TopoDS_Edge mrsev::getEdge() {
  assert(goodEdge);
  return edge;
}

gp_Pnt mrsev::getStart() {
  assert(goodEdge);
  return start;
}

gp_Pnt mrsev::getEnd() {
  assert(goodEdge);
  return end;
}

gp_Vec mrsev::getStartVec() {
  assert(goodEdge);
  return startV;
}

gp_Vec mrsev::getEndVec() {
  assert(goodEdge);
  return endV;
}

TopoDS_Solid getSolid() {
  assert(goodEdge);
  assert(goodSolid);
  return solid;
}