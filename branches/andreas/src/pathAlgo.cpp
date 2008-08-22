/*************************************************************
** License: GPL.  
** Derived from work by Olivier Coma. 
** Derived from work by Mark Pictor. 
** Copyright (C) 2007 Andreas Kain
*************************************************************/

//pathAlgo.cpp

//algorithms for computing the toolpath, tool approach in z-direction


#include "pathAlgo.h"

#include "occview.h" //added by AK for Screen output of offset shape
#include "interactive.h" //added by AK for Screen output of offset shape

extern QApplication* pA;

pathAlgo::pathAlgo(selectPatch *Patchtest) 
{
    safeHeightSet = false;
    safeHeight = 0;
    pathComputed=false;
    Patch=Patchtest;
}

pathAlgo::~pathAlgo() 
{
}
 //AK++++++++++++++++++++++++++++++++++
void pathAlgo::slotInit()
	// initialize variables etc
{
   
    
    int checkBorder=0;
    int switcherBorder=4;
  
    checkBorder= (staticVaultBorder( switcherBorder));
   
    pathComputed=false;
    
    
    
    bool checkList=false;
    bool checkProjected=false;
    bool checkProjFacesUsed=false;
    bool checkProjP=false;
    
    listOfVertexPoints.clear();
    
     //projectedPasses
    cout<<"projectedPasses.size ="<<projectedPasses.size()<<endl;
    projectedPasses.clear();
    cout<<"projectedPasses.size ="<<projectedPasses.size()<<endl;  
    checkProjected = projectedPasses.empty();
    cout<<"projectedPasses (1 == empty) = "<< checkProjected<<endl;
    
    //listOfFaces
    listOfFaces.clear();
    checkList = listOfFaces.empty();
    cout<<"listOfFaces (1 == empty) = "<< checkList<< endl;
    
    //proj.facesUsed
   proj.facesUsed.clear();
   checkProjFacesUsed = proj.facesUsed.empty();
   cout<<"proj.facesUsed (1 == empty) = "<<checkProjFacesUsed<<endl;
   
   //proj.P
   proj.P.Nullify();
   checkProjP=proj.P.IsNull();
   cout<<"proj.P.Nullify() (1== empty) = "<<checkProjP<<endl;
   
   //theProjLines
   //deletes lines in memory
   bool checkTheProjLines=false;
   theProjLines.Nullify();
   checkTheProjLines=theProjLines.IsNull();
   cout<<"proj.P.Nullify() (1== empty) = "<<checkTheProjLines<<endl;
}


void pathAlgo::slotToggleXzYz(bool toggled)
	// switching planes for cutting to XZ
{
    bool checkVault;
    int switcher=0;
    if (toggled)
	switcher=2;//write xzPlaneSelected=true
    if (!toggled)
	switcher=1;
    
    cout<<"pathAlgo::slotSetXZactive()" <<endl;
    checkVault = (staticVaultPlane(switcher));
    return;
}

void pathAlgo::slotToggleXpos(bool xPos)
	// switching planes for cutting
{
    bool checkPlane;
    int checkBorder;
    int switcherPlane=0;
    int switcherBorder=0;
    if (xPos){
	switcherPlane=1;//write xzPlaneSelected=true
	switcherBorder=1;//set positive Border active
	    }
        
    checkPlane = (staticVaultPlane(switcherPlane));
    checkBorder= (staticVaultBorder( switcherBorder));
    return;
}

void pathAlgo::slotToggleXneg(bool xNeg)
	// switching planes for cutting
  {
      bool checkPlane;
      int checkBorder;
      int switcherPlane=0;
      int switcherBorder=0;
    
      if (xNeg){
	  switcherPlane=1;//write xzPlaneSelected=true
	  switcherBorder=2;// set negative Border active
      }

    checkPlane = (staticVaultPlane(switcherPlane));
    checkBorder= (staticVaultBorder( switcherBorder));
    return;
  }
  
  void pathAlgo::slotToggleYpos(bool yPos)
	  // switching planes for cutting
  {
      bool checkPlane;
      int checkBorder;
    int switcherPlane=0;
    int switcherBorder=0;
    if (yPos){
	switcherPlane=2;//write xzPlaneSelected=false
	switcherBorder=1;//set positive Border active
    }

    checkPlane = (staticVaultPlane(switcherPlane));
    checkBorder= (staticVaultBorder( switcherBorder));
    return;
  }
  
  void pathAlgo::slotToggleYneg(bool yNeg)
	  // switching planes for cutting
  {
      bool checkPlane;
      int checkBorder;
    int switcherPlane=0;
    int switcherBorder=0;
    
    if (yNeg){
	switcherPlane=2;//write xzPlaneSelected=false
	switcherBorder=2;// set negative Border active
    }

    checkPlane = (staticVaultPlane(switcherPlane));
    checkBorder= (staticVaultBorder( switcherBorder));
    return;
  }
  
  
bool pathAlgo::staticVaultPlane(int switcher)
	//is to switch between xz parallel cutting planes	
{
    //switcher ==  1  : write true == xz plane active
    //switcher == 2  : write false == yz plane active
    //switcher == 3 :  read
    //switcher == 4: set initial
    
     static bool xzPlaneSelected=true; //initial value
     switch(switcher){
     case 1:// set xz plane active
	xzPlaneSelected=true;
	cout<<"xzPlaneSelected " <<xzPlaneSelected<<endl;
	emit setProgress(-1,"xz plane active");
	break;
    case 2://set yz plane active
	xzPlaneSelected=false;
	cout<<"xzPlaneSelected" <<xzPlaneSelected<<endl;
	emit setProgress(-1,"yz plane active");
	break;
    default:
	cout<<"read selected Plane" <<endl;
	break;
    }
    
   return xzPlaneSelected;   	
}

int pathAlgo::staticVaultBorder(int switcher)
	//get border for different types of chunks (isolated and related)
{  
     static int BorderActive=0; //initial value, -1 neg border +1 pos border  
     switch(switcher){
     case 1:// set pos Border active
	BorderActive=1;
	cout<<"posBorderActive = 1 yes: " <<BorderActive<<endl;
	emit setProgress(-1,"xz plane active");
	break;
    
    case 2://set negative Border active
	BorderActive=-1;
	cout<<"negBorderActive = -1 yes:" <<BorderActive<<endl;
	emit setProgress(-1,"yz plane active");
	break;
	 
    case 4://set negative Border active
	BorderActive=0;
	cout<<"negBorderActive = -1 yes:" <<BorderActive<<endl;
	emit setProgress(-1,"yz plane active");
	break;
    default:
	cout<<"read selected Border" <<endl;
	break;
    }
     
   return BorderActive;   
}

void pathAlgo::getVertices (TopoDS_Shape shape)
	// input a rectangular face and retrieve vertice points
{
    TopExp_Explorer Ex;
    TopoDS_Vertex Vertex;
    gp_Pnt pointValue;
    vertexPoints points;//just dummy
    int switcher=1;//write points
    
    //to find all Vertices in Shape shape
    for(Ex.Init(shape,TopAbs_VERTEX);Ex.More();Ex.Next()){
	Vertex=TopoDS::Vertex(Ex.Current());
	pointValue=BRep_Tool::Pnt(Vertex);
	staticVaultVertices(switcher, pointValue,points);
	cout<<"wirte vertex point"<<endl;
	switcher++;
    }
}

