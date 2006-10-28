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

//#include <Standard_Failure.hxx>
#include <TopoDS.hxx>
//#include <TopoDS_Iterator.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
//#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <gp_Pln.hxx>
//#include <TopTools_SequenceOfShape.hxx>
//#include <BRepProj_Projection.hxx>
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
    void init();


protected:
    //TopoDS_Face F;
    //bool continue_compute;
    //bool canBeComputed;
    //bool computed;
    Standard_Real safeHeight;	//height for rapids
    bool safeHeightSet;

    typedef struct {
	bool computed;
	uint faceNumber;
	TopoDS_Face F;	
    } mFace;  //machinable face - i.e. a face in listOfFaces

    typedef struct {
	bool displayed;
	vector<uint> facesUsed;  //when computed, store each faceNumber (from mFace)
	TopoDS_Shape P;
    } pPass;  //set of lines that were projected at one time, onto one or more faces


public:
    void AddFace(TopoDS_Face &aFace, TopoDS_Shape &theShape);
    vector<mFace> listOfFaces;
    vector<pPass> projectedPasses;   //now, one shape per pass.

public slots:
    //void slotCancel();
    void slotComputeSimplePathOnFace();

signals:
    void showPath();
    void setProgress(int p=-1, char* status="Ready");
private:
    //void projLine(TopoDS_Shape& lines, TopoDS_Face face, gp_Pnt pnt1, gp_Pnt pnt2, gp_Dir pDir);

};

#endif //PATHALGO_H
