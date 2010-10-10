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
                message("32-bit Linux")
        }
} else {
        message ("Not supported on any platform other than linux!")
}

CONFIG(thread) {
  DEFINES += MULTITHREADED
} else {
#  LIBS -= -lpthread
}

DEFINES += HAVE_CONFIG_H HAVE_IOSTREAM HAVE_FSTREAM HAVE_LIMITS

INCLUDEPATH += /opt/occ63/inc/ ../uio/ /usr/include/opencascade ../contrib/salome

LIBS += -L/opt/occ63/lib/ -L../../bin\
-lTKShHealing -lTKOffset -lTKBool  -lTKService -lTKV3d -lTKernel \
-lTKTopAlgo -lTKBRep -lOccUio
#-lstdc++

# -lTKSTEPBase -lTKSTEP -lTKIGES -lPTKernel -lTKSTL -lTKVRML -lTKPShape -lTKShapeSchema

SOURCES += g2m.cc canon.cc canonLine.cc canonMotionless.cc canonMotion.cc \
linearMotion.cc helicalMotion.cc tool.cc machineStatus.cc

HEADERS += g2m.hh canon.hh canonLine.hh canonMotionless.hh canonMotion.hh \
linearMotion.hh helicalMotion.hh tool.hh machineStatus.hh

DESTDIR = ../../bin

TARGET = g2model

OBJECTS_DIR = .obj/

UI_DIR = .moc/

MOC_DIR = .moc/


