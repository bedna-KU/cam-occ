cmake_minimum_required( VERSION 2.6 )
set ( CMAKE_BUILD_TYPE Release )
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )

#find OCE or OpenCASCADE
set( CASCADE_LIBS TKernel )
if( UNIX )
  set( OCE_DIR "/usr/local/share/cmake/" )
else()
  set( OCE_DIR "c:/OCE-0.3.0/share/cmake" )
endif()
find_package ( OCE )
if( ${OCE_FOUND} )
  add_definitions ( -DHAVE_CONFIG_H )
  set( OpenCASCADE_LIBRARIES ${CASCADE_LIBS} )
  set( OpenCASCADE_INCLUDE_DIR ${OCE_INCLUDE_DIRS} )
else() #look for OpenCASCADE
  set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/" )
  find_package ( OpenCASCADE REQUIRED COMPONENTS ${CASCADE_LIBS} )
endif()

include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
	${OpenCASCADE_INCLUDE_DIR}
	)
add_subdirectory( src )

IF( CMAKE_SIZEOF_VOID_P EQUAL 4 )
  set(PKG_ARCH i386)
ELSE( CMAKE_SIZEOF_VOID_P EQUAL 4 )
  set(PKG_ARCH amd64)
ENDIF( CMAKE_SIZEOF_VOID_P EQUAL 4 )

#-------------------- package generation -------------------------

set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Mark Pictor")
set(CPACK_PACKAGE_NAME "camocc2")
set(CPACK_PACKAGE_VERSION_MAJOR 0)
set(CPACK_PACKAGE_VERSION_MINOR 2)
set(CPACK_PACKAGE_VERSION_PATCH 0)
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_beta-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}_${PKG_ARCH}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "cam-occ2, gcode to solid model beta using opencascade")
set(CPACK_DEBIAN_PACKAGE_SECTION "science")
set(CPACK_DEBIAN_PACKAGE_VERSION "")

#find libs: ldd bin/camocc2 |sed -n 's/^.lib/lib/;s/\ .*$//;s/^lib/&/p'|sort|uniq
#then: apt-cache show `apt-file search libXt.so.6|grep -v -e lib32 -e -dbg -e -dev|head -n1|cut -d: -f1`|sed -n 's/Version:\ //;p'

set(CPACK_DEBIAN_PACKAGE_DEPENDS "libxt6 ")


include(CPack)

