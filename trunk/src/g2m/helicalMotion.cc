#include <string>
#include <climits>

#include <Precision.hxx>
#include <Handle_Geom_CylindricalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <GC_MakeArcOfCircle.hxx>
//#include <TopoDS.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <GCE2d_MakeSegment.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <TopoDS_Edge.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>

#include "helicalMotion.hh"
#include "machineStatus.hh"

//implements helicalMotion
helicalMotion::helicalMotion(std::string canonL, machineStatus prevStatus): canonMotion(canonL,prevStatus) {
  //part of processCanonLine
  //} else if (canon_line.startsWith( "ARC_FEED(" )) {
 gp_Dir arcDir;
 gp_Pnt c;
 double x,y,z,a1,a2,e1,e2,e3,ea,eb,ec,hdist;
 int rot=0;
 x=y=z=a1=a2=e1=e2=e3=ea=eb=ec=0;
 
 //edge.start = last;
 /* example output, starting from x0 y-1 z0 a0 b0 c0, command g02x1y0i0j1
 (names interleaved, see emc's saicanon.cc, line 497)
 8  N..... ARC_FEED(1.000000, 0.000000,    0.000000, 0.000000, 
 line Gline ARC_FEED(first_end, second_end, first_axis, second_axis,
 -1,        0.000000,   0.000000, 0.000000, 0.000000)
 rotation, axis_end_point,   a,        b,        c)
 */
 e1  = tok2d(3); //first_end
 e2  = tok2d(4); //second_end
 a1 = tok2d(5); //first_axis
 a2 = tok2d(6); //second_axis
 rot = tok2d(7); //rotation (ccw if rot==1,cw if rot==-1)
 e3 = tok2d(8); //axis_end_point
 ea = tok2d(9); //a
 eb = tok2d(10); //b
 ec = tok2d(11); //c
 switch (status.getPlane()) {
   /* 
   ** the order for these vars is copied from saicannon.cc, line 509+ 
   ** a,b,c are untouched - yay! 
   */
   case CANON_PLANE_XZ:
     status.setEndPose(gp_Pnt(e2,e3,e1)); 
     arcDir = gp_Dir(0,1,0);
     c = gp_Pnt(a2,status.getStartPose().Location().Y(),a1);
     hdist = e3 - status.getStartPose().Location().Y();
     break;
   case CANON_PLANE_YZ:
     status.setEndPose(gp_Pnt(e3,e1,e2));
     arcDir = gp_Dir(1,0,0);
     c = gp_Pnt(status.getStartPose().Location().X(),a1,a2);
     hdist = e3 - status.getStartPose().Location().X();
     break;
   case CANON_PLANE_XY:
   default:
     status.setEndPose(gp_Pnt(e1,e2,e3));
     arcDir = gp_Dir(0,0,1);
     c = gp_Pnt(a1,a2,status.getStartPose().Location().Z());
     hdist = e3 - status.getStartPose().Location().Z();
 }
 //last = edge.end;
 //skip arc if zero length; caught this bug thanks to tort.ngc
 if (status.getStartPose().Location().Distance(status.getEndPose().Location()) > Precision::Confusion()) {
   //center is c; ends are edge.start, edge.last
   if (fabs(hdist) > 0.000001) {
     helix(status.getStartPose().Location(), status.getEndPose().Location(), c, arcDir,rot);
     mtype = HELIX;
   } else {
     gp_Vec Vr = gp_Vec(c,status.getStartPose().Location());	//vector from center to start
     gp_Vec Va = gp_Vec(arcDir);		//vector along arc's axis
     gp_Vec startVec = Vr^Va;		//find perpendicular vector using cross product
     if (rot==1) startVec *= -1;
     //cout << "Arc with vector at start: " << toString(startVec).toStdString();
     arc(status.getStartPose().Location(), startVec, status.getEndPose().Location());
     mtype = ARC;
   }
   isTraverse = false;
   /*
   FIXME - rewrite or copy the old funcs into new src
   chkEdgeStruct check = checkEdge(feedEdges, feedEdges.size()-1);
   if (check.startGap != 0.0) {
     exit(-1); //what SHOULD we do here?!
   }
   if (check.endGap != 0.0) {
     last = check.realEnd;
     feedEdges.back().end = last;
     if (check.endGap > 100.0*Precision::Confusion()) {
       cout << " with center " << toString(c).toStdString();
       if (mtype == HELIX) cout << " and arcDir " << toString(arcDir).toStdString();
       cout << " from " << toString(status.getStartPose().Location()).toStdString() << " to " << toString(status.getEndPose).toStdString() << endl;
       cout << "params:  e1:"<< e1 <<"  e2:" << e2 <<"  a1:"<< a1 <<"  a2:"<< a2 <<"  rot:" << rot <<"  ep:" << ep << endl;
     }
   }
   */
 } else cout << "Skipped zero-length arc." << endl;
}

void helicalMotion::helix( gp_Pnt start, gp_Pnt end, gp_Pnt c, gp_Dir dir, int rot ) {
  Standard_Real pU,pV;
  Standard_Real radius = start.Distance(c);
  gp_Pnt2d p1,p2;
  Handle(Geom_CylindricalSurface) cyl = new Geom_CylindricalSurface(gp_Ax2(c,dir) , radius);
  GeomAPI_ProjectPointOnSurf proj;
  TopoDS_Edge h;
  int success = 0;
  
  h.Nullify();
  //cout << "Radius " << radius << "   Rot has the value " << rot << endl;
  proj.Init(start,cyl);
  if(proj.NbPoints() > 0) {
    proj.LowerDistanceParameters(pU, pV);
    if(proj.LowerDistance() > 1.0e-6 ) {
      //cout << "Point fitting distance " << double(proj.LowerDistance()) << endl;
    }
    success++;
    p1 = gp_Pnt2d(pU,pV);
  }
  
  proj.Init(end,cyl);
  if(proj.NbPoints() > 0) {
    proj.LowerDistanceParameters(pU, pV);
    if(proj.LowerDistance() > 1.0e-6 ) {
      //cout << "Point fitting distance " << double(proj.LowerDistance()) << endl;
    }
    success++;
    p2 = gp_Pnt2d(pU,pV);
  }
  
  if (success != 2) {
   /* FIXME
   cout << "Couldn't create a helix from " << toString(start).toStdString() << " to " << toString(end).toStdString() << ". Replacing with a line." <<endl;
   */
    errors=true;
    edge = BRepBuilderAPI_MakeEdge( start, end );
    return;
  }
  
  //for the 2d points, x axis is about the circumference.  Units are radians.
  //change direction if rot = 1, not if rot = -1
  //if (rot==1) p2.SetX((p1.X()-p2.X())-2*M_PI); << this is wrong!
  //cout << "p1x " << p1.X() << ", p2x " << p2.X() << endl;
  
  //switch direction if necessary, only works for simple cases
  //should always work for G02/G03 because they are less than 1 rotation
  if (rot==1) {
    p2.SetX(p2.X()-2*M_PI);
    //cout << "p2x now " << p2.X() << endl;
  }
  Handle(Geom2d_TrimmedCurve) segment = GCE2d_MakeSegment(p1 , p2);
  edge = BRepBuilderAPI_MakeEdge(segment , cyl);
  
  return;
}

void helicalMotion::arc(gp_Pnt start, gp_Vec startVec, gp_Pnt end) {
    Handle(Geom_TrimmedCurve) Tc;
    Tc = GC_MakeArcOfCircle ( start, startVec, end );
    edge = BRepBuilderAPI_MakeEdge ( Tc );
}
