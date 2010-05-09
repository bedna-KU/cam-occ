// temporary main(){}
//#include <stdio.h>
#include <QString>
#include "g2m.hh"

int main (int argc, char* argv[]) {
  QString f = "/opt/src/emc2/trunk/nc_files/m-arcspiral.ngc";
  g2m *gtom = new g2m(f);
}
