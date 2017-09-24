
set(LEXER_SOURCE_FILE "${PROJECT_SOURCE_DIR}/parser/src/lexer.l")
set(PARSER_SOURCE_FILE "${PROJECT_SOURCE_DIR}/parser/src/parser.y")

if (WIN32)
	set(FLEX_EXTRA_ARGS "--wincompat")
endif()

FLEX_TARGET(LEXER ${LEXER_SOURCE_FILE} ${CMAKE_CURRENT_BINARY_DIR}/src/lexer.cpp
	COMPILE_FLAGS "${FLEX_EXTRA_ARGS}"
	DEFINES_FILE "${CMAKE_CURRENT_BINARY_DIR}/src/lexer.hpp")

BISON_TARGET(PARSER ${PARSER_SOURCE_FILE} ${CMAKE_CURRENT_BINARY_DIR}/src/parser.cpp
	DEFINES_FILE "${CMAKE_CURRENT_BINARY_DIR}/src/parser.hpp")
ADD_FLEX_BISON_DEPENDENCY(LEXER PARSER)

include_directories ("${FLEX_INCLUDE_DIRS}")
include_directories ("${CMAKE_CURRENT_BINARY_DIR}/src")
include_directories ("${PROJECT_SOURCE_DIR}/parser/include")

file(GLOB parser_SOURCE_FILES src/*.cpp)
file(GLOB parser_HEADER_FILES include/*.hpp include/*.h)
add_library(dlpparser STATIC
	${parser_SOURCE_FILES} ${parser_HEADER_FILES}
	${BISON_PARSER_OUTPUTS} ${PARSER_SOURCE_FILE}
	${FLEX_LEXER_OUTPUTS} ${LEXER_SOURCE_FILE})
