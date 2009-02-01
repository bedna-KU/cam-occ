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
#ifndef SHAPEINFO_H
#define SHAPEINFO_H

#include <limits.h>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>


#include <QAction>
#include <QMenuBar>
#include <QObject>

#include "uiStuff.h"

class shapeInfo : public uiStuff
{
	Q_OBJECT

public:
	shapeInfo();
	void init(QoccHarnessWindow *window);
private slots:
	void infoButton();

private:
	void edgeInfo(TopoDS_Edge E);
	void faceInfo(TopoDS_Face F);
	void solidInfo(TopoDS_Solid S);
	void vertexInfo(TopoDS_Vertex V);

	QMenu *myMenu;
	QAction *infoAction;
};
#endif //SHAPEINFO_H
