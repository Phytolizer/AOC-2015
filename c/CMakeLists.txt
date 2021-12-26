cmake_minimum_required(VERSION 3.12...3.22)

project(
    "Advent of Code 2015"
    VERSION "2015.0.0"
    LANGUAGES "C"
    HOMEPAGE_URL "https://github.com/Phytolizer/aoc-2015"
)

set(CMAKE_C_STANDARD 11)

include(FetchContent)
FetchContent_Declare(
        "pcre"
        GIT_REPOSITORY "https://github.com/luvit/pcre2"
        GIT_TAG "master"
)
FetchContent_MakeAvailable("pcre")

add_library(
    "advent"
    "advent/advent.c"
    "advent/set.c"
)
target_include_directories("advent" PUBLIC "advent/inc")

macro(advent TARGET)
    add_executable("${TARGET}" "${TARGET}/main.c" ${ARGN})
    set(CMAKE_TARGET "${TARGET}")
    configure_file("config.h.cin" "${PROJECT_BINARY_DIR}/${TARGET}/config.h")
    target_include_directories("${TARGET}" PRIVATE "${PROJECT_BINARY_DIR}/${TARGET}")
    target_link_libraries("${TARGET}" PRIVATE "advent")
    set_target_properties("${TARGET}" PROPERTIES OUTPUT_NAME "${TARGET}_bin")
endmacro()

advent("day01")
advent("day02")
target_link_libraries("day02" PRIVATE "pcre2-8")
advent("day03")
advent("day04")
target_link_libraries("day04" PRIVATE "crypto" "m")
advent("day05")
advent("day06")
target_link_libraries("day06" PRIVATE "pcre2-8")
advent("day07")
target_link_libraries("day07" PRIVATE "pcre2-8" "asan")
advent("day08")
target_link_libraries("day08" PRIVATE "pcre2-8")
advent("day09")
target_link_libraries("day09" PRIVATE "pcre2-8")
advent("day10")
advent("day11")
advent("day12")
target_link_libraries("day12" PRIVATE "json-c")
