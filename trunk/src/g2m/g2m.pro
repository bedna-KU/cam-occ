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

INCLUDEPATH += /opt/occ63/inc/ ../inc/
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



SOURCES += canon.cc canonMotion.cc helicalMotion.cc machineStatus.cc tool.cc canonLine.cc g2m.cc linearMotion.cc tmpmain.cc

HEADERS += canon.hh canonMotion.hh g2m.hh linearMotion.hh tool.hh canonLine.hh canonMotionless.hh helicalMotion.hh machineStatus.hh

DESTDIR = .

TARGET = g2model

OBJECTS_DIR = .obj/

UI_DIR = .moc/

MOC_DIR = .moc/


