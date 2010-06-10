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

INCLUDEPATH += /opt/occ63/inc/ ../uio/
LIBS += -L/opt/occ63/lib/ -L../../bin\
-lTKShHealing -lTKOffset -lTKBool -lTKSTEPBase \
-lTKSTEP -lTKService -lTKV3d -lTKernel \
-lTKIGES -lPTKernel -lTKSTL -lTKVRML \
-lTKTopAlgo -lTKBRep -lTKPShape -lTKShapeSchema \
-lOccUio -lstdc++

SOURCES += canon.cc canonMotion.cc g2m.cc linearMotion.cc tool.cc \
canonLine.cc  canonMotionless.cc  helicalMotion.cc  machineStatus.cc dispShape.cc

HEADERS += canon.hh canonMotion.hh g2m.hh linearMotion.hh tool.hh canonLine.hh canonMotionless.hh helicalMotion.hh machineStatus.hh dispShape.hh

DESTDIR = ../../bin

TARGET = g2model

OBJECTS_DIR = .obj/

UI_DIR = .moc/

MOC_DIR = .moc/


