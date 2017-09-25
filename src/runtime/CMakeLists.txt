
include_directories (${DYNLOAD_INCLUDE_DIR})
include_directories ("${PROJECT_SOURCE_DIR}/runtime/include")

file(GLOB runtime_SOURCE_FILES src/*.cpp)
file(GLOB runtime_HEADER_FILES include/*.hpp)
add_library(dlprt SHARED ${runtime_SOURCE_FILES} ${runtime_HEADER_FILES})
target_link_libraries (dlprt ${DYNLOAD_LIBRARY})

		