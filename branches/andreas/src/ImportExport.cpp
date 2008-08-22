/*************************************************************
** License: GPL.  
** Derived from work by Olivier Coma. 
** Copyright (C) 2006 Mark Pictor
*************************************************************/

// ImportExport.cpp: implementation of the CImportExport class.
//
//////////////////////////////////////////////////////////////////////
#include "ImportExport.h"
#include <qmessagebox.h>
//======================================================================
//=                                                                    =
//=                      BREP                                          =
//=                                                                    =
//======================================================================

Standard_Boolean ImportExport::ReadBREP(const Standard_CString &aFileName,TopoDS_Shape& aShape)
{
    printf("Standard_Boolean ImportExport::ReadBREP\n");//AK
    BRep_Builder aBuilder;
    Standard_Boolean result = BRepTools::Read(aShape,aFileName,aBuilder);
    return result;
}


IFSelect_ReturnStatus ImportExport::ReadSTEP(const Standard_CString& aFileName,Handle_TopTools_HSequenceOfShape& aHSequenceOfShape)
{
    printf("IFSelect_ReturnStatus ImportExport::ReadSTEP\n");//AK
    TopoDS_Shape aShape;
     STEPControl_Reader aReader;
     IFSelect_ReturnStatus status = aReader.ReadFile(aFileName);

     if (status == IFSelect_RetDone)
     {
        Standard_CString LogFileName = "ReadStepFile.log";
        Standard_Boolean failsonly = Standard_False;
        aReader.PrintCheckLoad (failsonly, IFSelect_ItemsByEntity);
        // Root transfers
        Standard_Integer nbr = aReader.NbRootsForTransfer();
        aReader.PrintCheckTransfer (failsonly, IFSelect_ItemsByEntity);

        for (Standard_Integer n = 1; n<= nbr; n++)
           {
	  Standard_Boolean ok = aReader.TransferRoot(n);
 	 // Collecting resulting entities
 	  Standard_Integer nbs = aReader.NbShapes();
	if (nbs == 0)
 	    {
 	  	aHSequenceOfShape.Nullify();
		return IFSelect_RetVoid;
 	    }
 	 else
 	    {
 	    	for (Standard_Integer i =1; i<=nbs; i++)
 	                {
		    aShape=aReader.Shape(i);
		    aHSequenceOfShape->Append(aShape);
		  }
  	      }
	  if (!ok) {LogFileName = "";} //supress unused variable warnings
            }
        }
      else
            aHSequenceOfShape.Nullify();
      
      return status;
       
}

Standard_Boolean ImportExport::ReadSTEP(const Standard_CString &aFileName,TopoDS_Shape& aShape)
{
    printf("Standard_Boolean ImportExport::ReadSTEP\n");//AK
    Handle(TopTools_HSequenceOfShape) aSequence= new TopTools_HSequenceOfShape();
    IFSelect_ReturnStatus ReturnStatus = ReadSTEP(aFileName,aSequence);
    bool msg=false;
    
    switch (ReturnStatus) 
    {
       case IFSelect_RetError :
	QMessageBox::information( 0, "Import Step", "Not a valid Step file");
	msg=true;
	break;
       
       case IFSelect_RetFail :
	   QMessageBox::information( 0, "Import Step", "Reading has failed");
	   msg=true;
	   break;
       
       case IFSelect_RetVoid :
	   QMessageBox::information( 0, "Import Step", "Nothing to transfer");
	   msg=true;
	   break;
       
       default:
	   break;
       }
    
    if (!msg)
       {
	int i;
	for (i=1 ; i<=aSequence->Length() ; i++)
	    {
	    TopoDS_Shape S=aSequence->Value(i);
	    TopAbs_ShapeEnum type=S.ShapeType();
			
	    if (type==TopAbs_COMPSOLID || type==TopAbs_SOLID)
		{
		aShape=S;
		break;
		}
	        }
	}
    return (!msg);
}

    

