 

treeView::treeView(QVBox *leftFrame) {
      tabControl = new QHGroupBox(leftFrame);
	//tabControl->setMaximumHeight(40);
	buttonAdd = new QToolButton(tabControl,"Add");
	  buttonAdd->setText("Add");
	buttonDelete = new QToolButton(tabControl);
	  buttonDelete->setText("Delete");
	buttonProperties= new QToolButton(tabControl);
	  buttonProperties->setText("Properties");
      tabWidget = new QTabWidget(leftFrame);
        featureTab = new QListView(tabWidget);
	opTab = new QListView(tabWidget);
	toolTab = new QListView(tabWidget);
	tabWidget->addTab(featureTab,"Features");
	  featureTab->setRootIsDecorated(true);
	  featureTab->addColumn("Type");
	  featureTab->addColumn("id");
	  featureTab->addColumn("Referenced?");
	  featureTab->setSorting(-1);
	  edgeFeatHeader = new QListViewItem(featureTab,"Edge");
	  faceFeatHeader = new QListViewItem(featureTab,"Face");
	tabWidget->addTab(opTab,"Operations");
	  opTab->setRootIsDecorated(true);
	  opTab->addColumn("Type");
	  opTab->addColumn("id");
	  opTab->addColumn("Tool");
	  opTab->setSorting(-1);
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
	  finishTlHeader = new QListViewItem(toolTab,"Finish");
	  specTlHeader = new QListViewItem(toolTab,"Special");
	  roughTlHeader = new QListViewItem(toolTab,"Rough");
	  QListViewItem exTool = new QListViewItem(finishTlHeader,"","Ball","5","10");
}

treeView::~treeView() 
{

}

treeView::