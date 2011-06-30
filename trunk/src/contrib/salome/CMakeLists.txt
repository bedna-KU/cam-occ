project( SalomeGeomShHeal )
cmake_minimum_required( VERSION 2.6 )
set ( CMAKE_BUILD_TYPE Debug )
add_definitions ( -Wall )

set( CASCADE_LIBS "TKernel;TKCAF;PTKernel;TKBRep;TKMath;TKPCAF;TKXCAF;TKLCAF" )
if( ${OCE_FOUND} )
  add_definitions ( -DHAVE_CONFIG_H )
  set( OpenCASCADE_LIBRARIES ${CASCADE_LIBS} )
  set( OpenCASCADE_INCLUDE_DIR ${OCE_INCLUDE_DIRS} )
else() #look for OpenCASCADE
  find_package ( OpenCASCADE REQUIRED COMPONENTS ${CASCADE_LIBS})
endif()

find_package ( Qt4 REQUIRED )
include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
	${OpenCASCADE_INCLUDE_DIR} . )
set ( LIBRARY_OUTPUT_PATH ../../../bin )
set ( SalomeGeomShHeal_HDRS
	ShHealOper_ChangeOrientation.hxx
	ShHealOper_RemoveFace.hxx
	ShHealOper_SplitCurve2d.hxx
	ShHealOper_CloseContour.hxx
	ShHealOper_RemoveInternalWires.hxx
	ShHealOper_SplitCurve3d.hxx
	ShHealOper_EdgeDivide.hxx
	ShHealOper_Sewing.hxx
	ShHealOper_Tool.hxx
	ShHealOper_FillHoles.hxx
	ShHealOper_ShapeProcess.hxx
	)

set ( SalomeGeomShHeal_SRCS
	ShHealOper_ChangeOrientation.cpp
	ShHealOper_RemoveFace.cpp
	ShHealOper_SplitCurve2d.cpp
	ShHealOper_CloseContour.cpp
	ShHealOper_RemoveInternalWires.cpp
	ShHealOper_SplitCurve3d.cpp
	ShHealOper_EdgeDivide.cpp
	ShHealOper_Sewing.cpp
	ShHealOper_Tool.cpp
	ShHealOper_FillHoles.cpp
	ShHealOper_ShapeProcess.cpp
	)

add_library ( SalomeGeomShHeal SHARED ${SalomeGeomShHeal_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( SalomeGeomShHeal  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OpenCASCADE_LIBRARIES} )

install(TARGETS SalomeGeomShHeal DESTINATION lib)

