/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/


/*************************************************************
** License: GPL.  
** Derived from work by Olivier Coma. 
** Copyright (C) 2006 Mark Pictor
*************************************************************/
#include <qapp.h>
#include <qstatusbar.h>
#include <qmessagebox.h>


#include "interactive.h"
#include "occview.h"
#include "pathAlgo.h"


QSplitter *splitter;
occview *oview;
interactive *interact;
pathAlgo *pathAlg;
int viewIsSetup=0;

//set up QT connections.
void mainui::initActions() {
    //connect(fileNewAction, SIGNAL(activated()), this,SLOT(slotFileNew()));
    //connect(_Action, SIGNAL(activated()), _,SLOT(slot_()));
    connect(viewPerspectiveAxonometericAction, SIGNAL(activated()), oview,SLOT(slotCasAxo()));
    connect(viewZoomFit_AllAction, SIGNAL(activated()), oview,SLOT(slotCasZoomAll()));
    connect(viewPerspectiveTopAction, SIGNAL(activated()), oview,SLOT(slotCasTop()));
    connect(viewPerspectiveBottomAction, SIGNAL(activated()), oview,SLOT(slotCasBottom()));
    connect(viewPerspectiveFrontAction, SIGNAL(activated()), oview,SLOT(slotCasFront()));
    connect(viewPerspectiveBackAction, SIGNAL(activated()), oview,SLOT(slotCasBack()));
    connect(viewPerspectiveLeftAction, SIGNAL(activated()), oview,SLOT(slotCasLeft()));
    connect(viewPerspectiveRightAction, SIGNAL(activated()), oview,SLOT(slotCasRight()));
    connect(viewZoomDynamic_ZoomAction, SIGNAL(activated()), oview,SLOT(slotCasDynZoom()));
    //connect(viewZoomZoom_BoxAction, SIGNAL(activated()), oview,SLOT(slotCasZoomBox()));
    connect(viewZoomDynamic_PanningAction, SIGNAL(activated()), oview,SLOT(slotCasPan()));
    //connect(viewZoomGlobal_PanningAction, SIGNAL(activated()), oview,SLOT(slotCasGlobalPan()));
    connect(viewZoomDynamic_RotationAction, SIGNAL(activated()), oview,SLOT(slotCasRotate()));
    connect(viewZoomResetAction, SIGNAL(activated()), oview,SLOT(slotCasReset()));
    connect(viewRenderWireframeAction, SIGNAL(activated()), interact,SLOT(slotCasWireframe()));
    connect(viewRenderShadedAction, SIGNAL(activated()), interact,SLOT(slotCasShading()));
    connect(viewRenderTransparencyAction, SIGNAL(activated()), interact,SLOT(slotCasTransparency()));
    connect(viewRenderColorAction, SIGNAL(activated()), interact,SLOT(slotCasColor()));
    connect(viewRenderMaterialAction, SIGNAL(activated()), interact,SLOT(slotCasRMat()));
    connect(viewRenderHidden_Lines_OnAction, SIGNAL(activated()), oview,SLOT(slotCasHlrOn()));
    connect(viewRenderHidden_Lines_OffAction, SIGNAL(activated()), oview,SLOT(slotCasHlrOff()));
    connect(selectionModeNeutralAction, SIGNAL(activated()), interact,SLOT(slotNeutral()));
    connect(selectionModeVertexAction, SIGNAL(activated()), interact,SLOT(slotVertexSelection()));
    connect(selectionModeEdgeAction, SIGNAL(activated()), interact,SLOT(slotEdgeSelection()));
    connect(selectionModeFaceAction, SIGNAL(activated()), interact,SLOT(slotFaceSelection()));
    connect(SimpleToolpathOnFace, SIGNAL(activated()), pathAlg,SLOT(slotComputeSimplePathOnFace()));
    connect(outputProtoCode, SIGNAL(activated()), pathAlg,SLOT(slotOutputProtoCode()));
//TODO: add progress bar to statusbar? how?
}

void mainui::slotFileNew()
{
  statusBar()->message(tr("Creating new file..."));
  interact->newInteract();
  statusBar()->message(tr("Ready."));
}

void mainui::paintEvent( QPaintEvent * event )
{
   if (!viewIsSetup)
   {
       oview->init();
       viewIsSetup=1;
       interact->initContext(oview);
       interact->initPart();
   }
    if (&event == 0) {;}  //supress unused parameter warning
}

void mainui::init() {
    splitter = new QSplitter(this);

    pathAlg = new pathAlgo();
    interact = new interactive(this, splitter, pathAlg);  //interact = AIS
    oview = new occview(splitter, interact);
    setCentralWidget(splitter);
    QPaintEvent *pE;
    oview->paintEvent(pE);  //make the %$@^%$#^#@ view reset
    initActions();
}

void mainui::fileExitAction_activated()
{
    statusBar()->message(tr("Exiting application..."));
   ///////////////////////////////////////////////////////////////////
    // exits the Application
    if(interact->isModified()) {
	if(queryExit())
	    qApp->quit();
    } else {
	qApp->quit();
    }
    statusBar()->message(tr("Ready."));
}

bool mainui::queryExit()
{
  int exit=QMessageBox::information(this, tr("Quit..."),
                                    tr("Do you really want to quit?"),
                                    QMessageBox::Ok, QMessageBox::Cancel);
  return (exit==1);
}
