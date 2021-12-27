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
  "advent-c" "advent/advent.c" "advent/set.c" "advent/permute.c"
             "advent/string.c" "advent/combinatorics.c"
)
target_include_directories("advent-c" PUBLIC "advent/inc")

macro(advent TARGET)
  add_executable("${TARGET}-c" "${TARGET}/main.c" ${ARGN})
  set(CMAKE_TARGET "${TARGET}")
  configure_file("config.h.cin" "${PROJECT_BINARY_DIR}/${TARGET}/config.h")
  target_include_directories(
    "${TARGET}-c" PRIVATE "${PROJECT_BINARY_DIR}/${TARGET}"
  )
  target_link_libraries("${TARGET}-c" PRIVATE "advent-c")
  set_target_properties("${TARGET}-c" PROPERTIES OUTPUT_NAME "${TARGET}_bin")
endmacro()

advent("day01")
advent("day02")
target_link_libraries("day02-c" PRIVATE "pcre2-8")
advent("day03")
advent("day04")
target_link_libraries("day04-c" PRIVATE "crypto" "m")
advent("day05")
advent("day06")
target_link_libraries("day06-c" PRIVATE "pcre2-8")
advent("day07")
target_link_libraries("day07-c" PRIVATE "pcre2-8" "asan")
advent("day08")
target_link_libraries("day08-c" PRIVATE "pcre2-8")
advent("day09")
target_link_libraries("day09-c" PRIVATE "pcre2-8")
advent("day10")
advent("day11")
advent("day12")
target_link_libraries("day12-c" PRIVATE "json-c")
advent("day13")
target_link_libraries("day13-c" PRIVATE "pcre2-8")
advent("day14")
target_link_libraries("day14-c" PRIVATE "pcre2-8")
advent("day15")
target_link_libraries("day15-c" PRIVATE "pcre2-8")
advent("day16")
target_link_libraries("day16-c" PRIVATE "pcre2-8")
