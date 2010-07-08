/***************************************************************************
 *   Copyright (C) 2010 by Mark Pictor                                     *
 *   mpictor@gmail.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "uio.hh"
#include <string>

#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QMessageBox>
#include <QString>

#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_Trihedron.hxx>
#include <V3d_View.hxx>

/**
QShortcuts in use:
qtocc: NOSPBX ZYXCV FARGD
free: AEHIJKLMQRTUW

me:
M-g2m

*/

//initialize static variables
QoccHarnessWindow* uio::windowPtr = 0;
Handle_V3d_View uio::viewPtr = 0;
Handle_AIS_InteractiveContext uio::contextPtr = 0;
QoccViewWidget* uio::occPtr = 0;
QoccViewerContext* uio::vcPtr = 0;
QMenuBar* uio::mbPtr = 0;
QAction* uio::hmPtr = 0;
std::vector<TopoDS_Shape> uio::selectedShapes;
int uio::errors = 0;


uio::uio(QoccHarnessWindow* window) {

  windowPtr = window;
  viewPtr = window->getView();
  contextPtr = window->getContext();
  occPtr = window->getOCC();
  vcPtr = window->getVC();
  mbPtr = window->menuBar();
  hmPtr = window->getHelpMenu();

  initUI();
}

void uio::initUI() {

  slotNeutralSelection();

  QMenu *selectMenu;
  selectMenu = new QMenu("&Select");
  mbPtr->insertMenu(hmPtr,selectMenu);  //put this menu BEFORE the help menu
  //	mbPtr->addMenu(selectMenu);

  QAction *selectNeutralAction;
  selectNeutralAction = new QAction("Neutral", this);
  //selectNeutralAction->setShortcut(QString("Ctrl+N"));
  selectNeutralAction->setStatusTip("Select Nothing");
  connect(selectNeutralAction, SIGNAL(triggered()), this, SLOT(slotNeutralSelection()));
  selectMenu->addAction( selectNeutralAction );

  QAction *selectVertexAction;
  selectVertexAction = new QAction("Vertex", this);
  selectVertexAction->setStatusTip("Select Vertices");
  connect(selectVertexAction, SIGNAL(triggered()), this, SLOT(slotVertexSelection()));
  selectMenu->addAction( selectVertexAction );

  QAction *selectEdgeAction;
  selectEdgeAction = new QAction("Edge", this);
  selectEdgeAction->setStatusTip("Select Edges");
  connect(selectEdgeAction, SIGNAL(triggered()), this, SLOT(slotEdgeSelection()));
  selectMenu->addAction( selectEdgeAction );

  QAction *selectFaceAction;
  selectFaceAction = new QAction("Face", this);
  selectFaceAction->setStatusTip("Select Faces");
  connect(selectFaceAction, SIGNAL(triggered()), this, SLOT(slotFaceSelection()));
  selectMenu->addAction( selectFaceAction );

  QAction *selectSolidAction;
  selectSolidAction = new QAction("Solid", this);
  selectSolidAction->setStatusTip("Select Solids");
  connect(selectSolidAction, SIGNAL(triggered()), this, SLOT(slotSolidSelection()));
  selectMenu->addAction( selectSolidAction );

}

void uio::redraw() {
  viewPtr->Redraw();
}

void uio::getSelection() {
  selectedShapes.clear();
  for ( contextPtr->InitSelected(); contextPtr->MoreSelected(); contextPtr->NextSelected() ) {
    Handle_AIS_InteractiveObject IO = contextPtr->Interactive();
    if ( !contextPtr->HasSelectedShape() )
    {
      infoMsg("no selection");
      //                        contextPtr->ClearSelected();
      //                        break;
    }
    if ( IO->IsKind(STANDARD_TYPE(AIS_Trihedron)) )
    {
      //we don't want to save the trihedron
      //clearSelected apparently clears EVERYTHING, rather than deselecting one thing...
      continue;
    }
    selectedShapes.push_back(contextPtr->SelectedShape());
  }
  cout << "selectedShapes.size(): " << selectedShapes.size() << endl;
}



//TODO: pipe messages to a textBrowser at the bottom of the window
//	  if they are important enough, give them a message box as well


//normally called like this: debugMsg("error string",__FILE__,__LINE__);
void uio::debugMsg(std::string s, std::string f, int l) {
  errors++;
  cerr << "Error  #" << errors << ": " << s << "\n\tin file " << f << " at line " << l << "." << endl;
}


