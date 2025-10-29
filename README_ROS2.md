# Depth Clustering - ROS2 Migration Guide

This guide covers the ROS2-compatible version of the depth_clustering package for LiDAR point cloud clustering.

## 📋 Table of Contents
- [Overview](#overview)
- [Key Changes from ROS1](#key-changes-from-ros1)
- [Quick Start with Docker](#quick-start-with-docker)
- [Building from Source](#building-from-source)
- [Running the Nodes](#running-the-nodes)
- [Visualization](#visualization)
- [Configuration](#configuration)
- [Troubleshooting](#troubleshooting)

---

## 🎯 Overview

The ROS2 migration brings modern C++ standards and improved performance to the depth clustering algorithm. This package performs fast 3D LiDAR point cloud clustering using image-based segmentation techniques.

**Key Features:**
- ✅ ROS2 Humble compatible
- ✅ Message filters for synchronized cloud/odometry
- ✅ Qt-based real-time visualization
- ✅ Ground removal capabilities
- ✅ Configurable clustering parameters
- ✅ Docker support for easy deployment

**Supported LiDAR Sensors:**
- Velodyne VLP-16 (16 beams)
- Velodyne HDL-32 (32 beams)
- Velodyne HDL-64 (64 beams)
- **Ouster OS1-64 / OS2-64 (64 beams)** ⭐ Default configuration
- Ouster OS1-64 High-Resolution mode

---

## 🔄 Key Changes from ROS1

### API Changes

| ROS1 | ROS2 |
|------|------|
| `ros::NodeHandle` | `rclcpp::Node::SharedPtr` |
| `ros::init()` | `rclcpp::init()` |
| `ros::spin()` | `rclcpp::spin()` |
| `sensor_msgs::PointCloud2::ConstPtr` | `sensor_msgs::msg::PointCloud2::ConstSharedPtr` |
| `boost::bind()` | `std::bind()` with `std::placeholders` |
| `tf::poseMsgToEigen()` | `tf2::transformToEigen()` |
| `eigen_conversions` | `tf2_eigen` |
| `catkin` | `ament_cmake` |

### Message Differences
- **Header timestamps**: `msg->header.seq` → `msg->header.stamp.sec` and `msg->header.stamp.nanosec`
- **QoS Policies**: Added support for ROS2 QoS (Quality of Service) profiles
- **Smart Pointers**: All message pointers now use `std::shared_ptr`

### Build System
- CMake now uses `ament_cmake` instead of `catkin`
- Dependencies declared in `package.xml` format 3
- No more `devel` space - uses `install` space directly

---

## 🚀 Quick Start with Docker

### Prerequisites
```bash
# Install Docker and Docker Compose
sudo apt-get update
sudo apt-get install docker.io docker-compose

# Allow X11 forwarding (for visualization)
xhost +local:docker
```

### Option 1: Using Docker Compose (Recommended)

**1. Build the Docker image:**
```bash
cd /path/to/depth_clustering
docker-compose build
```

**2. Run the clustering node:**
```bash
docker-compose up depth_clustering
```

**3. With RViz2 visualization:**
```bash
# Terminal 1: Run the clustering node
docker-compose up depth_clustering

# Terminal 2: Run RViz2
docker-compose --profile visualization up rviz2
```

**4. Play a rosbag:**
```bash
# Place your bag file in ./data/your_bag_file
docker-compose --profile playback up rosbag_player
```

### Option 2: Manual Docker Commands

**Build:**
```bash
docker build -t depth_clustering:ros2 .
```

**Run (Ouster OS1-64):**
```bash
docker run -it --rm \
  --net=host \
  --privileged \
  -e DISPLAY=$DISPLAY \
  -v /tmp/.X11-unix:/tmp/.X11-unix:rw \
  depth_clustering:ros2 \
  bash -c "source /ros2_ws/install/setup.bash && \
           ros2 run depth_clustering show_objects_node --lidar OS1-64 --angle 10"
```

**Run (Velodyne HDL-64):**
```bash
docker run -it --rm \
  --net=host \
  --privileged \
  -e DISPLAY=$DISPLAY \
  -v /tmp/.X11-unix:/tmp/.X11-unix:rw \
  depth_clustering:ros2 \
  bash -c "source /ros2_ws/install/setup.bash && \
           ros2 run depth_clustering show_objects_node --lidar HDL-64 --angle 10"
```

---

## 🛠️ Building from Source

### System Requirements
- Ubuntu 22.04 (Jammy)
- ROS2 Humble Hawksbill
- C++14 or later
- CMake 3.8+

### Dependencies

**Install ROS2 Humble:**
```bash
# Follow official ROS2 installation guide
# https://docs.ros.org/en/humble/Installation.html
```

**Install system dependencies:**
```bash
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
    qt5-default \
    qtbase5-dev \
    ros-humble-tf2-eigen \
    ros-humble-message-filters \
    ros-humble-sensor-msgs \
    ros-humble-nav-msgs \
    ros-humble-rviz2
```

### Build Instructions

**1. Create ROS2 workspace:**
```bash
mkdir -p ~/ros2_ws/src
cd ~/ros2_ws/src
```

**2. Clone the repository:**
```bash
git clone https://github.com/yourusername/depth_clustering.git
cd depth_clustering
git checkout ros2-migration
```

**3. Build with colcon:**
```bash
cd ~/ros2_ws
source /opt/ros/humble/setup.bash
colcon build --symlink-install --cmake-args -DCMAKE_BUILD_TYPE=Release
```

**4. Source the workspace:**
```bash
source ~/ros2_ws/install/setup.bash
```

---

## ▶️ Running the Nodes

### Show Objects Node (with Qt Visualization)

This node performs clustering and displays results in a Qt-based 3D viewer.

**Basic usage (Ouster OS1-64 - Default):**
```bash
ros2 run depth_clustering show_objects_node --lidar OS1-64 --angle 10
```

**Parameters:**
- `--lidar`: LiDAR type [VLP-16, HDL-32, HDL-64, OS1-64, OS1-64-HIGHRES] (default: OS1-64)
- `--num_beams`: (Legacy) Number of LiDAR beams [16, 32, 64] for Velodyne
- `--angle`: Angle threshold in degrees for object separation (default: 10)

**Examples:**

Ouster OS1-64 (standard 1024 resolution):
```bash
ros2 run depth_clustering show_objects_node --lidar OS1-64 --angle 10
```

Ouster OS1-64 High-Resolution (2048):
```bash
ros2 run depth_clustering show_objects_node --lidar OS1-64-HIGHRES --angle 8
```

Velodyne HDL-64:
```bash
ros2 run depth_clustering show_objects_node --lidar HDL-64 --angle 10
```

Velodyne VLP-16 (legacy format):
```bash
ros2 run depth_clustering show_objects_node --num_beams 16 --angle 8
```

### Save Clusters Node

This node saves detected clusters to disk for offline analysis.

**Usage (Ouster):**
```bash
ros2 run depth_clustering save_clusters_node --lidar OS1-64 --angle 10
```

**Usage (Velodyne):**
```bash
ros2 run depth_clustering save_clusters_node --lidar HDL-64 --angle 10
```

Clusters are saved to:
- `./clusters/` - Individual cluster point clouds
- `./original_cloud/` - Original input clouds

### Using Launch Files

**Start with default parameters:**
```bash
ros2 launch depth_clustering depth_clustering.launch.py
```

**With custom parameters (Ouster):**
```bash
ros2 launch depth_clustering depth_clustering.launch.py \
    lidar:=OS1-64-HIGHRES \
    angle:=8 \
    topic_clouds:=/ouster/points
```

**With custom parameters (Velodyne):**
```bash
ros2 launch depth_clustering depth_clustering.launch.py \
    lidar:=HDL-32 \
    angle:=12 \
    topic_clouds:=/velodyne_points
```

---

## 📊 Visualization

### Option 1: Built-in Qt Visualizer (Recommended)

The `show_objects_node` includes a Qt-based 3D viewer that displays:
- Original point cloud
- Detected clusters with unique colors
- Ground plane removal
- Real-time performance metrics

**Controls:**
- **Left Mouse**: Rotate view
- **Right Mouse**: Zoom
- **Middle Mouse**: Pan
- **Mouse Wheel**: Zoom in/out

### Option 2: RViz2

**Launch RViz2:**
```bash
rviz2 -d ~/ros2_ws/src/depth_clustering/config/depth_clustering.rviz
```

**Configure RViz2 manually:**
1. Add `PointCloud2` display
2. Set topic to `/velodyne_points`
3. Adjust color scheme and point size
4. Set fixed frame to `velodyne` or your sensor frame

### Option 3: Docker + RViz2
```bash
# Terminal 1: Clustering node
docker-compose up depth_clustering

# Terminal 2: RViz2
docker-compose --profile visualization up rviz2
```

---

## ⚙️ Configuration

### Topic Configuration

By default, the nodes subscribe to `/velodyne_points` (legacy) or `/ouster/points` (for Ouster drivers).

**Topic remapping for Ouster:**
```bash
ros2 run depth_clustering show_objects_node --lidar OS1-64 --angle 10 \
    --ros-args -r /velodyne_points:=/ouster/points
```

**Topic remapping for custom topics:**
```bash
ros2 run depth_clustering show_objects_node --lidar OS1-64 --angle 10 \
    --ros-args -r /velodyne_points:=/your_custom_topic
```

**Edit the source (permanent change):**
```cpp
// In show_objects_node.cpp or save_clusters_node.cpp
string topic_clouds = "/ouster/points";  // or your custom topic
```

### Clustering Parameters

Adjust in the node source files:

```cpp
// Minimum cluster size (points)
int min_cluster_size = 20;

// Maximum cluster size (points)
int max_cluster_size = 100000;

// Ground removal smooth window
int smooth_window_size = 7;

// Ground removal angle threshold
Radians ground_remove_angle = 7_deg;
```

### QoS Settings

For sensor data topics, the subscriber uses `rmw_qos_profile_sensor_data`:
- **Reliability**: Best Effort
- **Durability**: Volatile
- **History**: Keep Last

To modify, edit `cloud_odom_ros_subscriber.cpp`:
```cpp
_subscriber_clouds = std::make_shared<Subscriber<PointCloud2, rclcpp::Node>>(
    _node, _topic_clouds, rmw_qos_profile_default);  // or custom QoS
```

---

## 🔧 Troubleshooting

### Build Issues

**Error: "Could not find ament_cmake"**
```bash
sudo apt-get install ros-humble-ament-cmake
source /opt/ros/humble/setup.bash
```

**Error: "tf2_eigen not found"**
```bash
sudo apt-get install ros-humble-tf2-eigen
```

**Qt linking errors:**
```bash
sudo apt-get install qt5-default qtbase5-dev libqglviewer-dev-qt5
```

### Runtime Issues

**No point clouds received:**
1. Check topic name:
   ```bash
   ros2 topic list
   ros2 topic echo /velodyne_points
   ```
2. Verify message type:
   ```bash
   ros2 topic info /velodyne_points
   ```
3. Check QoS compatibility

**Segmentation fault:**
- Verify LiDAR data has `ring` field (required!)
  ```bash
  ros2 topic echo /ouster/points --field fields  # Check for 'ring' field
  ```
- Check point cloud is not empty
- Ensure correct `--lidar` parameter matches your sensor
- For Ouster: Verify driver is publishing ring/channel information

**Poor clustering results:**
- Adjust `--angle` parameter (lower = more sensitive)
- Modify `min_cluster_size` and `max_cluster_size`
- Tune ground removal parameters

### Docker Issues

**X11 display error:**
```bash
xhost +local:docker
export DISPLAY=:0
```

**Network issues between containers:**
- Ensure `network_mode: host` in docker-compose.yml
- Check `ROS_DOMAIN_ID` is consistent

**Permission denied errors:**
```bash
sudo usermod -aG docker $USER
# Log out and back in
```

---

## 📚 Additional Resources

### Example Datasets
- [KITTI Dataset](http://www.cvlibs.net/datasets/kitti/) - Velodyne HDL-64
- [nuScenes Dataset](https://www.nuscenes.org/) - Various sensors
- [SemanticKITTI](http://semantic-kitti.org/) - Velodyne HDL-64

### Ouster Resources
- [Ouster Examples Guide](examples/ouster_examples.md) - Detailed Ouster configuration
- [Ouster ROS2 Driver](https://github.com/ouster-lidar/ouster-ros)
- [Ouster Configuration File](config/ouster_os1_64.cfg)

### ROS2 Documentation
- [ROS2 Humble Docs](https://docs.ros.org/en/humble/)
- [Message Filters](https://github.com/ros2/message_filters)
- [TF2 Migration Guide](https://docs.ros.org/en/humble/Tutorials/Intermediate/Tf2/Tf2-Main.html)

### Original Paper
```
@inproceedings{bogoslavskyi2016depth,
  title={Fast range image-based segmentation of sparse 3D laser scans for online operation},
  author={Bogoslavskyi, Igor and Stachniss, Cyrill},
  booktitle={2016 IEEE/RSJ International Conference on Intelligent Robots and Systems (IROS)},
  pages={163--169},
  year={2016}
}
```

---

## 📝 Migration Notes

### Known Limitations
- Some CMakeLists.txt configurations may need adjustment for specific distributions
- Qt visualization window behavior may differ slightly from ROS1
- Performance characteristics may vary due to ROS2 middleware

### Future Improvements
- [ ] Add ROS2 lifecycle node support
- [ ] Implement component-based architecture
- [ ] Add dynamic parameter reconfiguration
- [ ] Create ROS2 service interfaces
- [ ] Add unit tests for ROS2 callbacks
- [ ] Publish cluster markers for RViz2

---

## 🤝 Contributing

If you encounter issues or have improvements:
1. Check existing issues on GitHub
2. Create detailed bug reports with logs
3. Submit pull requests with clear descriptions
4. Follow the existing code style

---

## 📄 License

MIT License - See LICENSE.txt for details

---

## 👥 Authors & Acknowledgments

**Original Authors:**
- Igor Bogoslavskyi
- Cyrill Stachniss

**ROS2 Migration:**
- Migrated to ROS2 Humble
- Updated to modern C++ standards
- Docker integration added

For questions or support, please open an issue on GitHub.
