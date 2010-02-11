#include "canon.hh"
//canon.cc - implementation of classes in canon.hh

canonLine::canonLine(string canonL, machineStatus prevStatus) {
  status(prevStatus);
  myLine = canonL;
  tokenize(); //splits myLine using delimiters
  myStart = prevStatus.getPose();
  myEnd = myStart; //override in derivative classes
  
  //getPose(); //can't use this here - need to know what type of line it is
}

//returns the number after N on the line, -1 if none
const int canonLine::getN() {
  if ( (tokens[1].c_str[1] == '.') && (tokens[1].c_str[2] == '.') )
    return -1;
  else
    return tok2i(1,1);
}

//returns the canon line number
const int canonLine::getLineNum() {
  return tok2i(0);
}

//returns the machine's status after execution of this canon line
const machineStatus* canonLine::getStatus() {
  return *status;
}

static canonLine::setToolVecPtr(std::vector<millTool> *t) {
  *toolVec = *t;
}

//for LINEAR_* and ARC_FEED, first 3 are always xyz and last 3 always abc
static gp_Ax1 canonLine::getPose() {
  double x,y,z,a,b,c;
  
  //need 3,4,5,and -3,-2,-1
  x = tok2d(3);
  y = tok2d(4);
  z = tok2d(5);
  gp_Pnt p(x,y,z);
  
  uint s = tokens.size();
  c = tok2d(s-1);
  b = tok2d(s-2);
  a = tok2d(s-3);
  
  //now how to convert those angles to a unit vector (i.e. gp_Dir)?
  //for now we take the easy way out
  gp_Dir d(0,0,1); //vertical
  assert (a+b+c < 3.0 * Precision::Confusion());
  return gp_Ax1(p,d);
}

//converts tokens[n] to double
const inline double canonLine::tok2d(uint n) {
  double d = strtod( tokens[n], &end );
  assert ( *end == 0 );
  return d;  
}

//converts tokens[n] to int
const inline int canonLine::tok2i(uint n,uint offset=0) {
  int i = strtoi( tokens[n].c_str()[offset], &end );
  assert ( *end == 0 );
  return i;  
}

const string canonLine::getCanonicalCommand() {
  return tokens[2];
}

//from http://oopweb.com/CPP/Documents/CPPHOWTO/Volume/C++Programming-HOWTO-7.html
//0 is canon line
//1 is gcode Nnnnnn line
//2 is canonical command
void canonLine::tokenize(const string& delimiters = "(), ") {
  // Skip delimiters at beginning.
  string::size_type lastPos = myLine.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  string::size_type pos     = myLine.find_first_of(delimiters, lastPos);
  
  while (string::npos != pos || string::npos != lastPos)
  {
    // Found a token, add it to the vector.
    tokens.push_back(myLine.substr(lastPos, pos - lastPos));
    // Skip delimiters.  Note the "not_of"
    lastPos = myLine.find_first_not_of(delimiters, pos);
    // Find next "non-delimiter"
    pos = myLine.find_first_of(delimiters, lastPos);
  }
}

static canonLine * canonLine::canonLineFactory (string l, machineStatus s) {
  //check if canonical command is motion or something else
  //motion commands: LINEAR_TRAVERSE LINEAR_FEED ARC_FEED
  size_t lin,af,cmnt;
  cmnt=l.find("COMMENT");
  lin=l.find("LINEAR_");
  af=l.find("ARC_FEED");
  if (cmnt!=string::npos) { 		//check for comments first because one
    return new canonMotionless(l,s);	//might contain LINEAR_ or ARC_FEED
  } else if (lin!=string::npos) { //linear traverse or linear feed
    return new linearMotion(l,s);
  } else if (af!=string::npos) { //arc feed, may be a helix
    size_t c,p;
    c = l.find_last_of(",");     //find last comma and the )
    p = l.find_last_of(")");
    //get the number between, compare to Z in state
    double z = atod(l.substr(c+1,p-c));
    if (abs(z-s.getPos().Location().Z()) < 10*Precision::Confusion()) {
      return new arcMotion(l,s); //z is same, use arc
    } else {
      return new helicalMotion(l,s); //z is not same, need a helix
    }
  } else { //canonical command is not a motion command
    return new canonMotionless(l,s);
  }
}
