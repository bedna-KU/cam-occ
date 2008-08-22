/*************************************************************
** License: GPL.  
** Copyright (C) 2007 Andreas Kain
*************************************************************/

//selectPatch.h

#ifndef SELECTPATCH_H
#define SELECTPATCH_H

// Qt
#include <qobject.h>
#include <vector>
#include <qmessagebox.h>
#include <qfiledialog.h>

//openCASCADE
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopLoc_Location.hxx>

#include <BRep_Tool.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAdaptor_Surface.hxx>

#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>

#include <TopExp_Explorer.hxx>
#include <Standard_Real.hxx>
#include <gp_Pnt.hxx>
#include <GC_MakeSegment.hxx>

#include <BRepBndLib.hxx>
#include <BndLib_AddSurface.hxx>
#include <Bnd_Box.hxx>

#include <math.h>


class selectPatch: public QObject
{
	Q_OBJECT

public:
    selectPatch();
    virtual ~selectPatch();	 
    
    vector<TopoDS_Face> Faces; // store selected faces from pocket: bottom faces
    vector<TopoDS_Face> FacesSorted; // store selected faces in proper order, subsequently to apply chunks	
    TopoDS_Shape basicShape;
	  
    void sortFaces();	  
    bool comparePoints(gp_Pnt a,gp_Pnt b);
	
public slots:
    void slotGetPatches(TopoDS_Shape aShape);  	
    void slotActivateFaceTransfer();
	
signals:
    //	from pathAlgo.h
     void sendAddFace(TopoDS_Face aFace);
     void setProgress(int p=-1, char* status="Ready");
    // use it as e.g >>emit setProgress(int(round(100.0*(float)i/(float) listOfFaces.size())),"Adding faces")<<
};
#endif //SELECTPATCH_H
