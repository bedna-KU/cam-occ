/*************************************************************
** License: GPL.  
** Derived from work by Olivier Coma. 
** Copyright (C) 2006 Mark Pictor
*************************************************************/

//pathAlgo.cpp

//algorithms for computing the toolpath
//3-axis ONLY (at least for now)


#include "pathAlgo.h"
#include <BRepOffsetAPI_MakeOffset.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopExp.hxx>
#include <BRepTools_WireExplorer.hxx>

extern QApplication* pA;

pathAlgo::pathAlgo() 
{
    //??
    //computed = false;
    //canBeComputed = false;
    safeHeightSet = false;
    safeHeight = 0;
}

pathAlgo::~pathAlgo() 
{
}

void pathAlgo::init() 
{
	//clear selected face, vector,...
	//F.Nullify();
	//computed = false;
	//canBeComputed = false;
	listOfFaces.clear();
	projectedPasses.clear();
}

/*  10sept06
void pathAlgo::slotCancel()
{
    continue_compute=false;
}
*/

void pathAlgo::AddFace(TopoDS_Face &aFace, TopoDS_Shape &theShape) 
{
	//TopoDS_Iterator faceFinder;
	uint f = 0;
	bool keepGoing = true;
		//assign a number to this face
	TopExp_Explorer Ex;
	for (Ex.Init(theShape,TopAbs_FACE); (Ex.More() && keepGoing); Ex.Next()) {
		if (Ex.Current() == aFace) {
			//get out of loop, preserving f.
			keepGoing = false;
		} else {f++;}
	} 
	if (keepGoing) {
		printf("This face not in the shape!\n");
		return;
	}

		//check the face's number against others in listOfFaces
	bool duplicate = false;
	for (uint i=0;i<listOfFaces.size();i++) {
		if (listOfFaces.at(i).faceNumber == f) {
			duplicate = true;
			puts("Duplicate");
		}
	}

	if (!duplicate) {
		mFace mf;
		mf.faceNumber = f;
		mf.F=aFace;
		mf.computed=false;
		listOfFaces.push_back(mf);
		emit addFaceToList(f);
	}
}


