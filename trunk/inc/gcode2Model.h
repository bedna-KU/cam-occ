/***************************************************************************
 *   Copyright (C) 2009 by Mark Pictor					   *
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
#ifndef GCODE2MODEL_H
#define GCODE2MODEL_H

#include <limits.h>

#include <QAction>
#include <QMenuBar>
#include <QObject>

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <TopoDS_Edge.hxx>

#include "uiStuff.h"

class gcode2Model : public uiStuff
{
	Q_OBJECT

public:
	gcode2Model();
	void init(QoccHarnessWindow *window);
private slots:
	void myMenuItem();

private:
	TopoDS_Edge arc ( gp_Pnt a, gp_Pnt b, gp_Pnt c );
	TopoDS_Edge arc ( gp_Pnt a, gp_Vec V, gp_Pnt c );
	QMenu *myMenu;
	QAction *myAction;
};
#endif //GCODE2MODEL_H
