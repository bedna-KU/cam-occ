TEMPLATE = app

CONFIG -= thread
CONFIG += opengl
CONFIG += x11
CONFIG += debug
CONFIG -= release

DEFINES += _OCC64 LIN LININTEL 
DEFINES += HAVE_CONFIG_H HAVE_IOSTREAM HAVE_FSTREAM HAVE_LIMITS

QMAKE_CXXFLAGS_DEBUG += -m64
QMAKE_CXXFLAGS_RELEASE += -m64

INCLUDEPATH += /opt/occ63/inc/ ../lnk/
LIBS += -L/opt/occ63/lib/ -L../../bin \
-lTKShHealing -lTKOffset -lTKBool -lTKSTEPBase \
-lTKSTEP -lTKService -lTKV3d -lTKernel -lTKIGES \
-lPTKernel -lTKSTL -lTKVRML -lTKTopAlgo -lTKBRep \
-lTKPShape -lTKShapeSchema -lOccUio -lg2model

SOURCES += main.cpp \
qoccapplication.cpp \
qoccdocument.cpp \
qoccharnesswindow.cpp \
qoccinputoutput.cpp \
qoccmakebottle.cpp \
qoccviewercontext.cpp \
qoccviewwidget.cpp \
lnk.cc

DESTDIR = ../../bin

HEADERS += qoccinternal.h \
 qoccapplication.h \
 qoccdocument.h \
 qocc.h \
 qoccharnesswindow.h \
 qoccinputoutput.h \
 qoccviewercontext.h \
 qoccviewwidget.h \
 lnk.hh

TARGET = camocc2

OBJECTS_DIR = .obj/

UI_DIR = .moc/

MOC_DIR = .moc/


