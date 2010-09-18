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


#include "canon.hh"
#include "tool.hh"
//canon.cc

//static
std::map<toolNumber,millTool*> canon::toolTable;


canon::canon() {
  toolsBuilt = false;
}

void canon::addTool(toolNumber n) {
  assert(n>0);
  toolTable.insert(n, (millTool *) NULL);
}

void canon::buildTools(QString toolTableFile) {
  toolsBuilt = true;
  toolTable.iterator it;
  parseTable(toolTableFile);
  for (it=toolTable.begin(); it!=toolTable.end(); it++) {
    it->second = toolFromTable(it->first);  //replace NULL* from addTool() with pointer to actual object
  }
  cout << "tool table populated. contains " << toolTable.size() << " tools." << endl;
}

millTool & canon::getTool(toolNumber n) {
  assert(toolsBuilt);
  assert(n>0);
  toolTable.iterator it;
  it=toolTable.find(n);
  assert(it!=map::end);
  assert(it->second != NULL);
  return it->second;
}

void canon::parseTable(QString toolTableFile) {

}

millTool* canon::toolFromTable(toolNumber n) {

  return new ...
}
