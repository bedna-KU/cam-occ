/*************************************************************
** License: GPL.  
** Derived from work by Olivier Coma. 
** Copyright (C) 2006 Mark Pictor
*************************************************************/

//interactive.cpp --  AIS

#include <qcolordialog.h>
#include <qfiledialog.h>

#include <TopoDS.hxx>

#include "interactive.h"
#include "occview.h"
#include "positionWorkpieceDlg.h"

extern QApplication* pA;

interactive::interactive(mainui *mui, QSplitter *qs, pathAlgo *pAlg)
{
    mainIntf = mui;
    splitter = qs;
    Path = pAlg;
    modified = false;	//unused?
    displayedPaths.clear();
    setupFrame();
}

interactive::~interactive()
{
    
}

void interactive::setupFrame()
{
    leftFrame = new QVBox(splitter);
      leftFrame->setMaximumWidth(210);  //default to all tabs visible
      leftFrame->setMinimumWidth(100);
      tabWidget = new QTabWidget(leftFrame);
        faceView = new QListView(tabWidget);
	pathView = new QListView(tabWidget);
	toolView = new QListView(tabWidget);
	tabWidget->addTab(faceView,"Faces");
	tabWidget->addTab(pathView,"Paths");
	tabWidget->addTab(toolView,"Tools");
      pBar = new QProgressBar(leftFrame);
        pBar->setMaximumHeight(20);
}

void interactive::loadPart(const QString& filename)
{
	Part.Erase();			//erase part from screen
	Path->init();			//clear pathAlgo path data
	for (uint i=0;i < displayedPaths.size();i++) {
		occObject X = displayedPaths.at(i);
		X.Erase();
		X.~occObject();
	}  //this for loop erases these lines from the display
	displayedPaths.clear();	//erase displayable path data

	TopoDS_Shape myShape;
	ImportExport::ReadBREP((Standard_CString)(const char*)filename, myShape);
	Part.SetContext(myContext);
	Part.SetShape(myShape);
	Part.SetColor(Quantity_NOC_RED);
	Part.SetMaterial(Graphic3d_NOM_PLASTIC);
	Part.Display(true);

}

bool interactive::newInteract()
{
    slotNeutral();
    initPart();
    initPath();
    myGeomFile = QFileDialog::getOpenFileName( "../models", "File (*.brep *.step *.stp)");
    QPaintEvent *evt = 0;
    mainIntf->paintEvent(evt);
    pA->processEvents();
    if (!myGeomFile.isEmpty())
    {
	loadPart(myGeomFile);
	emit documentChanged(false,true);
	//emit clearTree();
  	return true;
    }
    return false;
}

bool interactive::isModified() const
{
  return (modified);
}

void interactive::initContext(occview* v)
{
    myContext =new AIS_InteractiveContext(v->myViewer);	
    connect(v, SIGNAL(selectionChanged()), this, SLOT(slotSelectionChanged()));
}

void interactive::initPath()
{
    connect (Path, SIGNAL(showPath()), this, SLOT(slotShowPath()));
    connect (Path, SIGNAL(setProgress(int,char*)), this, SLOT(slotSetProgress(int,char*)));
}

void interactive::initPart()
{
	Path->init();
	opened=false;
}

void interactive::getFaceEdges()
{
	TopoDS_Face F;
	emit getFace(F);
	TopTools_SequenceOfShape edges;
	emit getEdges(edges);
}

