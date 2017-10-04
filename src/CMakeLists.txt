cmake_minimum_required (VERSION 3.1)

project (dlp)

set(CMAKE_CXX_STANDARD 14)

find_package(FLEX 2.5 REQUIRED)
find_package(BISON 2.5 REQUIRED)
find_package(DYNLOAD REQUIRED)
find_package(LLVM REQUIRED CONFIG)

include_directories ("${PROJECT_SOURCE_DIR}/jit/include")
include_directories ("${PROJECT_SOURCE_DIR}/parser/include")
include_directories ("${PROJECT_SOURCE_DIR}/runtime/include")

add_subdirectory (parser)
add_subdirectory (runtime)
add_subdirectory (jit)
add_subdirectory (compiler)
