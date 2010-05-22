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
#ifndef UISTUFF_H
#define UISTUFF_H

//app includes
#include "qoccharnesswindow.h"

//system includes
#include <limits.h>

//qt includes
#include <QObject>
#include <QMenu>
#include <QToolBar>

//occ includes
#include <gp_Pnt.hxx>
#include <TopoDS_Shape.hxx>


class uiStuff : public QObject {
	Q_OBJECT

public:
	uiStuff(QoccHarnessWindow* window);
	uiStuff(){};  //for classes that inherit from this one
	~uiStuff(){};

signals:
	void theLongMsg(QString);  //for longMsgDlg

protected slots:
	void slotNeutralSelection();
	void slotVertexSelection();
	void slotEdgeSelection();
	void slotFaceSelection();
	void slotSolidSelection();

protected:
	static QoccHarnessWindow* theWindow;
	static std::vector<TopoDS_Shape> selectedShapes;
	static int errors;
	
	void getSelection();
	void redraw();
	void debugMsg( QString s, QString f, int l );
	void infoMsg( QString title, QString message );
	void infoMsg( QString message );
	QString toString( float a,float b, float c );
	QString toString( gp_Pnt p );
	QString toString( gp_Dir d );
	void checkShapeType(TopoDS_Shape Shape);
	void longMsg( QString message );
	void hideGrid();
	void fitAll();
	void axoView();

private:
	void addSelectionWidgets();


};

#endif //UISTUFF_H
