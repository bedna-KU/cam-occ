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
#include "rs274emc.h"  //AK 1 include headerfile
#include "basicCADmodule.h"  //AK
#include "selectPatch.h"//AK
#include "ImportExport.h"//AK
//causes error AK myContext->CloseAllContexts();


QSplitter *splitter;
occview *oview;
interactive *interact;
pathAlgo *pathAlg;
rs274emc *rs274em;  //AK 2. create "pointer" and 3. also in void mainui::init()
basicCADmodule *basicCADmodul;  //AK
selectPatch*selectPatc;//AK
ImportExport*ImportEx;

int viewIsSetup=0;

	
//set up QT connections.
void mainui::initActions() {
    //connect(fileNewAction, SIGNAL(activated()), this,SLOT(slotFileNew()));
    //connect(_Action, SIGNAL(activated()), _,SLOT(slot_()));
   
    // AK openCASCADE functionality
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
    
    // AK cam-occ additonal functionality
    connect(orientationPosition_WorkpieceAction, SIGNAL(activated()), interact,SLOT(slotOrientWorkpiece())); // AK workpiece alignment
    connect(SimpleToolpathOnFace, SIGNAL(activated()), pathAlg,SLOT(slotComputeSimplePathOnFace())); // AK tool path generation
    
    //connect(orientationPosition_WorkpieceAction, SIGNAL(activated()), interact,SLOT(slotOrientWorkpiece())); // AK workpiece alignment
    
    //AK+++++++++++++++++++++++++++++++++++++++
    connect(outputProtoCode, SIGNAL(activated()),  rs274em,SLOT(slotOutputProtoCode())); // AK g-Code generation
    connect(createCube, SIGNAL(activated()),  basicCADmodul,SLOT(slotGenerateCube())); // AK CAD moduele
    connect(toggleXzYz,SIGNAL(toggled(bool)),pathAlg,SLOT(slotToggleXzYz(bool))); // AK set XZ or YZ Cutting plane active
    
    connect(pocketXpos,SIGNAL(toggled(bool)),pathAlg,SLOT(slotToggleXpos(bool))); //AK
    connect(pocketXneg,SIGNAL(toggled(bool)),pathAlg,SLOT(slotToggleXneg(bool))); //AK
    connect(pocketYpos,SIGNAL(toggled(bool)),pathAlg,SLOT(slotToggleYpos(bool))); //AK
    connect(pocketYneg,SIGNAL(toggled(bool)),pathAlg,SLOT(slotToggleYneg(bool))); //AK
   
 
    
    connect(assisSelect,SIGNAL(activated()),selectPatc,SLOT(slotActivateFaceTransfer()));//AK
    connect (selectPatc, SIGNAL(sendAddFace(TopoDS_Face)), pathAlg, SLOT(slotAddFace(TopoDS_Face)));//AK
    connect (pathAlg, SIGNAL(activateRS274()), rs274em, SLOT(slotOutputProtoCode()));//AK

    connect (basicCADmodul, SIGNAL(setProgress(int,char*)), interact, SLOT(slotSetProgress(int,char*)));//AK
     connect (selectPatc, SIGNAL(setProgress(int,char*)), interact, SLOT(slotSetProgress(int,char*)));//AK
     connect(basicCADmodul, SIGNAL(setZoom()), oview,SLOT(slotCasZoomAll()));//AK
 
        connect(featureRecognition,SIGNAL(activated()),interact,SLOT(slotTransferShape())); //AK
	//connect(featureRecognition,SIGNAL(activated()),selectPatc,SLOT(slotRetrieveGeom(TopoDS_Shape))); //AK
     connect(interact, SIGNAL(transferShape(TopoDS_Shape)),selectPatc,SLOT(slotGetPatches(TopoDS_Shape)));//AK
     
       connect(pathAlg, SIGNAL(sendFeed(int)),rs274em,SLOT(slotGetFeed(int)));//AK       
       connect(pathAlg, SIGNAL(sendSpeed(int)),rs274em,SLOT(slotGetSpeed(int)));//AK	             
       connect(pathAlg, SIGNAL(sendToolDia(int)),rs274em,SLOT(slotGetToolDia(int)));//AK
   

        
    //++++++++++++++++++++++++++++++++++++++++AK
    
}

void mainui::slotFileNew()
{  
     statusBar()->message(tr("Deleting cache..."));
    
    rs274em->initStaticVault();// by AK    
    pathAlg->slotInit();
    pathAlg->cleanPathBuffer();
    //pathAlg->init();

   // statusBar()->message(tr("Creating new file..."));
   // interact->newInteract();
   
    interact->newMaze();
    statusBar()->message(tr("Ready."));
}

//AK++++++++++++++++++++++++++++++++++++++++++
void mainui::slotFileOpen()
{
    statusBar()->message(tr("Opening file..."));  
    rs274em->initStaticVault();// by AK    
      //pathAlg->slotInit();
      //pathAlg->init();

      
      interact->newInteract();
      statusBar()->message(tr("Ready."));
}
//++++++++++++++++++++++++++++++++++++++++++AK

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
    splitter = new QSplitter(this);  //split the window vertically

    // AK define "this" object
    pathAlg = new pathAlgo(selectPatc);
      basicCADmodul = new basicCADmodule(); //AK
    interact = new interactive(this, splitter, pathAlg,basicCADmodul);  //interact = AIS
    oview = new occview(splitter, interact);
    rs274em = new rs274emc(pathAlg); //AK 
    selectPatc = new selectPatch();//AK
  
    
    setCentralWidget(splitter);
    QPaintEvent *pE = 0;
    oview->paintEvent(pE);  //make the %$@^%$#^#@ view reset
    initActions();
}

// AK ++++++++++++++++++++++++++++++++++++++++++++++++++++
void mainui::fileExitAction_activated()
{
    statusBar()->message(tr("Exiting application..."));
    
    int exitdecision=QMessageBox::information(this, tr("Quit cam-occ..."),
              tr("Do you really want to quit?"),
              QMessageBox::Ok, QMessageBox::Cancel);
    
    if (exitdecision==1)
               {QApplication::exit(0);}
    
    else
                {statusBar()->message(tr("Ready."));}
}

void mainui::helpabout()
{
    statusBar()->message(tr("About..."));
    
    QMessageBox::information(this, tr("About cam-occ"),
                   tr("Release TU Munich\nbased on Release 17 by Marc Pictor\nbased on work by Olivier Coma\nunder GNU public license"),
                    QMessageBox::Ok);
    
    statusBar()->message(tr("Ready."));
}
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++AK


void mainui::slotFileSave()
{

    basicCADmodul->slotSaveSTEP();
}
