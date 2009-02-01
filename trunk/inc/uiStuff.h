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
#ifndef CAMFRAMEWORK_H
#define CAMFRAMEWORK_H

#include <QObject>
#include <QMenu>
#include <QToolBar>


#include <gp_Pnt.hxx>
#include <TopoDS_Shape.hxx>

#include "qoccharnesswindow.h"

class uiStuff : public QObject {
	Q_OBJECT

public:
	uiStuff();
	~uiStuff();
	void init(QoccHarnessWindow* window);

protected slots:
	void slotNeutralSelection();
	void slotVertexSelection();
	void slotEdgeSelection();
	void slotFaceSelection();
	void slotSolidSelection();

protected:
	QoccHarnessWindow* theWindow;
	std::vector<TopoDS_Shape> selectedShapes;
	void getSelection();
	void redraw();
	void debugMsg( QString s, QString f, int l );
	void infoMsg( QString title, QString message );
	void infoMsg( QString message );
	QString toString( float a,float b, float c );
	QString toString( gp_Pnt p );
	void addSelectionWidgets();

	int errors;
//	bool needsRedraw;

private:


};

#endif //CAMFRAMEWORK_H
