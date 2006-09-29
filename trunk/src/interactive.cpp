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

interactive::interactive(pathAlgo *pAlg)
{
    Path = pAlg;
    modified = false;	//unused?
    displayedPaths.clear();
}

interactive::~interactive()
{
    
}

void interactive::loadPart(const QString& filename)
{
	Part.Erase();			//erase part from screen
	Path->init();			//clear pathAlgo path data
	for (uint i=0;i < displayedPaths.size();i++) {
		occObject X = displayedPaths.at(i);
		X.Erase();
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
    //initMachine();
    initPart();
    initPath();
    //mySimFile="";
    myGeomFile = QFileDialog::getOpenFileName( QString::null, "File (*.brep *.step *.stp)");
    if (!myGeomFile.isEmpty())
    {
	//TODO: need to allow ui to update here.

	//Machine.ResetLocation();
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
    //computed=false;
}

void interactive::initPart()
{
	Path->init();
	//Path.SetMachine(&Machine);
	opened=false;
}

void interactive::getFaceEdges()
{
	TopoDS_Face F;
	emit getFace(F);
	TopTools_SequenceOfShape edges;
	emit getEdges(edges);
	//Path.SetPath(F, edges);
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
			Path->SetFace(F);
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
puts("start show");
	int numLines = Path->projectedLines.size();
	occObject tracedPath;
	TopoDS_Edge anEdge;
	TopoDS_Shape linesOnThisFace;
	myContext->CloseAllContexts();  //otherwise, lines will disapear if you click on certain buttons!

	tracedPath.Erase();
	for(int i=0;i < numLines; i++) {
		anEdge = Path->projectedLines.at(i);
		if (linesOnThisFace.IsNull()) {
			linesOnThisFace = anEdge;
		} else {
			linesOnThisFace = BRepAlgoAPI_Fuse(linesOnThisFace,anEdge);
		}
	}
	tracedPath.SetContext(myContext);
	tracedPath.SetShape(linesOnThisFace);
	tracedPath.SetColor(Quantity_NOC_BLUE1);
	tracedPath.Display();

	displayedPaths.push_back(tracedPath);
}

/*************************************************************************
void interactive::slotShowPath()
{
	vector<TopoDS_Edge> projectedVec = Path->projectedLines;
	int numLines = projectedVec.size();
	occObject tracedPath[numLines];

	myContext->CloseAllContexts();  //otherwise, lines will dissapear if you click on certain buttons!

	for(int i=0;i < numLines; i++)
	{
		tracedPath[i].Erase();
		tracedPath[i].SetContext(myContext);
		tracedPath[i].SetShape(projectedVec.at(i));
		tracedPath[i].SetColor(Quantity_NOC_BLUE1);
		//tracedPath[i].SetMaterial(Graphic3d_NOM_PLASTIC);
		tracedPath[i].Display();
	}

}
************************************************************************/