//pop up a dialog box for rotating, translating, and scaling the part.
//to make it easier, trihedron should be visible (i.e. scaled to match part)
//can trihedron be placed in display corner?
void interactive::slotOrientWorkpiece() 
{
	static double rx=0,ry=0,rz=0,tx=0,ty=0,tz=0;
	static double mult=1,ratio = 1;  //when mult=1, uncheck the checkbox
	static uint cadU=2,camU=2;
	static bool mCheck = false;
	gp_Trsf T1,T2;

	positionWorkpieceDlg orientDlg(0,"Workpiece setup", true);
	T1 = Part.GetTrsf();
	orientDlg.setValues(rx,ry,rz,tx,ty,tz,cadU,camU,mult,mCheck);

	if (orientDlg.exec()==1)
	{
		orientDlg.getValues(rx,ry,rz,tx,ty,tz,cadU,camU,mult,mCheck,ratio);
		T1.SetRotation(gp_Ax1(gp_Pnt(0,0,0),gp_Dir(1,0,0)),PI/180*rx);
		T2.SetRotation(gp_Ax1(gp_Pnt(0,0,0),gp_Dir(0,1,0)),PI/180*ry);
		T1=T1*T2;
		T2.SetRotation(gp_Ax1(gp_Pnt(0,0,0),gp_Dir(0,0,1)),PI/180*rz);
		T1=T1*T2;
		T1.SetTranslationPart(gp_Vec((Standard_Real)tx,(Standard_Real)ty,(Standard_Real)tz));
		T1.SetScaleFactor((Standard_Real)ratio);
		Part.SetTrsf(T1);
	}
}

void interactive::slotSetProgress(int p,char* status)//, char* status)
{
	if (p<0) {
		pBar->reset();
	} else {
		pBar->setProgress(p);
	}
	if (status != "")
		mainIntf->statusBar()->message(status);
	pA->processEvents();
}

void interactive::slotSelectionChanged()
{
	myContext->Select();
	bool msg=false;
//	bool sel=false;
	for(myContext->InitSelected(); myContext->MoreSelected(); myContext->NextSelected())
	{
		Handle_AIS_InteractiveObject IO=myContext->Interactive();
		if (IO->IsKind(STANDARD_TYPE(AIS_Trihedron)) || !myContext->HasSelectedShape())
		{
			myContext->ClearSelected();
			break;
		}

		msg=true;
		//l'object selectionne (= une face) devient un object topologique S puis F
		TopoDS_Shape S = myContext->SelectedShape();
		if (S.ShapeType()==TopAbs_FACE)
		{
			TopoDS_Face F=TopoDS::Face(S);
			Path->AddFace(F,Part.GetShape());
		}
		else if (S.ShapeType()==TopAbs_EDGE )
		{
			TopoDS_Edge E=TopoDS::Edge(S);
		}
		break;
	}

	emit hasSelected(msg);
}

/*
void interactive::slotEditTool()
{
	toolDlg dlg(0, "Point edit", true);
	dlg.set(Machine.Tool->dia, Machine.Tool->l);
	if (dlg.exec()==1)
	{
		
		gp_Trsf T1, T2;
		T1=Machine.Table_Z.GetTrsf();
		T2.SetTranslation(gp_Vec(0,0,dlg.l-Machine.Tool->l));
		Machine.Table_Z.SetTrsf(T1*T2);
		computed=false;
		Machine.ResetLocation();
		Part.ResetLocation();
		dlg.get(Machine.Tool->dia, Machine.Tool->l);
		Path.SetToolLength(Machine.Tool->l);
		Machine.Tool->Erase();
		Machine.Tool->CreateShape();
		Machine.Tool->SetColor(Quantity_NOC_WHITE);
		Machine.Tool->SetMaterial(Graphic3d_NOM_PLASTIC);
		Machine.Tool->Display();
	}
}
*/

////////////////////////////////////////////LocalContext
void interactive::slotNeutral()
{
  myContext->CloseAllContexts();
}

void interactive::slotVertexSelection()
{
  myContext->CloseAllContexts();
  myContext->OpenLocalContext();
  myContext->ActivateStandardMode(TopAbs_VERTEX);
}

void interactive::slotEdgeSelection()
{
  myContext->CloseAllContexts();
  myContext->OpenLocalContext();
  myContext->ActivateStandardMode(TopAbs_EDGE);
}

void interactive::slotFaceSelection()
{
  myContext->CloseAllContexts();
  myContext->OpenLocalContext();
  myContext->ActivateStandardMode(TopAbs_FACE);
}

