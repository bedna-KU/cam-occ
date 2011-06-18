# - Try to find OpenCASCADE libraries
### Does not test what version has been found,though
### that could be done by parsing Standard_Version.hxx


# Once done, this will define
#  OpenCASCADE_FOUND - true if OCC has been found
#  OpenCASCADE_INCLUDE_DIR - the OCC include dir
#  OpenCASCADE_LIBRARIES - names of OCC libraries
#  OpenCASCADE_LINK_DIRECTORY - location of OCC libraries

# 32 bit or 64 bit?
IF ( UNIX )
  IF( CMAKE_SIZEOF_VOID_P EQUAL 4 )
    SET( _ARCH 32 )
  ELSE()
    SET( _ARCH 64 )
    ADD_DEFINITIONS( -m64 -D_OCC64 )
  ENDIF()
ELSEIF( WIN32 ) #for some reason, the above does not work on windows
  IF( ${ENV{Platform}} STREQUAL "X64" ) #set in c:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\amd64\vcvars64.bat
    ADD_DEFINITIONS( /D_OCC64 )
    SET( _ARCH 64 )
  ELSE()
    SET( _ARCH 32 )
  ENDIF()
  MESSAGE( STATUS "This is a ${_ARCH}-bit build." )
ELSE()
  message( FATAL_ERROR "Unknown system! Exiting." )
ENDIF()



# ${OpenCASCADE_FOUND} is cached, so once OCC is found this block shouldn't have to run again
IF( NOT OpenCASCADE_FOUND STREQUAL TRUE )
  IF(UNIX)
    set( _incsearchpath /usr/include/opencascade /opt/occ/inc $ENV{CASROOT}/inc )
    if (APPLE)
      set( _testlibname libTKernel.dylib )
    else (APPLE)
      set( _testlibname libTKernel.so )
    endif (APPLE)
    set( _libsearchpath /usr/lib /opt/occ/lib $ENV{CASROOT}/lib )
  ELSEIF (WIN32)
    set( _incsearchpath $ENV{CASROOT}\\inc C:\\OpenCASCADE6.5.0\\ros\\inc )
    set( _testlibname TKernel.lib )
    set( _libsearchpath $ENV{CASROOT}\\Win${_ARCH}\\lib C:\\OpenCASCADE6.5.0\\ros\\Win${_ARCH}\\lib )
  ELSE()
    message( FATAL_ERROR "Unknown system! Exiting." )
  ENDIF()

  #find the include dir by looking for Standard_Real.hxx
  FIND_PATH( OpenCASCADE_INCLUDE_DIR Standard_Real.hxx PATHS ${_incsearchpath} DOC "Path to OCC includes" )
  IF( OpenCASCADE_INCLUDE_DIR STREQUAL Standard_Real.hxx-NOTFOUND )
    SET( OpenCASCADE_FOUND FALSE CACHE BOOL FORCE )
    MESSAGE( FATAL_ERROR "Cannot find OCC include dir. Install opencascade or set CASROOT or create a symlink /opt/occ/inc pointing to the correct directory." )
  ENDIF( OpenCASCADE_INCLUDE_DIR STREQUAL Standard_Real.hxx-NOTFOUND )

  # Find one lib and save its directory to OpenCASCADE_LINK_DIRECTORY. Because
  #  OCC has so many libs, there is increased risk of a name collision.
  #  Requiring that all libs be in the same directory reduces the risk.
  FIND_PATH( OpenCASCADE_LINK_DIRECTORY ${_testlibname} PATH ${_libsearchpath} DOC "Path to OCC libs" )
  IF( OpenCASCADE_LINK_DIRECTORY STREQUAL ${_testlibname}-NOTFOUND )
    SET( OpenCASCADE_FOUND FALSE CACHE BOOL FORCE )
    MESSAGE( FATAL_ERROR "Cannot find OCC lib dir. Install opencascade or set CASROOT or create a symlink /opt/occ/lib pointing to the dir where the OCC libs are." )
  ELSE( OpenCASCADE_LINK_DIRECTORY STREQUAL ${_testlibname}-NOTFOUND )
    SET( OpenCASCADE_FOUND TRUE CACHE BOOL "Has OCC been found?" FORCE )
    SET( _firsttime TRUE ) #so that messages are only printed once
    MESSAGE( STATUS "Found OCC include dir: ${OpenCASCADE_INCLUDE_DIR}" )
    MESSAGE( STATUS "Found OCC lib dir: ${OpenCASCADE_LINK_DIRECTORY}" )
  ENDIF( OpenCASCADE_LINK_DIRECTORY STREQUAL ${_testlibname}-NOTFOUND )
ELSE( NOT OpenCASCADE_FOUND STREQUAL TRUE )
  SET( _firsttime FALSE ) #so that messages are only printed once
ENDIF( NOT OpenCASCADE_FOUND STREQUAL TRUE )

IF( OpenCASCADE_FOUND STREQUAL TRUE )
  IF( DEFINED OpenCASCADE_FIND_COMPONENTS )
    FOREACH( _libname ${OpenCASCADE_FIND_COMPONENTS} )
      #look for libs in OpenCASCADE_LINK_DIRECTORY
      FIND_LIBRARY( OpenCASCADE_LIB_${_libname} ${_libname} ${OpenCASCADE_LINK_DIRECTORY} NO_DEFAULT_PATH)
      SET( _foundlib ${OpenCASCADE_LIB_${_libname}} )
      MARK_AS_ADVANCED( OpenCASCADE_LIB_${_libname} )
      IF( _foundlib STREQUAL OpenCASCADE_LIB_${_libname}-NOTFOUND )
        MESSAGE( FATAL_ERROR "Cannot find ${_libname}. Is it spelled correctly? Correct capitalization? Do you have another package with similarly-named libraries, installed at ${OpenCASCADE_LINK_DIRECTORY}? (That is where this script thinks the OCC libs are.)" )
      ENDIF( _foundlib STREQUAL OpenCASCADE_LIB_${_libname}-NOTFOUND )
      SET( OpenCASCADE_LIBRARIES ${OpenCASCADE_LIBRARIES} ${_foundlib} )
    ENDFOREACH( _libname ${OpenCASCADE_FIND_COMPONENTS} )

    IF (UNIX)
      ADD_DEFINITIONS( -DLIN -DLININTEL )
    ELSEIF (WIN32)
      ADD_DEFINITIONS( /DWNT )
    ENDIF()

    ADD_DEFINITIONS( -DHAVE_CONFIG_H -DHAVE_IOSTREAM -DHAVE_FSTREAM -DHAVE_LIMITS_H -DHAVE_IOMANIP )
  ELSE( DEFINED OpenCASCADE_FIND_COMPONENTS )
    MESSAGE( AUTHOR_WARNING "Developer must specify required libraries to link against in the cmake file, i.e. find_package( OpenCASCADE REQUIRED COMPONENTS TKernel TKBRep) . Otherwise no libs will be added - linking against ALL OCC libraries is slow!")
  ENDIF( DEFINED OpenCASCADE_FIND_COMPONENTS )
ENDIF( OpenCASCADE_FOUND STREQUAL TRUE )
