/*************************************************************
** License: GPL.  
** Copyright (C) 2006 Mark Pictor
**
** camTabs: UI tabs, lists, buttons for CAM.
*************************************************************/
 
#include "camTabs.h"
#include <qmessagebox.h>


myListView::myListView(QWidget * parent, const char * name, WFlags f):QListView(parent, name, f) {
}

void myListView::setSorting ( int column, bool ascending )
{
    if ( column > 0 )  
	QListView::setSorting (  column,  ascending );
}

camTabs::camTabs(QVBox *lf) 
{
    frame = lf;
    const char * toolTypeName[] = {"Finish","Special","Rough"};
    const char * opTypeName[] = {"Finish","Special/Other","Intermediate", "Rough"};
    setupTabsAndTrees();
}

camTabs::~camTabs() 
{

}

void camTabs::init() 
{
	//clear face list, clear op list, tools ??
}

void camTabs::setupTabsAndTrees()
{
      tabControl = new QHGroupBox(frame);
	//tabControl->setMaximumHeight(40);
	 buttonAdd = new QToolButton(tabControl,"Add");
	  buttonAdd->setText("Add");
	 buttonDel = new QToolButton(tabControl);
	  buttonDel->setText("Delete Selected");
	 buttonProp= new QToolButton(tabControl);
	  buttonProp->setText("Properties");

      tabWidget = new QTabWidget(frame);
              featureTab = new myListView(tabWidget);
	//AK opTab = new myListView(tabWidget);
	//AK toolTab = new myListView(tabWidget);
//Tab Feature	
       tabWidget->addTab(featureTab,"Features");
	  featureTab->setRootIsDecorated(true);
	  featureTab->addColumn("Type");
	  featureTab->addColumn("id");
	  featureTab->addColumn("Referenced?");
	  //featureTab->setSorting(-1);
	  featureTab->setSelectionMode(QListView::Single);
	  featureTab->setAllColumnsShowFocus(true);
	  
	  edgeFeatHeader = new QListViewItem(featureTab,"Edge");
	  faceFeatHeader = new QListViewItem(featureTab,"Face");
	/* AK
//Tab Operations	  
          tabWidget->addTab(opTab,"Operations");
	  opTab->setRootIsDecorated(true);
	  opTab->addColumn("Type");
	  opTab->addColumn("id");
	  opTab->addColumn("Tool");
	  opTab->setSorting(-1);
	  opTab->setSelectionMode(QListView::Single);
	  opTab->setAllColumnsShowFocus(true);
	  
	  finishOpHeader = new QListViewItem(opTab,"Finish");
	  specialOpHeader = new QListViewItem(opTab,"Special/Other");
	  intermediateOpHeader = new QListViewItem(opTab,"Intermediate");
	  roughOpHeader = new QListViewItem(opTab,"Rough");
	
//Tab Tooling	  
           tabWidget->addTab(toolTab,"Tooling");
	  toolTab->setSorting(-1);
	  toolTab->setRootIsDecorated(true);
	  toolTab->addColumn("Use");  //corresponds to an operation, above
	  toolTab->addColumn("Shape"); //ball, flat, V, etc
	  toolTab->addColumn("Tool Num");
	  toolTab->addColumn("Dia");
	  toolTab->setSelectionMode(QListView::Single);
	  toolTab->setAllColumnsShowFocus(true); 
	  
	  finishTlHeader = new QListViewItem(toolTab,"Finish");
	  specTlHeader = new QListViewItem(toolTab,"Special");
	  roughTlHeader = new QListViewItem(toolTab,"Rough");
	  
	  QListViewItem exTool = new QListViewItem(finishTlHeader,"","Ball","5","10");*/
    
    connect (buttonAdd,SIGNAL(clicked()),this,SLOT(clickedAddButton()));
    connect (buttonDel,SIGNAL(clicked()),this,SLOT(clickedDelButton()));
    connect (buttonProp,SIGNAL(clicked()),this,SLOT(clickedPropButton()));

}

void camTabs::clickedAddButton()
{
    if (tabWidget->currentPage() == featureTab) 
              {
	QMessageBox::information(frame,"Feature Selection","Click on the Face or Edge button on the Selection\nMode toolbar, then choose a face or edge in the display.");
               } 
   /* AK else if (tabWidget->currentPage() == opTab) 
              {
	addOperation();
              } 
    else if (tabWidget->currentPage() == toolTab) 
              {
	addTool();
               }*/
}

void camTabs::clickedDelButton()
{
    myListView* lv=(myListView*)tabWidget->currentPage();
    if((lv->selectedItem()) && (lv->selectedItem()->parent())) 
               {
	delete lv->selectedItem();
	 
               } 
    else 
              {
	emit setProgress(-1,"Can't delete: bad or no selection.");
              }
}

void camTabs::clickedPropButton()
{
    myListView* lv=(myListView*)tabWidget->currentPage();
    if((lv->selectedItem()) && (lv->selectedItem()->parent())) 
              {
	if (lv == featureTab) 
	        {
                       } 
	else if (lv == opTab) 
	        {
                       } 
	else if (lv == toolTab) 
	         {
                       }
               } 
    else 
               {
	emit setProgress(-1,"No properties: bad or no selection.");
                }
}

void camTabs::addOperation()
{

}

void camTabs::addTool()
{

}

void camTabs::addFeature(TopoDS_Edge E)
{

}

void camTabs::slotAddFace(uint f)
{
    QString s; s.sprintf("%u",f);
    (void) new QListViewItem(faceFeatHeader,"",s,"");
}
