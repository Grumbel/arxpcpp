# arch - Collection of string utilities
# Copyright (C) 2020 Ingo Ruhnke <grumbel@gmail.com>
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
project(arch)

set(TINYCMMC_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/external/cmake-modules/modules/")
find_package(tinycmmc CONFIG)
message(STATUS "tinycmmc module path: ${TINYCMMC_MODULE_PATH}")
list(APPEND CMAKE_MODULE_PATH ${TINYCMMC_MODULE_PATH})

include(GNUInstallDirs)
include(MaximumWarnings)
include(ClangTidy)

find_package(PkgConfig REQUIRED)

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
  if(NOT TARGET logmich)
    add_subdirectory(external/logmich/ EXCLUDE_FROM_ALL)
  endif()
endfunction()
build_dependencies()

file(GLOB ARCH_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  src/util/*.hpp
  src/archive/*.hpp)

file(GLOB ARCH_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  src/util/*.cpp
  src/archive/*.cpp)

add_library(arch STATIC ${ARCH_SOURCES})
set_target_properties(arch PROPERTIES PUBLIC_HEADER "${ARCH_HEADERS}")
set_target_properties(arch PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF)
target_link_libraries(arch PUBLIC
  logmich)
target_include_directories(arch PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

install(TARGETS arch
  EXPORT arch-targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/archive)

# CMake integration
configure_file(arch-config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_FILES_DIRECTORY}/arch-config.cmake" @ONLY)
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_FILES_DIRECTORY}/arch-config.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/arch/")
export(TARGETS arch
  FILE "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_FILES_DIRECTORY}/arch-targets.cmake")
export(PACKAGE arch)
install(EXPORT arch-targets
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/arch/")

if(BUILD_TESTS)
  # build interactive tests
  file(GLOB UITEST_ARCH_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} uitest/*_test.cpp)
  add_executable(uitest_arch ${UITEST_ARCH_SOURCES})
  set_target_properties(uitest_arch PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)
  target_compile_options(uitest_arch PRIVATE ${WARNINGS_CXX_FLAGS})
  target_include_directories(uitest_arch PRIVATE src/)
  target_link_libraries(uitest_arch
    arch
    uitest
    uitest_main)

  find_package(GTest REQUIRED)

  file(GLOB TEST_ARCH_SOURCES test/*_test.cpp)
  add_executable(test_arch ${TEST_ARCH_SOURCES})
  set_target_properties(test_arch PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)
  target_compile_options(test_arch PRIVATE ${WARNINGS_CXX_FLAGS})
  target_link_libraries(test_arch
    arch
    GTest::GTest
    GTest::Main)

  enable_testing()
  add_test(NAME test_arch
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/test/"
    COMMAND test_arch)
endif()

install(TARGETS arch
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}")

# EOF #
