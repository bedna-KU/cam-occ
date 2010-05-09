//linearmotion.cc
#include <string>
#include "linearMotion.hh"
#include "machineStatus.hh"
#include "canonMotion.hh"

linearMotion::linearMotion(std::string canonL, machineStatus prevStatus): canonMotion(canonL,prevStatus) {
  mtype = LINE;
  //FIXME isTraverse =

}
