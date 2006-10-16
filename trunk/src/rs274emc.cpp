/*************************************************************
** License: GPL.  
** Copyright (C) 2006 Mark Pictor
*************************************************************/

//rs274emc.cpp
//output g-code
#include "rs274emc.h"

rs274emc::rs274emc()
{
}

rs274emc::~rs274emc()
{
}

//saves g-code to a file
//originally, it was not going to print out usable g-code
/*  NEEDS MODIFICATION to handle change from projectedLines to projectedPasses
ALSO split into smaller functions

void rs274emc::slotOutputProtoCode() {
	int numLines = projectedPasses.size();
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
	//we will have to use an explorer to decompose the shape to find the lines to trace...
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

	

	//is it possible to recognize lines and arcs, to make better g-code?  Dump equation coefficients, see if it's a line or circle?

	fprintf(outG,"M5\n%%\n");
	fclose (outG);
} */


