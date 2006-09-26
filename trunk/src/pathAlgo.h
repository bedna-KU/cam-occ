/*************************************************************
** License: GPL.  
** Derived from work by Olivier Coma. 
** Copyright (C) 2006 Mark Pictor
*************************************************************/

//pathAlgo.h

//algorithms for computing the toolpath
//3-axis ONLY (at least for now)
#include "interactive.h"

#ifndef PATHALGO_H
#define PATHALGO_H

#include <qobject.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <vector>

#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <BRepProj_Projection.hxx>
#include <Bnd_Box.hxx>
#include <BndLib_AddSurface.hxx>
#include <Geom_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRep_Tool.hxx>


//#include <BRepAdaptor_Curve.hxx>
//#include <BRepAdaptor_Curve2d.hxx>
//class interactive;

class pathAlgo : public QObject
{
    Q_OBJECT

public:
    pathAlgo();
    virtual ~pathAlgo();
    vector<TopoDS_Face> listOfFaces;
    vector<TopoDS_Edge> projectedLines;


protected:
    TopoDS_Face F;
    bool continue_compute;
    bool computed;
    Handle_AIS_InteractiveContext Context;
    Standard_Real safeHeight;	//height for rapids
    bool safeHeightSet;

public:
    void SetFace(TopoDS_Face &aFace);
    TopoDS_Shape& GetFace() {return F;};
    void init();
    //void FreeA_Coord();
    void SetContext(Handle_AIS_InteractiveContext &C) {Context=C;};
    void outputProtoCode();

public slots:
    void slotCancel();
    void slotComputeSimplePathOnFace();
    void slotOutputProtoCode();
signals:
    void showPath();


};

#endif //PATHALGO_H
