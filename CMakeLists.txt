## Copyright (C) 2020  I. Bogoslavskyi, C. Stachniss
##
## Permission is hereby granted, free of charge, to any person obtaining a
## copy of this software and associated documentation files (the "Software"),
## to deal in the Software without restriction, including without limitation
## the rights to use, copy, modify, merge, publish, distribute, sublicense,
## and/or sell copies of the Software, and to permit persons to whom the
## Software is furnished to do so, subject to the following conditions:
##
## The above copyright notice and this permission notice shall be included in
## all copies or substantial portions of the Software.
##
## THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
## IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
## FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
## AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
## LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
## FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
## DEALINGS IN THE SOFTWARE.

cmake_minimum_required(VERSION 3.8)
project(depth_clustering)

include(DistVersion.cmake)
system_info(DISTRO)
message(STATUS "DISTRO: ${DISTRO}")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -fPIC")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

cmake_policy(SET CMP0045 OLD)

SET(CMAKE_MODULE_PATH
      ${CMAKE_MODULE_PATH}
      "${PROJECT_SOURCE_DIR}/cmake_modules"
      "/usr/share/cmake-2.8/Modules"  ## Hack for travis
    )

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

include(CTest)
IF(IS_DIRECTORY "/usr/src/gtest/")
  MESSAGE(STATUS "Found google test sources in /usr/src/gtest/")

  ADD_SUBDIRECTORY(/usr/src/gtest/ gtest)

  # mimick the behaviour of find_package(GTest)
  SET(GTEST_FOUND TRUE)
  SET(GTEST_BOTH_LIBRARIES gtest gtest_main)
  SET(GTEST_LIBRARIES gtest)
  SET(GTEST_MAIN_LIBRARIES gtest_main)
ELSE()
  find_package(GTest)
ENDIF()

set(Boost_USE_STATIC_LIBS OFF)

option(BUILD_WITH_COVERAGE "Enable compilation with coverage information." OFF)
if(BUILD_WITH_COVERAGE)
  message(STATUS "Enabled generation of code coverage information with gcc.")
  set(CMAKE_CXX_FLAGS "-fprofile-arcs -ftest-coverage ${CMAKE_CXX_FLAGS}")
endif()

find_package(Threads REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem regex program_options)
find_package(PCL QUIET COMPONENTS common io)

# Qt and QGLViewer disabled for headless ROS2 nodes
# find_package(OpenGL REQUIRED)
# find_package(QGLViewer REQUIRED)

## Find ament_cmake and ROS2 packages
find_package(ament_cmake QUIET)

if(ament_cmake_FOUND)
  message(STATUS "ROS2 (ament_cmake) found, building ROS2 related parts")
  find_package(rclcpp REQUIRED)
  find_package(sensor_msgs REQUIRED)
  find_package(std_msgs REQUIRED)
  find_package(nav_msgs REQUIRED)
  find_package(message_filters REQUIRED)
  find_package(tf2 REQUIRED)
  find_package(tf2_ros REQUIRED)
  find_package(tf2_eigen REQUIRED)
  set(ROS_FOUND YES)
  set(ROS2_FOUND YES)
else()
  message(WARNING "ROS2 (ament_cmake) NOT FOUND. NOT building ROS related parts!")
  set(ROS_FOUND NO)
  set(ROS2_FOUND NO)
endif()

# add some useful constants to configuration
configure_file(config/cmake_config.h.in
               ${PROJECT_SOURCE_DIR}/config/cmake_config.h)
include_directories(config)


if (${PCL_FOUND})
  message(STATUS "PCL is found. PCL-related code will be built.")
  add_definitions(-DPCL_FOUND)
else()
  message(WARNING "PCL NOT FOUND! PCL-related code WILL NOT be built")
endif()

include_directories(
  src
  ${PROJECT_BINARY_DIR}/src
  SYSTEM ${EIGEN3_INCLUDE_DIR}
  SYSTEM ${Boost_INCLUDE_DIRS}
  SYSTEM ${PCL_INCLUDE_DIRS}
  SYSTEM ${OpenCV_INCLUDE_DIRS}
)

add_subdirectory(src)
add_subdirectory(examples)

message(STATUS "BUILD_TESTING: ${BUILD_TESTING}")
if (${BUILD_TESTING})
  add_subdirectory(test)
endif()

# ROS2 ament_cmake package configuration
if(ament_cmake_FOUND)
  ament_package()
endif()
