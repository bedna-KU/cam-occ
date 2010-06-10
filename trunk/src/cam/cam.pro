TEMPLATE = lib

CONFIG -= thread
CONFIG += opengl x11
CONFIG += debug
CONFIG -= release

linux-g++ {
        DEFINES += LIN LININTEL
        HARDWARE_PLATFORM = $$system(uname -m)
        contains( HARDWARE_PLATFORM, x86_64 ) {
                # 64-bit Linux
                message ("Adding Linux 64 bits compile flags and definitions")
                DEFINES += _OCC64
                QMAKE_CXXFLAGS += -m64

        } else {
                # 32-bit Linux
        }
} else {
        message ("Not supported on any platform other than linux!")
}

DEFINES += HAVE_CONFIG_H HAVE_IOSTREAM HAVE_FSTREAM HAVE_LIMITS

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


