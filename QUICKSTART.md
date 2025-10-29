# Quick Start Guide - ROS2 Depth Clustering

## 🚀 Fastest Way to Get Started

### Using Docker (Recommended for Testing)

**1. Allow X11 forwarding:**
```bash
xhost +local:docker
```

**2. Build and run:**
```bash
docker-compose build
docker-compose up depth_clustering
```

That's it! The node is now running and waiting for LiDAR data on `/velodyne_points`.

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

# Run
source ~/ros2_ws/install/setup.bash
ros2 run depth_clustering show_objects_node --num_beams 64 --angle 10
```

---

## 📊 Supported Sensors

| Sensor | Beams | Command |
|--------|-------|---------|
| Velodyne VLP-16 | 16 | `--num_beams 16` |
| Velodyne HDL-32 | 32 | `--num_beams 32` |
| Velodyne HDL-64 | 64 | `--num_beams 64` |

---

## 🎯 Common Use Cases

### 1. Real-time Clustering
```bash
ros2 run depth_clustering show_objects_node --num_beams 64 --angle 10
```

### 2. Save Clusters to Disk
```bash
ros2 run depth_clustering save_clusters_node --num_beams 64 --angle 10
```

### 3. Custom Topic
```bash
ros2 run depth_clustering show_objects_node --num_beams 32 --angle 8 \
    --ros-args -r /velodyne_points:=/my_lidar_topic
```

### 4. Adjust Sensitivity
- **More clusters** (higher sensitivity): `--angle 5`
- **Fewer clusters** (lower sensitivity): `--angle 15`

---

## 📖 Next Steps

- **Full documentation**: See [README_ROS2.md](README_ROS2.md)
- **Migration details**: See [MIGRATION_SUMMARY.md](MIGRATION_SUMMARY.md)
- **Troubleshooting**: Check README_ROS2.md section "Troubleshooting"

---

## 💡 Tips

- **X11 Issues?** Run: `xhost +local:docker` before Docker commands
- **No visualization?** Make sure Qt libraries are installed
- **Wrong results?** Verify `--num_beams` matches your sensor
- **Topic not found?** Check with: `ros2 topic list`

---

## ⚠️ Requirements

- **For Docker**: Docker, docker-compose, X11
- **For Native**: ROS2 Humble, Ubuntu 22.04, Qt5, PCL

---

**Ready to dive deeper?** → [README_ROS2.md](README_ROS2.md)
