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

DEFINES += HAVE_CONFIG_H HAVE_IOSTREAM HAVE_FSTREAM HAVE_LIMITS

INCLUDEPATH += /opt/occ63/inc/ . /usr/include/opencascade

LIBS += -L/opt/occ63/lib/ -lTKLCAF
# -lTKShHealing -lTKernel
# -lTKTopAlgo -lTKBRep -lrt
# -lTKOffset -lTKBool -lTKSTEPBase -lTKSTEP -lTKV3d -lTKService
# -lPTKernel -lTKSTL -lTKIGES -lTKVRML  -lTKPShape -lTKShapeSchema


HEADERS += ShHealOper_ChangeOrientation.hxx  ShHealOper_RemoveFace.hxx \
ShHealOper_SplitCurve2d.hxx ShHealOper_CloseContour.hxx ShHealOper_RemoveInternalWires.hxx \
ShHealOper_SplitCurve3d.hxx ShHealOper_EdgeDivide.hxx ShHealOper_Sewing.hxx \
ShHealOper_Tool.hxx ShHealOper_FillHoles.hxx ShHealOper_ShapeProcess.hxx

SOURCES += ShHealOper_ChangeOrientation.cpp ShHealOper_RemoveFace.cpp \
ShHealOper_SplitCurve2d.cpp ShHealOper_CloseContour.cpp ShHealOper_RemoveInternalWires.cpp \
ShHealOper_SplitCurve3d.cpp ShHealOper_EdgeDivide.cpp ShHealOper_Sewing.cpp \
ShHealOper_Tool.cpp ShHealOper_FillHoles.cpp ShHealOper_ShapeProcess.cpp


DESTDIR = ../../../bin
TARGET = SalomeGeomShHeal

OBJECTS_DIR = .obj/
UI_DIR = .ui/
MOC_DIR = .moc/
