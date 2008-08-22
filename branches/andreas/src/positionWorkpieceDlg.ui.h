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

void positionWorkpieceDlg::updateRatio()
{
    double m1=0,m2=0;
    double mult = scaleMultiplierValue->text().toDouble();
    QString text;
    if (mult < 0.0) {  //0 or negative mult is invalid, reset to +1
	mult = -1.0*mult;
	text.sprintf("%01lg", mult);
	scaleMultiplierValue->setText((const char*)text);
    } else if (mult == 0.0) {
	mult = 1.0;
	text.sprintf("%01lg", mult);
	scaleMultiplierValue->setText((const char*)text);
	cbScaleMultiplier->setChecked(0);
    }
    switch (comboCadU->currentItem())
    {
    case 0:
	m1=10;
	break;
    case 1:
	m1=25.4;
	break;
    case 2:
	m1=1;
	break;
    case 3:
	m1=1000;
	break;
    case 4:
	m1=.001;
	break;
    case 5:
	m1=.254;
	break;
    case 6:
	m1=304.8;
	break;
    }
    
        switch (comboCamU->currentItem())
    {
    case 0:
	m2=.1;
	break;
    case 1:
	m2=1/25.4;
	break;
    case 2:
	m2=1;
	break;
    case 3:
	m2=.001;
	break;
    case 4:
	m2=1000;
	break;
    case 5:
	m2=254;
	break;
    case 6:
	m2=1/304.8;
	break;
    }
    double m = m1*m2;
    if (cbScaleMultiplier->isChecked())
	m=m*mult;
    text.sprintf("%01lg", m);
    scaleRatioDisp->setText((const char*)text);
}

void positionWorkpieceDlg::setValues(double rx,double ry,double rz,double tx,double ty,double tz,uint cadU,uint camU,double mult,bool mCheck)
{
    QString text;
    text.sprintf("%01lg", rx);
    rxEdit->setText((const char*)text);
    text.sprintf("%01lg", ry);
    ryEdit->setText((const char*)text);
    text.sprintf("%01lg", rz);
    rzEdit->setText((const char*)text);
    text.sprintf("%01lg", tx);
    pxEdit->setText((const char*)text);
    text.sprintf("%01lg", ty);
    pyEdit->setText((const char*)text);
    text.sprintf("%01lg", tz);
    pzEdit->setText((const char*)text);
    comboCadU->setCurrentItem(cadU);
    comboCamU->setCurrentItem(camU);
    text.sprintf("%01lg", mult);
    scaleMultiplierValue->setText((const char*)text);
    cbScaleMultiplier->setChecked(mCheck);

    updateRatio();
}

void positionWorkpieceDlg::getValues(double& rx,double& ry,double& rz,double& tx,double& ty,double& tz,uint& cadU,uint& camU,double& mult, bool& mCheck, double& ratio)
{
    rx=rxEdit->text().toDouble();
    ry=ryEdit->text().toDouble();
    rz=rzEdit->text().toDouble();
    tx=pxEdit->text().toDouble();
    ty=pyEdit->text().toDouble();
    tz=pzEdit->text().toDouble();
    cadU = comboCadU->currentItem();
    camU = comboCamU->currentItem();
    mCheck = cbScaleMultiplier->isChecked();
    mult = scaleMultiplierValue->text().toDouble();
    updateRatio();
    ratio = scaleRatioDisp->text().toDouble();
}


void positionWorkpieceDlg::multiplierChanged()
{
    if (scaleMultiplierValue->text().toDouble() == 1.0) {
	cbScaleMultiplier->setChecked(0);
    } else {
	cbScaleMultiplier->setChecked(1);
    }
    updateRatio();
}


void positionWorkpieceDlg::slotResetToDefaults()
{
    setValues(0,0,0,0,0,0,2,2,1,false);
}
