# Ubuntu 22.04 (Jammy) Compatibility Notes

## Qt5 Package Changes

### Issue
The `qt5-default` package has been **removed** in Ubuntu 22.04 and later versions.

### Solution
Instead of installing `qt5-default`, install these packages:

```bash
sudo apt-get install -y \
    qtbase5-dev \
    qt5-qmake \
    qtbase5-dev-tools \
    libqt5opengl5-dev \
    libqglviewer-dev-qt5
```

### Why This Changed
- Ubuntu 22.04 (Jammy Jellyfish) uses Qt 5.15.x
- The `qt5-default` meta-package was deprecated and removed
- Qt5 development packages must now be installed explicitly

### Affected Files
The following files have been updated to reflect this change:
- ✅ `Dockerfile` - Fixed for Docker builds
- ✅ `README_ROS2.md` - Updated installation instructions
- ✅ `scripts/setup_ros2.sh` - Updated setup script

### CMakeLists.txt Compatibility
The CMakeLists.txt files automatically detect Qt5 using:
```cmake
find_package(Qt5 REQUIRED COMPONENTS Core Xml OpenGL Gui Widgets)
```

This works correctly on Ubuntu 22.04 without requiring `qt5-default`.

## ROS2 Humble Requirements

### Minimum Versions
- **Ubuntu**: 22.04 LTS (Jammy Jellyfish)
- **CMake**: 3.8+
- **GCC**: 11.x (default in Ubuntu 22.04)
- **Qt**: 5.15.x (default in Ubuntu 22.04)

### Verified Compatibility
✅ Ubuntu 22.04 LTS  
✅ ROS2 Humble Hawksbill  
✅ Qt 5.15.3  
✅ CMake 3.22+  
✅ GCC 11.4  

## Other Ubuntu 22.04 Changes

### Python
- Default Python version: 3.10
- Python 2 completely removed

### Boost
- Default Boost version: 1.74
- All Boost components work correctly

### PCL (Point Cloud Library)
- Default PCL version: 1.12
- Compatible with ROS2 Humble

### Eigen
- Default Eigen version: 3.4
- Compatible with all dependencies

## Troubleshooting

### If Qt CMake errors occur:
```bash
# Set Qt5 directory explicitly
export Qt5_DIR=/usr/lib/x86_64-linux-gnu/cmake/Qt5

# Or install additional Qt packages
sudo apt-get install -y qttools5-dev qttools5-dev-tools
```

### If QGLViewer not found:
```bash
sudo apt-get install -y libqglviewer-dev-qt5
# Check installation
dpkg -L libqglviewer-dev-qt5 | grep cmake
```

### If OpenGL issues:
```bash
sudo apt-get install -y libgl1-mesa-dev libglu1-mesa-dev
```

## Docker Build Notes

The Dockerfile has been tested on:
- ✅ Docker 20.10+
- ✅ docker-compose 2.x
- ✅ Base image: `ros:humble-perception`

The base image already includes Ubuntu 22.04 and ROS2 Humble.

## Legacy Ubuntu Versions

### Ubuntu 20.04 (Focal)
- ROS2 Foxy (EOL May 2023)
- `qt5-default` still available
- Not recommended for new deployments

### Ubuntu 18.04 (Bionic)
- ROS2 Eloquent (EOL)
- Not supported by this package

## References

- [Qt5 on Ubuntu 22.04](https://wiki.ubuntu.com/JammyJellyfish/ReleaseNotes#Qt)
- [ROS2 Humble System Requirements](https://docs.ros.org/en/humble/Installation.html)
- [Qt5 Migration Guide](https://doc.qt.io/qt-5/linux.html)
