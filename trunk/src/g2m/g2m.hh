#include <QString>
#include "canonLine.hh"
#include "machineStatus.hh"
#include <vector>

class g2m {
  public:
    g2m(QString file);
    bool isOK() {return success;};
  protected:
    bool processCanonLine(std::string l);
    bool success;
    void infoMsg(QString);
    void sleepSecond();
    std::vector<canonLine*> lineVector;
};

