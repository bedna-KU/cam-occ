/****************************************************************************
**
** This file is part of the QtOpenCascade Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** Copyright (C) Peter Dolbey 2006. All rights reserved.
**
**
** Bugs (aka modifications) (C) 2009,2010 Mark Pictor.  These modifications
** can be re-licensed under GPL v3.
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPluginLoader>

#include "qoccviewercontext.h"
#include "qoccviewwidget.h"
#include "qoccinputoutput.h"
#include "lnk.hh"

class QAction;
class QLabel;
class QMenu;
class cam;

class QoccHarnessWindow : public QMainWindow
{
    Q_OBJECT

public:

	QoccHarnessWindow(QStringList ags);
	Handle_AIS_InteractiveContext& getContext() { return myVC->getContext(); };
	Handle_V3d_View getView() { return myOCC->getView(); };
	QAction* getHelpMenu() { return helpAction; };
    QString getArg(int n) {if(n<=args.count())return args[n];else return "n--";};
    QStringList* getArgs() {return &args;};

private slots:
    void newFile();
    void open();
    void save();
    void print();
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void about();
	void bottle();
    void aboutQt();
	void xyzPosition (V3d_Coordinate X,
					  V3d_Coordinate Y,
					  V3d_Coordinate Z);
	void addPoint (V3d_Coordinate X,
				   V3d_Coordinate Y,
				   V3d_Coordinate Z);
	void statusMessage (const QString aMessage);

private:
    QStringList args;
    void createActions();
    void createMenus();

    QMenu *fileMenu;
    QMenu *editMenu;
	QMenu *viewMenu;
	QMenu *viewActionsMenu;
	QMenu *viewDisplayMenu;
    QMenu *formatMenu;
    QMenu *helpMenu;
    QAction *helpAction;  //this is so we can insert menus before the help menu...
    QMenu *gridMenu;

    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *printAction;
    QAction *exitAction;
    QAction *undoAction;
    QAction *redoAction;
    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *aboutAction;

	//QAction *fitAction;
	QAction *fitAllAction;
	QAction *zoomAction;
	QAction *panAction;
	QAction *rotAction;

	QAction *gridXYAction;
	QAction *gridXZAction;
	QAction *gridYZAction;
	QAction *gridToggleAction;
	//QAction *gridOffAction;
	QAction *gridRectAction;
	QAction *gridCircAction;

    //QAction *viewDefaultAction;

	QAction *viewFrontAction;
	QAction *viewBackAction;
	QAction *viewTopAction;
	QAction *viewBottomAction;
	QAction *viewLeftAction;
	QAction *viewRightAction;
	QAction *viewAxoAction;
	QAction *viewResetAction;
	QAction *viewGridAction; /** Action for grid view */

	QAction *backgroundAction;

    QAction *aboutQtAction;

	//Sample Action
	QAction *bottleAction;

	// The OpenCASCADE Qt widget and context;
	QoccViewWidget*     myOCC;
	QoccViewerContext*  myVC;

	QString myLastFolder;

  public:
    	QoccViewWidget*     getOCC() { return myOCC; };
	QoccViewerContext*  getVC() { return myVC; };

};

#endif

