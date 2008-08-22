/*************************************************************
** License: GPL.  
** Derived from work by Olivier Coma. 
** Copyright (C) 2006 Mark Pictor
*************************************************************/


#ifndef OCCVIEW_H
#define OCCVIEW_H

#include "interactive.h"

#include <qtimer.h>
#include <qwidget.h>
#include <qpen.h>
#include <qobject.h>

#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <Xw_Window.hxx>
#include <Graphic3d_GraphicDevice.hxx>


#include <X11/Xlib.h>
#include <unistd.h>

enum CurrentAction3d {
  CurAction3d_Nothing,
  CurAction3d_DynamicZooming,
  CurAction3d_DynamicPanning,
  CurAction3d_DynamicRotation
};

class occview : public QWidget
{
    Q_OBJECT
public:
    occview(QWidget *parent=0, interactive *interac=0);
    ~occview();
    void init();
    virtual void paintEvent ( QPaintEvent * );
    virtual void resizeEvent ( QResizeEvent * );
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    
public:
    interactive *interact;
    Handle_Graphic3d_GraphicDevice myDevice;
    Handle_Xw_Window myWindow;
    Handle_V3d_Viewer myViewer;
    Handle_V3d_View myView;
    bool hasSelected;
    
    CurrentAction3d      myCurrentMode;
    Standard_Integer     myXmin;
    Standard_Integer     myYmin;
    Standard_Integer     myXmax;
    Standard_Integer     myYmax;
    Quantity_Factor      myCurZoom;
    Standard_Boolean     myDegenerateModeIsOn;
    
    
signals:
    void selectionChanged();
  
    
protected slots:
    void slotDocumentChanged(bool erase, bool axo);
    void slotHasSelected(bool b);

    void slotCasZoomAll();
    void slotCasDynZoom();

    void slotCasPan();

    void slotCasAxo();
    void slotCasFront();
    void slotCasTop();
    void slotCasLeft();
    void slotCasBack();
    void slotCasRight();
    void slotCasBottom();

    void slotCasRotate();
    void slotCasReset();

    void slotCasHlrOff();
    void slotCasHlrOn();
    void slotRedrawResize();
		
};

#endif //OCCVIEW_H	
