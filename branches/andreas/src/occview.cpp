/*************************************************************
** License: GPL.  
** Derived from work by Olivier Coma. 
** Copyright (C) 2006 Mark Pictor
*************************************************************/

#include "occview.h"
#include "interactive.h"

occview::occview(QWidget *parent, interactive *inter) : QWidget(parent)
{
    interact=inter;
    connect(interact, SIGNAL(documentChanged(bool,bool)), this, SLOT(slotDocumentChanged(bool,bool)));
    connect(interact, SIGNAL(hasSelected(bool)), this, SLOT(slotHasSelected(bool)));
    myXmin=0;
    myYmin=0;
    myXmax=0;
    myYmax=0;
    myCurZoom=0;

    // will be set in OnInitial update, but, for more security :
    myCurrentMode = CurAction3d_Nothing;
    myDegenerateModeIsOn=Standard_True;
    hasSelected=false;
}

occview::~occview()
{
    myView->Remove();
}

void occview::init() 
{
    	myDevice=new Graphic3d_GraphicDevice("");
	myWindow=new Xw_Window(myDevice,winId());
	myViewer=new V3d_Viewer(myDevice,(short *) "Visu3D");
	myViewer->SetDefaultLights();
	myViewer->SetLightOn();
	myView = myViewer->CreateView();

	myView->SetWindow(myWindow);
	//myView->SetBackgroundColor(Quantity_NOC_GAINSBORO); //TODO: fancy background via SetBackgroundImage
	myView->SetBackgroundImage("x.rgb", Aspect_FM_STRETCH,true);
//	myView->SetBackgroundImage("/home/mark/Desktop/bgpic-1.rgb", Aspect_FM_STRETCH, true);
//	myView->SetBackgroundImage("back_fade.rgb", Aspect_FM_STRETCH, true);

	
	myView->SetDegenerateModeOn();
	//myView->SetAntialiasingOn();  //don't use, makes gaps between face triangles visible. $%@#%#$^%$%^#$^%$%^$
	myView->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_BLACK, 0.08, V3d_ZBUFFER);
	if (!myWindow->IsMapped())
   		myWindow->Map();
	QWidget::setMouseTracking(true);
	
	
}

//redraw, with optional erase and/or viewpoint reset
void occview::slotDocumentChanged(bool erase, bool axo)
{
	if (erase)
	  interact->myContext->EraseAll();

	if (axo) {
		slotCasAxo();
		slotCasZoomAll();
	}
	myView->Redraw();
}

void occview::paintEvent(QPaintEvent* event) {
    if (!myView.IsNull())
	myView->Redraw();
    if (&event == 0) {;} //supress unused parameter warning
}

void occview::slotRedrawResize() {
	if (!myView.IsNull()) {
		myView->Redraw();
		myView->MustBeResized();
	}
}


void occview::resizeEvent(QResizeEvent* event)
{ 
    if (!myView.IsNull())
	myView->MustBeResized();

    if (&event == 0) {;} //supress unused parameter warning

	//not sure why, but without the delay below, the view won't be the right size.
    QTimer *timer = new QTimer(this);
    connect( timer, SIGNAL(timeout()), this, SLOT(slotRedrawResize()) );
    timer->start( 100, TRUE ); // .1 second single-shot timer
}

void occview::mousePressEvent(QMouseEvent* event)
{
  //  save the current mouse coordinate in min
  myXmin=event->x();  myYmin=event->y();
  myXmax=event->x();  myYmax=event->y();

  switch (myCurrentMode)
  {
    case CurAction3d_Nothing :
    	break;
    case CurAction3d_DynamicZooming  :
    	break;
    case CurAction3d_DynamicPanning  :
    	break;
    case  CurAction3d_DynamicRotation :
	if (!myDegenerateModeIsOn)
	    myView->SetDegenerateModeOn();
	myView->StartRotation(event->x(),event->y());
	break;
    default :
    	Standard_Failure::Raise(" incompatible Current Mode ");
    break;
  }
}
void occview::mouseReleaseEvent(QMouseEvent* event)
{
	if (myCurrentMode==CurAction3d_Nothing && (event->state() & Qt::LeftButton))
	{
		emit selectionChanged();
	}
	myCurrentMode  = CurAction3d_Nothing;
}

