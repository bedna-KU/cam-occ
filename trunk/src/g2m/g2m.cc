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

#include <iostream>
#include <cmath>
#include <fstream>

#include <QProcess>
#include <QStringList>
#include <QString>
#include <QTime>
#include <QCoreApplication>
#include <QEventLoop>
#include <QFileDialog>

#include "g2m.hh"
#include "uio.hh"
#include "canonLine.hh"
#include "nanotimer.hh"

#include <Handle_Geom_TrimmedCurve.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <TopoDS.hxx>
#include <gp_Pln.hxx>
#include <TopoDS_Edge.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include "dispShape.hh"

g2m::g2m() {
  if (uio::window()->getArgs()->contains("debug")){
    debug = true;
  } else debug = false;
  QMenu* myMenu = new QMenu("gcode");

  QAction* myAction = new QAction ( "Create 3D Model...", this );
  myAction->setShortcut(QString("Ctrl+M"));
  myAction->setStatusTip ( "Load a .ngc file and create a 3d model" );
  connect(myAction,SIGNAL(triggered()),this,SLOT(slotModelFromFile()));
  myMenu->addAction( myAction );
  uio::mb()->insertMenu(uio::hm(),myMenu);

  fromCmdLine = false;
  if (uio::window()->getArgs()->count() > 1) {
    if (uio::window()->getArg(0).compare("g2m") == 0) {
      fromCmdLine = true;
      uio::sleep(1);
      slotModelFromFile();
    }
  }
}

void g2m::slotModelFromFile() {
  lineVector.clear();
  dispVector.clear();

  if (fromCmdLine) {
    file = uio::window()->getArg(1);
    if (!uio::fileExists(file))  {
      fromCmdLine = false;  //if the file doesn't exist, we'll ask for a file
    }
  }
  uio::hideGrid();

  if (!fromCmdLine) {
    file = QFileDialog::getOpenFileName ( uio::window(), "Choose input file", "./ngc-in", "*.ngc *.canon" );
  }

  fromCmdLine = false;

  nanotimer timer;
  timer.start();
  if ( file.endsWith(".ngc") ) {
    interpret();
  } else if (file.endsWith(".canon")) { //just process each line
    std::ifstream inFile(file.toAscii());
    std::string sLine;
    while(std::getline(inFile, sLine)) {
      if (sLine.length() > 1) {
        processCanonLine(sLine);
      } //prevent segfault in canonLine::clMatch()
    }
  } else {
    uio::infoMsg("You must select a file ending with .ngc or .canon!");
    return;
  }
  double e = timer.getElapsed();
  std::cout << "Total time to process that file: " << e/1000000000.0 << std::endl;



  //uio::occ()->viewRight();
  //uio::occ()->viewTop();
  //uio::axoView();
  //uio::window()->showNormal(); //for debugging, have a small window



 uio::fitAll();
}

void g2m::interpret() {
  success = false;
  //FIXME: don't hardcode these paths
  QString ipath = "/opt/src/emc2/trunk/";
  QString interp = ipath + "bin/rs274";
  QString iparm = ipath + "configs/sim/sim_mm.var\n";
  QString itool = ipath + "configs/sim/sim_mm.tbl\n";
  QProcess toCanon;
  bool foundEOF;
  toCanon.start(interp,QStringList(file));
  /**************************************************
  Apparently, QProcess::setReadChannel screws with waitForReadyRead() and canReadLine()
  So, we just fly blind and assume that there are no errors when we navigate
  the interp's "menu", and that it requires no delays.
  **************************************************/

  //now give the interpreter the data it needs
  toCanon.write("2\n");	//set parameter file
  toCanon.write(iparm.toAscii());
  toCanon.write("3\n");	//set tool table file
  toCanon.write(itool.toAscii());
  //can also use 4 and 5

  toCanon.write("1\n"); //start interpreting
  //cout << "stderr: " << (const char*)toCanon.readAllStandardError() << endl;

  if (!toCanon.waitForReadyRead(1000) ) {
    if ( toCanon.state() == QProcess::NotRunning ){
      infoMsg("Interpreter died.  Bad tool table " + itool.toStdString() + " ?");
    } else  infoMsg("Interpreter timed out for an unknown reason.");
    cout << "stderr: " << (const char*)toCanon.readAllStandardError() << endl;
    cout << "stdout: " << (const char*)toCanon.readAllStandardOutput() << endl;
    toCanon.close();
    return;
  }

  //if readLine is used at the wrong time, it is possible to pick up a line fragment! will canReadLine() fix that?
  qint64 lineLength;
  char line[260];
  uint fails = 0;
  do {
    if (toCanon.canReadLine()) {
      lineLength = toCanon.readLine(line, sizeof(line));
      if (lineLength != -1 ) {
	foundEOF = processCanonLine(line);
      } else {	//shouldn't get here!
	fails++;
	sleepSecond();
    }
  } else {
    fails++;
    sleepSecond();
  }
} while ( (fails < 100) &&
( (toCanon.canReadLine()) || ( toCanon.state() != QProcess::NotRunning ) )  );
//((lineLength > 0) || 	//loop until interp quits and all lines are read.
//toCanon.canReadLine() ||
success = foundEOF;
return;
}

bool g2m::processCanonLine (std::string l) {
  if (debug){
    uio::infoMsg(l);
  }
  nanotimer nt;
  nt.start();
  static bool first = true;  //first line? if so, init status...
  canonLine * cl;
  if (first) {
    cl = canonLine::canonLineFactory (l,machineStatus(gp_Ax1(gp_Pnt(0,0,0),gp_Dir(0,0,1))));
    first = false;
  } else {
    cl = canonLine::canonLineFactory (l,*(lineVector.back())->getStatus());
  }


  lineVector.push_back(cl);

  if (!first) {
    dispShape *ds = new dispShape(cl->getShape(),cl->getN());
    dispVector.push_back(ds);
    ds->display();
  }

  double e = nt.getElapsed();
  cout << "Time for line " << cl->getN() << ": " << e/1000000000.0 << endl;
  if ((debug) || (lineVector.size()%20 == 0) ) {
    uio::fitAll(); //fitAll every 100 lines, or every time if debugging
  }

  return cl->checkErrors();
}

void g2m::sleepSecond() {
  //sleep 1s and process events
  //cout << "SLEEP..." << endl;
  QTime dieTime = QTime::currentTime().addSecs(1);
  while( QTime::currentTime() < dieTime )
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents, 100);
  return;
}

void g2m::infoMsg(std::string s) {
  cout << s << endl;
}


/*
  void g2m::slotSaveAll() {
  std::string saveFile = (QFileDialog::getSaveFileName ( uio::window(), "Choose base name for shapes", "./output", "*" )).toStdString();
  for (int i = 0; i < lineVector.size(); i++) {
    std::string t = saveFile;
    t += i;
    BRepTools::Write( lineVector[i]->getShape(), t.c_str());
  }
}
*/