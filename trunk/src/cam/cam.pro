TEMPLATE = lib

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



SOURCES += shapeInfo.cpp \
 cam.cpp \
 uiStuff.cpp \
 aSample.cpp \
 shapeOffset.cpp \
 gcode2ModelUi.cpp \
 gcode2ModelGc.cpp \
 gcode2Model3d.cpp

DESTDIR = ../../bin

HEADERS += ../inc/qoccinternal.h \
 ../inc/shapeInfo.h \
 ../inc/cam.h \
 ../inc/uiStuff.h \
 ../inc/aSample.h \
 ../inc/shapeOffset.h \
 ../inc/gcode2Model.h


TARGET = CamOcc

OBJECTS_DIR = .obj/

UI_DIR = .moc/

MOC_DIR = .moc/


