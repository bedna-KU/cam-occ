/*************************************************************
** License: GPL.  
** Derived from work by Olivier Coma. 
** Copyright (C) 2006 Mark Pictor
*************************************************************/

//interactive.h -- AIS

#ifndef INTERACTIVE_H
#define INTERACTIVE_H

//qt includes
#include <qobject.h>
#include <qsplitter.h>
#include <qlistview.h>
#include <qvbox.h>
#include <qprogressbar.h>
#include <qtabwidget.h>
#include <qstatusbar.h>
#include <qaction.h>
#include "mainui.h"

//occ includes
#include <AIS_InteractiveContext.hxx>
//#include <BRepAlgoAPI_Fuse.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <Graphic3d_NameOfMaterial.hxx>

//app includes
#include "occObject.h"
#include "ImportExport.h"
#include "pathAlgo.h"

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
    QTabWidget *tabWidget;
    QListView *faceView;
    QListView *pathView;
    QListView *passView;
    QListView *toolView;
    mainui *mainIntf;
    pathAlgo *Path;

    interactive(mainui *mui, QSplitter *qs, pathAlgo *pAlg);
    ~interactive();
    void setupFrame();
    bool newInteract();
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
    
    
public slots:
    void slotSelectionChanged();
    //void slotEditTool();
    void slotShowPath();
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

protected:
    bool modified;
private:
    vector<occObject> displayedPaths;
};

    
#endif //INTERACTIVE_H
