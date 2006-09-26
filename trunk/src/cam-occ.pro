TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_on release

LIBS	+= -L/opt/occ61/lib -lTKernel -lTKV3d -lTKService -lTKSTEP -lTKSTEPBase -lTKBool

DEFINES	+= HAVE_IOSTREAM HAVE_LIMITS

INCLUDEPATH	+= /opt/occ61/inc

HEADERS	+= occview.h \
	interactive.h \
	occObject.h \
	ImportExport.h \
	pathAlgo.h

SOURCES	+= main.cpp \
	occview.cpp \
	interactive.cpp \
	occObject.cpp \
	ImportExport.cpp \
	pathAlgo.cpp

FORMS	= mainui.ui

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}

