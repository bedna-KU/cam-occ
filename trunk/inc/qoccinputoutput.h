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
#ifndef QOCCINPUTOUTPUT_H
#define QOCCINPUTOUTPUT_H

#include <QtCore/QObject>
#include "qocc.h"

class QOCC_DECLSPEC QoccInputOutput : public QObject
{
	Q_OBJECT

public:

    enum FileFormat{	FormatBREP, 
						FormatIGES, 
						FormatSTEP, 
						FormatCSFDB, 
						FormatVRML, 
						FormatSTL	};

	QoccInputOutput(void);
	~QoccInputOutput(void);

    bool							  importModel( const QString fileName, 
												   const FileFormat format, 
												   const Handle_AIS_InteractiveContext& ic );

    bool                              exportModel( const QString fileName, 
												   const FileFormat format, 
												   const Handle_AIS_InteractiveContext& ic);

    QString                           info() const;

signals:

	void error (int errorCode, QString& errorDescription);

private:

    Handle_TopTools_HSequenceOfShape  importModel( const FileFormat format, 
												   const QString& fileName);
    bool                              exportModel( const FileFormat format, 
												   const QString&,
                                                   const Handle_TopTools_HSequenceOfShape& );
	
    Handle_TopTools_HSequenceOfShape getShapes( const Handle_AIS_InteractiveContext& oc);

	Handle_TopTools_HSequenceOfShape importBREP ( const QString& );

	Handle_TopTools_HSequenceOfShape importIGES ( const QString& );
	Handle_TopTools_HSequenceOfShape importSTEP ( const QString& );
	Handle_TopTools_HSequenceOfShape importCSFDB( const QString& );

	bool exportBREP ( const QString& fileName, const Handle_TopTools_HSequenceOfShape& shapes );
	bool exportIGES ( const QString& fileName, const Handle_TopTools_HSequenceOfShape& shapes );
	bool exportSTEP ( const QString& fileName, const Handle_TopTools_HSequenceOfShape& shapes );
	bool exportCSFDB( const QString& fileName, const Handle_TopTools_HSequenceOfShape& shapes );
	bool exportSTL  ( const QString& fileName, const Handle_TopTools_HSequenceOfShape& shapes );
	bool exportVRML ( const QString& fileName, const Handle_TopTools_HSequenceOfShape& shapes );

    bool checkFacetedBrep( const Handle_TopTools_HSequenceOfShape& );

	// Attributes
	
	QString myInfo;

};

#endif