void pathAlgo::staticVaultVertices (int switcher, gp_Pnt point, vertexPoints &points)
	// gets 8 vertice points, two for each edge, 4 edges build one rectangle. delete double points
{
    cout<<"static VaultVertices"<<endl;
    static gp_Pnt tempArray [8];
    static int i=0;
    int maxArrayNumeration = 7;

    if(!switcher){ //sort and read
	
	cout<<"read and sort values "<<endl;
	
	//set start value
	points.A=tempArray[0];

	//compare it with all the other values, take a different one
	for (int i=0; i<maxArrayNumeration; i++){
	    if (!comparePoints(points.A,tempArray[i]))
		points.B=tempArray[i];
	}
	
	//take the two values and take another different one 
	for (int i=0; i<maxArrayNumeration; i++){
	    if (!comparePoints(points.A,tempArray[i]) && (!comparePoints(points.B,tempArray[i])))
		points.C=tempArray[i];
	}
	
	//take the three values and take another different one
	for (int i=0; i<maxArrayNumeration; i++){
	    if (!comparePoints(points.A,tempArray[i]) && (!comparePoints(points.B,tempArray[i])) && (!comparePoints(points.C,tempArray[i])))
		points.D=tempArray[i];
	}
	
	//output coordinates of the 4 different points
	//printf("points.A.X() %f\tpoints.A.Y() %f\tpoints.A.Z() %f\n",points.A.X(),points.A.Y(), points.A.Z());       
	//printf("points.B.X() %f\tpoints.B.Y() %f\tpoints.B.Z() %f\n",points.B.X(), points.B.Y(), points.B.Z());
	//printf("points.C.X() %f\tpoints.C.Y() %f\tpoints.C.Z() %f\n",points.C.X(), points.C.Y(), points.C.Z());    
	//printf("points.D.X() %f\tpoints.D.Y() %f\tpoints.D.Z() %f\n",points.D.X(), points.D.Y(), points.D.Z());
     }
    
    if(switcher<= maxArrayNumeration){ //write values to vault
	if (i<=maxArrayNumeration){
	    cout<<"write points"<<endl;
	    tempArray[i]=point;
	    i++;
	}
	if(i==maxArrayNumeration)
	    i=0;
    }
    
    else
	cout<<"Too much Vertex points for Array to handle, max"<< maxArrayNumeration<<endl;
    return;    
}

bool pathAlgo::comparePoints(gp_Pnt a,gp_Pnt b)
	// compare two gp_Pnt points with each other
{
    bool result=0, xIdent=0, yIdent=0, zIdent=0;
    
    xIdent=!(a.X()-b.X());
    yIdent=!(a.Y()-b.Y());
    zIdent=!(a.Z()-b.Z());
    
    if (xIdent && yIdent && zIdent)
	result=1;// if Points compared are identical in X, Y, Z coordinates
    return result;
}

//+++++++++++++++++++++++++++++++++AK


void pathAlgo::AddFace(TopoDS_Face &aFace, TopoDS_Shape &theShape) 
{  
    uint f = 0;
    bool keepGoing = true;      
    TopExp_Explorer Ex;
   
    gp_Pnt dummyPoint;//AK
    vertexPoints lastPoints;// AK target for vertix points
 
    //assign a number to this face
    for (Ex.Init(theShape,TopAbs_FACE); (Ex.More() && keepGoing); Ex.Next()) {
	if (Ex.Current() == aFace) {
	    //get out of loop, preserving f.
	    keepGoing = false;
	} else {
	    f++;
	}
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
	//AK+++++++
	getVertices (mf.F); //calculate Vertex points from this face in VaultVertices
	staticVaultVertices (0, dummyPoint, lastPoints);//read calculated vertix points from Vault and save to lastPoints
	listOfVertexPoints.push_back(lastPoints);//add lastPoints to the Vector listOfVertices
	cout<<"edgepoints to vector"<<endl;
	//++++++++++AK
	mf.computed=false;
	listOfFaces.push_back(mf);
	emit addFaceToList(f);
    }
   //AK+++++++++++++++++++     
    
    // inlcude here checking of  vault vertices and also suggestion of following elements!!!

    if (!pathComputed)    //check if there is already one cell computed!!
	return;
      
   uint numElements =(uint) listOfVertexPoints.size();
    gp_Pnt compArray [numElements][4];
    gp_Pnt doublePoints [2];
    int k=0;
    gp_Pnt A(0,0,0);
    gp_Pnt B(0,0,0);
    gp_Pnt C(0,0,0);
    bool check=false;
    uint iVault=0;
    
    cout<<"numElements == listOfVertexPoints.size()"<<numElements<<endl;

    for (uint j=0;j < numElements; j++){    //transfer points from vector to array because it is easier to handle than a vector a vector properly
	compArray [j][0]=listOfVertexPoints.at(j).A;
	compArray [j][1]=listOfVertexPoints.at(j).B;
	compArray [j][2]=listOfVertexPoints.at(j).C;
	compArray [j][3]=listOfVertexPoints.at(j).D;
    }
    
    for (uint i=0; i<numElements; i++){ //check array
	for (int j=0; j<4;j++){
	    cout<<"compArray[ "<<i<<" ] [ "<<j<<" ] = "<< compArray[i] [j].X()<<" "<< compArray[i] [j].Y()<<" "<< compArray[i] [j].Z() <<endl; 
	}
    }
    
    for (uint i=0; i<numElements-1; i++){
	for (int j=0; j<4; j++){
	    for (int o=0; o<4;o++){
		if (comparePoints(compArray[i] [j],compArray[numElements-1] [o])) {// if these points are identical
		    if (k ){
			if(i == iVault){// same i position in array - take second point
			    doublePoints[k]=compArray[numElements-1] [o];
			    printf("doublePoints[%i] %f\t %f\t %f\n",k,doublePoints[k].X(),doublePoints[k].Y(), doublePoints[k].Z());
			    cout<<"test2"<< endl;
			    check=true;	    	
		    }
			if ((i != iVault) && (!check)){// not same i - delete point and set iVault and k++ and save point
			    k=0;
			    iVault=i;
			    doublePoints[k]=compArray[numElements-1] [o];
			    printf("doublePoints[%i] %f\t %f\t %f\n",k,doublePoints[k].X(),doublePoints[k].Y(), doublePoints[k].Z());
			    k++;
			    cout<<"test3"<< endl;
			}
		    }
		      if (!k){// if k is zero
			iVault=i;
			doublePoints[k]=compArray[numElements-1] [o];
			printf("doublePoints[%i] %f\t %f\t %f\n",k,doublePoints[k].X(),doublePoints[k].Y(), doublePoints[k].Z());
			cout<<"test1"<< endl;
			k++;
			    }   
		}
	    }    
	}	
    }	
    
    if (k && (!comparePoints(A,doublePoints[0])) && (!comparePoints(B,doublePoints[1])) ){ //only copy points if they are initialized
	A=doublePoints[0];
	B=doublePoints[1];
    }
          
    printf("A.X() %f\tA.Y() %f\tA.Z() %f\n",A.X(),A.Y(), A.Z());   //for checking values
    printf("B.X() %f\tB.Y() %f\tB.Z() %f\n",B.X(), B.Y(),B.Z());  

    if(comparePoints(A,B)){// if A and B are still zero return;
	 cout<<"no matching points to previous face"<<endl;
	 return;
     }
   
    for (int p=0; p<4; p++){   //get a third point from last selected plane to calculate the orientation of the toolPath
	if (!comparePoints(A,compArray[numElements-1][p]) && (!comparePoints(B,compArray[numElements-1] [p]))){
		C=compArray[numElements-1][p];
		cout<<"test4"<< endl;
	    }
	}
        
    //printf("A.X() %f\tA.Y() %f\tA.Z() %f\n",A.X(),A.Y(), A.Z());   
    //printf("B.X() %f\tB.Y() %f\tB.Z() %f\n",B.X(), B.Y(),B.Z());
    printf("C.X() %f\tC.Y() %f\tC.Z() %f\n",C.X(), C.Y(),C.Z());
    
    // calculate compensation of toolpath
    int selectPlane=0; //1 == xz-plane, 2 == yz-ülane
    int selectBorder=0;//1 ==  negative border, 2 == positive Border
    bool checkPlane;
    int checkBorder;
    QString m="",n="",o="";
    
    if(A.X()==B.X()){//vector along X-axis
	selectPlane=1;
	m.sprintf("X-axis");
	if (C.Y()==A.Y()){//vector AC 
	    if (A.X() < C.X()){
		selectBorder=2; //negBorder;
		n.sprintf("negative border");
	    }
	    else{
		selectBorder=1;//posBorder;
		n.sprintf("positive border");
	    }
	}
	    
	if (C.Y() == B.Y()){ // vector BC 	    
	    if (B.X() < C.X()){
		selectBorder=2;//negBorder
		n.sprintf("negative border");
	    }
	    else{
		selectBorder=1;//posBorder
		n.sprintf("positive border");
	    }
	}		   
    }
    
    if(A.Y() == B.Y()){//vector along Y-axis
	selectPlane=2;
	m.sprintf("Y-axis");
	if (C.X() == A.X()){  //vector AC
	    if(A.Y() < C.Y()){
		selectBorder=2;;//negBorder
		n.sprintf("negative border");
	    }
	    else{
		selectBorder=1;//posBorder
		n.sprintf("positive border");
	    }
	}
	  
	if (C.X() == B.X()){//vector BC
	    if (B.Y() < C.Y()){
		selectBorder=2; //negBorder
		n.sprintf("negative border");
	    }
	    else{
		selectBorder=1; //posBorder
		n.sprintf("positive border");
	    }
	}
    }
 o.sprintf("%s and %s", (const char *)m,(const char *)n);
 
 //message window will appear
 QMessageBox mb( "Hint for maze elementary type",o,
		 QMessageBox::Information,
		 QMessageBox::Yes | QMessageBox::Default,
		 QMessageBox::No,  
		 QMessageBox::Cancel | QMessageBox::Escape );
 mb.setButtonText( QMessageBox::Yes, "Accept" );
 mb.setButtonText( QMessageBox::No, "Discard" );
    
 switch( mb.exec() ) {
 case QMessageBox::Yes:
     //accept suggested Plane and Border
     checkPlane = (staticVaultPlane(selectPlane));
     checkBorder= (staticVaultBorder( selectBorder));
     slotComputeSimplePathOnFace() ;
     break;
 case QMessageBox::No:
     return;
     // exit without saving
     break;
 case QMessageBox::Cancel:
     return;
     // don't save and don't exit
     break;
    }
    //++++++++++++++++++++++++++++AK
}


