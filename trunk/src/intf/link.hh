//link class: link between qocc and my stuff

#ifndef LINK_HH
#define LINK_HH

//this will be part of a library, which will also 
//contain the ptr class and gooey/uiStuff
//this will speed compilation

class link {
  public:
    link(QOccMainWindow* w);
  private:
    ptr pt;
}

#endif //LINK_HH
