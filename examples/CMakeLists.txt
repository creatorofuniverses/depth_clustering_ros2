# Skip Qt-dependent simple_nodes for headless ROS2 builds
# Uncomment the line below if you want to build Qt-dependent examples
# add_subdirectory(simple_nodes)
if(ROS_FOUND)
  add_subdirectory(ros_nodes)
endif()