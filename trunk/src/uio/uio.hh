//class uio, i.e. user i/o
//contains functions that were in uiStuff
#ifndef UIO_HH
#define UIO_HH

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

#include <limits.h>
#include <vector>
#include <string>
#include <sstream>

#include <TopoDS_Shape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <AIS_InteractiveContext.hxx>
#include <V3d_View.hxx>

#include <QObject>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QSettings>

#include "../qocc/qoccharnesswindow.h"


/*!
\class uio
\brief This class does user i/o - i.e. gui stuff
	This class replaces uiStuff. It contains similar functions, as well as
	pointers to commonly-referenced objects in qocc. There is no need to
	inherit from this class to be able to use it's functions, unlike uiStuff.
	However, it is necessary for the ctor to be called before using any of
	the functions in the class!

	uio creates and populates the Select menu.

	TODO: add logging functionality from dr dobbs example

\author Mark Pictor
*/
class QoccHarnessWindow;
class uio : public QObject {
  Q_OBJECT
  private:
    //these vars are static so that they can be accessed by static functions
    static QoccHarnessWindow* windowPtr;
    static Handle_V3d_View viewPtr;
    static Handle_AIS_InteractiveContext contextPtr;
    static QoccViewWidget* occPtr;
    static QoccViewerContext* vcPtr;
    static QMenuBar* mbPtr;
    static QAction* hmPtr;
    void initUI();
    static std::vector<TopoDS_Shape> selectedShapes;
    static int errors;
    static TopTools_ListOfShape latestSelection;
    static bool debugParam;
    static QSettings settings;
    static int dump;

  public:
    uio(QoccHarnessWindow* window);
    static QoccHarnessWindow* window() {return windowPtr;};
    static Handle_V3d_View view() {return viewPtr;};
    static Handle_AIS_InteractiveContext context() {return contextPtr;};
    static QoccViewWidget* occ() {return occPtr;};
    static QoccViewerContext* vc() {return vcPtr;};
    static QMenuBar* mb() {return mbPtr;};
    static QAction* hm() {return hmPtr;};
    static void redraw();
    //static void getSelection();
    static void debugMsg(std::string s, std::string f, int l);
    static void infoMsg( std::string title, std::string message );
    static void infoMsg( std::string message );
    //static inline void infoMsg( QString m ) {infoMsg(m.toStdString());};
    static std::string toString(double a,double b, double c);
    static std::string toString(gp_Pnt p);
    static std::string toString(gp_Dir d);
    static inline std::string toString(int x){std::ostringstream o; o<<x; return o.str();};
    //adapted from stringify at http://www.parashift.com/c++-faq-lite/misc-technical-issues.html#faq-39.1 :
    static inline std::string toString(double x){std::ostringstream o; o<<x; return o.str();};
    static void checkShapeType(TopoDS_Shape Shape);
    //static void hideGrid();
    static void fitAll();
    static void axoView();
    static void sleep(uint n = 1,bool usrEv = false);
    static bool fileExists(QString f);
    static inline bool fileExists(std::string f) {return fileExists(QString(f.c_str()));};
    static double mass(TopoDS_Shape s);
    static void grabSelection();
    static bool debuggingOn() {return debugParam;};
    static QSettings& conf() {return settings;};
    static void tokenize(std::string str, std::vector<std::string>& tokenV, const std::string& delimiters = "(), ");
    static int getDump() {return dump;};

  public slots:
    static void slotNeutralSelection();
    static void slotVertexSelection();
    static void slotEdgeSelection();
    static void slotFaceSelection();
    static void slotSolidSelection();
    static void slotMassOfSelection();
    static void slotSaveSelection();
    static void slotCountFaces();

};

#endif //UIO_HH
