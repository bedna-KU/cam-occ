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

#include "g2m_threaded.hh"
#include "g2m.hh"
#include <QMutex>
#include <unistd.h>  // because we use sysconf() to find nr cpus for threading


QMutex g2m::vecModMutex;
QMutex g2m::vecGrowMutex;

g2m_threaded::g2m_threaded() {
  mthreadCpuCnt = -1;
  doThreads = true;

}

void g2m_threaded::threadSafeSleep() {
  timeval timeout;
  timeout.tv_sec = 0;  timeout.tv_usec = 10000; // 0.01 second delay
  select( 0, NULL, NULL, NULL, & timeout );
}

///return index of next unprocessed line from vector
///unless onlyWatch = false, vecGrowMutex must be locked and nextLineInVec is incremented
uint g2m_threaded::nextAvailInVec(bool onlyWatch) {
  static uint nextLineInVec = 0;
  uint t = nextLineInVec;
  if (!onlyWatch) {
    assert(!vecGrowMutex.tryLock());  //this mutex should already be locked
    nextLineInVec++;
  }  //else assert (inMainThread()==true)
  return t;
}

///using vecModMutex,return size of vector
uint g2m_threaded::getVecSize() {
  uint t;
  QMutexLocker modLocker(&vecModMutex);
  t = lineVector.size();
  return t;
}

///check occ env
bool g2m_threaded::checkIfSafeForThreading() {
  char * opt;
  opt = getenv("USETHREADS");
  if (opt != 0) {
    if (strcmp(opt, "1") == 0) {
      Standard::SetReentrant (Standard_True);  //make occ use thread-safe handles and mmgt

      opt = getenv("MMGT_OPT");
      if ((opt == NULL) || (strcmp(opt, "0") != 0))
        uio::infoMsg("Warning, threading enabled but MMGT_OPT is not 0.\nThis may reduce performance.");

      return true;
    } else {
      return false;
    }
  }
}

///called by pthread_create. A loop that looks for unprocessed objs in lineVector and processes them.
///creates and displays solid. Locks vecGrowMutex when not processing an obj.
void* g2m_threaded::makeSolidsThread(void *) {
  uint index;
  while (lineVector.size() == 0) {threadSafeSleep();}  //don't use 100%cpu waiting for 1st obj...
  while (1) {  //exit thread by using return, not pthread_exit(), to prevent memory leaks
    QMutexLocker growLocker(&vecGrowMutex);  //unlock on destroy
    index = nextAvailInVec();
    while ( getVecSize()-1 < index ) { //wait for lineVector to grow, unless interpDone==true
      if ((interpDone) && (getVecSize()-1 < index)) {
        return 0;  //destroy growLocker and free memory
      }
      threadSafeSleep();
    }
    growLocker.unlock();
    if (uio::debuggingOn()) {
      cout << "index" << index << "size" << lineVector.size() << endl;
    }
    makeSolid(index);
  }
}

/** mutexes - use QMutexLocker ( QMutex * mutex ) and QMutex()
vecModMutex   //used when a canonLine is pushed onto vector, and in getVecSize()
vecGrowMutex  //used in & blocks makeSolidsThread only, blocks while waiting for vector to grow
*/

/** \fn createThreads
*** Create 1 thread per cpu. Each thread calls makeSolidsThread()
**/
void g2m_threaded::createThreads() {
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
    cout << "thread " << tId << " created" << endl;
  }
}

///join threads when processing is done
void g2m_threaded::joinThreads() {
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

///draw shapes and update statusbar/ui until threads are almost done, then join and display the rest of the shapes
void g2m_threaded::finishAllSolids(nanotimer &timer) {
  bool threadsAlmostDone = false;
  int lastDrawn = -1;
  uio::window()->statusBar()->clearMessage();
  do {
    uint current = nextAvailInVec(true);
    uint size = lineVector.size();
    if (current < (size-mthreadCpuCnt)) {

      //loop over objs that are definitely done
      uint i;
      for (i=lastDrawn+1;i<lineVector.size();i++) {
        if (lineVector[i]->isSolidDone()) {
          lineVector[i]->display();
        } else {
          lastDrawn = i-1;
          break;
        }
      }

      if (lastDrawn%20 == 0) {
        uio::fitAll();
      }

      std::string s = "Processing ";
      s+= lineVector[current]->getLnum();
      s+= " : " + current;
      s+= " of " + size;
      statusBarUp(s,timer.getElapsedS()/double(current));
      uio::fitAll();
      uio::sleep();
    } else {
      threadsAlmostDone = true;
    }
  }  while ( !threadsAlmostDone );

  joinThreads();

  for (uint i=lastDrawn+1;i<lineVector.size();i++) {
    lineVector[i]->display();
  }

}
