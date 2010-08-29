/****************************************************************************
**
** This file is part of the QtOpenCascade Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** Copyright (C) Peter Dolbey 2006-7. All rights reserved.
**
**
** Bugs (aka modifications) (C) 2009 Mark Pictor.  These modifications
** can be re-licensed under GPL v3.
**
****************************************************************************/
/*!
\file	main.cpp
\brief	Top level application
*/

#include <iostream>
#include <fstream>
using namespace std;
#include <QtGui/QtGui>
#include <QtGui/QPlastiqueStyle>
#include <QStringList>

#include "qoccapplication.h"
#include "qoccharnesswindow.h"

int main(int argc, char *argv[])
{
  QoccApplication app( argc, argv );

  QStringList qsl;
  for(int i = 1; i < argc; i++) {
    qsl.append(argv[i]);
  }

  QoccHarnessWindow *window = new QoccHarnessWindow(qsl);

  window->show();

  int retval = app.exec();
  return retval;
}
