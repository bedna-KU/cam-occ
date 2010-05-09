#include "ptr.hh"

ptr::ptr(QMainWindow* window) {
  windowPtr = window;
  viewPtr = window->getView();
  contextPtr = window->getContext();
  occPtr = window->getOCC();
  vcPtr = window->getVC();
  mbPtr = window->menuBar();
  hmPtr = window->getHelpMenu();
  uioPtr = new uio(window);
}
