#include <cmath>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include <Precision.hxx>

#include "canonLine.hh"
#include "machineStatus.hh"
#include "canonMotionless.hh"
#include "canonMotion.hh"
#include "linearMotion.hh"
#include "helicalMotion.hh"


//canon.cc - implementation of canonLine

canonLine::canonLine(std::string canonL, machineStatus &prevStatus) {
  status = machineStatus(prevStatus);
  myLine = canonL;
  tokenize(); //splits myLine using delimiters
  //myStart = prevStatus.getStartPose();
  //myEnd = myStart; //override in derivative classes
  
  //getPose(); //can't use this here - need to know what type of line it is
}

//returns the number after N on the line, -1 if none
int canonLine::getN() {
  //std::string s = canonTokens[1];
  if ( (canonTokens[1].c_str()[1] == '.') && (canonTokens[1].c_str()[2] == '.') )
   //if ( (s.c_str[1] == '.') && (s.c_str[2] == '.') )
    return -1;
  else
    return tok2i(1,1);
}

//returns the canon line number
int canonLine::getLineNum() {
  return tok2i(0);
}

//returns the machine's status after execution of this canon line
const machineStatus* canonLine::getStatus() {
  return &status;
}

void canonLine::setToolVecPtr(std::vector<tool> *t) {
  *toolVec = *t;
}

//for LINEAR_* and ARC_FEED, first 3 are always xyz and last 3 always abc
//FIXME: shouldn't this be in class machineStatus?
gp_Ax1 canonLine::getPose() {
  double x,y,z,a,b,c;
  
  //need 3,4,5,and -3,-2,-1
  x = tok2d(3);
  y = tok2d(4);
  z = tok2d(5);
  gp_Pnt p(x,y,z);
  
  uint s = canonTokens.size();
  c = tok2d(s-1);
  b = tok2d(s-2);
  a = tok2d(s-3);
  
  //now how to convert those angles to a unit vector (i.e. gp_Dir)?
  //for now we take the easy way out
  gp_Dir d(0,0,1); //vertical
  assert (a+b+c < 3.0 * Precision::Confusion());
  return gp_Ax1(p,d);
}

//converts canonTokens[n] to double
inline double canonLine::tok2d(uint n) {
  char * end;
  double d = strtod( canonTokens[n].c_str(), &end );
  assert ( *end == 0 );
  return d;  
}

//converts canonTokens[n] to int
inline int canonLine::tok2i(uint n,uint offset) {
  char * end;
  int i = strtol( &canonTokens[n].c_str()[offset], &end, 10 );
  assert ( *end == 0 );
  return i;  
}

const std::string canonLine::getCanonicalCommand() {
  return canonTokens[2];
}

//from http://oopweb.com/CPP/Documents/CPPHOWTO/Volume/C++Programming-HOWTO-7.html
//0 is canon line
//1 is gcode Nnnnnn line
//2 is canonical command
void canonLine::tokenize(std::string str, 
			 std::vector<std::string>& tokenV, 
			 const std::string& delimiters) {
  // Skip delimiters at beginning.
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  std::string::size_type pos     = str.find_first_of(delimiters, lastPos);
  
  while (std::string::npos != pos || std::string::npos != lastPos)
  {
    // Found a token, add it to the vector.
    tokenV.push_back(str.substr(lastPos, pos - lastPos));
    // Skip delimiters.  Note the "not_of"
    lastPos = str.find_first_not_of(delimiters, pos);
    // Find next "non-delimiter"
    pos = str.find_first_of(delimiters, lastPos);
  }
}

inline void canonLine::tokenize() {
  tokenize(myLine,canonTokens);
}

inline bool canonLine::clMatch(string m) {
  return (m.compare(canonTokens[2]) == 0); //compare returns zero for a match
}


canonLine * canonLine::canonLineFactory (std::string l, machineStatus s) {
  //check if canonical command is motion or something else
  //motion commands: LINEAR_TRAVERSE LINEAR_FEED ARC_FEED
  size_t lin,af,cmnt,msg;
  cmnt=l.find("COMMENT");
  msg=l.find("MESSAGE");
  lin=l.find("LINEAR_");
  af=l.find("ARC_FEED");
  /*
  ** check for comments first because it is not impossible
  ** for one to contain the text "LINEAR_" or "ARC_FEED"
  */
  if ( (cmnt!=std::string::npos) || (msg!=std::string::npos) ) {       
    return new canonMotionless(l,s);    
  } else if (lin!=std::string::npos) { //linear traverse or linear feed
    return new linearMotion(l,s);
  } else if (af!=std::string::npos) { //arc feed
    return new helicalMotion(l,s); //arc or helix
  } else { //canonical command is not a motion command
    return new canonMotionless(l,s);
  }
}
