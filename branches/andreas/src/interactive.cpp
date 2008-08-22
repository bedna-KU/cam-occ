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

extern QApplication* pA;

interactive::interactive(mainui *mui, QSplitter *qs, pathAlgo *pAlg, basicCADmodule *bCmod)
{
    mainIntf = mui;
    splitter = qs;
    Path = pAlg;
    CAD=bCmod;
    modified = false;	//unused?
    displayedPaths.clear();
    setupFrame();
    connect (Path, SIGNAL(addFaceToList(uint)), camT, SLOT(slotAddFace(uint)));
      connect (Path, SIGNAL(showPath()), this, SLOT(slotShowPath()));//AK
      connect(Path,SIGNAL(setDisplayedPathFalse()),this,SLOT(slotSetDisplayedPathFalse()));//AK
      connect(CAD, SIGNAL(sendShape(TopoDS_Shape)),this,SLOT(slotShowCAD(TopoDS_Shape)));//AK
    //connect (camT, SIGNAL(selectFace(int)), Path, SLOT(slotSelectFaceFromList(int)));
}

interactive::~interactive()
{
    
}

//AK+++++++++++++++++++++
void interactive::slotSetDisplayedPathFalse()
{
    for (uint i=0;i < displayedPaths.size();i++) {
		occObject X = displayedPaths.at(i);
		X.Erase();
		//delete &X;
	}  //this for loop erases these lines from the display
	displayedPaths.clear();
    }
//+++++++++++++++++++++++++AK


void interactive::setupFrame()
{
    leftFrame = new QVBox(splitter);
      leftFrame->setMaximumWidth(290);  //default to all tabs visible
      //leftFrame->setMinimumWidth(310);
      camT = new camTabs(leftFrame);
      pBar = new QProgressBar(leftFrame);
        pBar->setMaximumHeight(20);

    connect (camT, SIGNAL(setProgress(int,char*)), this, SLOT(slotSetProgress(int,char*)));
}

void interactive::loadPart(const QString& filename)
{
	Part.Erase();			//erase part from screen
	Path->slotInit();			//clear pathAlgo path data
	for (uint i=0;i < displayedPaths.size();i++) {
		occObject X = displayedPaths.at(i);
		X.Erase();
		//delete &X;
	}  //this for loop erases these lines from the display
	displayedPaths.clear();	//erase displayable path data

	TopoDS_Shape myShape;
	//ImportExport::ReadBREP((Standard_CString)(const char*)filename, myShape);// for BREP import AK
	ImportExport::ReadSTEP((Standard_CString)(const char*)filename, myShape);// for STEP import AK
	ImportExport::ReadSTEP((Standard_CString)(const char*)filename, importedShape);// for STEP import AK
	Part.SetContext(myContext);
	//Part.SetShape(myShape);
	Part.SetShape(importedShape);
	Part.SetColor(Quantity_NOC_RED);
	Part.SetMaterial(Graphic3d_NOM_PLASTIC);
	Part.Display(false);

}
void interactive::slotTransferShape()
{
    emit transferShape(importedShape);
}

//AK+++++++++++++++++++++++++++++++++++++++
void interactive::slotShowCAD(TopoDS_Shape shapeGeometry)
{
    Part.Erase();			//erase part from screen
    Path->slotInit();			//clear pathAlgo path data
    for (uint i=0;i < displayedPaths.size();i++) {
		occObject X = displayedPaths.at(i);
		X.Erase();
		//delete &X;
}  //this for loop erases these lines from the display
	displayedPaths.clear();	//erase displayable path data

	TopoDS_Shape myShape;
	myShape=shapeGeometry;
	importedShape=myShape;
	//ImportExport::ReadBREP((Standard_CString)(const char*)filename, myShape);// for BREP import AK
	//ImportExport::ReadSTEP((Standard_CString)(const char*)filename, myShape);// for STEP import AK
	Part.SetContext(myContext);
	Part.SetShape(shapeGeometry);
	Part.SetShape(myShape);
	Part.SetColor(Quantity_NOC_RED);
	Part.SetMaterial(Graphic3d_NOM_PLASTIC);
	Part.Display(false);
	
	int checkVaultBorder=0;
	int readVaultBorder=4;
	checkVaultBorder = (Path->staticVaultBorder(readVaultBorder));//if negative then negative border selected!
}
//+++++++++++++++++++++++++++++++++++++++AK

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
    else//inserted by AK
	return false;
}

bool interactive::newMaze()
{
    slotNeutral();
    
    //initPart();
    //initPath();
    //myGeomFile = QFileDialog::getOpenFileName( "../models", "File (*.brep *.step *.stp)");
    QPaintEvent *evt = 0;
    mainIntf->paintEvent(evt);
    pA->processEvents();
    Part.Erase();
   /* if (!myGeomFile.isEmpty())
    {
	loadPart(myGeomFile);
	emit documentChanged(false,true);
	//emit clearTree();
  	return true;
    }
    else//inserted by AK
	return false;*/
    
    Path->slotInit();			//clear pathAlgo path data
	
    for (uint i=0;i < displayedPaths.size();i++) {
	occObject X = displayedPaths.at(i);	
	X.Erase();
		//delete &X;	
    }  //this for loop erases these lines from the display	
    displayedPaths.clear();	//erase displayable path data
    initPart();
    initPath();
    return true;
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
    Path->slotInit();
    //connect (Path, SIGNAL(showPath()), this, SLOT(slotShowPath()));
    connect (Path, SIGNAL(setProgress(int,char*)), this, SLOT(slotSetProgress(int,char*)));
}

void interactive::initPart()
{
	Part.Init();
	opened=false;
}

