TEMPLATE = app

CONFIG -= thread \
 release
CONFIG += opengl \
x11 \
 debug
DEFINES += _OCC64 \
LIN \
LININTEL \
HAVE_CONFIG_H \
HAVE_IOSTREAM \
HAVE_FSTREAM \
HAVE_LIMITS
QMAKE_CXXFLAGS_DEBUG += -m64

QMAKE_CXXFLAGS_RELEASE += -m64

INCLUDEPATH += /opt/occ63/inc/ \
../inc
LIBS += -L/opt/occ63/lib/ \
-lTKShHealing \
-lTKOffset \
-lTKBool \
-lTKSTEPBase \
-lTKSTEP \
-lTKService \
-lTKV3d \
-lTKernel \
-lTKIGES \
-lPTKernel \
-lTKSTL \
-lTKVRML \
-lTKTopAlgo \
-lTKBRep \
-lTKPShape \
 -lTKShapeSchema



SOURCES += main.cpp \
qoccapplication.cpp \
qoccdocument.cpp \
qoccharnesswindow.cpp \
qoccinputoutput.cpp \
qoccmakebottle.cpp \
qoccviewercontext.cpp \
qoccviewwidget.cpp \
 shapeInfo.cpp \
 cam.cpp \
 uiStuff.cpp \
 aSample.cpp \
 shapeOffset.cpp \
 gcode2Model.cpp

DESTDIR = ../bin

HEADERS += qoccinternal.h \
 ../inc/qoccapplication.h \
 ../inc/qoccdocument.h \
 ../inc/qocc.h \
 ../inc/qoccharnesswindow.h \
 ../inc/qoccinputoutput.h \
 ../inc/qoccviewercontext.h \
 ../inc/qoccviewwidget.h \
 ../inc/shapeInfo.h \
 ../inc/cam.h \
 ../inc/uiStuff.h \
 ../inc/aSample.h \
 ../inc/shapeOffset.h \
 .moc/ui_longMsgDlg.h \
 ../inc/gcode2Model.h





TARGET = camocc2

OBJECTS_DIR = .obj/

UI_DIR = .moc/

MOC_DIR = .moc/

FORMS += longMsgDlg.ui