/////////////////////////////////color and shading
void interactive::slotCasColor()
{
	QColor color=QColorDialog::getColor(QColor(0,0,0));
	if (color.isValid())
	{
		myContext->InitCurrent();
		int r,g,b;
		color.rgb(&r,&g,&b);
	  Quantity_Color CSFColor = Quantity_Color (r/255.,g/255.,b/255.,Quantity_TOC_RGB);
	  for (;myContext->MoreCurrent ();myContext->NextCurrent ())
		  myContext->SetColor (myContext->Current(),CSFColor.Name());
	}
}

//we're doing materials the easy way -- cycle through all 20.
void interactive::slotCasRMat()
{
	//int values 0 thru 20 inclusive for Graphic3d_NameOfMaterial

	//this matches the order the materials are defined in the ENUM.
	//there is no way to retrieve enum names in gnu C++ 
	char NoM[21][14] = {"BRASS","BRONZE","COPPER","GOLD","PEWTER","PLASTER","PLASTIC","SILVER","STEEL","STONE","SHINY_PLASTIC","SATIN","METALIZED","NEON_GNC","CHROME","ALUMINIUM","OBSIDIAN","NEON_PHC","JADE","DEFAULT","UserDefined"};

	static int theMaterial = 0;
	for ( myContext->InitCurrent(); myContext->MoreCurrent (); myContext->NextCurrent () )
		myContext->SetMaterial( myContext->Current(), (Graphic3d_NameOfMaterial)theMaterial );
	char matStr[25];
	sprintf(matStr,"Cycle through materials (currently: %s)", NoM[theMaterial]);

	//looks better if we set statusTip AND directly set statusbar (no flicker)
	mainIntf->statusBar()->message(tr(matStr));
	mainIntf->viewRenderMaterialAction->setStatusTip(tr(matStr));

	theMaterial++;  //increment for next time user clicks
	if (theMaterial > 20)  //only 0-20 are valid
		theMaterial = 0;
}

//Toggle transparency for each obj selected.  Toggle between 40% and 100%.
void interactive::slotCasTransparency()
{
	//TODO: fix crash when in face selection mode (and possibly others)
    static bool transparencyOff = true;
    for( myContext->InitCurrent(); myContext->MoreCurrent(); myContext->NextSelected() )
	if (transparencyOff) {
		myContext->SetTransparency( myContext->Current(), .4);
		transparencyOff = false;
	} else {
		myContext->UnsetTransparency( myContext->Current(), 1);
		transparencyOff = true;
	}
}

void interactive::slotCasShading()
{
	myContext->InitCurrent();
  for (;myContext->MoreCurrent ();myContext->NextCurrent ())
	  myContext->SetDisplayMode(myContext->Current(),1);
}

void interactive::slotCasWireframe()
{
	myContext->InitCurrent();
  for (;myContext->MoreCurrent ();myContext->NextCurrent ())
	  myContext->SetDisplayMode(myContext->Current(),0);
}

//probably causes a memory leak.  Need to do it better.

void interactive::slotShowPath()
{
//puts("start show");
	//int numFaces = Path->projectedPasses.size();
	//TopoDS_Edge anEdge;
	//TopoDS_Shape linesOnThisFace;
	myContext->CloseAllContexts();  //otherwise, lines will disapear if you click on certain buttons!

	
	for(uint i=0;i < Path->projectedPasses.size(); i++) {
//	puts("show one");
	//TODO: check projectedPasses.at(i).displayed - only display new ones.
		if (!Path->projectedPasses.at(i).displayed) {
			occObject tracedPath;
			tracedPath.Erase();
			tracedPath.SetContext(myContext);
			tracedPath.SetShape(Path->projectedPasses.at(i).P);
			tracedPath.SetColor(Quantity_NOC_BLUE1);
			tracedPath.Display();
			displayedPaths.push_back(tracedPath);
			Path->projectedPasses.at(i).displayed = true;
		}
	}
}
