/*************************************************************
** License: GPL.  
** Derived from work by Olivier Coma. 
** Copyright (C) 2006 Mark Pictor
*************************************************************/

#include <qapplication.h>
#include "mainui.h"

QPaintEvent *pev;	
QApplication *pA;

int main( int argc, char ** argv )
{
    QApplication a( argc, argv );
    pA=&a;
    mainui w;
    w.show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    w.paintEvent(pev);	//cause initialization of occ components
		//otherwise user must resize window before it will work...!
    return a.exec();
}
