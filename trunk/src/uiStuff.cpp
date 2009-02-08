/***************************************************************************
 *   Copyright (C) 2009 by Mark Pictor					   *
 *   mpictor@gmail.com							   *
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


#include "uiStuff.h"
#include "cam.h"
#include "ui_longMsgDlg.h"    //for our custom dialog box longMsgDlg.ui

#include <string>
#include <ostream>

#include <QMessageBox>
#include <QIcon>
#include <QAction>
#include <QMenuBar>


#include <V3d_View.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <Handle_AIS_InteractiveObject.hxx>
#include <AIS_Trihedron.hxx>


uiStuff::uiStuff() {
	errors = 0;
	theWindow = 0;
}

uiStuff::~uiStuff() {
}

void uiStuff::init(QoccHarnessWindow* window) {
	theWindow = window;
	slotNeutralSelection();
	addSelectionWidgets();
}

void uiStuff::redraw() {
	theWindow->getView()->Redraw();
}


void uiStuff::getSelection() {
	selectedShapes.clear();
	for ( theWindow->getContext()->InitSelected(); theWindow->getContext()->MoreSelected(); theWindow->getContext()->NextSelected() ) {
                Handle_AIS_InteractiveObject IO = theWindow->getContext()->Interactive();
                if ( !theWindow->getContext()->HasSelectedShape() )
                {
			infoMsg("no selection");
//                        theWindow->getContext()->ClearSelected();
//                        break;
                }
                if ( IO->IsKind(STANDARD_TYPE(AIS_Trihedron)) )
                {
			//we don't want to save the trihedron
			//clearSelected apparently clears EVERYTHING, rather than deselecting one thing...
			continue;
                }
                selectedShapes.push_back(theWindow->getContext()->SelectedShape());
	}
}



//normally called like this: debugMsg("error string",__FILE__,__LINE__);
void uiStuff::debugMsg(QString s, QString f, int l) {
	errors++;
	cerr << "Error  #" << errors << ": " << s.toStdString() << "\n\tin file " << f.toStdString() << " at line " << l << "." << endl;
}

void uiStuff::infoMsg( QString title, QString message ) {
	QMessageBox::information(theWindow,title,message);
}

void uiStuff::infoMsg( QString message ) {
	QMessageBox::information(theWindow,"Cam-occ2",message);
}

void uiStuff::longMsg( QString message ) {
//	QWidget *dialog = new QWidget;
//     	Ui::longMsgDlg dlg;
//	connect(this, SIGNAL(theLongMsg(QString)), dlg.textBrowser, SLOT(setText(QString)));
//	emit(theLongMsg(message));
	//dlg.textBrowser->setText(message);
  //   	dlg.setupUi(dialog);
//	dialog->show();

	QMessageBox msg(theWindow);
	msg.setIcon(QMessageBox::Information);
	msg.addButton("OK",QMessageBox::AcceptRole);
	msg.setText(message);
	msg.setTextFormat(Qt::RichText);
	msg.setWindowTitle("Cam-occ2");
	msg.adjustSize();
	msg.exec();
	//msg.show();

}

QString uiStuff::toString(float a,float b, float c) {
	return QString("%1, %2, %3").arg(a).arg(b).arg(c);
}

QString uiStuff::toString(gp_Pnt p) {
	return QString("%1, %2, %3").arg(p.X()).arg(p.Y()).arg(p.Z());
}

QString uiStuff::toString(gp_Dir d) {
	QString str;
	Standard_Real angTol = 0.000175;	//approx .01 degrees
	if(d.IsParallel( gp_Dir(0,0,1), angTol ) ){
		str = "Z axis";
	} else if (d.IsParallel( gp_Dir(0,1,0), angTol ) ) {
		str = "Y axis";
	} else if (d.IsParallel( gp_Dir(1,0,0), angTol ) ) {
		str = "X axis";
	} else if (d.IsNormal( gp_Dir(0,0,1), angTol ) ) {
		str = QString("horizontal, (%1, %2, %3)").arg(d.X()).arg(d.Y()).arg(d.Z());
	} else {
		str = QString("(%1, %2, %3)").arg(d.X()).arg(d.Y()).arg(d.Z());
	}
	return str;
}

void uiStuff::checkShapeType(TopoDS_Shape Shape)
{
	QString str;
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

void uiStuff::slotNeutralSelection()
{
  theWindow->getContext()->CloseAllContexts();
}

void uiStuff::slotVertexSelection()
{
  theWindow->getContext()->CloseAllContexts();
  theWindow->getContext()->OpenLocalContext();
  theWindow->getContext()->ActivateStandardMode(TopAbs_VERTEX);
}

void uiStuff::slotEdgeSelection()
{
  theWindow->getContext()->CloseAllContexts();
  theWindow->getContext()->OpenLocalContext();
  theWindow->getContext()->ActivateStandardMode(TopAbs_EDGE);
}

void uiStuff::slotFaceSelection()
{
  theWindow->getContext()->CloseAllContexts();
  theWindow->getContext()->OpenLocalContext();
  theWindow->getContext()->ActivateStandardMode(TopAbs_FACE);
}

void uiStuff::slotSolidSelection()
{
  theWindow->getContext()->CloseAllContexts();
  theWindow->getContext()->OpenLocalContext();
  theWindow->getContext()->ActivateStandardMode(TopAbs_SOLID);
}

void uiStuff::addSelectionWidgets() {
	QMenu *selectMenu;
	selectMenu = new QMenu("Select");
	theWindow->menuBar()->insertMenu(theWindow->getHelpMenu(),selectMenu);  //put this menu BEFORE the help menu
//	theWindow->menuBar()->addMenu(selectMenu);

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