void uio::infoMsg( std::string title, std::string message ) {
  QMessageBox::information(windowPtr,title.c_str(),message.c_str());
}

void uio::infoMsg( std::string message ) {
  QMessageBox::information(windowPtr,"Cam-occ2",message.c_str());
}

/*
void uio::longMsg( QString message ) {
  //	QWidget *dialog = new QWidget;
  //     	Ui::longMsgDlg dlg;
  //	connect(this, SIGNAL(theLongMsg(QString)), dlg.textBrowser, SLOT(setText(QString)));
  //	emit(theLongMsg(message));
  //dlg.textBrowser->setText(message);
  //   	dlg.setupUi(dialog);
  //	dialog->show();

  QMessageBox msg(windowPtr);
  msg.setIcon(QMessageBox::Information);
  msg.addButton("OK",QMessageBox::AcceptRole);
  msg.setText(message);
  msg.setTextFormat(Qt::RichText);
  msg.setWindowTitle("Cam-occ2");
  msg.adjustSize();
  msg.exec();
  //msg.show();

}
*/

/*
QString uio::toString(float a,float b, float c) {
  return QString("%1, %2, %3").arg(a).arg(b).arg(c);
}
*/

std::string uio::toString(double a,double b, double c) {
  std::ostringstream os;
  os << a << ", " << b << ", " << c;
  return os.str();
}

std::string uio::toString(gp_Pnt p) {
  return toString(p.X(),p.Y(),p.Z());
}

std::string uio::toString(gp_Dir d) {
  std::string str;
  Standard_Real angTol = 0.000175;	//approx .01 degrees
  if(d.IsParallel( gp_Dir(0,0,1), angTol ) ){
    str = "Z axis";
  } else if (d.IsParallel( gp_Dir(0,1,0), angTol ) ) {
    str = "Y axis";
  } else if (d.IsParallel( gp_Dir(1,0,0), angTol ) ) {
    str = "X axis";
  } else {
    if (d.IsNormal( gp_Dir(0,0,1), angTol ) ) {
      str = "horizontal, ";
    }
    str += "(" + toString(d.X(),d.Y(),d.Z()) + ")";
  }
  return str;
}

void uio::checkShapeType(TopoDS_Shape Shape)
{
  std::string str;
  if (Shape.IsNull()) {
    str = "null";
  } else if (Shape.ShapeType()==TopAbs_EDGE) {
    str = "edge";
  } else if (Shape.ShapeType()==TopAbs_FACE) {
    str = "face";
  } else if (Shape.ShapeType()==TopAbs_WIRE) {
    str = "wire";
  } else if (Shape.ShapeType()==TopAbs_SHAPE) {
    str = "shape";
  } else if (Shape.ShapeType()==TopAbs_COMPOUND) {
    str = "compound";
  } else if (Shape.ShapeType()==TopAbs_COMPSOLID) {
    str = "compsolid";
  } else if (Shape.ShapeType()==TopAbs_SOLID) {
    str = "solid";
  } else if (Shape.ShapeType()==TopAbs_SHELL) {
    str = "shell";
  } else if (Shape.ShapeType()==TopAbs_VERTEX) {
    str = "vertex";
  } else {
    str = "borked";
  }
  infoMsg("The shape is: " + str + ".");
}

void uio::slotNeutralSelection()
{
  contextPtr->CloseAllContexts();
}

void uio::slotVertexSelection()
{
  contextPtr->CloseAllContexts();
  contextPtr->OpenLocalContext();
  contextPtr->ActivateStandardMode(TopAbs_VERTEX);
}

void uio::slotEdgeSelection()
{
  contextPtr->CloseAllContexts();
  contextPtr->OpenLocalContext();
  contextPtr->ActivateStandardMode(TopAbs_EDGE);
}

void uio::slotFaceSelection()
{
  contextPtr->CloseAllContexts();
  contextPtr->OpenLocalContext();
  contextPtr->ActivateStandardMode(TopAbs_FACE);
}

void uio::slotSolidSelection()
{
  contextPtr->CloseAllContexts();
  contextPtr->OpenLocalContext();
  contextPtr->ActivateStandardMode(TopAbs_SOLID);
}


//getVC, getOCC

void uio::hideGrid() {
  vcPtr->gridOff();
}

void uio::fitAll() {
  occPtr->fitAll();
}

void uio::axoView() {
  occPtr->viewAxo();
}
