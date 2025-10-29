#!/bin/bash
# Setup script for depth_clustering ROS2 workspace

set -e

echo "================================================"
echo "Depth Clustering ROS2 Setup Script"
echo "================================================"

# Check if ROS2 is installed
if [ ! -d "/opt/ros/humble" ]; then
    echo "ERROR: ROS2 Humble not found in /opt/ros/humble"
    echo "Please install ROS2 Humble first: https://docs.ros.org/en/humble/Installation.html"
    exit 1
fi

# Source ROS2
echo "Sourcing ROS2 Humble..."
source /opt/ros/humble/setup.bash

# Install dependencies
echo "Installing system dependencies..."
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    libboost-all-dev \
    libeigen3-dev \
    libopencv-dev \
    libpcl-dev \
    libqglviewer-dev-qt5 \
    qtbase5-dev \
    ros-humble-tf2-eigen \
    ros-humble-message-filters \
    ros-humble-sensor-msgs \
    ros-humble-nav-msgs \
    ros-humble-rviz2 \
    ros-humble-pcl-ros

# Create workspace if it doesn't exist
WORKSPACE_DIR="$HOME/ros2_ws"
if [ ! -d "$WORKSPACE_DIR" ]; then
    echo "Creating ROS2 workspace at $WORKSPACE_DIR..."
    mkdir -p "$WORKSPACE_DIR/src"
fi

# Check if we're already in a depth_clustering directory
if [ -f "package.xml" ] && grep -q "depth_clustering" package.xml; then
    CURRENT_DIR=$(pwd)
    echo "Found depth_clustering package in current directory: $CURRENT_DIR"
    
    # Create symlink if not already in workspace
    if [[ "$CURRENT_DIR" != "$WORKSPACE_DIR/src"* ]]; then
        echo "Creating symlink in workspace..."
        ln -sf "$CURRENT_DIR" "$WORKSPACE_DIR/src/depth_clustering"
    fi
fi

# Build the workspace
echo "Building workspace..."
cd "$WORKSPACE_DIR"
colcon build --symlink-install --cmake-args -DCMAKE_BUILD_TYPE=Release

# Setup completion
echo ""
echo "================================================"
echo "Setup complete!"
echo "================================================"
echo ""
echo "To use the package, source the workspace:"
echo "  source $WORKSPACE_DIR/install/setup.bash"
echo ""
echo "Then run a node, for example:"
echo "  ros2 run depth_clustering show_objects_node --num_beams 64 --angle 10"
echo ""
echo "Or use the launch file:"
echo "  ros2 launch depth_clustering depth_clustering.launch.py"
echo ""
echo "For Docker setup, see README_ROS2.md"
echo "================================================"
