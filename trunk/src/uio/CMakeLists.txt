project( OccUio )
cmake_minimum_required( VERSION 2.6 )
set ( CMAKE_BUILD_TYPE Debug )
add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )

set( CASCADE_LIBS "TKernel;TKBRep;TKTopAlgo;TKService;TKMath" )
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
	. ../qocc ${OpenCASCADE_INCLUDE_DIR} )
set ( LIBRARY_OUTPUT_PATH ../../bin )
set ( OccUio_HDRS
	dispShape.hh
	tst.hh
	log.hh
	nanotimer.hh
	)

set ( OccUio_SRCS
	uio.cc
	dispShape.cc
	tst.cc
	nanotimer.cc
	)

set ( OccUio_MOCS
	uio.hh
	tst.hh
	)
QT4_WRAP_CPP(MOCS ${OccUio_MOCS})

add_library ( OccUio SHARED ${OccUio_SRCS} ${OccUio_HDRS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( OccUio  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY}
                        ${OpenCASCADE_LIBRARIES} -lstdc++ )

install(TARGETS OccUio DESTINATION lib)
