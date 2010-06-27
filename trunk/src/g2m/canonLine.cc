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
#include <cmath>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include <Precision.hxx>

#include "canonLine.hh"
#include "machineStatus.hh"
#include "canonMotionless.hh"
#include "canonMotion.hh"
#include "linearMotion.hh"
#include "helicalMotion.hh"


//canon.cc - implementation of canonLine

canonLine::canonLine(std::string canonL, machineStatus prevStatus): status(prevStatus) {
  myLine = canonL;
  tokenize(); //splits myLine using delimiters
}

///returns the number after N on the line, -1 if none
int canonLine::getN() {
  if ( (canonTokens[1].c_str()[1] == '.') && (canonTokens[1].c_str()[2] == '.') )
    return -1;
  else
    return tok2i(1,1);
}

///returns the canon line number
int canonLine::getLineNum() {
  return tok2i(0);
}

///returns the machine's status after execution of this canon line
const machineStatus* canonLine::getStatus() {
  return &status;
}

/*
void canonLine::setToolVecPtr(std::vector<tool> *t) {
  *toolVec = *t;
}
*/


///converts canonTokens[n] to double
inline double canonLine::tok2d(uint n) {
  char * end;
  double d = strtod( canonTokens[n].c_str(), &end );
  assert ( *end == 0 );
  return d;
}

///converts canonTokens[n] to int
inline int canonLine::tok2i(uint n,uint offset) {
  char * end;
  int i = strtol( &canonTokens[n].c_str()[offset], &end, 10 );
  assert ( *end == 0 );
  return i;
}

const std::string canonLine::getCanonicalCommand() {
  return canonTokens[2];
}

//from http://oopweb.com/CPP/Documents/CPPHOWTO/Volume/C++Programming-HOWTO-7.html
//0 is canon line
//1 is gcode Nnnnnn line
//2 is canonical command
///splits 'str' at any of 'delimiters' and puts the pieces in 'tokenV'
///delimiters defaults to both parenthesis, comma, space.
void canonLine::tokenize(std::string str,
			 std::vector<std::string>& tokenV,
			 const std::string& delimiters) {
  // Skip delimiters at beginning.
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

  while (std::string::npos != pos || std::string::npos != lastPos)
  {
    // Found a token, add it to the vector.
    tokenV.push_back(str.substr(lastPos, pos - lastPos));
    // Skip delimiters.  Note the "not_of"
    lastPos = str.find_first_not_of(delimiters, pos);
    // Find next "non-delimiter"
    pos = str.find_first_of(delimiters, lastPos);
  }
}

///tokenize myLine with the default delimiters `(), `
inline void canonLine::tokenize() {
  tokenize(myLine,canonTokens);
}

///return true if the canonical command for this line matches 'm'
inline bool canonLine::clMatch(std::string m) {
  return (m.compare(canonTokens[2]) == 0); //compare returns zero for a match
}

/**
\fn canonLine * canonLine::canonLineFactory (std::string l, machineStatus s)
\brief canonLineFactory creates objects that inherit from canonLine
canonLineFactory determines which type of object to create, and returns a pointer to that object
*/
canonLine * canonLine::canonLineFactory (std::string l, machineStatus s) {
  //check if canonical command is motion or something else
  //motion commands: STRAIGHT_TRAVERSE STRAIGHT_FEED ARC_FEED
  size_t lin,af,cmnt,msg;
  cmnt=l.find("COMMENT");
  msg=l.find("MESSAGE");
  lin=l.find("STRAIGHT_");
  af=l.find("ARC_FEED");
  /*
  ** check for comments first because it is not impossible
  ** for one to contain the text "LINEAR_" or "ARC_FEED"
  */
  if ( (cmnt!=std::string::npos) || (msg!=std::string::npos) ) {
    return new canonMotionless(l,s);
  } else if (lin!=std::string::npos) { //linear traverse or linear feed
    return new linearMotion(l,s);
  } else if (af!=std::string::npos) { //arc feed
    return new helicalMotion(l,s); //arc or helix
  } else { //canonical command is not a motion command
    return new canonMotionless(l,s);
  }
}
