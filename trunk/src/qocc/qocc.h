/****************************************************************************
**
** This file is part of the QtOpenCascade Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.
**
** Copyright (C) Peter Dolbey 2006-7. All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

/*!
\mainpage
\section    g2m Gcode-to-model

            Using EMC2's stand-alone interpreter, gcode is converted into a series of
            simple canonical commands which are then processed to create a 3d model of
            the material to be removed. See classes g2m and canonLine, and the classes
            that inherit from the latter.

            Copyright (C) Mark Pictor 2010. My original work and my modifications to
            pdolbey's work are licensed under GPL v2 or later.


\section    cam CAM (Computer-Aided Manufacturing)

            While I originally intended to write a CAM program, I haven't proceeded very
            far in that direction. Currently there is no way to generate g-code from
            this program. If you are looking for useable open-source CAM, try HeeksCNC.


\section    qtocc QtOpenCascade

			This app uses a version of the QtOpenCascade Toolkit. It may be used under
			the terms of the GNU General Public License version 2.0 as published by
			the Free Software Foundation and appearing in the file COPYING included in
			the packaging of this file.

			Copyright (C) Peter Dolbey 2006-7. All rights reserved.

			This is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
			WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.


\section	install Installation

			To install and compile the library, unzip the source file into a folder
			of your choice and open a command prompt there.
			<pre>
				qmake
				make
			</pre>
			to build the package.

\section    run Running the program

            Because cam-occ uses libraries, you will need to add bin/ to LD_LIBRARY_PATH.
            Alternately, edit the file camocc.sh to suit.

            Command line: Currently, cam-occ recognizes the argument 'g2m', followed by
            an optional file name. This will load a .ngc or .canon file and build a model
            from it. For example: './camocc.sh g2m ngc-in/cds.ngc'

            For debugging, there is debug.sh which sets environment variables and then
            runs gdb.
*/


#ifndef QOCC_H
#define QOCC_H
#include <limits.h>

#include <AIS_StatusOfDetection.hxx>
#include <AIS_StatusOfPick.hxx>
#include <Aspect_Drawable.hxx>
#include <Aspect_GridDrawMode.hxx>
#include <Aspect_GridType.hxx>
#include <Aspect_GraphicCallbackProc.hxx>
#include <Handle_AIS_InteractiveContext.hxx>
#include <Handle_V3d_View.hxx>
#include <Handle_V3d_Viewer.hxx>
#include <Handle_TopTools_HSequenceOfShape.hxx>
#include <Standard_TypeDef.hxx>
#include <Quantity_Factor.hxx>
#include <Quantity_Length.hxx>
#include <Quantity_NameOfColor.hxx>
#include <V3d_Coordinate.hxx>

#ifdef WNT
#include <Handle_WNT_Window.hxx>
#else
#include <Handle_Xw_Window.hxx>
#endif

#ifdef QOCC_STATIC
#define QOCC_DECLSPEC
#else
#ifdef QOCC_MAKEDLL
#define QOCC_DECLSPEC Q_DECL_EXPORT
#else
#define QOCC_DECLSPEC Q_DECL_IMPORT
#endif
#endif

#define SIGN(X) ((X) < 0. ? -1 : ((X) > 0. ? 1 : 0.))

#endif // Qocc_H

