/*************************************************************
** License: GPL.  
** Derived from work by Mark Pictor. 
** Copyright (C) 2007 Andreas Kain
*************************************************************/

//rs274emc.cpp
//output g-code
#include "rs274emc.h"
#include "pathAlgo.h"

#include <TopoDS_Shape.hxx>//AK

//AK+++++++++++++++++++++++++++++++++++
rs274emc::rs274emc(pathAlgo *pAlg)
{
    Path=pAlg; //makes Path->projectedPasses available here, it is vontained in class pathAlgo :)
    connect(pAlg,SIGNAL(setComputed(int)),this,SLOT(slotGetPathComputed(int)));
    connect(pAlg,SIGNAL(sendSafeHeight(Standard_Real)),this,SLOT(slotGetSafeHeight(Standard_Real)));
    feed=0;
    speed=0;
    toolDia=0;
    safeHeight=0;
}

rs274emc::~rs274emc()
{
}

void rs274emc::slotGetPathComputed(int switcher)
{
      cout<<" rs274emc::slotGetPathComputed(int switcher)" <<switcher<<endl;
    bool temp;
      temp = staticVaultComputed(switcher);
      return;
}

void rs274emc::slotGetSafeHeight(Standard_Real safeHeightPathAlgo)
{
    safeHeight=safeHeightPathAlgo;
}

void rs274emc::slotGetFeed(int feedValue)
{
    feed=feedValue;
}


void rs274emc::slotGetSpeed(int speedValue)
{
    speed=speedValue;
}


void rs274emc::slotGetToolDia(int toolDiaValue)
{
    toolDia=toolDiaValue;
}


void rs274emc::initStaticVault()
{
    staticVaultComputed(2);
}

bool rs274emc::staticVaultComputed(int switcher)
//functinality of setting pathComputed =true is better realized by signals, but don't know how
{
    //switcher ==  1  : write true
    //switcher == 2  : write false
    //switcher == default :  read
    
    static bool pathComputed=false; //initial value
    
     switch(switcher){
     case 1: //write true
	pathComputed=true;
	cout<<"pathComputed after switcher==1 write true " <<pathComputed<<endl;
	break;
    case 2: //write false
	pathComputed=false;
	cout<<"bool rs274emc::staticvault(int switcher) - pathComputed after switcher==2 write false " <<pathComputed<<endl;
	break;
    default:
	cout<<"bool rs274emc::staticvault(int switcher) - pathComputed " <<pathComputed<<endl;
	break;}
    emit sendPathComputed(pathComputed);
   return pathComputed;   	
}