//simple, as in don't check if the tool is gouging/assume ball nose...
void pathAlgo::slotComputeSimplePathOnFace() 
{
	TopoDS_Shape theProjLines;	//use TopoDS_Shape, because it can hold multiple edges
	Standard_Real bboxWidth;	//width (y) of bounding box
	Standard_Real lineY;		//for computing line to project
	Standard_Real passWidth = .75;	//cutting width, one pass
	int numPasses;	//number of passes that must be made to cover surface
	Bnd_Box aBox;
	Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
	TopoDS_Shape faces;
	TopoDS_Shape lines;
	pPass proj;

	for (uint i=0;i<listOfFaces.size();i++) {
		//add each face to listOfFaces
		if (!listOfFaces.at(i).computed) {
			TopoDS_Face curFace = listOfFaces.at(i).F; 
			if (faces.IsNull()) {
				faces=curFace;
			} else {
				//wtf pita
				if (BRepAlgoAPI_Fuse(faces,curFace).BuilderCanWork()) { //not sure if this has any effect...
					faces=BRepAlgoAPI_Fuse(faces,curFace);
				} else {puts ("brepalgoapi.fuse builder failure");}
			}
			//bounding box
			BRepAdaptor_Surface aSurf(curFace);
			BndLib_AddSurface::Add(aSurf,(Standard_Real) 0.0,aBox);
			proj.facesUsed.push_back(listOfFaces.at(i).faceNumber);  //keep face number associated with the toolpath
			listOfFaces.at(i).computed = true;
		}
		emit setProgress(int(round(100.0*(float)i/(float) listOfFaces.size())),"Adding faces");
	}
	emit setProgress(-1,"Done");

	aBox.Get(aXmin,aYmin,aZmin, aXmax,aYmax,aZmax);
	//printf("%f,%f,%f\n%f,%f,%f\n",aXmin,aYmin,aZmin, aXmax,aYmax,aZmax);
	bboxWidth = aYmax-aYmin;
	if (aXmax - aXmin < .1) {
		puts("bad face, possibly vertical. ignoring it.");
		return;
	}
	Standard_Real H = aZmax + 1 + (aZmax-aZmin)/10;  //max + 10% + a bit (TODO: needs fixed)
	if ((!safeHeightSet)||(H>safeHeight))
		safeHeight = H;

	//create a series of lines "covering" bbox (in Z)
	//and project lines onto surface
	numPasses = 1 + (int)round(bboxWidth/passWidth);

	//get a reasonable number of passes (this is only for testing the program)
	while (numPasses > 200) {
		numPasses = numPasses/10;
		passWidth = passWidth*10;
	}
	while (numPasses < 20) {
		numPasses = numPasses*10;
		passWidth = passWidth/10;
	}

	printf("Number of passes %i\n Pass width %f\n", numPasses, passWidth);

	///STILL skipping some faces! -- bottom.brep
	for (int j=0;j<numPasses;j++) {
		lineY = aYmin + j*passWidth;
		gp_Dir N(0,1,0);  //Normal for plane Pl
		gp_Pln Pl(gp_Pnt(aXmin,lineY,aZmax+1),N);
		TopoDS_Shape section = BRepAlgoAPI_Section (faces,Pl);
	///offset line here.
	//section is a COMPOUND shape - explore for wires (and edges?)
		TopExp_Explorer Ex;
		for (Ex.Init(section,TopAbs_EDGE); Ex.More(); Ex.Next()) {
			TopoDS_Wire W = BRepBuilderAPI_MakeWire(TopoDS::Edge(Ex.Current()));
			offsetPair offsets;
			CLOffsetFromWire(W,passWidth*2,offsets);
			TopoDS_Shape O;
			if (!offsets.O1.IsNull())
				O = offsets.O1;
			else
				O = offsets.O2;
	///ShapeFix_Wire::FixSelfIntersection
			if (theProjLines.IsNull()) {
				theProjLines = O;
				puts("null");
			} else {
				puts("prl fuse");
				theProjLines = BRepAlgoAPI_Fuse(theProjLines,O);
			}
		}
		int prog = int(round(100*(float)j/(float)numPasses));
		emit setProgress(prog,"Calculating cuts");
	}
	proj.P=theProjLines;
	proj.displayed=false;
	projectedPasses.push_back(proj);
	emit setProgress(-1,"Done");
	emit showPath();
}

void pathAlgo::slotSelectFaceFromList(int f)
{
    //unimplemented
}

