/*************************************************************
** License: GPL.  
** Copyright (C) 2007 Andreas Kain
*************************************************************/

//autoSelect.cpp
//auto selection of patches , order the faces properly, thus they are subsequently!!

#include "selectPatch.h"


selectPatch::selectPatch()
{   
}

selectPatch::~selectPatch()
{
}


void selectPatch::slotGetPatches(TopoDS_Shape aShape) 
	//Find all faces to proceed workking on: bottom levels of pockets (horizontal, depth -6)
{ 
    emit setProgress(-1,"Choose Pocket bottom faces");
    Faces.clear();// clear vector
    FacesSorted.clear();// clear vector
    
    Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;// maximal extensions of face 
    Standard_Real absXmin, absYmin, absZmin, absXmax, absYmax, absZmax;// maximal extensions of shape
    Bnd_Box aBoxForShape; // Bounding Box for determination of x,y,z maximal dimensions of the taken face aFace
   
    TopExp_Explorer ExFace; //openCASCADE Explorer
  
    //Standard_Real depth=-6; // pocket depth  
    
    TopoDS_Face aFace;
    
     // get maximal extensions of the ppart in along z-axis for automated pocket feature recognition
    BRepBndLib::Add(aShape,aBoxForShape);
    aBoxForShape.SetGap(0.0);
    aBoxForShape.Get(absXmin, absYmin, absZmin, absXmax, absYmax, absZmax);
    
    //retrieve faces out of shape and evaluate, if they build the pocket feature
    for (ExFace.Init(aShape,TopAbs_FACE); ExFace.More();ExFace.Next()){ 
	aFace = TopoDS::Face(ExFace.Current()); //Take face from a Shape and analyze it
	Bnd_Box aBox; // Bounding Box for determination of x,y,z maximal dimensions of the taken face aFace
	
	//create a box comprising the face to get maximal extensions
	 BRepAdaptor_Surface aSurfaceSelected(aFace);
	 BndLib_AddSurface::Add(aSurfaceSelected,(Standard_Real) 0.0,aBox);   
	 aBox.Get(aXmin,aYmin,aZmin, aXmax,aYmax,aZmax); //retrieve dimensions from Bounding Box
	 cout<<"aXmin ="<<aXmin<<"; aXmax="<<aXmax<<"; aYmin="<<aYmin<<"; aYmax="<<aYmax<<"; aZmin="<<aZmin<<"; aZmax="<<aZmax<<endl;
	
	//add face to vector, if it is part of the pocket bottom
	 //	evaluate zmin and zmax; pocket is built by the faces that are not at zmin and zmax
	if ((aZmin==aZmax)&&(aZmax!=absZmax)&&(aZmin!=absZmin) ) {
	    cout<<"Faces.push_back(aFace)"<<endl;
	Faces.push_back(aFace);
                }
    }
    cout<<"Faces.size("<<Faces.size()<<")"<<endl;
  
    // check if there are faces found
    if (!Faces.size()){ 
	puts("Warning, No patches at specifc depth (-6mm) detected");
	QMessageBox::warning( 0, "Warning", "No patches at specifc depth (-6mm) detected");
	return;
    }
    
    sortFaces(); // sort selected Faces in appropriate order  
    return;
}


