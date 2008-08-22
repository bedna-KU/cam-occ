/*************************************************************
** License: GPL.  
** Copyright (C) 2007 Andreas Kain
*************************************************************/

//basicCADmodule.h

#ifndef BASICCADMODULE_H
#define BASICCADMODULE_H

//Qt
#include <qobject.h>
#include <vector>
#include <qmessagebox.h>
#include <qfiledialog.h>

// openCASCADE
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopLoc_Location.hxx>
#include <Standard_Real.hxx>

#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeBox.hxx>

#include <GC_MakeSegment.hxx>
#include <gp_Pnt.hxx>
#include <math.h>

#include <stdlib.h>
#include <time.h>

//STEP by openCASCADE
#include <STEPControl_Controller.hxx>
#include <STEPControl_Reader.hxx>
#include <STEPControl_Writer.hxx>

#include <TopExp_Explorer.hxx>



class basicCADmodule : public QObject
{
	Q_OBJECT

public:
    basicCADmodule();	
    virtual ~basicCADmodule();
    

    typedef struct {
	gp_Pnt Cell;
	bool Used;
    } maze_Cell;
	  
    typedef struct {
	maze_Cell Cell;
	int Orient;// 1== pos x, 2== neg x, 3 ==pos y, 4== neg y
    } neighbor_Cell;
	  
    bool comparePoints(gp_Pnt a,gp_Pnt b);
    TopoDS_Shape Box0;
    
    
public slots:	
    void slotGenerateCube();
     void slotSaveSTEP();
     
     
signals:
    void setZoom();
    //	from pathAlgo
    void sendShape(TopoDS_Shape shapeGeometry);
    void setProgress(int p=-1, char* status="Ready");   // use it as e.g >>emit setProgress(int(round(100.0*(float)i/(float) listOfFaces.size())),"Adding faces")<< 
    
};
#endif //BASICCADMODULEHH
