/***************************************************************************
 *   Copyright (C) 2010 by Mark Pictor					   *
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
//renamed from link because 'link' caused problems in qoccharnesswindow.cpp
#include "lnk.hh"
#include "../uio/uio.hh"
#include "../g2m/g2m.hh"
#include <qoccharnesswindow.h>


lnk::lnk(QoccHarnessWindow* w) {
  cout << "lnk ctor" << endl;

  uio* user;
  g2m* g2model;

  user = new uio(w); //MUST be initialized before all classes that use uio!
  g2model = new g2m();
  //cam myCam();
  //misc myMisc(); // this will be all the stuff that's now "cam"
  cout << "lnk ctor end" << endl;
}

lnk::~lnk() {
}
