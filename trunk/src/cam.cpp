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

#include <assert.h>

#include "cam.h"
#include "uiStuff.h"
#include "shapeInfo.h"
#include "aSample.h"
#include "shapeOffset.h"
#include "gcode2Model.h"

cam::cam(QoccHarnessWindow* window) {
	assert(window != 0);

	uiStuff *ui;
	ui = new uiStuff();
	ui->init(window);

	shapeInfo *shapeI;
	shapeI = new shapeInfo();
	shapeI->init(window);

	aSample *sample;
	sample = new aSample();
	sample->init(window);
	
	shapeOffset *shapeO;
	shapeO = new shapeOffset();
	shapeO->init(window);

	gcode2Model *gToM;
	gToM = new gcode2Model();
	gToM->init(window);

}

cam::~cam() {
}
