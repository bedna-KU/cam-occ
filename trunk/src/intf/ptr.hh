//class ptr, i.e. interface
//interface to qocc
#ifndef PTR_HH
#define PTR_HH

#include "../qocc/qoccviewwidget.h"
#include "../qocc/qoccviewercontext.h"
#include <Handle_V3d_View.hxx>
#include <Handle_AIS_InteractiveContext.hxx>
#include <QMenuBar>
#include <QAction>
#include <QMainWindow>
#include "gooey.hh"

class ptr {
  private:
    QMainWindow* windowPtr;
    Handle_V3d_View* viewPtr;
    Handle_AIS_InteractiveContext* contextPtr;
    QoccViewWidget* occPtr;
    QoccViewerContext* vcPtr;
    QMenuBar* mbPtr;
    QAction* hmPtr;
    uio* uioPtr;
  public:
    ptr(QMainWindow* window);
    QMainWindow* window() {return windowPtr;};
    Handle_V3d_View* view() {return viewPtr;};
    Handle_AIS_InteractiveContext* context() {return contextPtr;};
    QoccViewWidget* occ() {return occPtr;};
    QoccViewerContext* vc() {return vcPtr;};
    QMenuBar* mb() {return mbPtr;};
    QAction* hm() {return hmPtr;};
    uio* uio() {return uioPtr;};
};

#endif //PTR_HH
