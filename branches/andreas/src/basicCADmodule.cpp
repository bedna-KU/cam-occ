/*************************************************************
** License: GPL.  
** Copyright (C) 2007 Andreas Kain
*************************************************************/

//basicCADmodule.cpp

#include "basicCADmodule.h"


basicCADmodule::basicCADmodule()
{
}

basicCADmodule::~basicCADmodule()
{
}

void basicCADmodule::slotGenerateCube() 
	//creates static maze with 7 patches, just copy and paste
{

    emit setProgress(-1,"Generating Maze");
    
    // parameters of maze
    int xUnits=5;//5 only 
    int yUnits=5;//7
    
    Standard_Real width=10; //along x;
    Standard_Real length=10; //along 
    Standard_Real depth=-6; //depth of pocket
    Standard_Real thickness=3;
    Standard_Real offset=3;
    gp_Pnt PntOrig(0,0,0);	
   
    vector<maze_Cell> cellVector;
    vector<maze_Cell> borderVector;
    vector<neighbor_Cell> neighCellVector;
   
    neighbor_Cell tempCell;
    maze_Cell currentCell;
   maze_Cell nextCell;
    
    TopoDS_Shape temp1;
    
    bool allCellsVisited=false;
    
    /////////////////////////////////////////////////////////////////////////////////////////////////
     //initialize cellVector
    for (int i=0; i<xUnits; i++){
	if ((i%2) ==0){
	    for(int j=0; j<yUnits; j++){
		if ((j%2)==0){
		    maze_Cell temp;
		    gp_Pnt temp2((i*width+offset),(j*length+offset),0);
		    temp.Cell=temp2;
		    temp.Used=false;
		    cellVector.push_back(temp);
		    cout<<"temp.Cell.X"<<temp.Cell.X()<<"  .Y"<<temp.Cell.Y()<<"  .Z"<<temp.Cell.Z()<< endl;
		}
	    }
	}
    }
    cout<<"cellVector.size("<<cellVector.size()<<")"<<endl;
       
    // initialize borderVector
    for (int i=0; i<xUnits; i++){
	if ((i%2)==0){
	    for(int j=0; j<yUnits-1; j++){
		if ((j%2)==0){
		    maze_Cell temp;
		    gp_Pnt temp2((i*width+offset),((j+1)*length+offset),0);
		    temp.Cell=temp2;
		    temp.Used=false;
		    borderVector.push_back(temp);
		     cout<<"temp.Cell.X"<<temp.Cell.X()<<"  .Y"<<temp.Cell.Y()<<"  .Z"<<temp.Cell.Z()<< endl;
		}
	    }
	}
	
	else{
	    for(int j=0; j<yUnits; j++){
		maze_Cell temp;
		gp_Pnt temp2((i*width+offset),(j*length+offset),0);
		temp.Cell=temp2;
		temp.Used=false;
		borderVector.push_back(temp);
		cout<<"temp.Cell.X"<<temp.Cell.X()<<"  .Y"<<temp.Cell.Y()<<"  .Z"<<temp.Cell.Z()<< endl;
	    }
	}
    }
  
    cout<<"borderVector.size("<<borderVector.size()<<")"<<endl;
    /////////////////////////////////////////////////////////////////////////////////////////////////
    
    // start maze generation
    
    // 	generate basic cube
    TopoDS_Solid Box00=BRepPrimAPI_MakeBox(PntOrig, (xUnits*width)+2*offset, (yUnits*length)+2*offset, (depth-thickness));
     Box0=Box00;
   
    // 	select first cell
    srand((unsigned)time(NULL));	
    int randomA= (int) (rand()%(((cellVector.size())-1)+1));
    currentCell=cellVector.at(randomA);

    //	first boolean operation
    TopoDS_Solid Box01=BRepPrimAPI_MakeBox(currentCell.Cell, width, length, depth);
    TopoDS_Shape Box1=Box01;
	
    temp1=Box1;
    //emit setProgress(-1,"Boolean Operation");
    BRepAlgoAPI_Cut cutOperation(Box0, Box1); // define end execute cut Operation: subdivide Box1 from Box0
    if (cutOperation.IsDone()){ // if cut operation is successful: return 1
	temp1=cutOperation.Shape(); // copy shape to temp1     
	cout<<"Boolean operation 1"<<endl;
	}	
    Box0=temp1; // replace origininal Box0 by temp1, that the result of cutting operation
    //emit setProgress(-1,"Boolean Operation Done");
    
    //	set  first cell as used in cellVector
    currentCell.Used=true;
    for (int q=0; q<cellVector.size();q++){
	maze_Cell temp=cellVector.at(q);
	gp_Pnt A=temp.Cell;
	gp_Pnt B=currentCell.Cell;
	if (comparePoints(A,B)) {// wall exists and is not yet broken
	    cellVector.erase(cellVector.begin()+q);
	    cellVector.push_back(currentCell);
	}
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////
   
    // further maze generation    
     while(!allCellsVisited){
	  
	 // counter to display maze generation
	 //	retrieve amount of  used cells in cellVector
	 int amount=0;
	 for (int q=0; q<cellVector.size();q++){
	     maze_Cell temp=cellVector.at(q);
	     if (temp.Used)
		 amount++;
	 }
	 
	 //	calculate status
	 float total=cellVector.size();// AK is quite detailed, but otherwise it doesn't work properly
	total=amount/total;//AK
	int prog = (int) roundf(100*total); // AK
	emit setProgress(prog,"Generating Maze"); // AK
		
		 
	 
	 
	 cout<<"currentCell.Cell.X"<<currentCell.Cell.X()<<"  .Y"<<currentCell.Cell.Y()<<"  .Z"<<currentCell.Cell.Z()<< endl;/////////////////////////////////////////////////////////////////////////////////////////////////
	 // 	get all neighbour cells of currentCell that are unused in cellVector
	 neighCellVector.clear();
	 
	 //		check pos x 
	 gp_Pnt pXpos((currentCell.Cell.X()+2*width),currentCell.Cell.Y(),0);
	 for (int q=0; q<cellVector.size();q++) {
	     maze_Cell temp=cellVector.at(q);
	     gp_Pnt A=temp.Cell;
	     //cout<<"21         temp.Used"<<temp.Used<<endl;
	     if (comparePoints(pXpos,A)&&(temp.Used==false)) {// cell exists and is not yet used 
		 neighbor_Cell temp2;
		 temp2.Cell=cellVector.at(q);
		 temp2.Orient=1;
		 neighCellVector .push_back(temp2);
		 cout<<"   curNeighCells.push_back(pXpos) Orient=1"<<endl;
	     }
	 }
	 
	 //		check neg x border
	 gp_Pnt pXneg((currentCell.Cell.X()-2*width),currentCell.Cell.Y(),0);
	 for (int q=0; q<cellVector.size();q++){		
	     maze_Cell temp=cellVector.at(q);
	     gp_Pnt A=temp.Cell;
	     if (comparePoints(pXneg,A)&&(temp.Used==false)) {// cell exists and is not yet used 
		 neighbor_Cell temp2; 
		 temp2.Cell=cellVector.at(q);
		 temp2.Orient=2;
		 neighCellVector .push_back(temp2);
		 cout<<"   curNeighCells.push_back(pXneg) Orient=2"<<endl;
	     }
	 }
	 
	 //		check pos y border
	 gp_Pnt pYpos(currentCell.Cell.X(),(currentCell.Cell.Y()+2*length),0);
	 for (int q=0; q<cellVector.size();q++){
	     maze_Cell temp=cellVector.at(q);
	     gp_Pnt A=temp.Cell;
	     
	     if (comparePoints(pYpos,A)&&(temp.Used==false)) {// cell exists and is not yet used 
		 neighbor_Cell temp2; 
		 temp2.Cell=cellVector.at(q);
		 temp2.Orient=3;
		 neighCellVector .push_back(temp2);
		 cout<<"   curNeighCells.push_back(pYpos) Orient=3"<<endl;
	     }
	 }
	 
	 //		check neg y border
	 gp_Pnt pYneg(currentCell.Cell.X(),(currentCell.Cell.Y()-2*length),0);
	 for (int q=0; q<cellVector.size();q++){
	     maze_Cell temp=cellVector.at(q);
	     gp_Pnt A=temp.Cell;
	     
	     if (comparePoints(pYneg,A)&&(temp.Used==false)) {// cell exists and is not yet used 
		 neighbor_Cell temp2;
		 temp2.Cell=cellVector.at(q);
		 temp2.Orient=4;
		 neighCellVector .push_back(temp2);
		 cout<<"   curNeighCells.push_back(pYneg) Orient=4"<<endl;
	     }
	 }
	 
	 cout<<"neighCellVector.size()  "<<neighCellVector.size()<<endl;/////////////////////////////////////////////////////////////////////////////////////////////////
	 
	 // 		get one neighbourCell,
	 if ( neighCellVector.size())
	 {
	     // 		get one out of the vector
	     //srand((unsigned)time(NULL));	
	     int rdNeigh= (int) (rand()%((( neighCellVector.size())-1)+1));
	     cout<<"rdNeigh"<<rdNeigh<<endl;////////////////////////////////////////////////////////////////////////////////////////////////
	     tempCell= neighCellVector.at(rdNeigh);
	     nextCell=tempCell.Cell;
	     nextCell.Used=true;
	     
	     
	     // 		execute boolean operation of next cell
	     TopoDS_Solid Box02=BRepPrimAPI_MakeBox(nextCell.Cell, width, length, depth);
	     TopoDS_Shape Box2=Box02;
	     
	     TopoDS_Shape temp2=Box2;
	     //emit setProgress(-1,"Boolean Operation");
	     BRepAlgoAPI_Cut cutOperation2(Box0, Box2); // define end execute cut Operation: subdivide Box1 from Box0
	     if (cutOperation2.IsDone()){ // if cut operation is successful: return 1
		 temp2=cutOperation2.Shape(); // copy shape to temp1     
		 cout<<"Boolean operation Cell"<<endl;
	     }		
	     Box0=temp2; 
	     //emit setProgress(-1,"Boolean Operation Done");
	     // 		set next cell used
	     for (int q=0; q<cellVector.size();q++){
		 maze_Cell temp=cellVector.at(q);
		 gp_Pnt A=temp.Cell;
		 gp_Pnt B=nextCell.Cell;
		 
		 if (comparePoints(A,B)) {// wall exists and is not yet broken
		     cellVector.erase(cellVector.begin()+q);
		     cellVector.push_back(nextCell);
		     cout<<"cellVector.push_back(nextCell);"<<endl;///////////////////////////////////////////////////////////////////
		 }
	     }
	     
	     // 	delete wall between current cell and next cell
	     //		identify particular wall
	     
	     maze_Cell destroyedWall;
	     destroyedWall.Used=true;
	     
	     cout<<"tempCell.Orient "<<tempCell.Orient<<endl;/////////////////////////////////////////////////////////////////////////////////////////////////
	     
	     if (tempCell.Orient==1){
		 // border in x pos
		 gp_Pnt tempPoint1((currentCell.Cell.X()+width),currentCell.Cell.Y(), currentCell.Cell.Z());
		 destroyedWall.Cell=tempPoint1;
		 
	     }
	     
	     if (tempCell.Orient==2){
		 //border in x neg
		 gp_Pnt tempPoint2((currentCell.Cell.X()-width),currentCell.Cell.Y(), currentCell.Cell.Z());
		 destroyedWall.Cell=tempPoint2;
		 
	     }
	     
	     if (tempCell.Orient==3){
		 // border in y pos
		 gp_Pnt tempPoint3(currentCell.Cell.X(),(currentCell.Cell.Y()+length), currentCell.Cell.Z());
		 destroyedWall.Cell=tempPoint3;
		 
	     }
	     
	     if (tempCell.Orient==4){
		 //border in y neg
		 gp_Pnt tempPoint4(currentCell.Cell.X(),(currentCell.Cell.Y()-length), currentCell.Cell.Z());
		 destroyedWall.Cell=tempPoint4;
		 
	     }
	     
	     
	     //		execute bool
	     gp_Pnt tempBase=destroyedWall.Cell;
	     TopoDS_Solid Box03=BRepPrimAPI_MakeBox(tempBase, width, length, depth);
	     TopoDS_Shape Box3=Box03;
	     
	     TopoDS_Shape temp3=Box3;
	     //emit setProgress(-1,"Boolean Operation");
	     BRepAlgoAPI_Cut cutOperation3(Box0, Box3); // define end execute cut Operation: subdivide Box1 from Box0
	     if (cutOperation3.IsDone()){ // if cut operation is successful: return 1
		 temp3=cutOperation3.Shape(); // copy shape to temp1     
		 cout<<"Boolean operation Wall"<<endl;
	     }		
	     Box0=temp3; 
	     //emit setProgress(-1,"Boolean Operation Done");
	     
	     //		set wall deletet
	     for (int q=0; q<borderVector.size();q++){
		 maze_Cell temp=borderVector.at(q);
		 gp_Pnt A=temp.Cell;
		 gp_Pnt B=destroyedWall.Cell;
		 
		 if (comparePoints(A,B)) {// wall exists and is not yet broken
		     borderVector.erase(borderVector.begin()+q);
		     borderVector.push_back(destroyedWall);
		     cout<<"borderVector.push_back(destroyedWall);"<<endl;///////////////////////////////////////////////////////////////////
		 }
	     }
	     
	     // nextcell=current cell
	     currentCell=nextCell;   
	     cout<<" currentCell=nextCell;   "<<endl;///////////////////////////////////////////////////////////////////
	 } 
	 
	 
	 else{
	     // If there are no more unused neighbor cells, then take another cell from the vector and check
	     
	     //	check if there is still a cell not used
	     bool notYetAll2=false;
	     
	     for (int i=0; i<cellVector.size(); i++){
		 maze_Cell temp=cellVector.at(i);
		 
		 if (temp.Used==0){
		     notYetAll2=true;
		     cout<<"notYetAll2=true"<<endl;/////////////////////////////////////////////////////////////////////////////////////////////////
		 }
		 
	     }
	     
	     // if there is no unused cell anymore exit
	     if(!notYetAll2){   
		 allCellsVisited=true;
		 cout<<"allCellsVisited=true;   "<<endl;/////////////////////////////////////////////////////////////////////////////////////////////////
	     }
	     
	     // if there are still unused cells 
	     bool check=0;
	     while(check==0){
		 int randomA= (int) (rand()%(((cellVector.size())-1)+1));//statt -2
		 maze_Cell temp=cellVector.at(randomA);
		 if (temp.Used==1){// only used cell as base for further mazing
		     currentCell=temp;
		     check=1;
		 }
	     }
	 }
	 
	 //	check if there is still a cell not used
	 bool notYetAll=false;
	 
	 for (int i=0; i<cellVector.size(); i++){
	     maze_Cell temp=cellVector.at(i);
	     if (temp.Used==0){
		 notYetAll=true;
		 cout<<"notYetAll=true;  "<<endl;/////////////////////////////////////////////////////////////////////////////////////////////////
	     }
	     
	 }
	 
	 if(!notYetAll){   
	     allCellsVisited=true;
	     cout<<"allCellsVisited=true;   "<<endl;/////////////////////////////////////////////////////////////////////////////////////////////////
	 }
     }
	
     emit setProgress(-1,"Maze generated");	
     emit sendShape(Box0); // send shape to selectPatch
     emit setZoom();
     
     cellVector.clear();
    borderVector.clear();
    neighCellVector.clear();
    return;
}

bool basicCADmodule::comparePoints(gp_Pnt a,gp_Pnt b)
{
    bool result=0, xIdent=0, yIdent=0, zIdent=0;
    
    xIdent=!(a.X()-b.X());
    yIdent=!(a.Y()-b.Y());
    zIdent=!(a.Z()-b.Z());
    
    if (xIdent && yIdent && zIdent)
	result=1;
    return result;
}

void basicCADmodule::slotSaveSTEP()
{
    //set saving directory
    char dir[]="maze STEP files"; 
     char parentdir[]="..";
    chdir(dir);
    
    //explorer to get solid out of shape
    TopExp_Explorer Ex;
    TopoDS_Solid solid;
    for (Ex.Init(Box0,TopAbs_SOLID); Ex.More(); Ex.Next()) {
	solid = BRepBuilderAPI_MakeSolid(TopoDS::Solid(Ex.Current()));
    }
    
    // transfer solid to STEP 
    STEPControl_Writer writer;
    writer.Transfer(solid, STEPControl_ManifoldSolidBrep);
    
    emit setProgress(-1,"Save Maze");

    /*
    // enumerating file name
    static int M=0;
    char format[] = "Maze-%i.stp"; 
    char filename[sizeof format+100]; 
    sprintf(filename,format,M); 
    M++;
    */

    // time stamp
    char filename[256]="";  
    struct tm *LokalZeit;
    time_t Zeit;  
    time(&Zeit);
    LokalZeit = localtime(&Zeit);  
   // sprintf(filename, "maze - %02d%02d%d.stp", LokalZeit->tm_mday, LokalZeit->tm_mon+1, LokalZeit->tm_year+1900);
      
    sprintf(filename, " rd maze - %d %02d %02d_time %02dh%02dmin.stp", LokalZeit->tm_year+1900, LokalZeit->tm_mon+1, LokalZeit->tm_mday,LokalZeit->tm_hour, LokalZeit->tm_min);
    
    writer.Write(filename);
    
   chdir(parentdir);
    return;
}
	    
	    
