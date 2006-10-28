/*************************************************************
** License: GPL.  
** Derived from work by Olivier Coma. 
** Copyright (C) 2006 Mark Pictor
*************************************************************/

//pathAlgo.cpp

//algorithms for computing the toolpath
//3-axis ONLY (at least for now)


#include "pathAlgo.h"

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
		if (theProjLines.IsNull()) {
			theProjLines = section;
		} else {
			theProjLines = BRepAlgoAPI_Fuse(theProjLines,section);
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

