# ROS2 Humble Depth Clustering Docker Image
FROM ros:humble-perception

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive
ENV ROS_DISTRO=humble

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libboost-all-dev \
    libeigen3-dev \
    libopencv-dev \
    libpcl-dev \
    libqglviewer-dev-qt5 \
    qt5-default \
    qtbase5-dev \
    qtdeclarative5-dev \
    libqt5opengl5-dev \
    libqt5widgets5 \
    ros-${ROS_DISTRO}-tf2-eigen \
    ros-${ROS_DISTRO}-message-filters \
    ros-${ROS_DISTRO}-sensor-msgs \
    ros-${ROS_DISTRO}-nav-msgs \
    ros-${ROS_DISTRO}-rviz2 \
    ros-${ROS_DISTRO}-pcl-ros \
    && rm -rf /var/lib/apt/lists/*

# Create workspace
WORKDIR /ros2_ws
RUN mkdir -p /ros2_ws/src

# Copy the depth_clustering source code
COPY . /ros2_ws/src/depth_clustering/

# Source ROS2 and build
RUN /bin/bash -c "source /opt/ros/${ROS_DISTRO}/setup.bash && \
    cd /ros2_ws && \
    colcon build --cmake-args -DCMAKE_BUILD_TYPE=Release"

# Setup entrypoint
COPY docker/ros_entrypoint.sh /
RUN chmod +x /ros_entrypoint.sh

ENTRYPOINT ["/ros_entrypoint.sh"]
CMD ["bash"]
