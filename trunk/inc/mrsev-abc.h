#ifndef MRSEV_ABC_H
#define MRSEV_ABC_H


/*class machineState
** Stores the machine's state at one moment in time.
** feedrate
** spindle speed
** etc
*/
class machineState {
  public:
    double feedrate, spindleSpeed;
};

/*class canonAux
** auxiliary info about the current canon line 
** the string (n/a if not using SAI)
** the canon line number
** gcode line number (Nxxxxxxx) or -1 if not present
*/
class canonAux {
  private:
    string str;
    int L,N;
  public:
    canonAux(int l=-1, int n=-1, string canonStr = "") {L=l;N=n;str=canonStr;};
    void setString(const string l){canonStr = l;};
    void setCanonLine(const int n){L=n;}; //nth canon command
    void setGcodeLine(const int n){N=n;}; //N-number from the gcode file
    const string getString() {return canonStr;};
    const int getCanonLine() {return l;};
    const int getGcodeLine() {return N;};
};

/*class mrsev
** Abstract base class for MRSEVs, Material Removal Shape Element Volumes.
** Name taken from an NIST paper. Do not expect this to implement all MRSEVs
** defined in that paper, and the implementation won't necessarily be analogous
** to the NIST concept of MRSEVs.
** At this time, the only MRSEVs implemented are for three canonical commands: 
** straight lines (STRAIGHT_FEED, STRAIGHT_TRAVERSE) and arcs and helices 
** (both from ARC_FEED) whose axis is parallel to X, Y, or Z
** certain arcs and helices may not be implemented for certain tool shapes.
*/
class mrsev {
  public:
    mrsev();
    const bool computeEdge() = 0; //returns true on success
    //NOTE all public functions below depend on the bool goodEdge == true
    //NOTE (which is set by computeEdge)
    const TopoDS_Edge getEdge(); //returns the computed edge
    const gp_Pnt getStart(); //start point of the edge
    const gp_Pnt getEnd(); //end point of the edge
    const gp_Vec getStartVec(); //vector at start
    const gp_Vec getEndVec(); //vector at end
    const bool computeSolid() = 0; //computes the solid WITHOUT the "endcaps", returns true for success
    //NOTE all public functions below depend on the bool goodSolid == true
    //NOTE (which is set by computeSolid) AND goodEdge == true
    const TopoDS_Shape getSolid(); //returns the 3d solid
    //more functions that depend on goodSolid & goodEdge?
  protected:
    APTtool tool;
    machineState state;
    canonAux aux;
    bool goodEdge,goodSolid;
    gp_Pnt start,end;
    TopoDS_Edge edge;
    TopoDS_Solid solid;
};

#endif //MRSEV_ABC_H
