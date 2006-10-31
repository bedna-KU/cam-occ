/*************************************************************
** License: GPL.  
** Copyright (C) 2006 Mark Pictor
**
** camTabs: UI tabs, lists, buttons for CAM.
*************************************************************/
 
#include "camTabs.h"
//#include <string>
#include <qmessagebox.h>


//QListView ( QWidget * parent = 0, const char * name = 0, WFlags f = 0 )
myListView::myListView(QWidget * parent, const char * name, WFlags f):QListView(parent, name, f) 
{
}

void myListView::setSorting ( int column, bool ascending )
{
    if ( column > 0 )  QListView::setSorting (  column,  ascending );
}


camTabs::camTabs(QVBox *lf) {
    frame = lf;
    const char * toolTypeName[] = {"Finish","Special","Rough"};
    const char * opTypeName[] = {"Finish","Special/Other","Intermediate", "Rough"};
    setupTabsAndTrees();
}

camTabs::~camTabs() 
{

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
	opTab = new myListView(tabWidget);
	toolTab = new myListView(tabWidget);
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
	  QListViewItem exTool = new QListViewItem(finishTlHeader,"","Ball","5","10");
    connect (buttonAdd,SIGNAL(clicked()),this,SLOT(clickedAddButton()));
    connect (buttonDel,SIGNAL(clicked()),this,SLOT(clickedDelButton()));
    connect (buttonProp,SIGNAL(clicked()),this,SLOT(clickedPropButton()));

}

// void featureTab::setSorting ( int column, bool ascending = TRUE )
// {
//     if ( column > 0 )  QListView::setSorting (  column,  ascending );
// }




void camTabs::clickedAddButton()
{
    if (tabWidget->currentPage() == featureTab) {
	    QMessageBox::information(frame,"Feature Selection","Click on the Face or Edge button on the\nSelection Mode toolbar, then choose a face or edge in the display.");
    } else if (tabWidget->currentPage() == opTab) {
	//if ((opTab->selectedItem()) && (selectedItem()->parent()))
	    //opTab->setSelected(opTab->selectedItem()->parent(),true);
	addOperation();
    } else if (tabWidget->currentPage() == toolTab) {
	addTool();
    }
}

void camTabs::clickedDelButton()
{
    myListView* lv=(myListView*)tabWidget->currentPage();
    if(lv->selectedItem()->parent()) {
	delete lv->selectedItem();
    }
}

void camTabs::clickedPropButton()
{
    if (tabWidget->currentPage() == featureTab) {

    } else if (tabWidget->currentPage() == opTab) {

    } else if (tabWidget->currentPage() == toolTab) {

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

void camTabs::addFeature(TopoDS_Face F)
{

}
