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

INCLUDEPATH += /opt/occ63/inc/ ../intf/
LIBS += -L/opt/occ63/lib/ -L../../bin \
-lTKShHealing -lTKOffset -lTKBool -lTKSTEPBase \
-lTKSTEP -lTKService -lTKV3d -lTKernel -lTKIGES \
-lPTKernel -lTKSTL -lTKVRML -lTKTopAlgo -lTKBRep \
-lTKPShape -lTKShapeSchema -lCommonOcc 
#-lCamOcc



SOURCES += main.cpp \
qoccapplication.cpp \
qoccdocument.cpp \
qoccharnesswindow.cpp \
qoccinputoutput.cpp \
qoccmakebottle.cpp \
qoccviewercontext.cpp \
qoccviewwidget.cpp \

DESTDIR = ../../bin

HEADERS += qoccinternal.h \
 qoccapplication.h \
 qoccdocument.h \
 qocc.h \
 qoccharnesswindow.h \
 qoccinputoutput.h \
 qoccviewercontext.h \
 qoccviewwidget.h \


TARGET = camocc2

OBJECTS_DIR = .obj/

UI_DIR = .moc/

MOC_DIR = .moc/


