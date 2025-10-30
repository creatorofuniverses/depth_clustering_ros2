# ROS2 Humble Depth Clustering Docker Image
FROM ros:humble-perception

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive
ENV ROS_DISTRO=humble

# Install dependencies (Qt and visualization removed for headless deployment)
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libboost-all-dev \
    libeigen3-dev \
    libopencv-dev \
    libpcl-dev \
    python3-colcon-common-extensions \
    python3-rosdep \
    python3-vcstool \
    ros-${ROS_DISTRO}-tf2-eigen \
    ros-${ROS_DISTRO}-message-filters \
    ros-${ROS_DISTRO}-sensor-msgs \
    ros-${ROS_DISTRO}-nav-msgs \
    ros-${ROS_DISTRO}-pcl-ros \
    ros-${ROS_DISTRO}-geographic-msgs \
    ros-${ROS_DISTRO}-geometry-msgs \
    ros-${ROS_DISTRO}-std-msgs \
    ros-${ROS_DISTRO}-builtin-interfaces \
    ros-${ROS_DISTRO}-unique-identifier-msgs \
    && rm -rf /var/lib/apt/lists/*

# Create workspace
WORKDIR /ros2_ws
RUN mkdir -p /ros2_ws/src

# Clone autoware_msgs (required for cluster publisher)
# Clone the repository and checkout version 1.8.0
RUN git clone https://github.com/autowarefoundation/autoware_msgs.git \
    /ros2_ws/src/autoware_msgs && \
    cd /ros2_ws/src/autoware_msgs && \
    git checkout 1.8.0 2>&1 || git checkout master

# Copy the depth_clustering source code
COPY . /ros2_ws/src/depth_clustering/

# Initialize rosdep and install dependencies for autoware_msgs
RUN /bin/bash -c "source /opt/ros/${ROS_DISTRO}/setup.bash && \
    rosdep init || true && \
    rosdep update"

# Install rosdep dependencies and build packages in two stages
# First build autoware_msgs, then build depth_clustering (which depends on it)
RUN /bin/bash -c "source /opt/ros/${ROS_DISTRO}/setup.bash && \
    cd /ros2_ws && \
    rosdep install --from-paths src/autoware_msgs --ignore-src -r -y || true && \
    colcon build --packages-select autoware_perception_msgs --cmake-args -DCMAKE_BUILD_TYPE=Release && \
    source install/setup.bash && \
    colcon build --packages-select depth_clustering --cmake-args -DCMAKE_BUILD_TYPE=Release"

# Setup entrypoint
COPY docker/ros_entrypoint.sh /
RUN chmod +x /ros_entrypoint.sh

ENTRYPOINT ["/ros_entrypoint.sh"]
CMD ["bash"]