void rs274emc::slotOutputProtoCode() 
//creates g-Code and saves results to a file
{
    //view Path->projectedPasses
    cout<<"Path->projectedPasses.size(" <<Path->projectedPasses.size()<<")"<<endl; //AK
    for(uint i=0;i < Path->projectedPasses.size(); i++){
	for(uint j=0;j < Path->projectedPasses.at(i).facesUsed.size(); j++)  //AK testing//AK testing
	    cout<<"Path->projectedPasses.at("<<i<<").facesUsed.size("<<j<<")" <<endl; //AK
    }
    
    //check if Toolpath is created
    bool checkVault=false;
    int readVault=3;
    checkVault = (staticVaultComputed(readVault));
    
    if (!checkVault) {
	QMessageBox::warning( 0, "Warning", "rs274emc::slotOutputProtoCode - Compute path first !");
	return;}
    
    
    /*for testing only
    else{
	QMessageBox::warning( 0, "Info", "rs274emc::slotOutputProtoCode - pathComputed==true");
	return;}*/
    
   //open File to write g-Code
    FILE *outG;
    
    //new for enumerating files
    static int M=0;
    char format[] = "G-Code-%i.txt"; 

    char dir[]="G-Code files";
     char parentdir[]="..";
    char filename[sizeof format+100]; 
    sprintf(filename,format,M); 
   // outG = fopen(filename,"w"); 
    chdir(dir);
        outG = fopen(filename,"w"); 
	M++;

    
    
    /* //create pop-up window and give filename and folder manually via gui
    QString fileName = QFileDialog::getSaveFileName(QString::null, "G-code (*.ngc)", 0, 0);
    outG = fopen (fileName, "w");//AK
    //outG = fopen ((const char*)fileName, "w");
    if (outG == NULL) {
	QMessageBox::warning( 0, "Warning", "Can't open that file for writing!");
	return;
// uncomment *1*    
}*/
    
    //set pathComputed false
    checkVault=(staticVaultComputed(2));
    
    //write file content
   
    int setZero=0;
    int stCounter=0; 

     gp_Pnt Cal1,Cal2,Cal1L,Cal2L,ToGo,ToGoL,Start,Buffer,veryFirst,verySecond,A,B,C,D;
   
     //g-code header     
     fprintf(outG,"%%_N_TESTMAZE_CAMOCC)\n");
     fprintf(outG,"; File %s created by cam-occ\n",filename); //ought to insert model's file name and the face ID into comment also...
     //  *1*  fprintf(outG,"; File %s created by cam-occ\n",(const char*)fileName); //ought to insert model's file name and the face ID into comment also...
     fprintf(outG,"; Info Safe height for rapids= %f \n",safeHeight);  //safeHeight set in slotComputeSimplePathOnFace() above  // edited by AK
     fprintf(outG,"; Info Tool Diameter = %i \n",toolDia);
     fprintf(outG,"; Info Feed = %i\n",feed);
     fprintf(outG,"; Info Speed = %i \n",speed);
     fprintf(outG,"\n; cancel tool diameter compensation\n");
     fprintf(outG,"N%4i \t G40\n",stCounter+=10);//set cool compensation zero 
     fprintf(outG,"\n; turn on air cooling\n");
     fprintf(outG,"N%4i \t M71\n",stCounter+=10);//turn on air cooling
     fprintf(outG,"\n; move to home position\n"); 
     fprintf(outG,"N%4i \t G00 X  0.00 \t Y  0.00 \t Z 10.00\n",stCounter+=10); //float limited to 2 decimals, move to home position
     //fprintf(outG,"; tool change\n"); //float limited to 2 decimals
     //fprintf(outG,"N%4i \t G00\t Z100\n",stCounter+=10); //float limited to 2 decimals
     //fprintf(outG,"N%4i \tT4 \t D1 \tM6 \n",stCounter+=10); //float limited to 2 decimals
     fprintf(outG,"\n; speed \t feed \t spindle on clockwise\n");
     fprintf(outG,"N%4i \t S%i \tF%i \tM3 \n",stCounter+=10, speed, feed); //speed, feed spinle clockwise

    
    //missing in g-code
    // move to retract home base
    //switch on spindle
    //move fast to safety distance
    //set working feed
    //move to first point
    
  // Schruppen
      // Senken -2,5 F50
     // schneiden mit F120
    
  //Schlichten
      // Senken -1,5 F 50
      //schneiden mit F200
    
	    
    cout <<"Path->projectedPasses.size()="<< Path->projectedPasses.size() <<endl;	    
    //this loop processes the blue lines drawn on the face
    for (int j=0;j<Path->projectedPasses.size();j++){
	
	int checkLoop = setZero;
	
	cout <<"Test1"<<endl;
	    
	//explorer to decompose the shape and find all edges in shape section AK
	TopExp_Explorer Ex;
	int forCounter=0;
		
	for (Ex.Init(Path->projectedPasses.at(j).P,TopAbs_EDGE); Ex.More(); Ex.Next()) {
	    TopoDS_Wire W = BRepBuilderAPI_MakeWire(TopoDS::Edge(Ex.Current()));
	    TopoDS_Vertex E1,E2;
	    
	    cout <<"Test2"<<endl;
	  
	    //find endpoints of wire, E1 E2 from pathAlgo analyzation
	    TopExp::Vertices(W,E1,E2);
	 
	    // Get the 3D point for the vertex.
	    Cal1 = BRep_Tool::Pnt(E1);
	    Cal2 = BRep_Tool::Pnt(E2);
		
	    if(!forCounter){  // fitst wire decomposed in start- and endpoint
		Cal1L=Cal1;
		Cal2L=Cal2;
		    
		veryFirst=Cal1L;
		verySecond=Cal2L;
	    }
	    
	    if(forCounter){  //second and more wire decomposed
		//to calculate we have Start, ToGo, ToGoL,Cal1, Cal2
		    
		//connect next wire to Cal1L    
		if(comparePoints(Cal1L,Cal1)){
		    ToGoL=Cal1L;
		    ToGo=Cal2;
		    Start=Cal2L;    
		}
	
		if (comparePoints(Cal1L,Cal2)) {
		    ToGoL=Cal1L;
		    ToGo=Cal1;
		    Start=Cal2L;	
		}
		    
		//Connect next Wire to Cal2L
		if(comparePoints(Cal2L,Cal1)){
		    ToGoL=Cal2L;
		    ToGo=Cal2;
		    Start=Cal1L;
		}
		    
		if(comparePoints(Cal2L,Cal2)) {
		    ToGoL=Cal2;
		    ToGo=Cal1;
		    Start=Cal1L;
		}
		
		if(forCounter==1&& (!j)){
		    fprintf(outG,"\n; move to starting point (z) of first level and plunge \n"); //float limited to 2 decimals 
		    fprintf(outG,"N%4i \t G00 X%6.2f \t Y%6.2f \t Z%6.2f \n",stCounter+=10,Start.X(),Start.Y(),safeHeight); //float limited to 2 decimals
		    fprintf(outG,"N%4i \t G01 X%6.2f \t Y%6.2f \t Z%6.2f \t F50 \n",stCounter+=10,Start.X(),Start.Y(),Start.Z()); //float limited to 2 decimals
		    
		    fprintf(outG,"\n; move along tool-path (xy) of first level \n"); //float limited to 2 decimals 
		    fprintf(outG,"N%4i \t G01 X%6.2f \t Y%6.2f \t Z%6.2f \t F%i \n",stCounter+=10,ToGoL.X(),ToGoL.Y(),ToGoL.Z(), feed); //float limited to 2 decimals
		}
		
		if(forCounter==1 && j){
		    //move to first point of new level but in z offset
		    fprintf(outG,"\n; move to starting point (z) of next level and plunge\n"); //float limited to 2 decimals
		    fprintf(outG,"N%4i \t G01 X%6.2f \t Y%6.2f \t Z%6.2f \t F%i \n",stCounter+=10,Start.X(),Start.Y(),Buffer.Z(), feed); //float limited to 2 decimals
		    fprintf(outG,"N%4i \t G01 X%6.2f \t Y%6.2f \t Z%6.2f \t F50 \n",stCounter+=10,Start.X(),Start.Y(),Start.Z()); //float limited to 2 decimals
		    
		    fprintf(outG,"\n; move along tool-path (xy) of next level \n"); //float limited to 2 decimals 
		    fprintf(outG,"N%4i \t G01 X%6.2f \t Y%6.2f \t Z%6.2f \t F%i \n",stCounter+=10,ToGoL.X(),ToGoL.Y(),ToGoL.Z(), feed); //float limited to 2 decimals
		}
		  
		//output standard points to move to
		fprintf(outG,"N%4i \t G01 X%6.2f \t Y%6.2f \t Z%6.2f \t F%i \n",stCounter+=10,ToGo.X(),ToGo.Y(),ToGo.Z(), feed); //float limited to 2 decimals
		  
		Buffer=ToGo;
		Cal1L=Cal1;
		Cal2L=Cal2;
	    }
	    
	    forCounter++;
	}
	   
	//go around the rectangle once to cut "cheese chunks
	if (veryFirst.X() == ToGoL.X() || veryFirst.Y() == ToGoL.Y()){
	    A=veryFirst;
	    B=ToGoL;
	    C=ToGo;
	    D=verySecond; 
	}
	   
	if (verySecond.X() == ToGoL.X() || verySecond.Y() == ToGoL.Y()){
	    A=verySecond;
	    B=ToGoL;
	    C=ToGo;
	    D=veryFirst; 
	}
	       
	fprintf(outG,"\n; move around rectangle (xy) \n"); //float limited to 2 decimals 
	fprintf(outG,"N%4i \t G01 X%6.2f \t Y%6.2f \t Z%6.2f \t F%i \n",stCounter+=10,A.X(),A.Y(),A.Z(), feed); //float limited to 2 decimals
	fprintf(outG,"N%4i \t G01 X%6.2f \t Y%6.2f \t Z%6.2f \t F%i \n",stCounter+=10,B.X(),B.Y(),B.Z(), feed); //float limited to 2 decimals
	fprintf(outG,"N%4i \t G01 X%6.2f \t Y%6.2f \t Z%6.2f \t F%i \n",stCounter+=10,C.X(),C.Y(),C.Z(), feed); //float limited to 2 decimals    
	fprintf(outG,"N%4i \t G01 X%6.2f \t Y%6.2f \t Z%6.2f \t F%i \n",stCounter+=10,D.X(),D.Y(),D.Z(), feed); //float limited to 2 decimals
	   
	checkLoop++;// rise loop counter
    }

    //g-code footer
    
    fprintf(outG,"\n;move to safeHeight\n");    
    fprintf(outG,"N%4i \t G01 X%6.2f \t Y%6.2f \t Z%6.2f\n",stCounter+=10,ToGo.X(),ToGo.Y(),safeHeight); //float limited to 2 decimals   
    fprintf(outG,"\n;stop spindle turning\n");  
    fprintf(outG,"N%4i \t M05 \t  \n",stCounter+=10);    
    fprintf(outG,"\n; move to home position\n");      
    fprintf(outG,"N%4i \t G00 X  0.00 \t Y  0.00 \t Z 10.00\n",stCounter+=10); //float limited to 2 decimals   
    fprintf(outG,"\n; end of main program\n");  
    fprintf(outG,"N%4i \t M30\n",stCounter+=10);
    
    fclose(outG);// close file
    
 chdir(parentdir);
    return ;
}  


