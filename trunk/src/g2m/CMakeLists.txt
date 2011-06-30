project( g2model )
cmake_minimum_required( VERSION 2.6 )
set ( CMAKE_BUILD_TYPE Debug )
add_definitions ( -Wall )

find_package ( Qt4 REQUIRED )

set( CASCADE_LIBS "TKernel;TKBRep;TKTopAlgo;TKMath;TKV3d;TKGeomBase;TKGeomAlgo;TKShHealing;TKBO;TKBool;TKOffset;TKLCAF;TKMath;TKService" )
if( ${OCE_FOUND} )
  add_definitions ( -DHAVE_CONFIG_H )
  set( OpenCASCADE_LIBRARIES ${CASCADE_LIBS} )
  set( OpenCASCADE_INCLUDE_DIR ${OCE_INCLUDE_DIRS} )
else() #look for OpenCASCADE
  find_package ( OpenCASCADE REQUIRED COMPONENTS ${CASCADE_LIBS})
endif()

include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
	../uio/ ${OpenCASCADE_INCLUDE_DIR}
	../contrib/salome)
set ( LIBRARY_OUTPUT_PATH ../../bin )
set ( g2model_HDRS
	g2m.hh
	canon.hh
	canonLine.hh
	canonMotionless.hh
	canonMotion.hh
	linearMotion.hh
	helicalMotion.hh
	tool.hh
	machineStatus.hh
	)

set ( g2model_SRCS
	g2m.cc
	canon.cc
	canonLine.cc
	canonMotionless.cc
	canonMotion.cc
	linearMotion.cc
	helicalMotion.cc
	tool.cc
	machineStatus.cc
	)

#fix vtable error
set ( g2model_MOCS g2m.hh )
QT4_WRAP_CPP(MOCS ${g2model_MOCS})

add_library ( g2model SHARED ${g2model_SRCS} ${g2model_HDRS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( g2model  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY}
                          ${OpenCASCADE_LIBRARIES} )

install(TARGETS g2model DESTINATION lib)

