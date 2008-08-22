/*************************************************************
** License: GPL.  
** Derived from work by Olivier Coma.
** Derived from work by Mark Pictor. 
** Copyright (C) 2007 Andreas Kain
*************************************************************/

//pathAlgo.h

//algorithms for computing the toolpath
//3-axis ONLY (at least for now)

#ifndef PATHALGO_H
#define PATHALGO_H

//qt
#include <qobject.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <vector>

//occ
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

#include <BRep_Tool.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Section.hxx>

#include <BRepOffsetAPI_MakeOffsetShape.hxx>//added by AK
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepAdaptor_Surface.hxx>

#include <gp_Pln.hxx>
#include <Bnd_Box.hxx>
#include <BndLib_AddSurface.hxx>
#include <Geom_Curve.hxx>
#include <GC_MakeSegment.hxx>
#include <GCE2d_MakeSegment.hxx>
#include <BRepOffset_MakeOffset.hxx> // added by AK

#include <math.h>


#include "selectPatch.h"
#include "cuttingParameter.h"

class pathAlgo : public QObject
{
    Q_OBJECT
    
public:
 
  pathAlgo(selectPatch *Patchtest);
  //virtual ~pathAlgo();
      ~pathAlgo();

      selectPatch *Patch;
      
  
  private:
   
      
  protected:
      Standard_Real safeHeight;// AK	height for rapids
      bool safeHeightSet; //AK
      float maxPassWidth; //AK
   
      typedef struct {
	  bool computed;
	  uint faceNumber;
	  TopoDS_Face F;	
      } mFace;  //machinable face - i.e. a face in listOfFaces
      
      typedef struct {
	  bool displayed;
	  vector<uint> facesUsed;  //when computed, store each faceNumber (from mFace) //AK Vector is a dynamic array
	  TopoDS_Shape P;
      } pPass;  //set of lines that were projected at one time, onto one or more faces
      
      typedef struct {
	  bool noErrors;
	  TopoDS_Shape O1,O2;
      } offsetPair;  //a pair of offsets as returned by CLOffsetFromWire

      
public:
    vector<mFace> listOfFaces;
    vector<pPass> projectedPasses;   //now, one shape per pass.
    pPass proj;//AK
    TopoDS_Shape theProjLines;//AK
    
    typedef struct {
	gp_Pnt A,B,C,D;
    } vertexPoints; //AK
    
    vector<vertexPoints> listOfVertexPoints;//AK
    void AddFace(TopoDS_Face &aFace, TopoDS_Shape &theShape);
    void edgePoints(TopoDS_Edge E, gp_Pnt &c, gp_Pnt &a, gp_Pnt &b, bool &isLine, Standard_Real &circRad);
    
    bool pathComputed;//by AK
    bool staticVaultPlane(int switcher); //by AK for vaulting selected value   
    int staticVaultBorder(int switcher); //by AK for vaulting selected value
    void staticVaultVertices (int switcher, gp_Pnt point, vertexPoints &points);//AK for vaulting vertices of selected shape to calculate tool path
    void getVertices (TopoDS_Shape shape);//AK
    void createCuttingLines(bool xzPlane, bool yzPlane, int numPasses, Standard_Real aYmin, Standard_Real aYmax,Standard_Real aXmin, Standard_Real aXmax,Standard_Real passWidth, Standard_Real aZmax,TopoDS_Shape faces);//AK
    void connectVertexPoints(bool xzPlane, bool yzPlane);//AK
    void cleanPathBuffer();//AK
    bool comparePoints(gp_Pnt a,gp_Pnt b);//AK

    
public slots:
    void slotInit();
    void slotComputeSimplePathOnFace();
    void slotSelectFaceFromList(int f);
    void slotToggleXzYz(bool toggled);//AK switching planes for cutting
    void slotToggleXpos(bool xPos);//AK
    void slotToggleXneg(bool xNeg);//AK
    void slotToggleYpos(bool yPos);//AK
    void slotToggleYneg(bool yneg);//AK
    void slotAddFace(TopoDS_Face aFace);//AK
   
    
signals:
    void setComputed(int);// AK
    void sendSafeHeight(Standard_Real);//  AK
    void showPath();
    void setProgress(int p=-1, char* status="Ready");
    void addFaceToList(uint f);
    void setDisplayedPathFalse();
    void activateRS274();
    void sendFeed(int);//AK
    void sendSpeed(int);//AK
    void sendToolDia(int);//AK
    
  
private:
     void CLOffsetFromShape(TopoDS_Shape &faces, Standard_Real Dist);//AK using BRepOffsetAPI_MakeOffsetShape.hxx
};

#endif //PATHALGO_H
