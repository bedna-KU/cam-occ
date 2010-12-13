cmake_minimum_required( VERSION 2.6 )
set ( CMAKE_BUILD_TYPE Release )
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )
find_package ( OpenCASCADE REQUIRED COMPONENTS TKernel )

include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
	${OpenCASCADE_INCLUDE_DIR}
	)
add_subdirectory( src )
