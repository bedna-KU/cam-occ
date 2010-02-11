typedef double degrees; //angle in degrees

class canonLine {
  public:
    const string getLine() {return myLine;};
    const gp_Ax1 getStart() {return myStart;};
    const gp_Ax1 getEnd() {return myEnd;};
    const int getN(); //returns the number after N on the line, -1 if none
    const int getLineNum(); //returns the canon line number
    const machineStatus* getStatus(); //returns the machine's status after execution of this canon line
    const bool isThisMotion() {return isMotion};
    static canonLine * canonLineFactory (string l);
    static setToolVecPtr(std::vector<millTool> *t);
    const string getCanonType();
    const TopoDS_Shape getUnSolid();
  protected:
    canonLine(string canonL, machineStatus prevStatus);
    string myLine;
    gp_Ax1 myStart,myEnd;
    machineStatus status; //the machine's status *after* execution of this canon line
    bool isMotion;
    static std::vector<tool> *toolVec;
    vector<string> tokens;
    const inline double tok2d(uint n);
    const inline int tok2i(uint n,uint offset=0);
    void tokenize(const string& delimiters = "(),");
}

//for LINEAR_TRAVERSE, LINEAR_FEED, ARC_FEED
typedef enum { ARC, HELIX, LINE } MOTION_TYPE;
class canonMotion:protected canonLine {
  public:
    const MOTION_TYPE getMotionType() {return mtype;};
    const bool thisIsTraverse() {return isTraverse};
    const TopoDS_Solid getSolid();
  protected:
    bool isTraverse;
    canonMotion(string canonL, machineStatus prevStatus): canonLine(canonL,prevStatus);
    MOTION_TYPE mtype;
}

//for LINEAR_TRAVERSE, LINEAR_FEED
class linearMotion: protected canonMotion {
  public:
    linearMotion(string canonL, machineStatus prevStatus): canonMotion(canonL,prevStatus);
}

//for ARC_FEED with motion in only two of XYZ
class arcMotion: protected canonMotion {
  public:
    arcMotion(string canonL, machineStatus prevStatus): canonMotion(canonL,prevStatus);
}

//for ARC_FEED with motion in all 3 of XYZ (helical move)
class helicalMotion: protected canonMotion {
  public:
    helicalMotion(string canonL, machineStatus prevStatus): canonMotion(canonL,prevStatus);
}

//for anything that doesn't cause axis motion - i.e. changes in feedrate, spindle speed, tool, coolant, etc
class canonMotionless: protected canonLine {
  public:
    canonMotionless(string canonL, machineStatus prevStatus);
    //TODO: remember to set myStart,myEnd - or overload the func's
}

