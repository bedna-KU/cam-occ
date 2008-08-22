/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/


void cuttingParameter::slotSetToolDia(int toolRad )
{
    QString text;
     text.sprintf("%01lg", (2*toolRad));
     toolDiameter->setText((const char*)text);
}

void cuttingParameter::slotSetFeed(int feedVal)
{/*
    QString text;
     text.sprintf("%01lg", feedVal);
    feed->setText((const char*)text);*/
}

void cuttingParameter::slotSetSpeed(int speedVal)
{/*
    QString text;
     text.sprintf("%01lg", speedVal);
    speed->setText((const char*)text);*/
}


void cuttingParameter::slotGetToolDia(int &toolDiameterVal )
{
    toolDiameterVal=toolDiameter->text().toInt(); // from Diameter to radius
     // toolRadius=(toolDiameter->text().toInt()); // from Diameter to radius
}

void cuttingParameter::slotGetFeed(int &feedVal)
{
 feedVal=feed->text().toInt();
}

void cuttingParameter::slotGetSpeed(int &speedVal)
{
  speedVal=speed->text().toInt();
}