//copy of add face for atomated processing
void pathAlgo::slotAddFace(TopoDS_Face aFace) 
{  
    uint f = 0;
    bool keepGoing = true;      
    TopExp_Explorer Ex;
   
    gp_Pnt dummyPoint;//AK
    vertexPoints lastPoints;// AK target for vertix points
 /*
    //assign a number to this face
    for (Ex.Init(theShape,TopAbs_FACE); (Ex.More() && keepGoing); Ex.Next()) {
	if (Ex.Current() == aFace) {
	    //get out of loop, preserving f.
	    keepGoing = false;
	} else {
	    f++;
	}
    } 
    
    if (keepGoing) {
	printf("This face not in the shape!\n");
	return;
    }
    */
    //check the face's number against others in listOfFaces
   // bool duplicate = false;
    //for (uint i=0;i<listOfFaces.size();i++) {
//	if (listOfFaces.at(i).faceNumber == f) {
//	    duplicate = true;
//	    puts("Duplicate");
//	}
  //  }
    
    //if (!duplicate) {
	mFace mf;
	mf.faceNumber = f;
	mf.F=aFace;
	//AK+++++++
	getVertices (mf.F); //calculate Vertex points from this face in VaultVertices
	staticVaultVertices (0, dummyPoint, lastPoints);//read calculated vertix points from Vault and save to lastPoints
	listOfVertexPoints.push_back(lastPoints);//add lastPoints to the Vector listOfVertices
	cout<<"edgepoints to vector"<<endl;
	//++++++++++AK
	mf.computed=false;
	listOfFaces.push_back(mf);
	emit addFaceToList(f);
   // }
   //AK+++++++++++++++++++     
    
    // inlcude here checking of  vault vertices and also suggestion of following elements!!!

	if (!pathComputed){    //check if there is already one cell computed!!
	slotComputeSimplePathOnFace() ;
	 activateRS274();
	return;}
      
   uint numElements =(uint) listOfVertexPoints.size();
    gp_Pnt compArray [numElements][4];
    gp_Pnt doublePoints [2];
    int k=0;
    gp_Pnt A(0,0,0);
    gp_Pnt B(0,0,0);
    gp_Pnt C(0,0,0);
    bool check=false;
    uint iVault=0;
    
    cout<<"numElements == listOfVertexPoints.size()"<<numElements<<endl;

    for (uint j=0;j < numElements; j++){    //transfer points from vector to array because I don't know to handle a vector properly
	compArray [j][0]=listOfVertexPoints.at(j).A;
	compArray [j][1]=listOfVertexPoints.at(j).B;
	compArray [j][2]=listOfVertexPoints.at(j).C;
	compArray [j][3]=listOfVertexPoints.at(j).D;
    }
    
    for (uint i=0; i<numElements; i++){ //check array
	for (int j=0; j<4;j++){
	    cout<<"compArray[ "<<i<<" ] [ "<<j<<" ] = "<< compArray[i] [j].X()<<" "<< compArray[i] [j].Y()<<" "<< compArray[i] [j].Z() <<endl; 
	}
    }
    
    for (uint i=0; i<numElements-1; i++){
	for (int j=0; j<4; j++){
	    for (int o=0; o<4;o++){
		if (comparePoints(compArray[i] [j],compArray[numElements-1] [o])) {// if these points are identical
		    if (k ){
			if(i == iVault){// same i position in array - take second point
			    doublePoints[k]=compArray[numElements-1] [o];
			    printf("doublePoints[%i] %f\t %f\t %f\n",k,doublePoints[k].X(),doublePoints[k].Y(), doublePoints[k].Z());
			    cout<<"test2"<< endl;
			    check=true;	    	
		    }
			if ((i != iVault) && (!check)){// not same i - delete point and set iVault and k++ and save point
			    k=0;
			    iVault=i;
			    doublePoints[k]=compArray[numElements-1] [o];
			    printf("doublePoints[%i] %f\t %f\t %f\n",k,doublePoints[k].X(),doublePoints[k].Y(), doublePoints[k].Z());
			    k++;
			    cout<<"test3"<< endl;
			}
		    }
		      if (!k){// if k is zero
			iVault=i;
			doublePoints[k]=compArray[numElements-1] [o];
			printf("doublePoints[%i] %f\t %f\t %f\n",k,doublePoints[k].X(),doublePoints[k].Y(), doublePoints[k].Z());
			cout<<"test1"<< endl;
			k++;
			    }   
		}
	    }    
	}	
    }	
    
    if (k && (!comparePoints(A,doublePoints[0])) && (!comparePoints(B,doublePoints[1])) ){ //only copy points if they are initialized
	A=doublePoints[0];
	B=doublePoints[1];
    }
          
    printf("A.X() %f\tA.Y() %f\tA.Z() %f\n",A.X(),A.Y(), A.Z());   //for checking values
    printf("B.X() %f\tB.Y() %f\tB.Z() %f\n",B.X(), B.Y(),B.Z());  

    if(comparePoints(A,B)){// if A and B are still zero return;
	 cout<<"no matching points to previous face"<<endl;
	 return;
     }
   
    for (int p=0; p<4; p++){   //get a third point from last selected plane to calculate the orientation of the toolPath
	if (!comparePoints(A,compArray[numElements-1][p]) && (!comparePoints(B,compArray[numElements-1] [p]))){
		C=compArray[numElements-1][p];
		cout<<"test4"<< endl;
	    }
	}
        
    //printf("A.X() %f\tA.Y() %f\tA.Z() %f\n",A.X(),A.Y(), A.Z());   
    //printf("B.X() %f\tB.Y() %f\tB.Z() %f\n",B.X(), B.Y(),B.Z());
    printf("C.X() %f\tC.Y() %f\tC.Z() %f\n",C.X(), C.Y(),C.Z());
    
    // calculate compensation of toolpath
    int selectPlane=0; //1 == xz-plane, 2 == yz-ülane
    int selectBorder=0;//1 ==  negative border, 2 == positive Border
    bool checkPlane;
    int checkBorder;
    QString m="",n="",o="";
    
    if(A.X()==B.X()){//vector along X-axis
	selectPlane=1;
	m.sprintf("X-axis");
	if (C.Y()==A.Y()){//vector AC 
	    if (A.X() < C.X()){
		selectBorder=2; //negBorder;
		n.sprintf("negative border");
	    }
	    else{
		selectBorder=1;//posBorder;
		n.sprintf("positive border");
	    }
	}
	    
	if (C.Y() == B.Y()){ // vector BC 	    
	    if (B.X() < C.X()){
		selectBorder=2;//negBorder
		n.sprintf("negative border");
	    }
	    else{
		selectBorder=1;//posBorder
		n.sprintf("positive border");
	    }
	}		   
    }
    
    if(A.Y() == B.Y()){//vector along Y-axis
	selectPlane=2;
	m.sprintf("Y-axis");
	if (C.X() == A.X()){  //vector AC
	    if(A.Y() < C.Y()){
		selectBorder=2;;//negBorder
		n.sprintf("negative border");
	    }
	    else{
		selectBorder=1;//posBorder
		n.sprintf("positive border");
	    }
	}
	  
	if (C.X() == B.X()){//vector BC
	    if (B.Y() < C.Y()){
		selectBorder=2; //negBorder
		n.sprintf("negative border");
	    }
	    else{
		selectBorder=1; //posBorder
		n.sprintf("positive border");
	    }
	}
    }
    
   	
    //accept suggested Plane and Border
    checkPlane = (staticVaultPlane(selectPlane));
    checkBorder= (staticVaultBorder( selectBorder));	
    slotComputeSimplePathOnFace() ;
    activateRS274();
/*	
 o.sprintf("%s and %s", (const char *)m,(const char *)n);
 
 //message window will appear
      QMessageBox mb( "Hint for maze elementary type",o,
      QMessageBox::Information,
      QMessageBox::Yes | QMessageBox::Default,
      QMessageBox::No,  
      QMessageBox::Cancel | QMessageBox::Escape );
    mb.setButtonText( QMessageBox::Yes, "Accept" );
    mb.setButtonText( QMessageBox::No, "Discard" );
    
    switch( mb.exec() ) {
    case QMessageBox::Yes:
	//accept suggested Plane and Border
	checkPlane = (staticVaultPlane(selectPlane));
	checkBorder= (staticVaultBorder( selectBorder));
	slotComputeSimplePathOnFace() ;
        break;
    case QMessageBox::No:
        // exit without saving
       break;
    case QMessageBox::Cancel:
        // don't save and don't exit
        break;
    }*/
    //slotComputeSimplePathOnFace() ;//start it automatically
    return;
}
//copy ends

