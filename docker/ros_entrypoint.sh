#!/bin/bash
set -e

# Source ROS2 setup
source /opt/ros/${ROS_DISTRO}/setup.bash

# Source workspace if built
if [ -f "/ros2_ws/install/setup.bash" ]; then
    source /ros2_ws/install/setup.bash
fi

# Execute the command passed to the script
exec "$@"