//unused?
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
	Part.trsfDlg();
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
		
			
		if (S.ShapeType()==TopAbs_FACE) {
			TopoDS_Face F=TopoDS::Face(S);
			Path->AddFace(F,Part.GetShape());
			//puts("add face");
		} else if (S.ShapeType()==TopAbs_EDGE ) {
			TopoDS_Edge E=TopoDS::Edge(S);
			usrRadiusMessage(E);
		    
		} else if (S.ShapeType()==TopAbs_VERTEX ) {
			TopoDS_Vertex V=TopoDS::Vertex(S);
			usrVertexMessage(V);
		    }
		break;
	}

	emit hasSelected(msg);
}


//there are two ways to go from  a TopoDS entity to a form where basics like center of a circle can be extracted.  We mix&match since BRepTool does not test shape type and the entity types returned by BRepAdaptor do not provide a method to turn parameters into points.

//E or Et for curve and adaptor???
void interactive::usrRadiusMessage(TopoDS_Edge E) 
{
	Standard_Real first, last;
	gp_Pnt p1,p2,c;

	BRepBuilderAPI_Transform trsf(E,Part.GetAxis());
	TopoDS_Edge Et=TopoDS::Edge(trsf.Shape());

	Handle(Geom_Curve) C = BRep_Tool::Curve(Et,first,last);
	BRepAdaptor_Curve adaptor = BRepAdaptor_Curve(Et);
	QString m="",n="";
	if (adaptor.GetType()==GeomAbs_Circle) {

		gp_Circ circ = adaptor.Circle();
		c = circ.Location();
		Standard_Real R = circ.Radius();
		m.sprintf("Center of that arc: %f %f %f\nRadius %f",c.X(),c.Y(),c.Z(),R);
	} else if (adaptor.GetType()==GeomAbs_Line) {
		gp_Lin line = adaptor.Line();
		m.sprintf("Line");
	}
	if (m == "") m = "Edge";
	p1=C->Value(first);
	p2=C->Value(last);
	n.sprintf("%s\nFirst Point\tX%6.2f \tY%6.2f \tZ%6.2f\nLast Point\tX%6.2f \tY%6.2f \tZ%6.2f\n", (const char *)m,p1.X(),p1.Y(),p1.Z(),p2.X(),p2.Y(),p2.Z());
	QMessageBox::information(mainIntf,"Selection",n);
    }

//AK++++++++++++++++
void interactive::usrVertexMessage (TopoDS_Vertex V)
{
    
    Standard_Real first, last;
    gp_Pnt p1,p2,c;
    
    BRepBuilderAPI_Transform trsf(V,Part.GetAxis());
    TopoDS_Vertex Vt=TopoDS::Vertex(trsf.Shape());
    p1=BRep_Tool::Pnt(Vt);
    
    QString m="",n="";	
    if (m == "") m = "Vertex";
	
	n.sprintf("%s\nPoint\tX%6.2f \tY%6.2f\t Z%6.2f", (const char *)m,p1.X(),p1.Y(),p1.Z());
	QMessageBox::information(mainIntf,"Selection",n);
}
//+++++++++++++++++++++AK


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
}tracedPath.Display();tracedPath.Display();
			displayedPaths.push_back(tracedPath);
			displayedPaths.push_back(tracedPath);
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
  cout<<"interactive.cpp - void interactive::slotFaceSelection()"<<endl; 
  myContext->CloseAllContexts();
  myContext->OpenLocalContext();
  myContext->ActivateStandardMode(TopAbs_FACE);
}

/////////////////////////////////color and shading
void interactive::slotCasColor()
{
	QColor color=QColorDialog::getColor(QColor(0,0,0));
	if (color.isValid()) {
		int r,g,b;
		color.rgb(&r,&g,&b);
		Quantity_Color CSFColor = Quantity_Color (r/255.,g/255.,b/255.,Quantity_TOC_RGB);
		Part.SetColor(CSFColor.Name());
	}
}

//we're doing materials the easy way -- cycle through all 20.
void interactive::slotCasRMat()
{
	//int values 0 thru 20 inclusive for Graphic3d_NameOfMaterial

	//this matches the order the materials are defined in the ENUM.
	//there is no way to retrieve enum names in gnu C++ 
	char NoM[21][14] = {"BRASS","BRONZE","COPPER","GOLD","PEWTER","PLASTER","PLASTIC","SILVER","STEEL","STONE","SHINY_PLASTIC","SATIN","METALIZED","NEON_GNC","CHROME","ALUMINIUM","OBSIDIAN","NEON_PHC","JADE","DEFAULT","UserDefined"};

	static uint theMaterial = 0;
	Part.SetMaterial((Graphic3d_NameOfMaterial)theMaterial);
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
	//fix by simply changing properties on Part?  also for material, above?
//fixed here
    static bool transparencyOff = true;
	if (transparencyOff) {
		Part.SetTransparency(.4);
		transparencyOff = false;
	} else {
		Part.SetTransparency(-1.0);
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
	gp_Trsf T = Part.GetAxis();  //GetAxis vs GetTrsf...?!

	for(uint i=0;i < Path->projectedPasses.size(); i++) {
	    if (!Path->projectedPasses.at(i).displayed) {
			occObject tracedPath;
			tracedPath.Erase();
			tracedPath.SetContext(myContext);
			tracedPath.SetShape(Path->projectedPasses.at(i).P);
			tracedPath.SetColor(Quantity_NOC_BLUE1);
			tracedPath.SetTrsf(T);  //apply the same transform as is used on the Part.  Don't understand why it isn't automatically applied...@$#%#
			tracedPath.Display();
			displayedPaths.push_back(tracedPath);
			Path->projectedPasses.at(i).displayed = true;
		}
	}
}
