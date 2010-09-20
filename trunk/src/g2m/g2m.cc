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
#include "dispShape.hh"

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
//bool g2m::interpDone = false;

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
  //interpDone = false;

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

  if ( file.endsWith(".ngc") ) {
    interpret();
  } else if (file.endsWith(".canon")) { //just process each line
    if (!chooseToolTable()) {
      uio::infoMsg("Can't find tool table. Aborting.");
      return;
    }
    std::ifstream inFile(file.toAscii());
    std::string sLine;
    while(std::getline(inFile, sLine)) {
      if (sLine.length() > 1) {  //helps to prevent segfault in canonLine::cmdMatch()
        processCanonLine(sLine);
      }
    }
  } else {
    uio::infoMsg("File name must end with .ngc or .canon!");
    return;
  }

  //interpDone = true;  //for g2m_threaded. tells threads they can quit when they reach the end of the vector.

  //must be called before canonLine solids are created, because it calculates the minimum tool length
  createBlankWorkpiece();
  canon::buildTools(tooltable.toStdString(),minToolLength);

  createThreads();  //does nothing in g2m. overridden in g2m_threaded.

  //in g2m, this creates the solids
  //overridden in g2m_threaded - waits for the threads to finish
  finishAllSolids(timer);

  //now display the workpiece
  dispShape wp(workpiece);
  wp.display();

  double e = timer.getElapsedS();
  std::cout << "Total time to process that file: " << timer.humanreadable(e) << std::endl;

  uio::fitAll();
}

///call makeSolid on each obj in lineVector
///this is overridden by g2m_threaded
void g2m::finishAllSolids(nanotimer &timer) {
  uio::window()->statusBar()->clearMessage();

  for (uint i=0;i<lineVector.size();i++) {
    makeSolid(i);
    lineVector[i]->display();
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

///ask for a tool table, even if one is configured - user may wish to change it
bool g2m::chooseToolTable() {
  QString loc;
  bool ttconf = uio::conf().contains("rs274/tool-table");
  if (ttconf) {
    //passing the file name as the path means that it is preselected
    loc = uio::conf().value("rs274/tool-table").toString();
  } else {
    loc =  "/usr/share/doc/emc2/examples/sample-configs/sim";
  }
  tooltable = QFileDialog::getOpenFileName ( uio::window(), "Locate tool table", loc, "*.tbl" );

  if (!QFileInfo(tooltable).exists()){
    return false;
  }
  uio::conf().setValue("rs274/tool-table",tooltable);
  return true;
}

bool g2m::startInterp(QProcess &tc) {
  //bool success = true;
  QString interp;

  //if the user has specified a location for the interpreter, use it. if not, guess.
  //if that fails, ask. save the result.
//  QSettings settings("camocc.googlecode.com","cam-occ");
  interp = uio::conf().value("rs274/binary","/usr/bin/rs274").toString();
  if (!QFileInfo(interp).isExecutable()) {
    uio::infoMsg("Tried to use " + interp.toStdString() + " as the interpreter, but it doesn't exist or isn't executable.");
    interp = QFileDialog::getOpenFileName ( uio::window(), "Locate rs274 interpreter", "~", "rs274" );
    if (!QFileInfo(interp).isExecutable()) {
      return false;
    }
    uio::conf().setValue("rs274/binary",interp);
  }

  if (!chooseToolTable())
    return false;

  tc.start(interp,QStringList(file));

  /**************************************************
  Apparently, QProcess::setReadChannel screws with waitForReadyRead() and canReadLine()
  So we just fly blind and assume that
  - there are no errors when we navigate the interp's "menu", and
  - it requires no delays.
  **************************************************/

  tc.write("1\n"); //start interpreting
  tc.write(tooltable.toAscii());
  tc.write("\n");

  return true;
}

void g2m::interpret() {
  success = false;
  QProcess toCanon;
  bool foundEOF;

  if (!startInterp(toCanon))
    return;

  //cout << "stderr: " << (const char*)toCanon.readAllStandardError() << endl;
  uio::window()->statusBar()->showMessage("Starting interpreter...");
  if (!toCanon.waitForReadyRead(1000) ) {
    if ( toCanon.state() == QProcess::NotRunning ){
      infoMsg("Interpreter died.  Bad tool table?");
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

  //lockMutex();  //no longer necessary since subtracting requires things be done in a different order
  lineVector.push_back(cl);
  //unlockMutex();

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

///call this before creating solids, and use bbox size to figure out length of tools
void g2m::createBlankWorkpiece() {
  pntPair f,t;
  std::string fs,ts,ws;
  gp_Pnt a,b;

  f = machineStatus::getFeedBounds();
  t = machineStatus::getTraverseBounds();
  a = f.a; a.SetZ(a.Z()-0.5);
  b = f.b; b.SetZ(b.Z()+0.5);
  minToolLength = b.Z() - f.a.Z();
  workpiece = BRepPrimAPI_MakeBox(a,b).Solid();

  fs = "All motion at feedrate fits within a bounding box with corners ";
  fs += uio::toString(f.a) + " and " + uio::toString(f.b) + ".\n";
  ts = "All traverse motion fits within a bounding box with corners ";
  ts += uio::toString(t.a) + " and " + uio::toString(t.b) + ".\n";
  ws = "Creating workpiece between " + uio::toString(a) + " and " + uio::toString(b) + ".\n";
  uio::infoMsg("Workpiece size", fs + ts + ws);
}

void g2m::makeSolid(uint index) {
  MOTION_TYPE mt = lineVector[index]->getMotionType();
  //if (lineVector[index]->isMotion()) {
  if (mt == NOT_DEFINED) {
    infoMsg("motion type not defined at " + lineVector[index]->getLnum() );
    return;
  }
  if ( mt != MOTIONLESS ) {
    //enum SOLID_MODE { SWEPT,BRUTEFORCE,ASSEMBLED }
    ((canonMotion*)lineVector[index])->setSolidMode(SWEPT);
    ((canonMotion*)lineVector[index])->computeSolid();
    #ifdef MULTITHREADED
    #error subtraction cannot be performed in parallel
    #endif //MULTITHREADED
    workpiece = ((canonMotion*)lineVector[index])->subtract(workpiece);
  }
  //DISPLAY_MODE { NO_DISP,THIN_MOTION,THIN,ONLY_MOTION,BEST}
  lineVector[index]->setSolidDone();
  lineVector[index]->setDispMode(THIN_MOTION);
  //lineVector[index]->display();
}
