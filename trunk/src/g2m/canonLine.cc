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
#include "uio.hh"

//canon.cc - implementation of canonLine

canonLine::canonLine(std::string canonL, machineStatus prevStatus): myLine(canonL), status(prevStatus) /*, myUnSolid()*/ {
  myUnSolid.Nullify();
  tokenize(); //splits myLine using delimiters
  solidErrors = false;
  unsolidErrors = false;
  solidIsDone = false;
  aisShape = 0;
}

/*canonLine::~canonLine() {
  if (!aisShape == 0)
    delete aisShape;
}*/

///returns the number after N on the line, -1 if none
int canonLine::getN() {
  if ( (cantok(1).compare("N.....") == 0))//c_str()[1] == '.') && (cantok(1).c_str()[2] == '.') )
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


/** converts canonTokens[n] to double
\param n this is the token to convert
\returns token n, converted to double
*/
double canonLine::tok2d(uint n) {
  if (canonTokens.size() < n+1 ) return NAN;
  char * end;
  double d = strtod( canonTokens[n].c_str(), &end );
  assert ( *end == 0 );
  return d;
}

/** converts canonTokens[n] to int
\param n this is the token to convert
\param offset skip this many chars at the beginning of the token
\returns token n, converted to integer
*/
inline int canonLine::tok2i(uint n,uint offset) {
  if (canonTokens.size() < n+1 ) return INT_MIN;
  char * end;
  int i = strtol( &canonTokens[n].c_str()[offset], &end, 10 );
  //assert ( *end != 0 );
  return i;
}

const std::string canonLine::getCanonicalCommand() {
  if (canonTokens.size() < 3 ) return "BAD_LINE_NO_CMD";
  return canonTokens[2];
}

///return a line identifier as string: getN() if !=-1, else getLineNum()
const std::string canonLine::getLnum() {
  return ((getN()==-1) ? (cantok(0)) : (cantok(1)));
}

///tokenize myLine with the default delimiters `(), `
///\sa uio::tokenize(std::string str,std::vector<std::string>& tokenV,const std::string& delimiters)
inline void canonLine::tokenize() {
  uio::tokenize(myLine,canonTokens);
}

/**Create objects that inherit from canonLine. It determines which type of object to create, and returns a pointer to that object
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
  ** for one to contain the text "STRAIGHT" or "ARC_FEED"
  */
  if ( (cmnt!=std::string::npos) || (msg!=std::string::npos) ) {
    return new canonMotionless(l,s);
  } else if (lin!=std::string::npos) { //straight traverse or straight feed
    return new linearMotion(l,s);
  } else if (af!=std::string::npos) { //arc feed
    return new helicalMotion(l,s); //arc or helix
  } else { //canonical command is not a motion command
    return new canonMotionless(l,s);
  }
}

