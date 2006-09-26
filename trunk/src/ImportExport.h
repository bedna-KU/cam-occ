/*************************************************************
** License: GPL.  
** Derived from work by Olivier Coma. 
** Copyright (C) 2006 Mark Pictor
*************************************************************/

// ImportExport.h: interface for the CImportExport class.
//
//////////////////////////////////////////////////////////////////////

#ifndef IMPORTEXPORT_H
#define IMPORTEXPORT_H

//BREP
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>

//STEP
#include <STEPControl_Controller.hxx>
#include <STEPControl_Reader.hxx>
#include <STEPControl_Writer.hxx>

//General
#include <Standard_CString.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopAbs.hxx>
#include <TopoDS_Shape.hxx>

class ImportExport
{
public:

	ImportExport() {};
	virtual ~ImportExport(){};
public :

    //======================================================================
    static Standard_Boolean ReadBREP(const Standard_CString &aFileName,
                                    TopoDS_Shape& aShape);

    //======================================================================
		static IFSelect_ReturnStatus ReadSTEP(const Standard_CString& aFileName,
                                              Handle_TopTools_HSequenceOfShape& aHSequenceOfShape);
    static Standard_Boolean ReadSTEP(const Standard_CString &aFileName,
                                    TopoDS_Shape& aShape);
};

#endif
