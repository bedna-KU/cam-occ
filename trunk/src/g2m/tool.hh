#ifndef TOOL_HH
#define TOOL_HH

#include <assert.h>
#include <limits.h>

#include <TopoDS_Face.hxx>
#include <TopoDS_Solid.hxx>

#include "canon.hh"

typedef enum {ROTARY_TOOL,TURNING_TOOL, UNDEFINED} TOOLTYPE;
class tool {
  public:
    const TopoDS_Face getProfile() const {assert (valid);return profile;};
    TOOLTYPE getType() const {return type;};
    bool isValid() const {return valid;};
  protected:
    tool();
    bool isRotaryTool;
    bool valid;
    TopoDS_Face profile;
    TOOLTYPE type; 
};

typedef enum { BALLNOSE, CYLINDRICAL, TOROIDAL, ENGRAVING, TAPERED, OTHER, UNDEF } SHAPE_TYPE;
class millTool: protected tool {
  public:
    //getProfile() = 0;
    SHAPE_TYPE getShape() const {return shape;};
    const TopoDS_Solid getRevol();
  protected:
    millTool();
    SHAPE_TYPE shape;
    double d,l; //diameter, length
    TopoDS_Solid revol;
};

class aptTool: protected millTool {
  public:
    aptTool(double dd,double aa,double bb,
	    degrees AA,degrees BB, double ll);
  private:
    double a,b; //d and l are in millTool
    degrees A,B;
};

class ballnoseTool: protected millTool {
  ballnoseTool(double diameter, double length);
};

class latheTool: public tool {
};

#endif //TOOL_HH
