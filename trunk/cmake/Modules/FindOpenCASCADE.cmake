# - Try to find OpenCASCADE libraries
### Does not test what version has been found,though
### that could be done by parsing Standard_Version.hxx


# Once done, this will define
#  OpenCASCADE_FOUND - true if OCC has been found
#  OpenCASCADE_INCLUDE_DIR - the OCC include dir
#  OpenCASCADE_LIBRARIES - names of OCC libraries
#  OpenCASCADE_LINK_DIRECTORY - location of OCC libraries

#CMAKE_POLICY(SET CMP0015 NEW)

#IF (UNIX)
  FIND_PATH(OpenCASCADE_INCLUDE_DIR Standard_Real.hxx /usr/include/opencascade /opt/occ/inc $ENV{CASROOT}/inc)
  # Find one lib and save its directory to OpenCASCADE_LINK_DIRECTORY. Because
  #  OCC has so many libs, there is increased risk of a name collision.
  #  Requiring that all libs be in the same directory reduces the risk.
  FIND_PATH( OpenCASCADE_LINK_DIRECTORY libTKernel.so /usr/lib /opt/occ/lib $ENV{CASROOT}/lib )
  #message("occ link dir: ${OpenCASCADE_LINK_DIRECTORY} ")
  IF( OpenCASCADE_LINK_DIRECTORY STREQUAL libTKernel.so-NOTFOUND )
    MESSAGE( FATAL_ERROR "Cannot find OCC lib dir. Install opencascade or set CASROOT or create a symlink /opt/occ/lib pointing to the dir where the OCC libs are." )
  ELSE( OpenCASCADE_LINK_DIRECTORY STREQUAL libTKernel.so-NOTFOUND )
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
  ENDIF( OpenCASCADE_LINK_DIRECTORY STREQUAL libTKernel.so-NOTFOUND )
  ADD_DEFINITIONS( -DLIN -DLININTEL )
#ELSE (UNIX)
#  IF (WIN32)

    #do windows-specific stuff here - and uncomment the commented IF-blocks

#  ENDIF (WIN32)
#ENDIF (UNIX)

IF( CMAKE_SIZEOF_VOID_P EQUAL 4 )
  #MESSAGE( STATUS "This is a 32-bit system." )
ELSE( CMAKE_SIZEOF_VOID_P EQUAL 4 )
  #MESSAGE( STATUS "This is a 64-bit system. Adding appropriate compiler flags." )
  ADD_DEFINITIONS( -D_OCC64 -m64 )
ENDIF( CMAKE_SIZEOF_VOID_P EQUAL 4 )

ADD_DEFINITIONS( -DHAVE_CONFIG_H -DHAVE_IOSTREAM -DHAVE_FSTREAM -DHAVE_LIMITS )
#LINK_DIRECTORIES( OpenCASCADE_LINK_DIRECTORY )
