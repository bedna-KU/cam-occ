TEMPLATE = app

CONFIG -= thread
CONFIG += opengl
CONFIG += x11
CONFIG += debug
CONFIG -= release
CONFIG -= mallocproxy
#CONFIG += mallocproxy

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

DEFINES += HAVE_CONFIG_H HAVE_IOSTREAM HAVE_FSTREAM HAVE_LIMITS
#DEFINES += OCC_PATCHED

CONFIG(mallocproxy) {
  DEFINES += USE_TBB_MALLOC_PROXY
  LIBS += -ltbbmalloc_proxy
} else {
  DEFINES -= USE_TBB_MALLOC_PROXY
  LIBS -= -ltbbmalloc_proxy
}

INCLUDEPATH += /opt/occ63/inc/ ../uio/ /usr/include/opencascade
LIBS += -L/opt/occ63/lib -L../../bin \
-lTKShHealing -lTKOffset -lTKBool -lTKSTEPBase \
-lTKSTEP -lTKService -lTKV3d -lTKernel -lTKIGES \
-lPTKernel -lTKSTL -lTKVRML -lTKTopAlgo -lTKBRep \
-lTKPShape -lTKShapeSchema -lOccUio -lg2model -lstdc++ \
-lTKLCAF -lSalomeGeomShHeal

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


