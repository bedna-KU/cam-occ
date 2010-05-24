/***************************************************************************
 *   Copyright (C) 2010 by Mark Pictor					   *
 *   mpictor@gmail.com							   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef GTOM_HH
#define GTOM_HH

#include <vector>

#include <QString>
#include <QObject>

#include "canonLine.hh"
#include "machineStatus.hh"

class g2m: public QObject {
  Q_OBJECT;
  public:
    g2m();
    ~g2m();
    void start (QString file);
    bool isOK() {return success;};
  public slots:
    void slotModelFromFile();
  protected:
    void interpret( QString file );
    bool processCanonLine(std::string l);
    bool success;
    void infoMsg(QString);
    void sleepSecond();
    std::vector<canonLine*> lineVector;
};

#endif //GTOM_HH