//AK+++++++++++++++++++++++++++++++++++
void pathAlgo::cleanPathBuffer()
	// init is executed when creating a new part, cleanPathBuffer is executed for each tool-path
{    
    pathComputed=false;
    bool checkProjected=false;
    projectedPasses.clear();
    checkProjected = projectedPasses.empty();
    cout<<"projectedPasses (1 == empty) = "<< checkProjected<<endl;
    
    bool checkTheProjLines=false;
    theProjLines.Nullify();
    checkTheProjLines=theProjLines.IsNull();
    cout<<"proj.P.Nullify() (1== empty) = "<<checkTheProjLines<<endl;   
    
    bool checkProjP=false;
   proj.P.Nullify();
   checkProjP=proj.P.IsNull();
   cout<<"proj.P.Nullify() (1== empty) = "<<checkProjP<<endl;
      
   bool checkProjFacesUsed=false;
   proj.facesUsed.clear();
   checkProjFacesUsed = proj.facesUsed.empty();
   cout<<"proj.facesUsed (1 == empty) = "<<checkProjFacesUsed<<endl;
   
   // uncommented otherwise breakdown
   // bool checkList=false;
   //listOfFaces.clear();
    //checkList = listOfFaces.empty();
    //cout<<"listOfFaces (1 == empty) = "<< checkList<< endl;
}
//+++++++++++++++++++++++++++++++++++++AK


