################################################################################
# RapidJSON source dir
set( RapidJSON_SOURCE_DIR "E:/vcpkg-master/vcpkg-master/buildtrees/rapidjson/src/c44a026b53-d10e2ddcd7.clean")

################################################################################
# RapidJSON build dir
set( RapidJSON_DIR "E:/vcpkg-master/vcpkg-master/buildtrees/rapidjson/x64-windows-rel")

################################################################################
# Compute paths
get_filename_component(RapidJSON_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

set( RapidJSON_INCLUDE_DIR  "${RapidJSON_CMAKE_DIR}/../../include" )
set( RapidJSON_INCLUDE_DIRS  "${RapidJSON_CMAKE_DIR}/../../include" )
message(STATUS "RapidJSON found. Headers: ${RapidJSON_INCLUDE_DIRS}")

set(RAPIDJSON_INCLUDE_DIRS "${RapidJSON_INCLUDE_DIRS}")
