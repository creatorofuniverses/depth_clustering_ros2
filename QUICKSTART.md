# Quick Start Guide - ROS2 Depth Clustering

## 🚀 Fastest Way to Get Started

### Using Docker (Recommended for Testing)

**1. Allow X11 forwarding:**
```bash
xhost +local:docker
```

**2. Build and run (configured for Ouster OS1-64 by default):**
```bash
docker-compose build
docker-compose up depth_clustering
```

That's it! The node is now running and waiting for LiDAR data on `/ouster/points` or `/velodyne_points`.

### Test with Sample Data

**Option A: Using rosbag**
```bash
# Place your ROS2 bag in ./data/ directory
docker-compose --profile playback up rosbag_player
```

**Option B: Using visualization**
```bash
# Terminal 1: Run clustering
docker-compose up depth_clustering

# Terminal 2: Run RViz2
docker-compose --profile visualization up rviz2
```

---

## 🛠️ Native Installation (For Development)

**One-command setup:**
```bash
./scripts/setup_ros2.sh
```

**Manual setup:**
```bash
# Create workspace
mkdir -p ~/ros2_ws/src
cd ~/ros2_ws/src
git clone <repository-url> depth_clustering
cd depth_clustering
git checkout ros2-migration

# Build
cd ~/ros2_ws
source /opt/ros/humble/setup.bash
colcon build --symlink-install

# Run (Ouster OS1-64 - Default)
source ~/ros2_ws/install/setup.bash
ros2 run depth_clustering show_objects_node --lidar OS1-64 --angle 10
```

---

## 📊 Supported Sensors

| Sensor | Beams | Resolution | Command |
|--------|-------|------------|---------|
| **Ouster OS1-64** ⭐ | 64 | 1024 | `--lidar OS1-64` |
| **Ouster OS1-64 HighRes** | 64 | 2048 | `--lidar OS1-64-HIGHRES` |
| Velodyne VLP-16 | 16 | 870 | `--lidar VLP-16` |
| Velodyne HDL-32 | 32 | 870 | `--lidar HDL-32` |
| Velodyne HDL-64 | 64 | 870 | `--lidar HDL-64` |

---

## 🎯 Common Use Cases

### 1. Real-time Clustering (Ouster)
```bash
ros2 run depth_clustering show_objects_node --lidar OS1-64 --angle 10 \
    --ros-args -r /velodyne_points:=/ouster/points
```

### 2. Real-time Clustering (Velodyne)
```bash
ros2 run depth_clustering show_objects_node --lidar HDL-64 --angle 10
```

### 3. Save Clusters to Disk
```bash
ros2 run depth_clustering save_clusters_node --lidar OS1-64 --angle 10
```

### 4. High-Resolution Mode (Ouster)
```bash
ros2 run depth_clustering show_objects_node --lidar OS1-64-HIGHRES --angle 8
```

### 5. Custom Topic Remapping
```bash
ros2 run depth_clustering show_objects_node --lidar OS1-64 --angle 8 \
    --ros-args -r /velodyne_points:=/my_custom_lidar_topic
```

### 6. Adjust Sensitivity
- **More clusters** (higher sensitivity): `--angle 5` to `--angle 7`
- **Fewer clusters** (lower sensitivity): `--angle 12` to `--angle 15`
- **Default** (balanced): `--angle 10`

---

## 📖 Next Steps

- **Full documentation**: See [README_ROS2.md](README_ROS2.md)
- **Migration details**: See [MIGRATION_SUMMARY.md](MIGRATION_SUMMARY.md)
- **Troubleshooting**: Check README_ROS2.md section "Troubleshooting"

---

## 💡 Tips

- **X11 Issues?** Run: `xhost +local:docker` before Docker commands
- **No visualization?** Make sure Qt libraries are installed
- **Wrong results?** Verify `--lidar` matches your sensor type
- **Using Ouster?** Make sure to remap topic: `--ros-args -r /velodyne_points:=/ouster/points`
- **Topic not found?** Check with: `ros2 topic list`
- **Missing ring field?** Verify with: `ros2 topic echo /ouster/points --field fields`

---

## ⚠️ Requirements

- **For Docker**: Docker, docker-compose, X11
- **For Native**: ROS2 Humble, Ubuntu 22.04, Qt5, PCL

## 🎓 Sensor-Specific Guides

- **Ouster Sensors**: See [examples/ouster_examples.md](examples/ouster_examples.md)
- **Velodyne Sensors**: Works with legacy `--num_beams` argument
- **All Sensors**: See [README_ROS2.md](README_ROS2.md) for complete guide

---

**Ready to dive deeper?** → [README_ROS2.md](README_ROS2.md)