//second parm of perform does what?
void pathAlgo::CLOffsetFromWire(	TopoDS_Wire theWire, 
					Standard_Real Dist,
					offsetPair &theResult)
{
	Standard_Real LinTol = .1; //for gp_Pnt::IsEqual()
	theResult.noErrors = true;
	BRepOffsetAPI_MakeOffset offsetMaker;
	offsetMaker.Init();
	offsetMaker.AddWire(theWire);
	offsetMaker.Perform(Dist);  //second parm ???
	TopoDS_Shape Loop;
	Loop.Nullify();
	if (offsetMaker.IsDone())
		Loop = offsetMaker.Shape();
	else
		return;

	if (Loop.IsNull()) puts("loop null");
	if (theWire.IsNull()) puts("wire null");
	//if (Loop.IsNull) puts("");
	
	//find endpoints of wire, E1 E2
	TopoDS_Vertex E1,E2;//,V1,V2;
	TopExp::Vertices(theWire,E1,E2);
	if ((!E1.IsNull()) && (!E2.IsNull()) && (E1 != E2)) {

	//find two arcs whose centers are E1 and E2 - their ends go in start,end
		gp_Pnt start[2], end[2],a,b,c;
		TopoDS_Edge endArc[2];
		int numArcs = 0;
		bool isLine;
		Standard_Real circRad;
		//TopExp_Explorer loopEx;
		//for (loopEx.Init(Loop,TopAbs_EDGE); loopEx.More(); loopEx.Next()) {
		BRepTools_WireExplorer loopEx; 
		loopEx.Init(TopoDS::Wire(Loop));
		for ( ; loopEx.More(); loopEx.Next()) {
			puts("in loopex");
			edgePoints(loopEx.Current(),c,a,b,isLine,circRad);
			if (circRad != 0) {
				if ((((gp_Pnt)BRep_Tool::Pnt(E1)).IsEqual(c,LinTol)) || (((gp_Pnt)BRep_Tool::Pnt(E2)).IsEqual(c,LinTol))) {
				    if (numArcs >= 2) {
					puts("ERROR! Bad CLOffset, too many arcs!");
					theResult.noErrors = false; 
					return;
				    }
				    start[numArcs] = a;
				    end[numArcs] = b;
				    endArc[numArcs] = loopEx.Current();
				    if (a.IsEqual(b,LinTol)) 
					puts("Arc: identical endpoints!");
				    else numArcs++;
				}
			}
		}
		if (numArcs < 2) {
			puts("ERROR! Bad CLOffset, too few arcs!");
			theResult.noErrors = false; 
			if (!Loop.IsNull())
				theResult.O1 = Loop;
			else
				theResult.O1 = theWire;
			return;
		}

		//find edges in between E1,E2
		BRepBuilderAPI_MakeWire makeW[2];
		bool wireTwo = false;
		BRepTools_WireExplorer wEx;// = BRepTools_WireExplorer(TopoDS::Wire(Loop));
		wEx.Init(TopoDS::Wire(Loop));
		for ( ; wEx.More(); wEx.Next()) {
			//TopExp::Vertices(wEx.Current(),&V1,&V2);
			if ((wEx.Current() == endArc[0]) || (wEx.Current() == endArc[1])) {
				wireTwo = !wireTwo;
			} else {
				makeW[1*(wireTwo)].Add(wEx.Current());
			}
		}
		//check wires
		TopoDS_Vertex Ver;
		if (makeW[0].IsDone()) {
			theResult.O1 = makeW[0].Wire();
		} else {
			if (TopExp::CommonVertex(endArc[0],endArc[1],Ver)) {
				theResult.O1 = Ver;
			} else {
				puts("ERROR, Cannot find offset.");
				theResult.O1.Nullify();
				theResult.noErrors = false;
			}
		}
		if (makeW[1].IsDone()) {
			theResult.O2 = makeW[1].Wire();
		} else {
			if (TopExp::CommonVertex(endArc[0],endArc[1],Ver)) {
				theResult.O2 = Ver;
			} else {
				puts("ERROR, Cannot find offset.");
				theResult.O2.Nullify();
				theResult.noErrors = false;
			}
		}
		return;
	} else {
		puts("ERROR, problem with endpoints of original wire");
		theResult.noErrors = false; 
		theResult.O1.Nullify();
		theResult.O2.Nullify();
		return;
	}
}

//Takes an edge which may be a circular arc, straight line, or other.  If it is circular, returns the center as c and radius as circRad.  Otherwise, c is null and circRad = 0.  Returns ends of edge as a,b.  If E is not a line, isLine is false.
void pathAlgo::edgePoints(TopoDS_Edge E, gp_Pnt &c, gp_Pnt &a, gp_Pnt &b, bool &isLine, Standard_Real &circRad) 
{
	isLine = false;
	circRad = 0;
	Standard_Real first, last;

	Handle(Geom_Curve) C = BRep_Tool::Curve(E,first,last);
	BRepAdaptor_Curve adaptor = BRepAdaptor_Curve(E);
	if (adaptor.GetType() == GeomAbs_Circle) {
		gp_Circ circ = adaptor.Circle();
		c = circ.Location();
		circRad = circ.Radius();
		puts ("circle");
	} else if (adaptor.GetType() == GeomAbs_Line) {
		isLine = true;
	}
	a=C->Value(first);
	b=C->Value(last);
}
