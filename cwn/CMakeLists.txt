project(
  "Advent of Code 2015 (CWN)"
  LANGUAGES "CXX"
  VERSION "2015.0.0"
  DESCRIPTION "C with namespaces"
)

function(workaround_file_extension)
  set_source_files_properties(${ARGN} PROPERTIES LANGUAGE CXX)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set_source_files_properties(${ARGN} PROPERTIES COMPILE_FLAGS "-x c++")
  endif()
endfunction()

function(advent TARGET)
  cmake_parse_arguments(PARSE_ARGV 0 "ADVENT" "" "" "SOURCES;LIBRARIES;INCDIRS")
  list(PREPEND ADVENT_SOURCES "main.cwn")
  string(REGEX REPLACE "-cwn$" "" TARGET_FOLDER "${TARGET}")
  list(TRANSFORM ADVENT_SOURCES PREPEND "${TARGET_FOLDER}/src/")
  add_executable("${TARGET}" ${ADVENT_SOURCES})
  workaround_file_extension(${ADVENT_SOURCES})
  target_link_libraries("${TARGET}" PRIVATE "advent-cwn" ${ADVENT_LIBRARIES})
  set(CMAKE_TARGET "${TARGET_FOLDER}")
  configure_file(
    "config.hwn.cin" "${PROJECT_BINARY_DIR}/${TARGET_FOLDER}/config.hwn"
  )
  target_include_directories(
    "${TARGET}"
    PRIVATE "${TARGET_FOLDER}/inc" "${PROJECT_BINARY_DIR}/${TARGET_FOLDER}"
            ${ADVENT_INCDIRS}
  )
  set_target_properties("${TARGET}" PROPERTIES LINKER_LANGUAGE "CXX")
endfunction()

set(ADVENT_LIB_SOURCES "advent/src/advent.cwn" "advent/src/set.cwn")
add_library("advent-cwn" ${ADVENT_LIB_SOURCES})
target_include_directories("advent-cwn" PUBLIC "advent/inc")
target_compile_features("advent-cwn" PUBLIC "cxx_std_20")
workaround_file_extension(${ADVENT_LIB_SOURCES})

advent(day01-cwn)
advent(day02-cwn LIBRARIES pcre2-8)
advent(day03-cwn)
advent(day04-cwn LIBRARIES crypto)
advent(day05-cwn)
