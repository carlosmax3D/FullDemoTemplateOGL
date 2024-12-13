#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Irrlicht" for configuration "Debug"
set_property(TARGET Irrlicht APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(Irrlicht PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/debug/lib/Irrlicht.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/bin/Irrlicht.dll"
  )

list(APPEND _cmake_import_check_targets Irrlicht )
list(APPEND _cmake_import_check_files_for_Irrlicht "${_IMPORT_PREFIX}/debug/lib/Irrlicht.lib" "${_IMPORT_PREFIX}/debug/bin/Irrlicht.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
