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
** Copyright (C) 2006-7 Peter Dolbey. All rights reserved.
**
**
** Bugs (aka modifications) (C) 2009 Mark Pictor.  These modifications
** can be re-licensed under GPL v3.
**
****************************************************************************/
/*!
\class	QtApplication
\brief	This class provides an application extension for the QtOpenCascade Toolkit.
\author	Peter C. Dolbey
*/
#include <QtCore/QEvent>
#include <QtCore/QDataStream>
#include <QtGui/QCursor>
#include <QtCore/QWaitCondition>
#include <QtCore/QMutex>
#include <QtCore/QThread>
#include <QtCore/QDir>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>

#include "qoccinternal.h"
#include "qoccapplication.h"

QoccApplication::QoccApplication ( int &argc, char **argv, int _internal ) :
		QApplication ( argc, argv, _internal ),
		mySplash ( NULL )
{
	//splashScreen(NULL);
	//splashFinish(mySplash,1000);
}

QoccApplication::~QoccApplication()
{
	if ( mySplash )
	{
		delete mySplash;
		mySplash = NULL;
	}
}


void QoccApplication::splashScreen ( const QPixmap &pixmap )
{
	QPixmap* p = ( QPixmap* ) &pixmap;
	if ( p->isNull() )
	{
		p = new QPixmap ( "images/qoccsplash.png" );
	}
	mySplash = new QSplashScreen ( *p, Qt::WindowStaysOnTopHint );
	if ( mySplash )
	{
		mySplash->show();
		splashMessage ( tr ( "Initializing Application..." ), Qt::AlignRight | Qt::AlignTop );
	}
}

void QoccApplication::splashMessage ( const QString &message, int alignment, const QColor &color )
{
	if ( mySplash )
	{
		mySplash->showMessage ( message, alignment, color );
	}
}

void QoccApplication::splashFinish ( QWidget* widget, long millisecs )
{
	if ( mySplash )
	{
		msleep ( millisecs );
		mySplash->finish ( widget );
		delete mySplash;
		mySplash = NULL;
	}
}

void QoccApplication::msleep ( unsigned long millisecs )
{
	QMutex mutex;
	QWaitCondition waitCondition;
	mutex.lock();
	waitCondition.wait ( &mutex, millisecs );
	mutex.unlock();
}
