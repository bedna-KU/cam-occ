#include "g2m.hh"
#include <QProcess>
#include <QStringList>
#include <QString>
#include <QTime>
#include <QCoreApplication>
#include <QEventLoop>
#include <iostream>
#include "canonLine.hh"

g2m::g2m ( QString file )
{
  success = false;
  QString ipath = "/opt/src/emc2/trunk/";
  QString interp = ipath + "bin/rs274";
  QString iparm = ipath + "configs/sim/sim_mm.var\n";
  QString itool = ipath + "configs/sim/sim_mm.tbl\n";
  QProcess toCanon;
  bool foundEOF;
  toCanon.start(interp,QStringList(file));
  /**************************************************
  Apparently, QProcess::setReadChannel screws with waitForReadyRead() and canReadLine()
  So, we just fly blind and assume that there are no errors when we navigate
  the interp's "menu", and that it requires no delays.
  **************************************************/
  
  //now give the interpreter the data it needs
  toCanon.write("2\n");	//set parameter file
  toCanon.write(iparm.toAscii());
  toCanon.write("3\n");	//set tool table file
  toCanon.write(itool.toAscii());
  //can also use 4 and 5
  
  toCanon.write("1\n"); //start interpreting
  //cout << "stderr: " << (const char*)toCanon.readAllStandardError() << endl;
  
  if (!toCanon.waitForReadyRead(1000) ) {
    if ( toCanon.state() == QProcess::NotRunning ){
      infoMsg("Interpreter died.  Bad tool table " + itool + " ?");
    } else  infoMsg("Interpreter timed out for an unknown reason.");
    cout << "stderr: " << (const char*)toCanon.readAllStandardError() << endl;
    cout << "stdout: " << (const char*)toCanon.readAllStandardOutput() << endl;
    toCanon.close();
    return;
  }
  
  //if readLine is used at the wrong time, it is possible to pick up a line fragment! will canReadLine() fix that?
  qint64 lineLength;
  char line[260];
  uint fails = 0;
  do {
    if (toCanon.canReadLine()) {
      lineLength = toCanon.readLine(line, sizeof(line));
      if (lineLength != -1 ) {
	foundEOF = processCanonLine(line);
      } else {	//shouldn't get here!
	fails++;
	sleepSecond();
      }
    } else {
      fails++;
      sleepSecond();
    }
  } while ( (fails < 100) && 
  ( (toCanon.canReadLine()) || ( toCanon.state() != QProcess::NotRunning ) )  );
  //((lineLength > 0) || 	//loop until interp quits and all lines are read.
  //toCanon.canReadLine() ||  
  success = foundEOF;
  return;
}

bool g2m::processCanonLine ( std::string l )
{
  //need a vector to store canonLine in
  //TODO in header file?
  
  //create the object and get its pointer
  canonLine * cl = canonLine::canonLineFactory
  		(l,*lineVector.back()->getStatus());
  //store it
  lineVector.push_back(cl);
  return cl->checkErrors();
}

void g2m::sleepSecond() {
	//sleep 1s and process events
	//cout << "SLEEP..." << endl;
	QTime dieTime = QTime::currentTime().addSecs(1);
	while( QTime::currentTime() < dieTime )
		QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents, 100);
	return;
}

void g2m::infoMsg(QString s) {
  cout << s.toStdString() << endl;
}
