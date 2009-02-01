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
**
** Bugs (aka modifications) (C) 2009 Mark Pictor.  These modifications
** can be re-licensed under GPL v3.
**
****************************************************************************/
#ifndef QOCCAPPLICATION_H
#define QOCCAPPLICATION_H

#include <QtCore/QObject>
#include <QtGui/QApplication>
#include <QtGui/QPixmap>
#include <QtGui/QSplashScreen>
//#include <QMessageBox>

#include "qocc.h"

class QOCC_DECLSPEC QoccApplication : public QApplication
{

	Q_OBJECT

public:

    QoccApplication( int &argc, char **argv, int = QT_VERSION );
    ~QoccApplication( );

	void splashScreen ( const QPixmap &pixmap = QPixmap() );
	void splashMessage ( const QString &message, 
						 int alignment = Qt::AlignLeft,
						 const QColor &color = Qt::black );
	void splashFinish ( QWidget* w, long millisecs );
	void msleep ( unsigned long millisecs );
	
private:

	QSplashScreen* mySplash;
};

#endif // QOCCAPPLICATION_H
