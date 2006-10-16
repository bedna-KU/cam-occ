/*************************************************************
** License: GPL.  
** Copyright (C) 2006 Mark Pictor
*************************************************************/

//rs274emc.h

//output emc-compatible g-code (and POSSIBLY import it as well)

#ifndef RS274EMC_H

#include "pathAlgo.h"

#include <qobject.h>
#include <vector>
#include <TopoDS_Iterator.hxx>
#include <TopLoc_Location.hxx>
#include <Standard_Real.hxx>
#include <qmessagebox.h>
#include <qfiledialog.h>


class rs274emc: public QObject
{
	Q_OBJECT

public:
	rs274emc();
	~rs274emc();
	void slotOutputProtoCode();  //this is temporary and will be replaced
	void linesToIntermediateCode();
	void optimizeIntermediateCode();
	void intermediateToNC();


	typedef struct {
		int x,y,z;
	} gPoint;
	typedef struct {
		enum {rapid,linearFeed,leftArcFeed,rightArcFeed,other} motionType;  //other is a special case
		gPoint start,finish;  //better to use gp_Pnt?
		bool notNewLine; //only used for special case
		int mgCodes[10];  //Code value *10 so decimal codes (G61.1) can be stored in here.  Order does not matter.  When adding codes, put in first array cell that IS zero (G0, if specified, is in motionType, so it won't interfere).  If value is <10000, it's a G code.  If value >=10000 and <20000, M code.  Otherwise, it is invalid.
		float F,H,I,J,K,L,P,Q,R,S;  //ignored for special case

		/*SPECIAL CASE, motionType 'other':
		purpose: make it easy to add more codes to a line, without taking up huge amounts of memory.
		behavior:
		start,finish are ignored.  If notNewLine is true, Gcodes generated from this struct are placed on the same line as those from the previous struct.
		F,H,I,J,K,L,P,Q,R,S are ignored if zero.  Otherwise, they are used on the current g-code line.

		*/
	} intermediateStruct;
	vector<intermediateStruct> intermediateData;





};
#endif //RS274EMC_H
