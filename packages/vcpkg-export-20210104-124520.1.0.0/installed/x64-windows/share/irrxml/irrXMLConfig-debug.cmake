#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "irrXML::irrXML" for configuration "Debug"
set_property(TARGET irrXML::irrXML APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(irrXML::irrXML PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${CMAKE_CURRENT_LIST_DIR}/../../debug/lib/irrXML.lib"
  IMPORTED_LOCATION_DEBUG "${CMAKE_CURRENT_LIST_DIR}/../../debug/bin/irrXML.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS irrXML::irrXML )
list(APPEND _IMPORT_CHECK_FILES_FOR_irrXML::irrXML "${CMAKE_CURRENT_LIST_DIR}/../../debug/lib/irrXML.lib" "${CMAKE_CURRENT_LIST_DIR}/../../debug/bin/irrXML.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
