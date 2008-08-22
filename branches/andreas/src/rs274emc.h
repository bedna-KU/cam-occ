/*************************************************************
** License: GPL.  
** Derived from work by Mark Pictor. 
** Copyright (C) 2007 Andreas Kain
*************************************************************/

//rs274emc.h

//output emc-compatible g-code (and POSSIBLY import it as well)

#ifndef RS274EMC_H
#define RS274EMC_H

//qt inlcudes
#include <qapplication.h>//AK
#include <qobject.h>
#include <qsplitter.h>//AK
#include <qmessagebox.h>
#include <qfiledialog.h>

//occ inlcudes
#include <vector>

#include <TopLoc_Location.hxx>
#include <TopExp.hxx>
#include <TopTools_ListOfShape.hxx>

#include <Standard_Real.hxx>
#include <gp_Pln.hxx>
#include <Bnd_Box.hxx>
#include <BndLib_AddSurface.hxx>
#include <Geom_Curve.hxx>
#include <TopExp_Explorer.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>

#include <BRep_Tool.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepAdaptor_Surface.hxx>

// from pathAlgo
#include "mainui.h"//AK
#include "pathAlgo.h"


class rs274emc : public QObject
{
	Q_OBJECT

public:
    rs274emc(pathAlgo *pAlg);//AK
    virtual ~rs274emc();//virtual added by AK
    pathAlgo *Path;
    bool staticVaultComputed(int switcher);//AK
    bool comparePoints(gp_Pnt a,gp_Pnt b);//AK
    void initPath();//AK
    
    
public slots://added by AK
    void slotOutputProtoCode();  //this is temporary and will be replaced
    //AK+++++++++++++++++++++++
    void slotGetPathComputed(int);
    void slotGetSafeHeight(Standard_Real);
    void slotGetFeed(int);
    void slotGetSpeed(int);
    void slotGetToolDia(int);
    //+++++++++++++++++++++++AK
    
    
public:
    void linesToIntermediateCode();
    void optimizeIntermediateCode();
    void intermediateToNC();
    void initStaticVault();//AK
    
    
private:
    Standard_Real safeHeight;
    int feed;
    int speed;
    int toolDia;
    
    
signals://from pathAlgo
    //AK+++++++++++++++++++++++++++++++++
    void sendPathComputed(bool pathComputed);
    void setProgress(int p=-1, char* status="Ready");
    // use it as e.g >>emit setProgress(int(round(100.0*(float)i/(float) listOfFaces.size())),"Adding faces")<<
    //maybe use a signal for return values AK
    //++++++++++++++++++++++++++++++++++AK

};
#endif //RS274EMC_H
