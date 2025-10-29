# Ouster LiDAR Examples

This document provides examples for using depth_clustering with Ouster LiDAR sensors.

## Supported Ouster Models

- **OS0-64**: 64 beams, 90° vertical FOV
- **OS1-64**: 64 beams, 33.2° vertical FOV (most common)
- **OS2-64**: 64 beams, 33.2° vertical FOV (extended range)

## Configuration

### Standard Resolution (1024 columns)
```bash
ros2 run depth_clustering show_objects_node --lidar OS1-64 --angle 10
```

### High Resolution (2048 columns)
```bash
ros2 run depth_clustering show_objects_node --lidar OS1-64-HIGHRES --angle 10
```

## Topic Configuration

Ouster ROS2 driver typically publishes on `/ouster/points`. To remap:

```bash
ros2 run depth_clustering show_objects_node \
    --lidar OS1-64 \
    --angle 10 \
    --ros-args -r /velodyne_points:=/ouster/points
```

## Launch File Examples

### Basic Launch
```bash
ros2 launch depth_clustering depth_clustering.launch.py \
    lidar:=OS1-64 \
    angle:=10 \
    topic_clouds:=/ouster/points
```

### High-Resolution Mode
```bash
ros2 launch depth_clustering depth_clustering.launch.py \
    lidar:=OS1-64-HIGHRES \
    angle:=8 \
    topic_clouds:=/ouster/points
```

## Docker Examples

### docker-compose (Default: Ouster OS1-64)
```bash
docker-compose up depth_clustering
```

### Manual Docker with Custom Settings
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

## Parameter Tuning for Ouster

### Recommended Settings

| Parameter | OS1-64 | OS1-64-HIGHRES | Notes |
|-----------|--------|----------------|-------|
| `--angle` | 8-10 | 8-10 | Lower for dense environments |
| `min_cluster_size` | 20 | 30 | Increase for high-res |
| `max_cluster_size` | 100000 | 150000 | More points with high-res |
| `ground_remove_angle` | 7° | 7° | Adjust based on mounting |

### Angle Threshold Guidelines

- **Urban/Indoor** (dense): `--angle 6` to `--angle 8`
- **Highway** (sparse): `--angle 10` to `--angle 12`
- **Off-road** (very sparse): `--angle 12` to `--angle 15`

## Integration with Ouster ROS2 Driver

### 1. Install Ouster ROS2 Driver
```bash
sudo apt install ros-humble-ouster-ros
```

### 2. Launch Ouster Driver
```bash
ros2 launch ouster_ros driver.launch.py \
    sensor_hostname:=<your_sensor_ip> \
    lidar_mode:=1024x10
```

### 3. Launch Clustering (in another terminal)
```bash
ros2 run depth_clustering show_objects_node \
    --lidar OS1-64 \
    --angle 10 \
    --ros-args -r /velodyne_points:=/ouster/points
```

## Point Cloud Format

Ouster point clouds must include:
- `x`, `y`, `z` coordinates (float32)
- `ring` field (uint16) - beam/channel ID
- Optional: `intensity`, `t` (timestamp), `reflectivity`

If your Ouster data doesn't have a `ring` field, it may not work correctly. Ensure your Ouster driver publishes the ring information.

## Performance Considerations

### Standard vs High-Resolution

| Mode | Columns | Processing Time | Memory Usage | Recommended Use |
|------|---------|----------------|--------------|-----------------|
| Standard | 1024 | ~30-50ms | Lower | Real-time applications |
| High-Res | 2048 | ~60-100ms | Higher | Precision mapping |

### Optimization Tips

1. **Use standard resolution** unless you need extra detail
2. **Adjust cluster sizes** based on your resolution
3. **Ground removal** is critical for Ouster's uniform beam distribution
4. **Consider subsampling** for very dense data

## Common Issues

### Issue: Poor clustering results
**Solution**: Ouster has uniform beam spacing unlike Velodyne. Try:
- Decreasing angle threshold: `--angle 7` or `--angle 8`
- Adjusting ground removal angle
- Checking if ring information is present

### Issue: Missing ring field
**Solution**: Ensure Ouster ROS2 driver is configured to publish ring:
```bash
ros2 topic echo /ouster/points --field fields
```
Should show a field named "ring" or "channel".

### Issue: High latency
**Solution**: 
- Use standard resolution instead of high-res
- Reduce horizontal resolution in Ouster config
- Check CPU usage

## Coordinate Frames

Ouster sensors use different coordinate frames than Velodyne:
- **X-axis**: Forward
- **Y-axis**: Left
- **Z-axis**: Up

Make sure your TF tree is correctly configured if using odometry.

## Example Configuration Files

See `/workspace/config/ouster_os1_64.cfg` for a complete configuration file that can be loaded with:

```bash
ros2 run depth_clustering show_objects_node \
    --config /workspace/config/ouster_os1_64.cfg \
    --angle 10
```

## Testing with Sample Data

If you have Ouster rosbag files:

```bash
# Terminal 1: Play bag
ros2 bag play your_ouster_bag.db3

# Terminal 2: Run clustering
ros2 run depth_clustering show_objects_node \
    --lidar OS1-64 \
    --angle 10 \
    --ros-args -r /velodyne_points:=/ouster/points
```

## Visualization

The Qt viewer should display:
- Original point cloud in grayscale
- Ground-removed points
- Colored clusters
- Real-time FPS counter

For RViz2 visualization:
```bash
rviz2 -d /workspace/config/depth_clustering.rviz
```
Update the topic to `/ouster/points` in RViz2.

## Further Reading

- [Ouster ROS2 Driver Documentation](https://github.com/ouster-lidar/ouster-ros)
- [Ouster SDK](https://github.com/ouster-lidar/ouster_example)
- [Depth Clustering Paper](https://www.ipb.uni-bonn.de/pdfs/bogoslavskyi16iros.pdf)
