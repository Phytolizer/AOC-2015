cmake_minimum_required(VERSION 3.12...3.22)

project(
    "Advent of Code 2015"
    VERSION "2015.0.0"
    LANGUAGES "C"
    HOMEPAGE_URL "https://github.com/Phytolizer/aoc-2015"
)

set(CMAKE_C_STANDARD 11)

include(FetchContent)

add_library(
    "advent"
    "advent/advent.c"
    "advent/inc/advent.h"
)
target_include_directories("advent" PUBLIC "advent/inc")

macro(copy_puzzle_input TARGET)
    add_custom_target(
        "${TARGET}_input"
        DEPENDS "${TARGET}/input.txt"
        COMMAND "${CMAKE_COMMAND}" -E make_directory
                "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}"
        COMMAND
            "${CMAKE_COMMAND}" -E copy
            "${PROJECT_SOURCE_DIR}/${TARGET}/input.txt"
            "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}/input.txt"
    )
    add_dependencies("${TARGET}" "${TARGET}_input")
endmacro()

macro(advent TARGET)
    add_executable("${TARGET}" "${TARGET}/main.c" ${ARGN})
    target_link_libraries("${TARGET}" PRIVATE "advent")
    set_target_properties("${TARGET}" PROPERTIES OUTPUT_NAME "${TARGET}_bin")
    copy_puzzle_input("${TARGET}")
endmacro()

advent("day01")
