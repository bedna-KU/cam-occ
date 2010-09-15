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
#ifndef GTOM_THREADED_HH
#define GTOM_THREADED_HH

#include "g2m.hh"
#include <QMutex>


/**
\class g2m_threaded
\brief This class extends g2m to allow parallel processing.
It was split out of g2m and is <b>not guaranteed to function</b> - there's no point testing when it will definitely crash.
It will cause numerous segfaults with opencascade 6.3.0 and earlier.
*/
class g2m_threaded: public g2m {
  public:
    g2m_threaded();
  protected:
    ///is interpreter done? must be false until we're done adding to lineVector
    bool doThreads;
    std::list< pthread_t* > threadIdList;
    ///cpu count for multithreading. this is the size of the array
    long mthreadCpuCnt;
    void createThreads();
    void joinThreads();
    bool checkIfSafeForThreading();
    static void* makeSolidsThread(void * v);
    static void threadSafeSleep();
    static uint nextAvailInVec(bool onlyWatch = false);
    static uint getVecSize();
    static QMutex vecModMutex;   //used when a canonLine is pushed onto vector, and in getVecSize()
    static QMutex vecGrowMutex;  //used in & blocks makeSolidsThread only, blocks while waiting for vector to grow

    //functions that are virtual in g2m, overridden here to enable threading
    void finishAllSolids(nanotimer &timer);
    //void makeAllSolids() {};
    void lockMutex() {vecModMutex.lock();};
    void unlockMutex() {vecModMutex.unlock();};



};

#endif //GTOM_THREADED_HH
