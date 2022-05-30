# arxp - Archive Explorer for C++
# Copyright (C) 2020-2022 Ingo Ruhnke <grumbel@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.6)
project(arxp VERSION 0.0.0)

set(TINYCMMC_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/external/cmake-modules/modules/")
find_package(tinycmmc CONFIG)
message(STATUS "tinycmmc module path: ${TINYCMMC_MODULE_PATH}")
list(APPEND CMAKE_MODULE_PATH ${TINYCMMC_MODULE_PATH})

option(BUILD_TESTS "Build test cases" OFF)
option(BUILD_EXTRA "Build extra stuff" OFF)

include(GNUInstallDirs)
include(MaximumWarnings)
include(ClangTidy)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS OFF)

find_package(fmt REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_search_module(LIBARCHIVE REQUIRED libarchive IMPORTED_TARGET)

function(build_dependencies)
  if(BUILD_TESTS)
    set(BUILD_TESTS OFF)

    find_package(uitest)
    if(NOT TARGET uitest)
      add_subdirectory(external/uitest/ EXCLUDE_FROM_ALL)
    endif()
  endif()

  set(BUILD_TESTS OFF)

  find_package(logmich)
  if(NOT TARGET logmich::logmich)
    add_subdirectory(external/logmich/ EXCLUDE_FROM_ALL)
  endif()

  find_package(exsp)
  if(NOT TARGET exsp::exsp)
    add_subdirectory(external/exspcpp/ EXCLUDE_FROM_ALL)
  endif()
endfunction()
build_dependencies()

file(GLOB ARXP_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  src/util/*.cpp
  src/*.cpp)

add_library(arxp STATIC ${ARXP_SOURCES})
target_link_libraries(arxp PUBLIC
  PkgConfig::LIBARCHIVE
  logmich::logmich
  exsp::exsp
  fmt::fmt)
target_include_directories(arxp PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/arxp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

if(BUILD_EXTRA)
  file(GLOB ARXP_EXTRA_SOURCES_CXX RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    extra/*.cpp)
  foreach(EXTRA_SOURCE ${ARXP_EXTRA_SOURCES_CXX})
    string(REGEX REPLACE ".*/\([^/]*\).cpp" "\\1" EXTRA_EXE ${EXTRA_SOURCE})
    set(TARGET "${EXTRA_EXE}_exe")
    add_executable(${TARGET} ${EXTRA_SOURCE})
    set_target_properties(${TARGET} PROPERTIES
      OUTPUT_NAME "${EXTRA_EXE}")
    target_compile_options(${TARGET} PRIVATE ${WARNINGS_CXX_FLAGS})
    target_link_libraries(${TARGET} fmt::fmt arxp)
  endforeach(EXTRA_SOURCE)
endif()

if(BUILD_TESTS)
  # build interactive tests
  file(GLOB UITEST_ARXP_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} uitest/*_test.cpp)
  add_executable(uitest_arxp ${UITEST_ARXP_SOURCES})
  target_compile_options(uitest_arxp PRIVATE ${WARNINGS_CXX_FLAGS})
  target_include_directories(uitest_arxp PRIVATE src/)
  target_link_libraries(uitest_arxp
    arxp
    uitest
    uitest_main)

  find_package(GTest REQUIRED)

  file(GLOB TEST_ARXP_SOURCES test/*_test.cpp)
  add_executable(test_arxp ${TEST_ARXP_SOURCES})
  target_compile_options(test_arxp PRIVATE ${WARNINGS_CXX_FLAGS})
  target_link_libraries(test_arxp
    arxp
    GTest::GTest
    GTest::Main)

  enable_testing()
  add_test(NAME test_arxp
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/test/"
    COMMAND test_arxp)
endif()

install(DIRECTORY include/
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
  FILES_MATCHING PATTERN "*.hpp")

include(ExportAndInstallLibrary)

# EOF #
