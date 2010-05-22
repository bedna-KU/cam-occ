#ifndef APTTOOL_H
#define APTTOOL_H

typedef double angleDegrees; //angle in degrees

/*class APTtool
** this generates a tool profile from the classic APT tool description (image
** at http://www.anderswallin.net/2008/02/z-map-model-for-3-axis-machining/ )
** r= major radius, b=minor radius, A=tip angle from perpendicular (WRT tool
** axis), a=dist from tool axis to center of minor radius, B=side angle from
** parallel, h = height of tool 
** profile will be on the XZ plane, symmeteric about Z, the center of the
**  tool's tip will be at the origin, rest of the tool above (+Z) the origin
** always: a+b <= r, A<90,A+B<=90
** standard cylindrical: b=0,a=r,A=0,B=0
** ball nose: b=r,a=r/2,A=0,B=0
** etc
** corner-rounding tools CANNOT be defined with this method, unfortunately
*/
class APTtool {
  private:
    typedef enum { BALLNOSE,  CYLINDRICAL, TOROIDAL, ENGRAVING,
    		   TAPERED,     OTHER,     UNDEFINED            } SHAPE_TYPE;
    SHAPE_TYPE shapeType;
    TopoDS_Face profile;
    double r,a,b, h;
    angleDegrees A,B;
    bool valid;
    void classify();
    void generate();
  protected:
    void failure();
  public:
    APTtool(double rr, double hh); //ballnose
    APTtool(double rr,double aa,double bb,
	    angleDegrees AA,angleDegrees BB, double hh);
    const bool isValid() {return valid;};
    const SHAPE_TYPE getType() {return shapeType;};
    const TopoDS_Face getProfile() {return profile;};
};

#endif //APTTOOL_H