project( camocc2 )
cmake_minimum_required( VERSION 2.6 )
set ( CMAKE_BUILD_TYPE Debug )
add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )

set( CASCADE_LIBS "TKernel;TKBool;TKBO;TKHLR;TKTopAlgo;TKShHealing;TKV3d;TKGeomBase;TKBRep;TKOffset;TKIGES;TKSTEP;PTKernel;TKShapeSchema;TKSTL;TKVRML;TKService;TKMath;TKG3d;TKG2d;TKPrim;TKV2d;TKFillet;TKXSBase;TKPShape" )
if( ${OCE_FOUND} )
  add_definitions ( -DHAVE_CONFIG_H )
  set( OpenCASCADE_LIBRARIES ${CASCADE_LIBS} )
  set( OpenCASCADE_INCLUDE_DIR ${OCE_INCLUDE_DIRS} )
else() #look for OpenCASCADE
  find_package ( OpenCASCADE REQUIRED COMPONENTS ${CASCADE_LIBS})
endif()


find_package ( OpenGL REQUIRED )

include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
	../uio/ ${OpenCASCADE_INCLUDE_DIR} )
set ( EXECUTABLE_OUTPUT_PATH ../../bin )
set ( camocc2_HDRS
	qoccinternal.h
	qocc.h
	lnk.hh
	)

set ( camocc2_SRCS
	main.cpp
	qoccapplication.cpp
	qoccdocument.cpp
	qoccharnesswindow.cpp
	qoccinputoutput.cpp
	qoccmakebottle.cpp
	qoccviewercontext.cpp
	qoccviewwidget.cpp
	lnk.cc
	)

set ( camocc2_MOCS
	qoccapplication.h
	qoccdocument.h
	qoccharnesswindow.h
	qoccinputoutput.h
	qoccviewercontext.h
	qoccviewwidget.h
	lnk.hh
	)
set ( camocc2_LIBS
    -L../../bin libg2model.so
    libOccUio.so
    libSalomeGeomShHeal.so
	)
QT4_WRAP_CPP(MOCS ${camocc2_MOCS})

add_executable ( camocc2 ${camocc2_SRCS} ${camocc2_HDRS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( camocc2  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY}
                                 ${OpenCASCADE_LIBRARIES} ${OPENGL_LIBRARIES} ${camocc2_LIBS} )

install(TARGETS camocc2 DESTINATION bin)