void occview::mouseMoveEvent(QMouseEvent* event)
{
	// control and right mouse button pressed
	if (((event->state() & Qt::RightButton) && (event->state() & Qt::ControlButton)) || ((event->state() & Qt::LeftButton) && (myCurrentMode == CurAction3d_DynamicRotation)))
	{
    		if (!myDegenerateModeIsOn)
	    		myView->SetDegenerateModeOn();
	  	if (myCurrentMode != CurAction3d_DynamicRotation)
	  	{
	  		myView->StartRotation(event->x(),event->y());
	  		myCurrentMode=CurAction3d_DynamicRotation;
	  	}
    		myView->Rotation(event->x(),event->y());
    		myView->Redraw();
	}
	//ctrl-middle button
	else if (((event->state() & Qt::MidButton) && (event->state() & Qt::ControlButton)) || ((event->state() & Qt::LeftButton) && (myCurrentMode == CurAction3d_DynamicPanning)))
	{
    		if (!myDegenerateModeIsOn)
	    		myView->SetDegenerateModeOn();
    		myView->Pan(event->x()-myXmax,myYmax-event->y()); // Realize the panning
		myXmax = event->x();
		myYmax = event->y();
	}
	//ctrl-left button
	else if (((event->state() & Qt::LeftButton) && (event->state() & Qt::ControlButton)) || ((event->state() & Qt::LeftButton) && (myCurrentMode == CurAction3d_DynamicZooming)))
	{
	    	if (!myDegenerateModeIsOn)
	    		myView->SetDegenerateModeOn();
	  	myView->Zoom(myXmax,myYmax,event->x(),event->y());
		myXmax=event->x();  myYmax=event->y();
	}	
	else
	{
		interact->myContext->MoveTo(event->x(),event->y(),myView);
		//ligne pour eviter un bug de mise a jour avec certaines cartes graphiques
	                //translates to (?) "this avoids a bug with certain graphics cards"
		if (interact->myContext->HasOpenedContext())
			myView->Redraw();
	}
}

//unnecessary?  TODO:verify usefulness
void occview::slotHasSelected(bool b)
{
	hasSelected=b;
}

void occview::slotCasZoomAll()
{
  	myView->FitAll();
  	myView->ZFitAll();
}


void occview::slotCasDynZoom()
{
	myCurrentMode = CurAction3d_DynamicZooming;
}

void occview::slotCasPan()
{
    	myCurrentMode = CurAction3d_DynamicPanning;
}

void occview::slotCasAxo()
{
	myView->SetProj(V3d_XposYnegZpos);
}

void occview::slotCasFront()
{
	myView->SetProj(V3d_Xpos);
}

void occview::slotCasTop()
{
	myView->SetProj(V3d_Zpos);
}

void occview::slotCasLeft()
{
	myView->SetProj(V3d_Ypos);
}

void occview::slotCasBack()
{
	myView->SetProj(V3d_Xneg);
}

void occview::slotCasRight()
{
	myView->SetProj(V3d_Yneg);
}

void occview::slotCasBottom()
{
	myView->SetProj(V3d_Zneg);
}

void occview::slotCasRotate()
{
	myCurrentMode = CurAction3d_DynamicRotation;
}

void occview::slotCasReset()
{
	myView->Reset();
	myView->SetProj(V3d_XposYnegZpos);
}		

void occview::slotCasHlrOn()
{
  	myView->SetDegenerateModeOn();
  	myDegenerateModeIsOn = Standard_True;
}

void occview::slotCasHlrOff()
{
  	myView->SetDegenerateModeOff();
  	myDegenerateModeIsOn = Standard_False;
}
