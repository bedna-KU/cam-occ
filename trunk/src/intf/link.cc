#include "link.hh"

link::link(QOccMainWindow* w) {
  //set up pointers
  pt(w);
  //cam myCam(pt);
  misc myMisc(pt); // this will be all the stuff that's now "cam"
  g2m gtom(pt); //FIXME g2m has no ui initialization, etc - put elsewhere?
}