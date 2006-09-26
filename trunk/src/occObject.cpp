/*************************************************************
** License: GPL.  
** Derived from work by Olivier Coma. 
** Copyright (C) 2006 Mark Pictor
*************************************************************/

//occObject.cpp

#include "occObject.h"

occObject::occObject()
{

}

occObject::~occObject()
{

}

void occObject::SetColor(Quantity_NameOfColor aColor)
{
	Context->SetColor(AisShape, aColor);
}

void occObject::SetMaterial(Graphic3d_NameOfMaterial aName)
{
	Context->SetMaterial(AisShape, aName);
}

void occObject::Display()
{
	if (!AisShape.IsNull())
		Context->Display(AisShape);// ,1,-1,Standard_False,Standard_False);
	Context->Display(Trihedron);// ,1,-1,Standard_False,Standard_False);
}

void occObject::Erase()
{
	if (!AisShape.IsNull())
		Context->Erase(AisShape);
	Context->Erase(Trihedron);
}

void occObject::SetOrigin(const gp_Pnt &p)
{
	Trsf.SetTranslation(gp_Pnt(0, 0, 0), p);
	Axis=Trsf;

	Context->SetLocation(AisShape, Trsf);
	Context->SetLocation(Trihedron, Trsf);
}

void occObject::SetTrsf(	double r11, double r12, double r13, double t1,
								double r21, double r22, double r23, double t2,
								double r31, double r32, double r33, double t3)
{

	Trsf.SetValues(	r11, r12, r13, t1,
					r21, r22, r23, t2,
					r31, r32, r33, t3, 1e-10, 1e-10);

	Context->SetLocation(AisShape, Trsf);
	Context->SetLocation(Trihedron, Trsf);
}

void occObject::SetTrsf(const gp_Trsf &T)
{
	Axis=Trsf=T;

	Context->SetLocation(AisShape, Trsf);
	Context->SetLocation(Trihedron, Trsf);
}

void occObject::Transform(double r11, double r12, double r13, double t1, double r21, double r22, double r23, double t2,double r31, double r32, double r33, double t3)
{
	gp_Trsf T;
	T.SetValues(	r11, r12, r13, t1,
			r21, r22, r23, t2,
			r31, r32, r33, t3, 1e-10, 1e-10);
	Axis=T*Trsf;

	Context->SetLocation(AisShape, T*Trsf);
	Context->SetLocation(Trihedron, T*Trsf);
}

void occObject::Transform(const gp_Trsf &T)
{
	Axis=T*Trsf;
	Context->SetLocation(AisShape, T*Trsf);
	Context->SetLocation(Trihedron, T*Trsf);
}

void occObject::ResetLocation()
{
	Context->SetLocation(AisShape, Trsf);
//	Context->ResetLocation(AisShape);	//a masquer avec CAS4.0
}

void occObject::SetShape(TopoDS_Shape S)
{
	Shape=S;
	AisShape = new AIS_Shape (Shape);
	Trihedron = new AIS_Trihedron(new Geom_Axis2Placement(gp::XOY()));
}

void occObject::Init()
{
	gp_Trsf T;
	Transform(T);
}
