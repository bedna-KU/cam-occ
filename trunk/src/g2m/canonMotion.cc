//for LINEAR_TRAVERSE, LINEAR_FEED, ARC_FEED
#include <string>


#include "canonMotion.hh"
#include "canonLine.hh"

canonMotion::canonMotion(std::string canonL, machineStatus prevStatus): canonLine(canonL,prevStatus) {

}

