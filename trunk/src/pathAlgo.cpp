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

pathAlgo::pathAlgo() {
    //??
    computed = false;
    canBeComputed = false;
    safeHeightSet = false;
    safeHeight = 0;
}

pathAlgo::~pathAlgo() {
}

void pathAlgo::init() {
	//clear selected face, vector,...
	F.Nullify();
	computed = false;
	canBeComputed = false;
	listOfFaces.clear();
	projectedLines.clear();
}

void pathAlgo::slotCancel()
{
    continue_compute=false;
}

void pathAlgo::SetFace(TopoDS_Face &aFace) {
	F = aFace;
	listOfFaces.push_back(F);
	computed = false;
	canBeComputed = true;
	//puts("pathAlgo face set.\n");
}

//compute simple path on most recently selected face (F).
//simple, as in don't check if the tool is gouging/assume ball nose...
void pathAlgo::slotComputeSimplePathOnFace() {
	projectedLines.clear();
	Standard_Real bboxWidth;	//width (y) of bounding box
	Standard_Real lineY;		//for computing line to project
	Standard_Real passWidth = .75;	//cutting width, one pass
	int numPasses;	//number of passes that must be made to cover surface
	TopoDS_Iterator wireIterator;

	if ((!computed) && (canBeComputed)) {
		//face is stored in global F
	//compute bounding box
		Bnd_Box aBox;
		BRepAdaptor_Surface aSurf(F);
  		BndLib_AddSurface::Add(aSurf,(Standard_Real) 0.0,aBox);
  		Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  		aBox.Get(aXmin,aYmin,aZmin, aXmax,aYmax,aZmax);
		bboxWidth = aYmax-aYmin;
		if (aXmax - aXmin < .1) {
			puts("bad face, possibly vertical. ignoring it.");
			return;
		}
		Standard_Real H = aZmax + (aZmax-aZmin)/10;  //max + 10%
		if ((!safeHeightSet)||(H>safeHeight))
			safeHeight = H;

	//create a series of lines "covering" bbox (in Z)
	//and project lines onto surface
		numPasses = 1 + (int)round(bboxWidth/passWidth);

		//get a reasonable number of passes
		while (numPasses > 200) {
			numPasses = numPasses/10;
			passWidth = passWidth*10;
		}
		while (numPasses < 20) {
			numPasses = numPasses*10;
			passWidth = passWidth/10;
		}

		printf("Number of passes %i\n Pass width %f\n", numPasses, passWidth);


		for (int i=0;i<=numPasses;i++) {
			lineY = aYmin + i*passWidth;
			TopoDS_Edge lineToProject = BRepBuilderAPI_MakeEdge( gp_Pnt(aXmin,lineY,aZmax+1), gp_Pnt(aXmax,lineY,aZmax+1) );

			//compute result
			TopoDS_Shape projShape = BRepProj_Projection( lineToProject, F, gp_Dir(0,0,-1)).Shape();
			if (!projShape.IsNull()) {
				if (projShape.ShapeType()==TopAbs_EDGE) {
					//store in vector projectedLines.
					projectedLines.push_back(TopoDS::Edge(projShape));
				} else {  //break whatever-it-is down, add any edges
					for(wireIterator.Initialize(projShape); wireIterator.More();wireIterator.Next())
					{
						TopoDS_Shape S = wireIterator.Value();
						if (S.ShapeType()==TopAbs_EDGE) {
							projectedLines.push_back(TopoDS::Edge(S));
						}
					}
				}
			}
		}
	computed = true;
	canBeComputed = false;
	emit showPath();
	}
}


//saves g-code to a file
//originally, it was not going to print out usable g-code
void pathAlgo::slotOutputProtoCode() {
	int numLines = projectedLines.size();
	TopoDS_Iterator lineIterator;
	bool odd = true;		//used to flip every other curve around (see unidirectional)
	bool unidirectional = true;	//false -> back-and-forth, true -> cut one direction only
	TopLoc_Location loc;	//transform used for edge
	Standard_Real first, last, tmp;  //parameters at start/end of curve created from edge, tmp is used to swap them
	FILE *outG;

	if (!computed) {
		QMessageBox::warning( 0, "Warning", "Compute path first !");
		return;
	}
	QString fileName = QFileDialog::getSaveFileName(QString::null, "G-code (*.ngc)", 0, 0);
	outG = fopen ((const char*)fileName, "w");
	if (outG == NULL) {
		QMessageBox::warning( 0, "Warning", "Can't open that file for writing!");
		return;
	}
	
	//g-code to place at beginning of file
	fprintf(outG,"(File %s created by cam-occ)\n",(const char*)fileName);
			//ought to insert model's file name and the face ID into comment also...
	fprintf(outG,"#101 = %f (Safe height for rapids)\n",safeHeight);  //safeHeight set in slotComputeSimplePathOnFace() above
	fprintf(outG,"M09 M05 M48 G17 G21 G40\nG54 G64 G90 G92.2 G94\nM06 T1 G43 H1\n");  //reset everything, tool change-tool 1
	fprintf(outG,"G00 Z#101\nX0.0 Y0.0\n");  	//rapid to safeZ, then to x0y0
	fprintf(outG,"M03 S1000 F100 M08\n\n");  		//spindle CW 1000rpm feed 100 flood cool

	//this loop processes the blue lines drawn on the face
	for(int i=0;i < numLines; i++)
	{
		bool betweenCuts = true;  	//used to determine when to rapid
		TopoDS_Edge E = projectedLines.at(i);
		Handle(Geom_Curve) C = BRep_Tool::Curve(E,loc,first,last);
		C->Transform(loc.Transformation());  //transform C using loc
		if (odd & !unidirectional) {
			odd = false;
			tmp = last;
			last = first;
			first = last;
		} else {
			odd = true;  //for next time around
		}
		for (Standard_Real j=first;j<=last;j=j+(last-first)/100) {
			gp_Pnt pt = C->Value(j);
			if (betweenCuts) {
				fprintf(outG,"G00 X%f Y%f\nG01 Z%f\n",pt.X(),pt.Y(),pt.Z());
				betweenCuts = false;
			} else {
				fprintf(outG,"G01 X%f Y%f Z%f (curve %i parm %f)\n",pt.X(),pt.Y(),pt.Z(),i,j);
			}
		}
		fprintf(outG,"G01 Z#101 G4 P0\n");  //feedrate move to safe height (var #101)
	}

	/*TODO:
	*  create struct for motion type, origin, and destination
	*  motion type -- rapid, linear, arc, ... (only makes sense if not outputting g-code directly)
	*/

	//is it possible to recognize lines and arcs, to make better g-code?  Dump equation coefficients, see if it's a line or circle?

	fprintf(outG,"M5\n%%\n");
	fclose (outG);
}
