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

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/external/cmake-modules/)

include(GNUInstallDirs)
include(MaximumWarnings)
include(ClangTidy)

find_package(PkgConfig REQUIRED)

function(build_dependencies)
  if(BUILD_TESTS)
    set(BUILD_TESTS OFF)
    if(NOT TARGET uitest)
      add_subdirectory(external/uitest/ EXCLUDE_FROM_ALL)
    endif()
  endif()

  if(NOT TARGET logmich)
    add_subdirectory(external/logmich/ EXCLUDE_FROM_ALL)
  endif()
endfunction()
build_dependencies()

file(GLOB ARCH_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  include/arch/*.hpp)

file(GLOB ARCH_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  src/util/*.cpp
  src/archive/*.cpp)

add_library(arch STATIC ${ARCH_SOURCES})
set_target_properties(arch PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF)
target_link_libraries(arch PUBLIC
  logmich)
target_include_directories(arch INTERFACE src/) # include/
target_include_directories(arch PRIVATE src/)
set_target_properties(arch PROPERTIES PUBLIC_HEADER "${ARCH_HEADERS}")

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

  # file(GLOB TEST_ARCH_SOURCES test/*_test.cpp)
  # add_executable(test_arch ${TEST_ARCH_SOURCES})
  # set_target_properties(test_arch PROPERTIES
  #   CXX_STANDARD 20
  #   CXX_STANDARD_REQUIRED ON
  #   CXX_EXTENSIONS OFF)
  # target_compile_options(test_arch PRIVATE ${WARNINGS_CXX_FLAGS})
  # target_link_libraries(test_arch
  #   arch
  #   GTest::GTest
  #   GTest::Main)

  # enable_testing()
  # add_test(NAME test_arch
  #   WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  #   COMMAND test_arch)
endif()

install(TARGETS arch
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}")

# EOF #
