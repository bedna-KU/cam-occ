# - Try to find OpenCASCADE libraries
### Does not test what version has been found,though
### that could be done by parsing Standard_Version.hxx


# Once done, this will define
#  OpenCASCADE_FOUND - true if OCC has been found
#  OpenCASCADE_INCLUDE_DIR - the OCC include dir
#  OpenCASCADE_LIBRARIES - names of OCC libraries
#  OpenCASCADE_LINK_DIRECTORY - location of OCC libraries

IF (UNIX)
  set( focc_incsearchpath /usr/include/opencascade /opt/occ/inc $ENV{CASROOT}/inc )
  set( focc_testlibname libTKernel.so )
  set( focc_libsearchpath /usr/lib /opt/occ/lib $ENV{CASROOT}/lib )
ELSEIF (UNIX)
  IF (WIN32)
    MESSAGE ( "************ This has not been tried on windows and may or may not work! *************" )
    set( focc_incsearchpath $ENV{CASROOT}\\inc C:\\OpenCASCADE6.3.0\\ros\\inc )
    set( focc_testlibname TKernel.dll )
    set( focc_libsearchpath $ENV{CASROOT}\\win32\\bin C:\\OpenCASCADE6.3.0\\ros\\win32\\bin )
  ENDIF (WIN32)
ENDIF (UNIX)

FIND_PATH(OpenCASCADE_INCLUDE_DIR Standard_Real.hxx ${focc_incsearchpath} )
# Find one lib and save its directory to OpenCASCADE_LINK_DIRECTORY. Because
#  OCC has so many libs, there is increased risk of a name collision.
#  Requiring that all libs be in the same directory reduces the risk.
FIND_PATH( OpenCASCADE_LINK_DIRECTORY ${focc_testlibname} ${focc_libsearchpath} )
#message("occ link dir: ${OpenCASCADE_LINK_DIRECTORY} ")
IF( OpenCASCADE_LINK_DIRECTORY STREQUAL ${focc_testlibname}-NOTFOUND )
  MESSAGE( FATAL_ERROR "Cannot find OCC lib dir. Install opencascade or set CASROOT or create a symlink /opt/occ/lib pointing to the dir where the OCC libs are." )
ELSE( OpenCASCADE_LINK_DIRECTORY STREQUAL ${focc_testlibname}-NOTFOUND )
  IF( OpenCASCADE_FIND_COMPONENTS )

    FOREACH( _libname ${OpenCASCADE_FIND_COMPONENTS} )
      #look for libs in OpenCASCADE_LINK_DIRECTORY
      FIND_LIBRARY( ${_libname}_OCCLIB ${_libname} ${OpenCASCADE_LINK_DIRECTORY} NO_DEFAULT_PATH)
      SET( _foundlib ${${_libname}_OCCLIB} )
      #message("findlib: ${_libname} -- ${_foundlib} ")
      IF ( _foundlib STREQUAL ${_libname}_OCCLIB-NOTFOUND )
        MESSAGE ( FATAL_ERROR "Cannot find ${_libname}. Is it spelled correctly? Same capitalization? Do you have another package with similarly-named libraries, installed at ${OpenCASCADE_LINK_DIRECTORY}? (That is where this script thinks the OCC libs are.)" )
      ENDIF ( _foundlib STREQUAL ${_libname}_OCCLIB-NOTFOUND )
      SET ( OpenCASCADE_LIBRARIES ${OpenCASCADE_LIBRARIES} ${_foundlib}   )
    ENDFOREACH( _libname ${OpenCASCADE_FIND_COMPONENTS} )

  ELSE( OpenCASCADE_FIND_COMPONENTS )
    MESSAGE( AUTHOR_WARNING "Developer must specify required libraries to link against in the cmake file, i.e. find_package( OpenCASCADE REQUIRED COMPONENTS TKernel TKBRep) . Otherwise no libs will be added - linking against ALL OCC libraries is slow!")
  ENDIF( OpenCASCADE_FIND_COMPONENTS )
  SET ( OpenCASCADE_FOUND TRUE )
ENDIF( OpenCASCADE_LINK_DIRECTORY STREQUAL ${focc_testlibname}-NOTFOUND )
ADD_DEFINITIONS( -DLIN -DLININTEL )

IF( CMAKE_SIZEOF_VOID_P EQUAL 4 )
  #MESSAGE( STATUS "This is a 32-bit system." )
ELSE( CMAKE_SIZEOF_VOID_P EQUAL 4 )
  #MESSAGE( STATUS "This is a 64-bit system. Adding appropriate compiler flags." )
  ADD_DEFINITIONS( -D_OCC64 -m64 )
ENDIF( CMAKE_SIZEOF_VOID_P EQUAL 4 )

ADD_DEFINITIONS( -DHAVE_CONFIG_H -DHAVE_IOSTREAM -DHAVE_FSTREAM -DHAVE_LIMITS )
#LINK_DIRECTORIES( OpenCASCADE_LINK_DIRECTORY )
