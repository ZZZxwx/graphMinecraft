#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "irrXML::irrXML" for configuration "Release"
set_property(TARGET irrXML::irrXML APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(irrXML::irrXML PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${CMAKE_CURRENT_LIST_DIR}/../../lib/irrXML.lib"
  IMPORTED_LOCATION_RELEASE "${CMAKE_CURRENT_LIST_DIR}/../../bin/irrXML.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS irrXML::irrXML )
list(APPEND _IMPORT_CHECK_FILES_FOR_irrXML::irrXML "${CMAKE_CURRENT_LIST_DIR}/../../lib/irrXML.lib" "${CMAKE_CURRENT_LIST_DIR}/../../bin/irrXML.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
