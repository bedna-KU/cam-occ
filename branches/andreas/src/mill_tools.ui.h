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



void Mill_tools::enableDisableShapeParm( QString shape )
{
    if (shape == "Corner-rounding") {
	editShapeParm->setEnabled(true);
	editShapeParm->setText("Corner Radius");
    } else if (shape == "V (Drill Bit/Engraver)") {
	editShapeParm->setEnabled(true);
	editShapeParm->setText("Included Angle");
    } else {
	editShapeParm->setEnabled(false);
    }
}


void Mill_tools::setValue(double toolDiameter)
{/*
QString text;
    text.sprintf("%01lg", toolDiameter);
    toolDiamterEdit->setText((const char*)text);
*/    
}


void Mill_tools::getValue(double toolDiameter)
{
    /*
toolDiamter=toolDiamterEdit->text().toDouble();
*/
}