void pathAlgo::slotComputeSimplePathOnFace() 
{
    // AK++++++++++++++++++++++++++++++++++++++++++
    cleanPathBuffer();
        //extend toolpath by not cleaning Buffer!
    //Difference between init() and cleanBuffer()
    //cleanBuffer() //projectedPasses                       //proj.facesUsed //proj.P  //theProjLines 
    //init();           //projectedPasses  //listOfFaces  //proj.facesUsed  //proj.P  //theProjLines
   
   //setDisplayedPathFalse();  //deletes all projected lines, quite usefull
    
/*
    //message box for execution check
    puts("pathAlgo::slotComputeSimplePathonFace - insert Machine Feedback");	
    QMessageBox::warning( 0, "Hint", "Apply adapted machining parameter  >>Tool diameter, Feed, Speed<<");
*/
    
    //TopoDS_Shape theProjLines;	//use TopoDS_Shape, because it can hold multiple edges
    Standard_Real bboxWidth;	//width (y) of bounding box
    Standard_Real lineY, lineX;		//for computing line to project by AKK
    Standard_Real passWidth = 2;	//cutting width, one pass == tool radius without  ovverlapping
   // float overlapping=0.75; // 75% overlapping
    maxPassWidth=5; // cell size of array is 10 to 10, for 5 one toolpath is generated
   
     
    int numPasses;	//number of passes that must be made to cover surface
    Bnd_Box aBox;
    Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
    
    //hidden by AK TopoDS_Shape faces;f);
    TopoDS_Shape faces;
    TopoDS_Shape lines;
    pathComputed = false;//was uncommented AK
    

    
    int toolDiameter=4; //defaultvalue;
    int feed=120; //default value
    int speed=7300; //default value
    
     // when embedding a new widget you may need to compile it twice due to moc compiler
    // let Widget cuttingParameter appear
    cuttingParameter dlg (0,"cutt",true);
    int result = dlg.exec();
    if (result== QDialog::Accepted){ 
	// if OK then perform this code
	dlg.slotGetFeed(feed);
	dlg.slotGetSpeed(speed);
	dlg.slotGetToolDia(toolDiameter);
    }
    
    if (result== QDialog::Rejected){ 
	// if OK then perform this code
	return;
    }
   
    // emit to rs274emc to display the values in G-code file
    emit sendFeed(feed);
    emit sendSpeed(speed);
    emit sendToolDia(toolDiameter);
    
    
    passWidth=0.5*toolDiameter; //passwidth including overlapping
    //+++++++++++++++++++++++++++++++++++++++++++AK
    
    
    //get Face
    for (uint i=0;i<listOfFaces.size();i++) {
	if (!listOfFaces.at(i).computed) {//add each face to listOfFaces
	    TopoDS_Face curFace = listOfFaces.at(i).F; 
	    
	    if (faces.IsNull()) {
		faces=curFace;
	    } 
	    else {
		//wtf pita
		if (BRepAlgoAPI_Fuse(faces,curFace).BuilderCanWork()) { //not sure if this has any effect...
		    faces=BRepAlgoAPI_Fuse(faces,curFace);
		}
		else {
		    puts ("brepalgoapi.fuse builder failure");
		}
	    }
	    //bounding box
	    BRepAdaptor_Surface aSurf(curFace);
	    BndLib_AddSurface::Add(aSurf,(Standard_Real) 0.0,aBox);
	    
	    proj.facesUsed.push_back(listOfFaces.at(i).faceNumber);  //keep face number associated with the toolpath
	    listOfFaces.at(i).computed = true;
	}
	float value=listOfFaces.size();;// AK is quite detailed, but otherwise it doesn't work properly
	value=i/value;//AK
	int prog = (int) roundf(100*value); // AK
	emit setProgress(prog,"Adding faces"); // AK
    }
    emit setProgress(-1,"Done");
    
    
    aBox.Get(aXmin,aYmin,aZmin, aXmax,aYmax,aZmax);
    printf("aXmin %f,aYmin %f,aZmin %f\naXmax %f,aYmax %f,aZmax %f\n\n",aXmin,aYmin,aZmin, aXmax,aYmax,aZmax);//AK unhidden
    
    //AK+++++++++++++++++++++++++++++++++++++++++++++
    
    bool checkVaultPlane=false;
    int readVaultplane=3;
    checkVaultPlane = (staticVaultPlane(readVaultplane));
    bool xzPlane=false, yzPlane=false;
    
    //bboxWidth referrs to the part width and relates to either yz or xz plane
    if  (checkVaultPlane){
	xzPlane=true;
	bboxWidth = aYmax-aYmin;
    }
    else{
	yzPlane=true;
	bboxWidth = aXmax-aXmin;
    }
   
    maxPassWidth=0.5*bboxWidth;// maxPassWidth is equivalent to max tool radius
    
    if (passWidth>maxPassWidth){
	passWidth=maxPassWidth;
	cout<<"(passWidth>maxPassWidth) --> passWidth=maxPassWidth;"<<endl;
    }
    
    if (bboxWidth<.1)
	return;
    
    //++++++++++++++++++++++++++++++++++++++++++++AK
    Standard_Real H = aZmax;
    if ((!safeHeightSet)||(H>safeHeight)){  
	safeHeight=aZmax+10;
	sendSafeHeight(safeHeight);
    }
    
    //create a series of lines "covering" bbox (in Z) and project lines onto surface
    numPasses = 1 + (int)round((bboxWidth-2*passWidth)/passWidth);
    printf("bboxWidth=(aYmax-aYmin)= %f, passWidth %f, --> numPasses=(bboxWidth/passWidth)= %i\n\n",bboxWidth,passWidth,numPasses);//AK);
    
    //AK++++++++++++++++++++++++++++	
    
    
    //create toolpath level to top surface z minus cutting depth 1 and more not yet implemented
    /*
      //Amount of  offset levels
    
     Standard_Real roughHeight=2.5;
    Standard_Real finishHeight=1.5;
    
  
   int checkDiv=(Depth-finishHeight)%roughHeight;
    int numRoughLevels=0;
    float restDepth=0;
    if (!checkDiv){
	numRoughLevels=(Depth-finishHeight)/roughHeight;
    }
    
    if (checkDiv){
	numRoughLevels=(int)(Depth-finishHeight)/roughHeight;
	restDepth=(Depth-finishHeight-(roughHeight*numRoughLevels));
    }
    */
    
    //first offset
    CLOffsetFromShape(faces,3);//offsetS includes the offsetted Face3
    createCuttingLines(xzPlane, yzPlane,numPasses, aYmin, aYmax,  aXmin, aXmax, passWidth, aZmax, faces);

    
    proj.P=theProjLines;
    proj.displayed=false;
   
    connectVertexPoints(xzPlane, yzPlane);    //connect cutted lines to a toolpath, everything is added to proj.P
    projectedPasses.push_back(proj);//Ak adds element proj to dynamic array projected passes
  
    proj.P.Nullify();  
    theProjLines.Nullify();
     
    //repetition for second offset layer
    //offset from first offset
    CLOffsetFromShape(faces,-3);//offsetS includes the offsetted Face -3
     createCuttingLines(xzPlane, yzPlane,numPasses, aYmin,aYmax, aXmin,aXmax,passWidth, aZmax, faces);
     
     proj.P=theProjLines;
     proj.displayed=false;
    
     connectVertexPoints(xzPlane, yzPlane);//connect cutted lines to a toolpath, everything is added to proj.P
      projectedPasses.push_back(proj);//Ak adds element proj to dynamic array projected passes
    //end - repetition
    //AK+++++++++++++++++++++++++++++++++++++
      
    cout<<"projectedPasses.size(" <<projectedPasses.size()<<")"<<endl; //AK
    for(uint i=0;i < projectedPasses.size(); i++){
	for(uint j=0;j <projectedPasses.at(i).facesUsed.size(); j++)  //AK testing//AK testing
	    cout<<"projectedPasses.at("<<i<<").facesUsed.size("<<j<<")" <<endl; //AK
	                  }
    pathComputed=1;
    emit setComputed(1); // by AK set pathComputed =true
    emit setProgress(-1,"Done");
    emit showPath();
}

void pathAlgo::slotSelectFaceFromList(int f)
{
    //unimplemented
}

// AK+++++++++++++++++++++++++++++++++
void pathAlgo::CLOffsetFromShape(TopoDS_Shape &faces, Standard_Real Dist)
	//creates an offset of the selected face
	// after that the face can be sectioned by using the existing code to create tool path
	// this is necessary, because BRepOffsetAPI_MakeOffset is buggy and not able to offset lines (reported in www)	
{
    //using BRepOffsetAPI_MakeOffsetShape
    //Parameters
    Standard_Real Tol=.1;
    BRepOffset_Mode Mode=BRepOffset_Skin;
    Standard_Boolean Intersection=Standard_False;
    Standard_Boolean SelfInter=Standard_False;
    GeomAbs_JoinType JoinType=GeomAbs_Arc;
   
    //BRepOffsetAPI_MakeOffsetShape offset(faces, Dist, Tol, Mode, Intersection, SelfInter, JoinType);
    BRepOffsetAPI_MakeOffsetShape offset(faces, Dist, 0);//further parameters not neccesary
    faces=offset.Shape();//to get offset shape and give it to faces
   
    return; 
}



