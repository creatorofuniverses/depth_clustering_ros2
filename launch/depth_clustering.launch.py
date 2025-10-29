#!/usr/bin/env python3
"""
ROS2 Launch file for depth_clustering nodes
"""

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, ExecuteProcess
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    """Generate launch description for depth clustering."""
    
    # Declare launch arguments
    num_beams_arg = DeclareLaunchArgument(
        'num_beams',
        default_value='64',
        description='Number of vertical beams in laser [16, 32, 64]'
    )
    
    angle_arg = DeclareLaunchArgument(
        'angle',
        default_value='10',
        description='Threshold angle in degrees for object separation'
    )
    
    topic_clouds_arg = DeclareLaunchArgument(
        'topic_clouds',
        default_value='/velodyne_points',
        description='Point cloud topic name'
    )
    
    use_rviz_arg = DeclareLaunchArgument(
        'use_rviz',
        default_value='true',
        description='Launch RViz2 for visualization'
    )

    # Show objects node
    show_objects_node = ExecuteProcess(
        cmd=[
            'ros2', 'run', 'depth_clustering', 'show_objects_node',
            '--num_beams', LaunchConfiguration('num_beams'),
            '--angle', LaunchConfiguration('angle')
        ],
        output='screen'
    )
    
    # RViz2 node (conditional)
    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        arguments=['-d', '$(find depth_clustering)/config/depth_clustering.rviz'],
        condition=launch.conditions.IfCondition(LaunchConfiguration('use_rviz'))
    )

    return LaunchDescription([
        num_beams_arg,
        angle_arg,
        topic_clouds_arg,
        use_rviz_arg,
        show_objects_node,
        # rviz_node,  # Uncomment if you want to auto-launch RViz2
    ])
