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

//ui stuff for gcode2model

#include "gcode2Model.h"
#include <ostream>
#include <assert.h>

#include <QKeySequence>
#include <QtGui>
#include <QProcess>

using std::cout;
gcode2Model::gcode2Model()
{
	myMenu = new QMenu("gcode2Model");
	theWindow->menuBar()->insertMenu(theWindow->getHelpMenu(),myMenu);

	//these five lines set up a menu item.  Uncomment the second one to have a shortcut.
	myAction = new QAction ( "gcode2Model", this );
	myAction->setShortcut(QString("Ctrl+M"));
	myAction->setStatusTip ( "gcode2Model" );
	connect ( myAction, SIGNAL ( triggered() ), this, SLOT ( myMenuItem() ) );
	myMenu->addAction( myAction );

// do next: show segments one at a time
	nextAction = new QAction ( "Do next", this );
	nextAction->setShortcut(QString("Ctrl+."));
	nextAction->setStatusTip ( "Do next" );
	connect ( nextAction, SIGNAL ( triggered() ), this, SLOT ( myNextMenuItem() ) );
	myMenu->addAction( nextAction );

	hasProcessedNgc = false;
};

void gcode2Model::myMenuItem()
{
  	feedEdges.clear();
	slotNeutralSelection();
	
	QString file = QFileDialog::getOpenFileName ( theWindow, "Choose .ngc file", "./ngc-in", "*.ngc" );
	if ( ! file.endsWith(".ngc") ) {
		infoMsg("You must select a file ending with .ngc!");
		return;
	}
	interpret ( file );
	cout << "sweeping..." << endl;
	sweepEm();
//	feedEdges.clear(); //so when user loads a new file, the old data is not prepended.
	hasProcessedNgc = true;
}

// do next: show segments one at a time
void gcode2Model::myNextMenuItem() {
	static uint segment = 0;
  	if (!hasProcessedNgc) return;
	drawOne(segment);
	segment++;
}

void gcode2Model::sleepSecond() {
	//sleep 1s and process events
	//cout << "SLEEP..." << endl;
	QTime dieTime = QTime::currentTime().addSecs(1);
	while( QTime::currentTime() < dieTime )
		QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents, 100);
	return;
}

/*
bool gcode2Model::waitRead(QProcess &canon){
	if ( ! canon.waitForReadyRead(1000) ) {
		infoMsg("Interpreter timed out at startup.");
		canon.close();
		return false;
	} else {
		cout << "stderr: " << (const char*)canon.readAllStandardError() << endl;
		return true;  
	}
}
*/

