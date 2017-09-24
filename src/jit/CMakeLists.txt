
include_directories (${DYNLOAD_INCLUDE_DIR})
include_directories ("${PROJECT_SOURCE_DIR}/runtime/include")
include_directories ("${PROJECT_SOURCE_DIR}/parser/include")
include_directories ("${PROJECT_SOURCE_DIR}/jit/include")

# dynload might work with cmake like llvm does ...
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

file(GLOB jit_SOURCE_FILES src/*.cpp)
file(GLOB jit_HEADER_FILES include/*.hpp)
add_library(dlpjit STATIC ${jit_SOURCE_FILES} ${jit_HEADER_FILES})
