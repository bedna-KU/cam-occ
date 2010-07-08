
/*
** had to rename to lnk because something else must define link:
**    qoccharnesswindow.cpp:55: warning:
**    statement is a reference, not call, to function ‘link’
** must be a name collision - renaming makes it go away
*/

#ifndef LNK_HH
#define LNK_HH

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

#include "qoccharnesswindow.h"

class QoccHarnessWindow;

/*!
\class lnk
\brief This class is the link between qocc and my cam- and gcode-related code.
  It was originally named 'link' but that caused compilation problems in qoccharnesswindow.
\author Mark Pictor
*/
class lnk : public QObject {
  Q_OBJECT;
  public:
    lnk(QoccHarnessWindow *w);
};

#endif //LNK_HH
