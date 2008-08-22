/*************************************************************
** License: GPL.  
** Derived from work by Olivier Coma. 
** Copyright (C) 2006 Mark Pictor
*************************************************************/

//interactive.h -- AIS

#ifndef INTERACTIVE_H
#define INTERACTIVE_H

//qt includes
#include <qapplication.h>
#include <qobject.h>
#include <qsplitter.h>
//#include <qlistview.h>
//#include <qvbox.h>
//#include <qhgroupbox.h>
//#include <qpushbutton.h>
//#include <qtoolbutton.h>
#include <qprogressbar.h>
//#include <qtabwidget.h>
#include <qstatusbar.h>
#include <qaction.h>
#include "mainui.h"

//occ includes
#include <AIS_InteractiveContext.hxx>
#include <BRepAdaptor_Curve.hxx>
//#include <BRepTools_Modifier.hxx>
//#include <BRepTools_TrsfModification.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <gp_Circ.hxx>
#include <gp_Lin.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <gp_Trsf.hxx>

//app includes
#include "occObject.h"
#include "ImportExport.h"
#include "pathAlgo.h"
#include "camTabs.h"
#include "basicCADmodule.h"
class occview;


class interactive : public QObject
{
    Q_OBJECT
public:
    Handle_AIS_InteractiveContext myContext;
    occObject Part;
    //occObject tracedPath;
    QString myGeomFile;
    bool computed;
    bool opened;

    QSplitter *splitter;
    QVBox *leftFrame;
    QProgressBar *pBar;
    camTabs *camT;

    mainui *mainIntf;
    pathAlgo *Path;
    basicCADmodule *CAD;

    interactive(mainui *mui, QSplitter *qs, pathAlgo *pAlg, basicCADmodule *bCmod);
    ~interactive();
    
    TopoDS_Shape importedShape;
    
    void setupFrame();
    bool newInteract();
      bool newMaze();
    void loadPart(const QString& filename);
    bool isModified() const;
    void initContext(occview*);
    void initPart();
    void initPath();
    void getFaceEdges();
    
    
signals:
    void documentChanged(bool erase=true, bool axo=false);
    void hasSelected(bool);
    //void clickFace(const TopoDS_Face& F, bool edge);
    //void clickEdge(const TopoDS_Edge& E);
    void getFace(TopoDS_Face& F);
    void getEdges(TopTools_SequenceOfShape& edges);
    void transferShape(TopoDS_Shape aShape);
    
    
public slots:
    void slotSelectionChanged();
    //void slotEditTool();
    void slotShowPath();
    void slotOrientWorkpiece();
    void slotSetProgress(int,char*); //progress bar, statusbar

    void slotSetDisplayedPathFalse();
    
    ////////////////////////////local context
    void slotNeutral();
    void slotVertexSelection();
    void slotEdgeSelection();
    void slotFaceSelection();
    
    ///////////////////////////color, shading, wireframe
    void slotCasColor();
    void slotCasShading();
    void slotCasWireframe();
    void slotCasRMat();
    void slotCasTransparency();
    
    void slotShowCAD(TopoDS_Shape shapeGeometry);
    
    void slotTransferShape();//AK

protected:
    bool modified;
private:
    void usrRadiusMessage(TopoDS_Edge E);
    void usrVertexMessage (TopoDS_Vertex V);
    vector<occObject> displayedPaths;
};

    
#endif //INTERACTIVE_H