bool rs274emc::comparePoints(gp_Pnt a,gp_Pnt b)
{
    bool result=0, xIdent=0, yIdent=0, zIdent=0;
    
    xIdent=!(a.X()-b.X());
    yIdent=!(a.Y()-b.Y());
    zIdent=!(a.Z()-b.Z());
    
    if (xIdent && yIdent && zIdent)
	result=1;
    return result;
}
//++++++++++++++++++++++++++++++++++++++AK
    
	/*

	  
// fprintf(outG,"G01 PT1 X%f Y%f\ Z%f\n",pt1.X(),pt1.Y(),pt1.Z());
	  //fprintf(outG,"N%i G01 PT1 X%f Y%f\ Z%f\n",j,pt1.X(),pt1.Y(),pt1.Z());
	 //fprintf(outG,"G01 PT2 X%f Y%f\ Z%f\n",pt2.X(),pt2.Y(),pt2.Z());
	 // fprintf(outG,"N%i G01 PT2 X%f Y%f\ Z%f\n",(j+1),pt2.X(),pt2.Y(),pt2.Z());
	//Handle(Geom_Curve) C = BRep_Tool::Curve(E,loc,first,last);
	//C->Transform(loc.Transformation());  //transform C using loc
	if (odd & !unidirectional) {
	    odd = false;
	    tmp = last;
	    last = first;
	    first = last;
	} 
	else {
	    odd = true;  //for next time around
	}
	
	for (Standard_Real j=first;j<=last;j=j+(last-first)/100) {
	    gp_Pnt pt = C->Value(j);
	    
	    if (betweenCuts) {
		fprintf(outG,"G00 X%f Y%f\nG01 Z%f\n",pt.X(),pt.Y(),pt.Z());
		betweenCuts = false;xzPlane, yzPlane
	    }
	    else {
		fprintf(outG,"G01 X%f Y%f Z%f (curve %i parm %f)\n",pt.X(),pt.Y(),pt.Z(),i,j);
	    }
	}
	fprintf(outG,"G01 Z#101 G4 P0\n");  //feedrate move to safe height (var #101)
    }
    //projectedLines // don*t know???
    
    //is it possible to recognize lines and arcs, to make better g-code?  Dump equation coefficients, see if it's a line or circle?
    */


