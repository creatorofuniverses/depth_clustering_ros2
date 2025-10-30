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

add_library(ros_bridge SHARED 
            cloud_odom_ros_subscriber.cpp
            cluster_publisher.cpp)
target_link_libraries(ros_bridge
                      identifiable
                      bbox
                      ${rclcpp_LIBRARIES}
                      ${sensor_msgs_LIBRARIES}
                      ${nav_msgs_LIBRARIES}
                      ${message_filters_LIBRARIES}
                      ${tf2_eigen_LIBRARIES}
                      ${PCL_LIBRARIES})
ament_target_dependencies(ros_bridge 
                          rclcpp 
                          sensor_msgs 
                          nav_msgs 
                          message_filters 
                          tf2_eigen)
# Find autoware_perception_msgs if available
find_package(autoware_perception_msgs QUIET)
if(autoware_perception_msgs_FOUND)
  ament_target_dependencies(ros_bridge autoware_perception_msgs)
  target_compile_definitions(ros_bridge PRIVATE AUTOWARE_PERCEPTION_MSGS_FOUND)
  message(STATUS "autoware_perception_msgs found - cluster publishing enabled")
else()
  message(WARNING "autoware_perception_msgs not found - cluster publishing disabled")
endif()
if(ament_cmake_FOUND)
  install(TARGETS ros_bridge
          ARCHIVE DESTINATION lib
          LIBRARY DESTINATION lib
          RUNTIME DESTINATION bin)
endif()
