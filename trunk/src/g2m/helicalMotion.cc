/***************************************************************************
*   Copyright (C) 2010 by Mark Pictor                                     *
*   mpictor@gmail.com                                                     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
#include <string>
#include <climits>

#include <Precision.hxx>
#include <Handle_Geom_CylindricalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <GCE2d_MakeSegment.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>

#include "helicalMotion.hh"
#include "machineStatus.hh"
#include "uio.hh"

helicalMotion::helicalMotion(std::string canonL, machineStatus prevStatus): canonMotion(canonL,prevStatus) {

  gp_Pnt start, end;
  start = status.getStartPose().Location();
  //end must be set after determining which plane is in use

  gp_Dir arcDir;
  gp_Pnt c;
  double x,y,z,a1,a2,e1,e2,e3,ea,eb,ec,hdist;
  int rot=0;
  x=y=z=a1=a2=e1=e2=e3=ea=eb=ec=0;

  e1  = tok2d(3); //first_end
  e2  = tok2d(4); //second_end
  a1 = tok2d(5); //first_axis
  a2 = tok2d(6); //second_axis
  rot = tok2d(7); //rotation (ccw if rot==1,cw if rot==-1)
  e3 = tok2d(8); //axis_end_point
  ea = tok2d(9); //a
  eb = tok2d(10); //b
  ec = tok2d(11); //c
  /// Shuffle variables around based on the active plane.
  switch (status.getPlane()) {
    /*
    ** the order for these vars is copied from saicannon.cc, line 509+
    ** a,b,c remain in order
    */
    case CANON_PLANE_XZ:
      status.setEndPose(gp_Pnt(e2,e3,e1));
      arcDir = gp_Dir(0,1,0);
      c = gp_Pnt(a2,start.Y(),a1);
      hdist = e3 - start.Y();
      break;
    case CANON_PLANE_YZ:
      status.setEndPose(gp_Pnt(e3,e1,e2));
      arcDir = gp_Dir(1,0,0);
      c = gp_Pnt(start.X(),a1,a2);
      hdist = e3 - start.X();
      break;
    case CANON_PLANE_XY:
    default:
      status.setEndPose(gp_Pnt(e1,e2,e3));
      arcDir = gp_Dir(0,0,1);
      c = gp_Pnt(a1,a2,start.Z());
      hdist = e3 - start.Z();
  }
  end = status.getEndPose().Location();

  if (start.Distance(end) < Precision::Confusion()) { //skip arc if zero length; caught this bug thanks to tort.ngc
    cout << "Skipped zero-length arc at N" << getN() << endl;
    status.setEndDir(status.getPrevEndDir());
  } else {
    cout << myLine; //endl supplied by helix() or arc()
    if (fabs(hdist) > 0.000001) {
      /// Create a helix if the third-axis delta is > .000001.
      helix(start, end, c, arcDir,rot);
    } else {
      /// Otherwise, create an arc.
      gp_Vec Vr = gp_Vec(c,start);	//vector from center to start
      gp_Vec Va = gp_Vec(arcDir);		//vector along arc's axis
      gp_Vec startVec = Vr^Va;		//find perpendicular vector using cross product
      if (rot==1) startVec *= -1;
      //cout << "Arc with vector at start: " << uio::toString(startVec) << endl;
      arc(start, startVec, end);
    }
    // FIXME - check for gaps

    /// Find vector direction at start and end of the edge, and save them in status
    double f,l,fd,ld;
    gp_Pnt fp,lp;
    gp_Vec fv,lv;

    BRepAdaptor_Curve bac(TopoDS::Edge(myUnSolid));
    f = bac.FirstParameter();
    l = bac.LastParameter();
    bac.D1(f,fp,fv);
    bac.D1(l,lp,lv);

    fd = fp.SquareDistance(start);
    ld = lp.SquareDistance(start);
    if ( fd > ld ) { //compare the distances to decide which one is at the start
      //"first" end (fd) is farther from the starting point, so
      status.setEndDir(fv);          //use first vector at end
      status.setStartDir(lv);        //and use last vector at start
    } else {                         //other way around
      status.setEndDir(lv);
      status.setStartDir(fv);
    }
  }
}


/// Create a helix, place it in myUnSolid
void helicalMotion::helix( gp_Pnt start, gp_Pnt end, gp_Pnt c, gp_Dir dir, int rot ) {
  cout << "helix" << endl;
  Standard_Real pU,pV;
  Standard_Real radius = start.Distance(c);
  gp_Pnt2d p1,p2;
  Handle(Geom_CylindricalSurface) cyl = new Geom_CylindricalSurface(gp_Ax2(c,dir) , radius);
  GeomAPI_ProjectPointOnSurf proj;
  int success = 0;

  //myUnSolid.Nullify();
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
    //cout << "Couldn't create a helix from " << uio::toString(start) << " to " << uio::toString(end)  << ". Replacing with a line." <<endl;

    errors=true;
    myUnSolid = BRepBuilderAPI_MakeEdge( start, end );
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
  myUnSolid = BRepBuilderAPI_MakeEdge(segment , cyl);

  return;
}

/// Create an arc, place it in myUnSolid
void helicalMotion::arc(gp_Pnt start, gp_Vec startVec, gp_Pnt end) {
  cout << "arc" << endl;
  Handle(Geom_TrimmedCurve) Tc;
  Tc = GC_MakeArcOfCircle ( start, startVec, end );
  myUnSolid = BRepBuilderAPI_MakeEdge ( Tc );
}