void pathAlgo::createCuttingLines(bool xzPlane, bool yzPlane, int numPasses, Standard_Real aYmin,Standard_Real aYmax, Standard_Real aXmin, Standard_Real aXmax, Standard_Real passWidth, Standard_Real aZmax,TopoDS_Shape faces)
	//creates the cutting lines according to slected cutting plane and selected border that refers to chunk type
{  
    int checkVaultBorder=0;
    int readVaultBorder=3;
    checkVaultBorder = (staticVaultBorder(readVaultBorder));//if negative then negative border selected!
    TopoDS_Edge aEdge;
    TopoDS_Wire W;
    gp_Vec Vector(0 , 0 , -90);// AK better solution needed	
    TopExp_Explorer Ex; //to find all edges in shape section AK
    TopoDS_Vertex E1,E2;	
    gp_Pnt end;
    float overlapping=1.5;
    
    Standard_Real lineY, lineX;
     if (xzPlane){ //xz -plane
	for (int j=0;j<numPasses;j++) {
	    if (((aYmin+passWidth) + j*overlapping*passWidth)<=(aYmax-passWidth)){
		lineY = (aYmin+passWidth) + j*overlapping*passWidth;
	    }
	    
	    if (((aYmin+passWidth) + j*overlapping*passWidth)>(aYmax-passWidth)){
		lineY = aYmax-passWidth;
	    }
	    
	    //Standard_Real lineY = aYmin + j*passWidth;
	    printf("lineY = aYmin + j*passWidth;\n");// AK
	    printf("lineY %f, aYmin %f, j %i, passWidth %f, (j*passWidth) %f\n\n",lineY,aYmin, j,passWidth,j*passWidth);// AK
	    
	    if (checkVaultBorder==1){// pos border
		gp_Pnt A ((aXmin+passWidth), lineY,aXmax);// AK Z needs changes
		gp_Pnt B((aXmax+passWidth), lineY, aXmax);// AK Z needs changes
		 aEdge = BRepBuilderAPI_MakeEdge(A, B);
		 TopoDS_Shape cuttingFace = BRepPrimAPI_MakePrism(aEdge , Vector);
		 TopoDS_Shape section = BRepAlgoAPI_Section (faces,cuttingFace);
	      
		 for (Ex.Init(section,TopAbs_EDGE); Ex.More(); Ex.Next()) {
		     W = BRepBuilderAPI_MakeWire(TopoDS::Edge(Ex.Current()));
		     TopExp::Vertices(W,E1,E2);
		   
		     // Get the 3D point for the vertexes.
		     gp_Pnt Cal1 = BRep_Tool::Pnt(E1);
		     gp_Pnt Cal2 = BRep_Tool::Pnt(E2);
		     
		     //choose right point to draw an edge with
		     if (Cal1.X()<Cal2.X())
			 end=Cal1;
		     else
			 end=Cal2;
		     
		     //set new B1 Point, is B but with Cal1.Z() in Z-value
		      gp_Pnt B1((aXmax+passWidth),lineY, Cal1.Z());
		      TopoDS_Edge aEdgeElongated = BRepBuilderAPI_MakeEdge(end , B1);
		            
		      W = BRepBuilderAPI_MakeWire(aEdgeElongated);
			
		      if (theProjLines.IsNull()) {
			  theProjLines = W;
			  puts("null");
		      } else {
			  theProjLines = BRepAlgoAPI_Fuse(theProjLines,W);
			  puts("cutting line fused with theProjLines");
		      }
		  }
	     }
	    
	    
	    if(checkVaultBorder==-1){//neg border
		gp_Pnt A((aXmin-passWidth), lineY,aXmax);// AK Z needs changes
		gp_Pnt B((aXmax-passWidth), lineY, aXmax);// AK Z needs changes
		 aEdge = BRepBuilderAPI_MakeEdge(A, B);
		 TopoDS_Shape cuttingFace = BRepPrimAPI_MakePrism(aEdge , Vector);
		 TopoDS_Shape section = BRepAlgoAPI_Section (faces,cuttingFace);
	      
		 for (Ex.Init(section,TopAbs_EDGE); Ex.More(); Ex.Next()) {
		     W = BRepBuilderAPI_MakeWire(TopoDS::Edge(Ex.Current()));
		     TopExp::Vertices(W,E1,E2);
		   
		     // Get the 3D point for the vertexes.
		     gp_Pnt Cal1 = BRep_Tool::Pnt(E1);
		     gp_Pnt Cal2 = BRep_Tool::Pnt(E2);
		     
		      //choose right point to draw an edge with
		     if (Cal1.X()>Cal2.X())
			 end=Cal1;
		     else
			 end=Cal2;
		     
		     //set new B1 Point, is B but with Cal1.Z() in Z-value
		      gp_Pnt B1((aXmin-passWidth),lineY, Cal2.Z());
		      TopoDS_Edge aEdgeElongated = BRepBuilderAPI_MakeEdge(end , B1);
		      
		      W = BRepBuilderAPI_MakeWire(aEdgeElongated);
			
		      if (theProjLines.IsNull()) {
			  theProjLines = W;
			  puts("null");
		      } else {
			  theProjLines = BRepAlgoAPI_Fuse(theProjLines,W);
			  puts("cutting line fused with theProjLines");
		      }
		  }
	     }	
	      float value=numPasses;;// AK is quite detailed, but otherwise it doesn't work properly
	     value=j/value;// AK
	     int prog = (int) roundf(100*value); // AK	 
	     emit setProgress(prog,"Calculating cuts"); // AK
	}
	
	// special case for isolated chunk and max passwidth
	if(passWidth==maxPassWidth){
	    passWidth=0.5*passWidth;
	    numPasses=2*numPasses;
	    cout<<"passWidth"<<passWidth<<endl;
		}
	
	for (int j=0;j<numPasses;j++) {
	    if (((aYmin+passWidth) + j*overlapping*passWidth)<=(aYmax-passWidth)){
		lineY = (aYmin+passWidth) + j*overlapping*passWidth;
	    }
	    
	    if (((aYmin+passWidth) + j*overlapping*passWidth)>(aYmax-passWidth)){
		lineY = aYmax-passWidth;
	    }
	    
	    if (checkVaultBorder==0){// initial status
		// is a point results, decrease pathWidth
		
		
		gp_Pnt A ((aXmin+passWidth), lineY,aXmax);// AK Z needs changes
		gp_Pnt B((aXmax-passWidth), lineY, aXmax);// AK Z needs changes 
		cout<<"test1"<<endl;
		aEdge = BRepBuilderAPI_MakeEdge(A, B);
		TopoDS_Shape cuttingFace = BRepPrimAPI_MakePrism(aEdge , Vector); 
		TopoDS_Shape section = BRepAlgoAPI_Section (faces,cuttingFace);
	      
		
		 for (Ex.Init(section,TopAbs_EDGE); Ex.More(); Ex.Next()) {
		     W = BRepBuilderAPI_MakeWire(TopoDS::Edge(Ex.Current()));
		     TopExp::Vertices(W,E1,E2);
		   
		     // Get the 3D point for the vertexes.
		     gp_Pnt Cal1 = BRep_Tool::Pnt(E1);
		     gp_Pnt Cal2 = BRep_Tool::Pnt(E2);
		     
		     //choose right point to draw an edge with
		     if (Cal1.X()<Cal2.X())
			 end=Cal1;
		     else
			 end=Cal2;
		     
		     //set new B1 Point, is B but with Cal1.Z() in Z-value
		      //gp_Pnt B1((aXmax),lineY, Cal1.Z());
		      TopoDS_Edge aEdgeElongated = BRepBuilderAPI_MakeEdge(Cal1 , Cal2);
		            
		      W = BRepBuilderAPI_MakeWire(aEdgeElongated);
			
		      if (theProjLines.IsNull()) {
			  theProjLines = W;
			  puts("null");
		      } else {
			  theProjLines = BRepAlgoAPI_Fuse(theProjLines,W);
			  puts("cutting line fused with theProjLines");
		      }
		  }
	     }
	
	    
	     float value=numPasses;;// AK is quite detailed, but otherwise it doesn't work properly
	     value=j/value;// AK
	     int prog = (int) roundf(100*value); // AK	 
	     emit setProgress(prog,"Calculating cuts"); // AK
	}
    }
    
    if(yzPlane) { //yz-plane
	for (int j=0;j<numPasses;j++) {
	    
	     if (((aXmin+passWidth) + j*overlapping*passWidth)<=(aXmax-passWidth)){
		lineX = (aXmin+passWidth) + j*overlapping*passWidth;
	    }
	    
	    if (((aXmin+passWidth) + j*overlapping*passWidth)>(aXmax-passWidth)){
		lineX = aXmax-passWidth;
	    }
	    
	    //Standard_Real lineX = aXmin + j*passWidth;
	    printf("lineX = aXmin + j*passWidth;\n");//AK
	    printf("lineX %f, aXmin %f, j %i, passWidth %f, (j*passWidth) %f\n\n",lineX,aXmin, j,passWidth,j*passWidth);//AK
	    
	    if(checkVaultBorder==1){//pos border
		
		gp_Pnt A(lineX,(aYmin+passWidth),aXmax);// AK Z needs changes 
		gp_Pnt B(lineX,aYmax, aXmax);// AK Z needs changes 
		 aEdge = BRepBuilderAPI_MakeEdge(A, B);
		 TopoDS_Shape cuttingFace = BRepPrimAPI_MakePrism(aEdge , Vector);
		 TopoDS_Shape section = BRepAlgoAPI_Section (faces,cuttingFace);
	      
		 for (Ex.Init(section,TopAbs_EDGE); Ex.More(); Ex.Next()) {
		     W = BRepBuilderAPI_MakeWire(TopoDS::Edge(Ex.Current()));
		     TopExp::Vertices(W,E1,E2);
		   
		     // Get the 3D point for the vertexes.
		     gp_Pnt Cal1 = BRep_Tool::Pnt(E1);
		     gp_Pnt Cal2 = BRep_Tool::Pnt(E2);
		     
		      //choose right point to draw an edge with
		     if (Cal1.Y() < Cal2.Y())
			 end=Cal1;
		     else
			 end=Cal2;
		     
		     //set new B1 Point, is B but with Cal1.Z() in Z-value
		      gp_Pnt B1(lineX,( aYmax+passWidth), Cal1.Z());
		      TopoDS_Edge aEdgeElongated = BRepBuilderAPI_MakeEdge(end , B1);
		      
		      W = BRepBuilderAPI_MakeWire(aEdgeElongated);
	
		      if (theProjLines.IsNull()) {
			  theProjLines = W;
			  puts("null");
		      } else {
			  theProjLines = BRepAlgoAPI_Fuse(theProjLines,W);
			  puts("cutting line fused with theProjLines");
		      }
		  }
	     }
	    
	    if (checkVaultBorder==-1){//neg border
		gp_Pnt A(lineX,(aYmin-passWidth),aXmax);// AK Z needs changes
		gp_Pnt B(lineX, (aYmax-passWidth), aXmax);// AK Z needs changes
		aEdge = BRepBuilderAPI_MakeEdge(A, B);
		TopoDS_Shape cuttingFace = BRepPrimAPI_MakePrism(aEdge , Vector);
		TopoDS_Shape section = BRepAlgoAPI_Section (faces,cuttingFace); 
		
		for (Ex.Init(section,TopAbs_EDGE); Ex.More(); Ex.Next()) {
		    TopoDS_Wire W = BRepBuilderAPI_MakeWire(TopoDS::Edge(Ex.Current()));
		    TopExp::Vertices(W,E1,E2);
		   
		     // Get the 3D point for the vertexes.
		     gp_Pnt Cal1 = BRep_Tool::Pnt(E1);
		     gp_Pnt Cal2 = BRep_Tool::Pnt(E2);
		      
		      //choose right point to draw an edge with
		     if (Cal1.Y() > Cal2.Y())
			 end=Cal1;
		     else
			 end=Cal2;
		     
		     //set new B1 Point, is B but with Cal1.Z() in Z-value
		      gp_Pnt B1(lineX,( aYmin-passWidth), Cal2.Z());
		      TopoDS_Edge aEdgeElongated = BRepBuilderAPI_MakeEdge(end , B1);
		         
		      W = BRepBuilderAPI_MakeWire(aEdgeElongated);
	
		      if (theProjLines.IsNull()) {
			  theProjLines = W;
			  puts("null");
		      } else {
			  theProjLines = BRepAlgoAPI_Fuse(theProjLines,W);
			  puts("cutting line fused with theProjLines");
		      }
		  }
	    }
	      float value=numPasses;;// AK is quite detailed, but otherwise it doesn't work properly
	     value=j/value;// AK
	     int prog = (int) roundf(100*value); // AK	 
	     emit setProgress(prog,"Calculating cuts"); // AK
	}
	
	
	// special case for isolated chunk and max passwidth
	if(passWidth==maxPassWidth){
	    passWidth=0.5*passWidth;
	    numPasses=2*numPasses;
	    cout<<"passWidth"<<passWidth<<endl;
		}
	
	for (int j=0;j<numPasses;j++) {
	
	if (((aXmin+passWidth) + j*overlapping*passWidth)<=(aXmax-passWidth)){
		lineX = (aXmin+passWidth) + j*overlapping*passWidth;
	    }
	    
	    if (((aXmin+passWidth) + j*overlapping*passWidth)>(aXmax-passWidth)){
		lineX = aXmax-passWidth;
	    }
	    
	     if (checkVaultBorder==0){//initial status
		// is a point results, decrease pathWidth
		 if(passWidth=maxPassWidth)
		    passWidth=passWidth/2;
		 
		 gp_Pnt A(lineX,(aYmin+passWidth),aXmax);// AK Z needs changes
		gp_Pnt B(lineX, (aYmax-passWidth), aXmax);// AK Z needs changes
		aEdge = BRepBuilderAPI_MakeEdge(A, B);
		TopoDS_Shape cuttingFace = BRepPrimAPI_MakePrism(aEdge , Vector);
		TopoDS_Shape section = BRepAlgoAPI_Section (faces,cuttingFace); 
		
		for (Ex.Init(section,TopAbs_EDGE); Ex.More(); Ex.Next()) {
		    TopoDS_Wire W = BRepBuilderAPI_MakeWire(TopoDS::Edge(Ex.Current()));
		    TopExp::Vertices(W,E1,E2);
		   
		     // Get the 3D point for the vertexes.
		     gp_Pnt Cal1 = BRep_Tool::Pnt(E1);
		     gp_Pnt Cal2 = BRep_Tool::Pnt(E2);
		      
		      //choose right point to draw an edge with
		     if (Cal1.Y() > Cal2.Y())
			 end=Cal1;
		     else
			 end=Cal2;
		     
		     //set new B1 Point, is B but with Cal1.Z() in Z-value
		      //gp_Pnt B1(lineX,( aYmin), Cal2.Z());
		      TopoDS_Edge aEdgeElongated = BRepBuilderAPI_MakeEdge(Cal1 , Cal2);
		         
		      W = BRepBuilderAPI_MakeWire(aEdgeElongated);
	
		      if (theProjLines.IsNull()) {
			  theProjLines = W;
			  puts("null");
		      } else {
			  theProjLines = BRepAlgoAPI_Fuse(theProjLines,W);
			  puts("cutting line fused with theProjLines");
		      }
		  }
	    }
	 
	     
	    float value=numPasses;;// AK is quite detailed, but otherwise it doesn't work properly
	    value=j/value;// AK
	    int prog = (int) roundf(100*value); // AK	
	    emit setProgress(prog,"Calculating cuts"); // AK
	}
    }

    return;
}
//++++++++++++++++++++++++++++++++++AK