void selectPatch::sortFaces()
	// sort pocket bottom faces in appropriate order (subsequent faces)
	// take the first one, take the second one and compare identical points, if ==2 then take the second face, if ==2 && ==4 face is already taken
{
    emit setProgress(-1,"Sort faces in appropriate order");
    
    TopExp_Explorer ExVertexToCheck, ExVertexSorted; // openCASCADE Explorer
    int amElementsFaces=Faces.size(); // the amount of detected faces on the bottom of the pocket
    int amElementsFacesSorted=0; // the amount of faces that are already sorted, if identical to amount of  detected faces-> all faces in proper order
    gp_Pnt pointArray[8]; // when retrieving vertices, a square provides 8, 4 edges= 8 vertixes. filtering is necessary to eleminate doubled points
    gp_Pnt pointArrayFiltered[4];  	
    gp_Pnt pointArraySorted[ amElementsFaces][8];
    gp_Pnt pointArraySortedFiltered[ amElementsFaces][4];
    TopoDS_Face tempFace, tempFaceSorted;
    
    // transfer first face
    FacesSorted.push_back(Faces.at(0)); 
    
    // if there are any more faces recognized then sort them
    if (Faces.size()>1){
	//	while there are still faces not sorted
	while (amElementsFacesSorted < amElementsFaces){
	    
	    // 	import following faces one by one
	    for(uint k=1; k<Faces.size();k++){
		//cout<<"1"<<endl;
		tempFace=Faces.at(k);
		int i=0;
		
		// extract vertex points out of face and store them in array - array is easier to handle than vector
		for (ExVertexToCheck.Init(tempFace,TopAbs_VERTEX); ExVertexToCheck.More();ExVertexToCheck.Next()){
		    TopoDS_Vertex Vt=TopoDS::Vertex(ExVertexToCheck.Current());
		    pointArray[i]=BRep_Tool::Pnt(Vt); // i referrs tp points
		    i++;
		    //cout<<"2"<<endl;
		}
		
		//+++++++++++++++++++++++ 
		// 8 points, delete 4
		
		//set start value
		pointArrayFiltered[0]=pointArray[0];
		
		//compare it with all the other values, take a different one
		for (int i=0; i<8; i++){
		    if (!comparePoints(pointArrayFiltered[0],pointArray[i]))
			pointArrayFiltered[1]=pointArray[i];
		}
		
		//take the two values and take another different one 
		for (int i=0; i<8; i++){
		    if (!comparePoints(pointArrayFiltered[0],pointArray[i]) && (!comparePoints(pointArrayFiltered[1],pointArray[i])))
			pointArrayFiltered[2]=pointArray[i];
		}
		
		//take the three values and take another different one
		for (int i=0; i<8; i++){
		    if (!comparePoints(pointArrayFiltered[0],pointArray[i]) && (!comparePoints(pointArrayFiltered[1],pointArray[i])) && (!comparePoints(pointArrayFiltered[2],pointArray[i])))
			pointArrayFiltered[3]=pointArray[i];
		}
		
		//display content
		for (int w=0; w<4;w++){
		    cout<<"pointArrayFiltered["<<w<<"].X() = "<<pointArrayFiltered[w].X()<<"  .Y() = "<<pointArrayFiltered[w].Y()<<"  .Z() = "<<pointArrayFiltered[w].Z()<<endl;
		}
		
		//+++++++++++++++++++++++ 
		
		
		//extract vertex points out of Vector FacesSorted
		for(int k=0; k<FacesSorted.size();k++){// import faces from FacesSorted
		    //cout<<"3"<<endl;
		    tempFaceSorted=FacesSorted.at(k);
		    
		    int l=0;
		    
		    for (ExVertexSorted.Init(tempFaceSorted,TopAbs_VERTEX); ExVertexSorted.More();ExVertexSorted.Next()){// calculate Vertex points and save to array
			TopoDS_Vertex Vt=TopoDS::Vertex(ExVertexSorted.Current());
			pointArraySorted[k][l]=BRep_Tool::Pnt(Vt); // l is 1 to 4 referrs to points, k refers to faces
			l++;
			//cout<<"4"<<endl;
		    }
		}
		
		
		//+++++++++++++++++++++++ 
		// 8 points, delete 4
		
		//set start value
		
		for(int k=0; k<FacesSorted.size();k++){
		    pointArraySortedFiltered[k][0]=pointArraySorted[k][0];
		    
		    //compare it with all the other values, take a different one
		    for (int i=0; i<8; i++){
			if (!comparePoints(pointArraySortedFiltered[k][0],pointArraySorted[k][i]))
			    pointArraySortedFiltered[k][1]=pointArraySorted[k][i];
		    }
		    
		    //take the two values and take another different one 
		    for (int i=0; i<8; i++){
			if (!comparePoints(pointArraySortedFiltered[k][0],pointArraySorted[k][i]) && (!comparePoints(pointArraySortedFiltered[k][1],pointArraySorted[k][i])))
			    pointArraySortedFiltered[k][2]=pointArraySorted[k][i];
		    }
		    
		    //take the three values and take another different one
		    for (int i=0; i<8; i++){
			if (!comparePoints(pointArraySortedFiltered[k][0],pointArraySorted[k][i]) && (!comparePoints(pointArraySortedFiltered[k][1],pointArraySorted[k][i])) && (!comparePoints(pointArraySortedFiltered[k][2],pointArraySorted[k][i])))
			    pointArraySortedFiltered[k][3]=pointArraySorted[k][i];
		    }	
		}
		
		//display content
		for (int v=0; v<FacesSorted.size();v++){
		    for (int w=0; w<4;w++){
			cout<<"pointArraySortedFiltered["<<v<<"] ["<<w<<"].X() = "<<pointArraySortedFiltered[v][w].X()<<"  .Y() = "<<pointArraySortedFiltered[v][w].Y()<<" .Z() = "<<pointArraySortedFiltered[v][w].Z()<<endl;
		    }
		}
		//+++++++++++++++++++++++++++
		
		
		// compare if there is already a face in FacesSorted, with 2 identical points of the current face
		int counter=0;
		bool ident=0;
		bool contact=0;
		
		for (int o=0;o<FacesSorted.size();o++){// amount faces in total in FacesSorted
		    for (int p=0; p<4; p++){	  
			for (int q=0; q< 4; q++){
			    if (comparePoints(pointArrayFiltered [q], pointArraySortedFiltered[o][p])){// if identical return is true!
				counter++;
				cout<<"counter="<<counter<<endl;
			    }
			}
		    }
		    
		    // evaluate counter2
		    if (counter==2){   
			contact=1; // 2 identical points
			cout<<"contact="<<contact<<endl;
		    }
		    
		    if (counter==4){	    
			ident=1; // 4 identical points -> face is already contained in the vector FacesSorted
			cout<<"ident="<<ident<<endl;
		    }
		    
		    counter=0;
		}
		
		//  evaluate contact (2 identical points) and not ident (4 identical points) in a face
		if((contact)&&(!ident)){
		    FacesSorted.push_back(tempFace);  
		    cout<<"FacesSorted.push_back(tempFace);"<<endl;   
		    cout<<"FacesSorted.size("<<FacesSorted.size()<<")"<<"   Faces.size("<<Faces.size()<<")"<<endl;	
		    amElementsFacesSorted=FacesSorted.size(); 
		}
		
		if (amElementsFacesSorted==amElementsFaces)	 
		    break;
	    }	
	}
    }      
    cout<<"FacesSorted.size("<<FacesSorted.size()<<")"<<endl;
    return;   
}

      
void selectPatch::slotActivateFaceTransfer()
	//send stored faces piecewise activated by GUI interface	
{
    // check wehter there are faces to transfer or not
    if (!FacesSorted.size()){
	puts("No faces selected and sorted, FacesSorted.size() == 0");	
	QMessageBox::warning( 0, "Warning", "No faces selected and sorted, FacesSorted.size() == 0");
	return;
    }
      
    // transfer faces to interactive.cpp  
    for(uint k=0; k<FacesSorted.size();k++){
	cout<<"sendFace(Faces.at("<<k<<")"<<endl;
	sendAddFace(FacesSorted.at(k));
    }
    return;	    
}


bool selectPatch::comparePoints(gp_Pnt a,gp_Pnt b)
	// used to compare points in openCASCADE format gp_Pnt; if identical return value 1
{
    bool result=0, xIdent=0, yIdent=0, zIdent=0;
    
    xIdent=!(a.X()-b.X());
    yIdent=!(a.Y()-b.Y());
    zIdent=!(a.Z()-b.Z());
    
    if (xIdent && yIdent && zIdent)
	result=1;// if Points compared are identical in X, Y, Z coordinates
    return result;
}

	

