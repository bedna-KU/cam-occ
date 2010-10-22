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
#include "uio.hh"

//static
std::map<toolNumber,millTool*> canon::toolTable;
bool canon::toolsBuilt = false;
std::string canon::ttname = "";
double canon::toolLength = 0;



canon::canon() {
  toolsBuilt = false;
}

void canon::addTool(toolNumber n) {
  if(n>0) {
    toolTable.insert(std::pair<toolNumber,millTool*>(n, (millTool *) NULL));
  } else {
    cout << "Tool numbers must be greater than 0!" << endl;
  }
}

void canon::buildTools(std::string toolTableFile, double toolLen) {
  toolLength = toolLen;
  ttname = toolTableFile;
  toolsBuilt = true;
  readTTfile();
  checkTTskips();
  cout << "tool table populated. contains " << toolTable.size() << " tools." << endl;
}

void canon::checkTTskips() {
  std::map<toolNumber,millTool*>::iterator it;
  for(it=toolTable.begin() ; it != toolTable.end(); it++ ) {
    if (it->first == 0) continue;
    if (it->second == NULL) {
      double dia = (double)it->first/(double)4.0;
      it->second = createTool(dia,"ball");
      cout << "Missing tool " << it->first << ", replacing with ballnose tool diam=" << dia << endl;
    }
  }
}

millTool * canon::getTool(toolNumber n) {
  assert(toolsBuilt);
  if (n>0) {
    std::map<toolNumber,millTool*>::iterator it;
    it=toolTable.find(n);
    assert(it!=toolTable.end());
    assert(it->second != NULL);
    return it->second;
  } else {
    return NULL;
  }
}

bool canon::checkRange(std::string tline, std::string item,size_t c) {
  if (c != std::string::npos) {
    return true;
  } else {
    infoMsg("Out of range error searching for " + item + " on line " + tline + " of tool table " + ttname + ".");
    return false;
  }
}

///create a tool. for now, only supports cylindricalTool and ballnoseTool
millTool* canon::createTool(double diam,std::string comment) {
  size_t ball,end;
  assert(diam>0);
  ball = comment.find("ball");
  end = comment.find("end");
  if ((end != std::string::npos) && (ball == std::string::npos)) {
    //std end mill
    return new cylindricalTool(diam,toolLength);
  } else {
    //ball mill
    return new ballnoseTool(diam,toolLength);
  }
}

void canon::readTTfile() {
  if (uio::fileExists(ttname)) {
    std::ifstream inFile(ttname.c_str());
    std::string line;
    while(std::getline(inFile, line)) {
      //find the numbers after T and D, and the comment
      size_t t,d,c,sp;
      std::string data,comment;
      int tool;
      double diam;
      char * end; //use with strtol
      c = line.find(";");
      if (c != std::string::npos) {
        comment = line.substr(c+1); //everything after ;
        data = line.substr(0,c); //everything before ;
      } else {
        comment = "";
        data = line;
      }

      t = data.find("T");
      if (!checkRange(line,"T",t)) {
          continue;
        }
      sp = data.find(" ",t);
      if (!checkRange(line,"T_",sp)) {
          continue;
        }
      tool = strtol(data.substr(t+1,sp).c_str(),&end,10);
      if (end == 0) {
        if (!checkRange(line,"Tnnn")) {
          continue;
        }
      }

      d = data.find("D");
      if (!checkRange(line,"D",d)) {
          continue;
        }
      sp = data.find(" ",d);
      if (!checkRange(line,"D_",sp)) {
          continue;
        }
      diam = strtod(data.substr(d+1,sp).c_str(),&end);
      if (end == 0) {
        if (!checkRange(line,"Dnnn")) {
          continue;
        }
      }
      //now check if this tool number is in toolTable - if it is, create the tool obj
      std::map<toolNumber,millTool*>::iterator it;
      it = toolTable.find(tool);
      if (it != toolTable.end()) { //found it, so replace the null pointer that's there now
        if (it->second != NULL) {
          infoMsg("Overwriting duplicate tool " + uio::toString(tool) + " in " + ttname);
        }
        it->second = createTool(diam,comment);
      }
    }
  } else {
    infoMsg("Can't find tool table - ?!");
  }
}


