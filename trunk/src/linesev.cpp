#include linesev.h

//gcode2ModelGc.cpp:132+
//
linesev::linesev(gp_Pnt end, string str, machineState &state) {
  myState = state;
  myStart = myState.position;
  myEnd = end;
  if (canon_line.contains("FEED"))
    edge.motion = FEED;
  else
    edge.motion = TRAVERSE;
  edge.shape = LINE;
  
}

bool linesev::computeEdge() {
  assert(!goodEdge);
  TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(start,end)
}

bool linesev::computeSolid() {
  assert(goodEdge);
}

