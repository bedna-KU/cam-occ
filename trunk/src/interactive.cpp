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
    modified = false;
}

interactive::~interactive()
{
    
}

void interactive::loadPart(const QString& filename)
{
	Part.Erase();
	TopoDS_Shape myShape;
	ImportExport::ReadBREP((Standard_CString)(const char*)filename, myShape);
	Part.SetContext(myContext);
	Part.SetShape(myShape);
	Part.SetColor(Quantity_NOC_RED);
	Part.SetMaterial(Graphic3d_NOM_PLASTIC);
	Part.Display();
	Path->SetContext(myContext);
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
    computed=false;
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

//this is a KLUDGE.  There is a better way.
/*
void interactive::slotShowPath()
{
	//use the occObject tracedPath to display it.
cout << "slot show path" << endl;

	TopoDS_Iterator lineIterator; 
	vector<TopoDS_Edge> projectedVec = Path->projectedLines;
puts("vec");
	TopoDS_Vertex Vtx1, Vtx2;
	bool firstLINE = true;
puts("b makew");
	BRepBuilderAPI_MakeWire projWire;
puts("makew");

	int numLines = projectedVec.size();
printf("total %i",numLines);
	for(int i=0;i < numLines; i++)
	{
printf("for i=%i total %i",i,numLines);
		//We can only display one "item" with the occobject... therefore all edges must be added to a wire, and we must "connect the dots" between them on the fly - the wire will fail if the edges are not connectable in the order given.
		//So, we must decompose each edge and find two vertices which will be used to create edges to fill in.
		TopoDS_Edge curPline = projectedVec.at(i);
		bool firstVTX = true;
		
		for(lineIterator.Initialize(curPline);lineIterator.More();lineIterator.Next())
		{
			TopoDS_Shape S = lineIterator.Value();
			if (S.ShapeType()==TopAbs_VERTEX)
			{
				if (firstVTX) {
					Vtx1 = TopoDS::Vertex(S);
					firstVTX = false;
					if (firstLINE) { //first line has nothing before it to connect to
						firstLINE = false;
					} else { //not the first line, connect to previous line
						projWire.Add(BRepBuilderAPI_MakeEdge(Vtx2,Vtx1));
					}
				} else {
					//We keep setting vtx2 until we get the last one. (I'm not certain if a TopoDS_Edge can have more than 2 or not.)
					Vtx2 = TopoDS::Vertex(S);
				}  //firstVTX
			}  //check shape type
		}  //iterate through subshapes
		projWire.Add(curPline);
	}  //add each line from vector to wire
	puts("disp");

	tracedPath.Erase();
	tracedPath.SetContext(myContext);
	tracedPath.SetShape(projWire);
	tracedPath.SetColor(Quantity_NOC_BLUE1);
	//tracedPath.SetMaterial(...);
	tracedPath.Display();

}*/

//well s---!  fixes more than one problem with above code.
void interactive::slotShowPath()
{
	vector<TopoDS_Edge> projectedVec = Path->projectedLines;
	int numLines = projectedVec.size();
	occObject tracedPath[numLines];
	for(int i=0;i < numLines; i++)
	{
		tracedPath[i].Erase();
		tracedPath[i].SetContext(myContext);
		tracedPath[i].SetShape(projectedVec.at(i));
		tracedPath[i].SetColor(Quantity_NOC_BLUE1);
		tracedPath[i].Display();
	}

}
