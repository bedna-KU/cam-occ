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
#include <QMutexLocker>

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
//#include "dispShape.hh"

std::vector<canonLine*> g2m::lineVector;
bool g2m::interpDone = false;

g2m::g2m() {
  mthreadCpuCnt = -1;

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
  checkIfSafeForThreading();
  lineVector.clear();
//  dispVector.clear();

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

  createThreads();

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
  interpDone = true;

  //update status until threads are almost done
  bool threadsAlmostDone = false;
  do {
    uint current = nextAvailInVec(true);
    uint size = lineVector.size();
    if (current < (size-mthreadCpuCnt)) {
      if (lineVector.size()%20 == 0) {
        uio::fitAll();
      }
      std::string s = "Processing ";
      s+= (lineVector[current]->getN()==-1) ?
          uio::toString(lineVector[current]->getLineNum()) :
          std::string("N"+lineVector[current]->getN());
      s+= " : " + current;
      s+= " of " + size;
      statusBarUp(s,timer.getElapsedS()/double(current));
      uio::sleep();
    } else {
      threadsAlmostDone = true;
    }
  }  while ( !threadsAlmostDone );

  joinThreads();

  double e = timer.getElapsedS();
  std::cout << "Total time to process that file: " << timer.humanreadable(e) << std::endl;

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
  double e = 0;
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
  lineVector.push_back(cl);

  /* need to highlight the first *solid* rather than the first obj
  if (lineVector.size()==1) {
    lineVector[1].setFirst(); //different color, etc
    ///need to do something similar for the last vector...
}
*/

  double t = nt.getElapsedS();
  if (debug) cout << "Line " << cl->getLineNum() << "/N" << cl->getN() << " - time " << nt.humanreadable(t) << endl;
  e += t;

  //every 20 lines, or every time if debugging
  if ((debug) || (lineVector.size()%20 == 0) ) {
    std::string s = "Last processed:";
    s+= (cl->getN()==-1) ? uio::toString(cl->getLineNum()) : std::string("N"+cl->getN());
    statusBarUp(s, e/(double)lineVector.size());
    //    uio::fitAll();
  }
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

/** mutexes - use QMutexLocker ( QMutex * mutex ) and QMutex()
vecModMutex   //used when a canonLine is pushed onto vector, and in getVecSize()
vecGrowMutex  //used in & blocks makeSolidsThread only, blocks while waiting for vector to grow
*/
void g2m::createThreads() {
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  //figure out number of cores, we'll create 1 thread per
  mthreadCpuCnt = sysconf(_SC_NPROCESSORS_ONLN);
  if (mthreadCpuCnt < 1) {
    uio::infoMsg("Unknown number of processors. Only creating one thread!");
    mthreadCpuCnt = 1;
  }

//  threadIDarr = new pthread_t[mthreadCpuCnt];

  for (int j=0; j<mthreadCpuCnt; j++) {
    pthread_t *tId( new pthread_t );
    threadIdList.push_back(tId);
    int rc = pthread_create(tId, &attr, makeSolidsThread, NULL);
    if (rc) {
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }
}

///join threads when processing is done
void g2m::joinThreads() {
    std::list< pthread_t* >::iterator it;

  for ( it=threadIdList.begin() ; it != threadIdList.end(); it++ ) {
//  for (int j=0; j<mthreadCpuCnt; j++) {
    int rc = pthread_join(**it, NULL);
    if (rc) {
      printf("ERROR; return code from pthread_join() is %d\n", rc);
      exit(-1);
    }
  }
}

///called by pthread_create. A loop that looks for unprocessed objs in lineVector and processes them.
///creates and displays solid. Locks vecGrowMutex when not processing an obj.
void* g2m::makeSolidsThread(void *) {
  uint index;
  while (lineVector.size() == 0) {threadSafeSleep();}  //don't use 100%cpu waiting for 1st obj...
  while (1) {  //exit thread by using return, not pthread_exit(), to prevent memory leaks
    QMutexLocker growLocker(&vecGrowMutex);  //unlock on destroy
    index = nextAvailInVec();
    while ( getVecSize() < index ) { //wait for lineVector to grow, unless interpDone==true
      if ((interpDone) && (getVecSize() < index)) {
        return 0;  //destroy growLocker and free memory
      }
      threadSafeSleep();
    }
    growLocker.unlock();

    if (lineVector[index]->isMotion()) {
      //enum SOLID_MODE { SWEPT,BRUTEFORCE,ASSEMBLED }
      ((canonMotion*)lineVector[index])->setSolidMode(SWEPT);
      ((canonMotion*)lineVector[index])->computeSolid();
    }
    //DISPLAY_MODE { NO_DISP,THIN_MOTION,THIN,ONLY_MOTION,BEST}
    lineVector[index]->setDispMode(BEST);
    lineVector[index]->display();
  }
}

void g2m::threadSafeSleep() {
  timeval timeout;
  timeout.tv_sec = 0;  timeout.tv_usec = 10000; // 0.01 second delay
  select( 0, NULL, NULL, NULL, & timeout );
}

///return index of next unprocessed line from vector
///unless onlyWatch = false, vecGrowMutex must be locked and nextLineInVec is incremented
uint g2m::nextAvailInVec(bool onlyWatch) {
  static uint nextLineInVec = 0;
  uint t = nextLineInVec;
  if (!onlyWatch) {
    assert(!vecGrowMutex.tryLock());  //this mutex should already be locked
    nextLineInVec++;
  }  //else assert (inMainThread()==true)
  return t;
}

///using vecModMutex,return size of vector
uint g2m::getVecSize() {
  uint t;
  QMutexLocker modLocker(&vecModMutex);
  t = lineVector.size();
  return t;
}

///check occ env
void checkIfSafeForThreading() {
  bool envgood = true;
  std::string msg = "Warning, environment variables incorrectly set. Expect problems!";
  char * opt;
  opt = getenv("MMGT_OPT");
  if (opt == NULL) {
    msg += "MMGT_OPT not defined!\n";
    envgood = false;
  } else {
    if (strcmp(opt, "0") == 0) {
      //occ's mmgt disabled. this is good.
    } else {
      msg += "MMGT_OPT not 0!\n";
      envgood = false;
    }
  }
  Standard::SetReentrant (Standard_True);  //make occ use thread-safe handles
  if (!envgood)
    uio::infoMsg(msg);
}

/* *intended to be called by a thread
\param start the first item to get from the vector
\param skip how many to skip over (4 for 4 threads)
* /
void g2m::makeSolids(uint start, uint incr) {
  uint curr = start;
  nanotimer nt;
  if (start == (uint)0){
    nt.start();
  }
  while ((!interpDone) || (lineVector.size() > curr)) {
    if (lineVector.size() > curr) {
      if (lineVector[curr]->isMotion()) {
        //enum SOLID_MODE { SWEPT,BRUTEFORCE,ASSEMBLED }
        ((canonMotion*)lineVector[curr])->setSolidMode(SWEPT);
        ((canonMotion*)lineVector[curr])->computeSolid();
      }
      //DISPLAY_MODE { NO_DISP,THIN_MOTION,THIN,ONLY_MOTION,BEST}
      lineVector[curr]->setDispMode(BEST);
      lineVector[curr]->display();

      if (start == (uint)0){
        double e = nt.getElapsedS()/(double)(curr/incr);
        statusBarUp("Creating Solids: ~" + uio::toString(curr) + " done",e);
      }
      if (debug) {
        infoMsg("Current: "+uio::toString(curr));
      }
      curr += incr;
    } else {
      infoMsg("sleep in makesolids");
      uio::sleep(1);
    }
  }
}
*/