void pathAlgo::edgePoints(TopoDS_Edge E, gp_Pnt &c, gp_Pnt &a, gp_Pnt &b, bool &isLine, Standard_Real &circRad) 
	//Takes an edge which may be a circular arc, straight line, or other.  If it is circular, returns the center as c and radius as circRad.  Otherwise, c is null and circRad = 0.  Returns ends of edge as a,b.  If E is not a line, isLine is false.
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


//AK++++++++++++++++++++++++++++++++++++
void pathAlgo::connectVertexPoints(bool xzPlane, bool yzPlane)
	//connects the blue wires obtained by cutting
{
    int numLines= proj.facesUsed.size();
    cout<<"numLines"<<numLines<<endl;
    
    for(int i=0;i < numLines; i++){
	//explorer to decompose the shape and find all edges in shape section AK
	TopExp_Explorer Ex;
	
	int checkLoop=0; //initialize loop counter
	gp_Pnt Cal1,Cal2,Last, Act,Next,veryFirst,verySecond,begin1,begin2,end1,end2;
	TopoDS_Vertex E1,E2;	
	 cout<<"inside for numLines"<<endl;
	
	for (Ex.Init(proj.P,TopAbs_EDGE); Ex.More(); Ex.Next()) {
	   
	    cout<<"inside for Ex.Init"<<endl;
	    
	    //Convert Egde to Wire
	    TopoDS_Wire W = BRepBuilderAPI_MakeWire(TopoDS::Edge(Ex.Current()));
	    
	       cout<<"inside for Ex.Init - TopoDS_Wire created"<<endl;
	       
	    //find endpoints of wire, E1 E2 from pathAlgo analyzation
	    TopExp::Vertices(W,E1,E2);
	 
	    // Get the 3D point for the vertexes.
	    Cal1 = BRep_Tool::Pnt(E1);
	    Cal2 = BRep_Tool::Pnt(E2);
	    
	    // check first time run of loop
	    if (!checkLoop){
		Last=Cal1;
		Act=Cal2;
		cout<<"initialize loop for connecting vertices"<< endl;
		// for drawing the very last contour
		veryFirst=Last;
		verySecond=Act;
	    }
	   
	    // check second time run and more    
	    if (checkLoop){
		float xDistActCal1=fabs(fabs(Act.X())-(fabs(Cal1.X())));
		float xDistActCal2=fabs(fabs(Act.X())-(fabs(Cal2.X())));
		    
		float yDistActCal1=fabs(fabs(Act.Y())-(fabs(Cal1.Y())));
		float yDistActCal2=fabs(fabs(Act.Y())-(fabs(Cal2.Y())));
		    
		float xDistLastAct=fabs(fabs(Last.X())-(fabs(Act.X())));
		float yDistLastAct=fabs(fabs(Last.Y())-(fabs(Act.Y())));
		    
		int flag=0; //to check decision
		
		//may occur
		if ((!yDistLastAct) && (!xDistActCal1) && (!xDistActCal2)){
		    cout<<"(!yDistLastAct) && (!xDistActCal1) && (!xDistActCal2)"<< endl;
		    return;
		}
		
		if ((!yDistLastAct) && (!xDistActCal1)){
		    Next=Cal1;
		    flag=1;
		    cout<<"(!yDistLastAct) && (!xDistActCal1)"<< endl;
		}
		
		if ((!yDistLastAct) && (!xDistActCal2)){
		    Next=Cal2;
		    cout<<"(!yDistLastAct) && (!xDistActCal2)"<< endl;
		}
		
		if ((!xDistLastAct) && (!yDistActCal1)){
		    Next=Cal1;
		    flag=1;
		    cout<<"(!xDistLastAct) && (!yDistActCal1)"<< endl;
		}
		
		if ((!xDistLastAct) && (!yDistActCal2)){
		    Next=Cal2;
		    cout<<"(!xDistLastAct) && (!yDistActCal2)"<< endl;
		}
		
		//else{
		  //  QMessageBox::warning( 0, "Warning", "Only rectangle patched lines can be connected");
		    //return;
		//}
		
		//create an edge, convert it to a wire and save it in the shape, tutorial page 8ff
		//long way to create a wire with additional if loop for control
		cout<<"before - GC_MakeSegment mkSeg (Act, Next);"<<endl;
		GC_MakeSegment mkSeg (Act, Next);
		
		//set new Act Point
		if (flag){
		    Act=Cal2;
		    cout<<"flag ==1 "<< endl;
		}
		else{
		    Act=Cal1;
		    cout<<"flag == 0"<< endl;
		}
		
		//Set new Last Point
		Last=Next;
		
		//draw wire the long way
		Handle (Geom_TrimmedCurve) aSegment;
		TopoDS_Edge aEdge;
		TopoDS_Wire aWire;
		
		if (mkSeg.IsDone()){
		    aSegment=mkSeg.Value();
		    aEdge= BRepBuilderAPI_MakeEdge (aSegment);
		    aWire= BRepBuilderAPI_MakeWire(aEdge);
		    cout<<"Segment/ Wire/ Shape created"<<endl;
		}
		else{
		    cout<<"Segment/ Wire/ Shape Not created"<<endl;
		    return;
		}
		
		/*//short way to create a wire
		TopoDS_Edge aEdge = BRepBuilderAPI_MakeEdge( Act, Next);
		TopoDS_Wire aWire= BRepBuilderAPI_MakeWire(aEdge);
		*/
		
		TopoDS_Shape Temp=aWire;
		    
		if (BRepAlgoAPI_Fuse(proj.P,Temp).BuilderCanWork()) { 
		    proj.P = BRepAlgoAPI_Fuse(proj.P,Temp);
		    cout<<"proj.P and shapeTemp fused"<<endl;
		}
		
		else{
		    cout<<"proj.P and shapeTemp NOT fused"<<endl;
		    return;
		}
	    }
	    
	    checkLoop++;
	}	
    }
    
    return;
}
//++++++++++++++++++++++++++++++++++++++++++++++++AK


