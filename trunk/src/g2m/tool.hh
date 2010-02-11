typedef enum {ROTARY_TOOL,TURNING_TOOL, UNDEFINED} TOOLTYPE;
class tool {
  public:
    const TopoDS_Face getProfile() {assert (valid);return profile;};
    const TOOLTYPE getType() {return type;};
    const bool isValid() {return valid;};
  protected:
    bool isRotaryTool;
    bool valid;
    TopoDS_Face profile;
    TOOLTYPE type; 
}

typedef enum { BALLNOSE, CYLINDRICAL, TOROIDAL, ENGRAVING, TAPERED, OTHER, UNDEFINED } SHAPE_TYPE;
class millTool: protected tool {
  public:
    //getProfile() = 0;
    const SHAPE_TYPE getShape() {return shape;};
    const TopoDS_Solid getRevol();
  protected:
    SHAPE_TYPE shape;
    double d,l; //diameter, length
    TopoDS_Solid revol;
}

class aptTool: protected millTool {
  public:
    aptTool(double dd,double aa,double bb,
	    degrees AA,degrees BB, double ll);
  private:
    double a,b; //d and l are in millTool
    degrees A,B;
}

class ballnoseTool: protected millTool {
  ballnoseTool(double diameter, double length);
}

class latheTool: public tool {
}