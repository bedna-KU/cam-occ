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

//occ includes
#include <AIS_InteractiveContext.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>

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
    pathAlgo *Path;
    
public:
    interactive(pathAlgo *pAlg);
    ~interactive();
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
    
protected:
    bool modified;
private:
    vector<occObject> displayedPaths;


};

    
#endif //INTERACTIVE_H
