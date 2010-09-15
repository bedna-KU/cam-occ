/***************************************************************************
*   Copyright (C) 2010 by Mark Pictor                                      *
*   mpictor@gmail.com                                                      *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
*                                                                          *
*   This program is distributed in the hope that it will be useful,        *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
*   GNU General Public License for more details.                           *
*                                                                          *
*   You should have received a copy of the GNU General Public License      *
*   along with this program; if not, write to the                          *
*   Free Software Foundation, Inc.,                                        *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.              *
***************************************************************************/

#include <iostream>
#include <cmath>
#include <fstream>
#include <unistd.h>  // because we use sysconf() to find nr cpus for threading

#include <QProcess>
#include <QStringList>
#include <QString>
#include <QTime>
#include <QCoreApplication>
#include <QEventLoop>
#include <QFileDialog>
#include <QStatusBar>

#include "g2m.hh"
#include "uio.hh"
#include "canonLine.hh"
#include "canonMotion.hh"
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

//init static members
std::vector<canonLine*> g2m::lineVector;
bool g2m::interpDone = false;

g2m::g2m() {

  QMenu* myMenu = new QMenu("gcode");

  QAction* myAction = new QAction ( "Create 3D Model...", this );
  myAction->setShortcut(QString("Ctrl+M"));
  myAction->setStatusTip ( "Load a .ngc file and create a 3d model" );
  connect(myAction,SIGNAL(triggered()),this,SLOT(slotModelFromFile()));
  myMenu->addAction( myAction );
  uio::mb()->insertMenu(uio::hm(),myMenu);

  debug = uio::debuggingOn();

  fromCmdLine = false;
  if (uio::window()->getArgs()->count() > 1) {
    if (uio::window()->getArg(0).compare("g2m") == 0) {
      fromCmdLine = true;
      uio::sleep(1); //this gives the app time to draw the main window. otherwise we don't see jack unless/until it succeeds
      slotModelFromFile();
    }
  }
}

void g2m::slotModelFromFile() {
  lineVector.clear();
  uio::hideGrid();
  interpDone = false;

  if (fromCmdLine) {
    file = uio::window()->getArg(1);
    if (!uio::fileExists(file))  {
      fromCmdLine = false;  //the file doesn't exist, we'll ask for a file
    }
  }
  if (!fromCmdLine) {
    file = QFileDialog::getOpenFileName ( uio::window(), "Choose input file", "./ngc-in", "*.ngc *.canon" );
  }
  fromCmdLine = false;  //so that if a second file is processed, this time within gui, the program won't use the one from the command line instead

  nanotimer timer;
  timer.start();

  createThreads();  //does nothing in g2m. overridden in g2m_threaded.

  if ( file.endsWith(".ngc") ) {
    interpret();
  } else if (file.endsWith(".canon")) { //just process each line
    std::ifstream inFile(file.toAscii());
    std::string sLine;
    while(std::getline(inFile, sLine)) {
      if (sLine.length() > 1) {  //helps to prevent segfault in canonLine::cmdMatch()
        processCanonLine(sLine);
      }
    }
  } else {
    uio::infoMsg("You must select a file ending with .ngc or .canon!");
    return;
  }

  interpDone = true;  //for g2m_threaded. tells threads they can quit when they reach the end of the vector.

  //in g2m, this creates the solids
  //overridden in g2m_threaded - waits for the threads to finish
  finishAllSolids(timer);

  double e = timer.getElapsedS();
  std::cout << "Total time to process that file: " << timer.humanreadable(e) << std::endl;

  uio::fitAll();
}

void g2m::finishAllSolids(nanotimer &timer) {
  uio::window()->statusBar()->clearMessage();

  for (uint i=0;i<lineVector.size();i++) {
    //lineVector[i]->display();
    makeSolid(i);
    if (i%20 == 0) { //every 20
      uio::fitAll();
      std::string s = "Processing ";
      s+= lineVector[i]->getLnum();
      s+= " : " + i;
      s+= " of " + lineVector.size();
      statusBarUp(s,timer.getElapsedS()/double(i));
    }
  }

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
  uio::window()->statusBar()->showMessage("Starting interpreter...");
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
    infoMsg(l);
  }
  nanotimer nt;
  nt.start();
  canonLine * cl;
  if (lineVector.size()==0) {
    cl = canonLine::canonLineFactory (l,machineStatus(gp_Ax1(gp_Pnt(0,0,0),gp_Dir(0,0,1))));
  } else {
    cl = canonLine::canonLineFactory (l,*(lineVector.back())->getStatus());
  }

  lockMutex();
  lineVector.push_back(cl);
  unlockMutex();

  /* need to highlight the first *solid* rather than the first obj
  if (lineVector.size()==1) {
    lineVector[1].setFirst(); //different color, etc
    ///need to do something similar for the last vector...
}
*/

  double t = nt.getElapsedS();
  if (debug) cout << "Line " << cl->getLineNum() << "/N" << cl->getN() << " - time " << nt.humanreadable(t) << endl;

  return cl->checkErrors();
}

void g2m::statusBarUp(std::string s, double avgtime) {
  //  std::string s = "Last processed:";
  s+= "   |   Avg time: " + nanotimer::humanreadable(avgtime);
  uio::window()->statusBar()->showMessage(s.c_str());
}

///Sleep 1s and process events
void g2m::sleepSecond() {
  uio::window()->statusBar()->showMessage("Waiting for interpreter...");
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

void g2m::makeSolid(uint index) {
  if (lineVector[index]->isMotion()) {
    //enum SOLID_MODE { SWEPT,BRUTEFORCE,ASSEMBLED }
    ((canonMotion*)lineVector[index])->setSolidMode(SWEPT);
    ((canonMotion*)lineVector[index])->computeSolid();
  }
  //DISPLAY_MODE { NO_DISP,THIN_MOTION,THIN,ONLY_MOTION,BEST}
  lineVector[index]->setSolidDone();
  lineVector[index]->setDispMode(BEST);
  //lineVector[index]->display();
}
