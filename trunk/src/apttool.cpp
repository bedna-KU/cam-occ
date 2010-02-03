#include apttool.h

APTtool::APTtool() {
  valid = false;
  r=a=b=h=0.0;
  A=B=0.0;
  shapeType = UNDEFINED;
}
//generate ball nose tool with radius rr
APTtool::APTtool(double rr, double hh) {
  r = rr, h = hh;
  Handle(Geom_TrimmedCurve) Tc;
  Tc = GC_MakeArcOfCircle (gp_Pnt(rr,0,r), gp_Pnt(0,0,0), gp_Pnt(-r,0,r));
  TopoDS_Edge Ec = BRepBuilderAPI_MakeEdge(Tc);
  TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(gp_Pnt(r,0,r), gp_Pnt(r,0,h));
  TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(gp_Pnt(-r,0,r), gp_Pnt(-r,0,h));
  TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(gp_Pnt(-r,0,h), gp_Pnt(r,0,h));
  BRepBuilderAPI_MakeWire wm(Ec,E1,E2,E3);
  //assert( wm.IsDone() );
  TopoDS_Wire w = wm.Wire();
  //assert( w.Closed() );
  BRepBuilderAPI_MakeFace f(w);
  //assert (f.IsDone());
  profile = f.Face();
  bool valid = true;
  shapeType = BALLNOSE;
}

APTtool::APTtool(double rr,double aa,double bb,
		 angleDegrees AA,angleDegrees BB, double hh) {
  r = rr, a = aa, b = bb;
  A = AA, B = BB, h = hh;
  classify();
  if (!valid) {
    //tool creation failure
  } else {
    //generate the shape
  }
}

/*
** classify()
** figure out what the tool is, based on the parameters
** side effect: valid=true if the parameters are ok, otherwise false
** this is incomplete, really need to come up with a generic formula
** useful (?) code to find tangency at
**   $CASROOT/../samples/standard/java/src/SampleGeometryPackage/
**   SampleGeometryPackage.cxx:2026 and :2106
*/
void APTtool::classify() {
  const double small = Precision::Confusion();
  valid = false;
  if( h < 4.0 * r) return; //too short
  if( a+b > r ) return; //makes no sense
  if( A >= 90.0 ) return; //ditto
  if( A+B > 90.0 ) return; //ditto
  if( (A < small) && (B < small) ) { //no angles
    if( a < small ) { 
      if( abs(b - r/2.0) < small ) {
	shapeType = BALLNOSE;
	valid = true;
      }
      return;
    } else if( (abs(a-r) < small) && (b < small) ) {
      shapeType = CYLINDRICAL;
      valid = true;
    }
  } 
}