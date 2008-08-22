/*************************************************************
** License: GPL.  
** Derived from work by Olivier Coma. 
** Copyright (C) 2006 Mark Pictor
*************************************************************/

//occObject.h

//#include <qobject.h>
#include "positionWorkpieceDlg.h"

#include <AIS_Trihedron.hxx>
#include <Geom_Axis2Placement.hxx>
#include <gp.hxx>
#include <gp_Ax2.hxx>
#include <gp_Trsf.hxx>
#include <AIS_InteractiveContext.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS_Shape.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <AIS_Shape.hxx>
#include <vector>

class occObject //: public QObject
{
  //Q_OBJECT

protected:
	TopoDS_Shape Shape;
	Handle_AIS_Shape AisShape;
	Handle_AIS_Trihedron Trihedron;
	gp_Trsf Trsf;		//position of origin in the assembly (TR)
	gp_Trsf Axis;		//position reelle (TR actual position(?))
	Handle_AIS_InteractiveContext Context;

	//next three lines are for dialog box
	double rx,ry,rz,tx,ty,tz,mult,ratio;
	uint cadU,camU;
	bool mCheck;

public:
	occObject();
	virtual ~occObject();

	void SetContext(Handle_AIS_InteractiveContext &C) {Context=C;};
	void SetColor(Quantity_NameOfColor aColor);
	void SetMaterial(Graphic3d_NameOfMaterial aName);
	void SetTransparency(float trans);
	void Display(bool t = false);
	void Erase();
	void Init();

	void SetShape(TopoDS_Shape S);
	TopoDS_Shape& GetShape() {return Shape;};
	Handle_AIS_Shape& GetAisShape() {return AisShape;};
	gp_Trsf& GetAxis() {return Axis;};

	void SetOrigin(const gp_Pnt &p);
	void SetTrsf(	double r11, double r12, double r13, double t1,
			double r21, double r22, double r23, double t2,
			double r31, double r32, double r33, double t3);
	void SetTrsf(const gp_Trsf &T);
	void Transform(	double r11, double r12, double r13, double t1,
			double r21, double r22, double r23, double t2,
			double r31, double r32, double r33, double t3);
	void Transform(const gp_Trsf &T);
	gp_Trsf& GetTrsf() {return Trsf;};
	void ResetLocation();
	void trsfDlg();
/*
private:
	//saveable defaults for dlg (implement???)
	double defaultMult;
	uint defaultCadU, defaultCamU;
*/
};
