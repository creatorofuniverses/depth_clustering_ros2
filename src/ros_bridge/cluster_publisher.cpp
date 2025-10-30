// Copyright (C) 2020  I. Bogoslavskyi, C. Stachniss
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#include "ros_bridge/cluster_publisher.h"

#ifdef CLUSTER_PUBLISHER_ENABLED

#include <autoware_perception_msgs/msg/detected_object.hpp>
#include <autoware_perception_msgs/msg/detected_object_kinematics.hpp>
#include <autoware_perception_msgs/msg/shape.hpp>
#include <autoware_perception_msgs/msg/object_classification.hpp>
#include <geometry_msgs/msg/pose_with_covariance.hpp>
#include <geometry_msgs/msg/vector3.hpp>

#include "utils/bbox.h"

namespace depth_clustering {

ClusterPublisher::ClusterPublisher(rclcpp::Node::SharedPtr node, const std::string& topic_name)
    : _node(node) {
  _publisher = _node->create_publisher<autoware_perception_msgs::msg::DetectedObjects>(
      topic_name, 10);
  _frame_id = "base_link";  // Default frame, can be overridden via parameter
  
  // Try to get frame_id from parameter
  _node->declare_parameter<std::string>("frame_id", "base_link");
  _frame_id = _node->get_parameter("frame_id").as_string();
  
  fprintf(stderr, "INFO: ClusterPublisher initialized, publishing to: %s\n", topic_name.c_str());
}

void ClusterPublisher::OnNewObjectReceived(
    const std::unordered_map<uint16_t, Cloud>& clusters, const int) {
  autoware_perception_msgs::msg::DetectedObjects msg;
  
  // Set header
  msg.header.stamp = _node->now();
  msg.header.frame_id = _frame_id;
  
  // Convert each cluster to a DetectedObject
  for (const auto& kv : clusters) {
    const auto& cluster = kv.second;
    if (cluster.empty()) {
      continue;
    }
    
    autoware_perception_msgs::msg::DetectedObject detected_obj;
    
    // Compute bounding box
    Bbox bbox(cluster);
    const auto& center = bbox.center();
    const auto& scale = bbox.scale();
    
    // Set existence probability (default to 1.0 for detected objects)
    detected_obj.existence_probability = 1.0f;
    
    // Set classification (UNKNOWN by default)
    autoware_perception_msgs::msg::ObjectClassification classification;
    classification.label = autoware_perception_msgs::msg::ObjectClassification::UNKNOWN;
    classification.probability = 1.0f;
    detected_obj.classification.push_back(classification);
    
    // Set kinematics
    autoware_perception_msgs::msg::DetectedObjectKinematics kinematics;
    kinematics.has_position_covariance = false;
    kinematics.orientation_availability = 
        autoware_perception_msgs::msg::DetectedObjectKinematics::UNAVAILABLE;
    kinematics.has_twist = false;
    kinematics.has_twist_covariance = false;
    
    // Set pose (center of bounding box)
    kinematics.pose_with_covariance.pose.position.x = center.x();
    kinematics.pose_with_covariance.pose.position.y = center.y();
    kinematics.pose_with_covariance.pose.position.z = center.z();
    // Orientation is identity (quaternion default)
    kinematics.pose_with_covariance.pose.orientation.w = 1.0;
    
    detected_obj.kinematics = kinematics;
    
    // Set shape (bounding box)
    autoware_perception_msgs::msg::Shape shape;
    shape.type = autoware_perception_msgs::msg::Shape::BOUNDING_BOX;
    shape.dimensions.x = scale.x();
    shape.dimensions.y = scale.y();
    shape.dimensions.z = scale.z();
    
    // Create a simple footprint polygon (4 corners of the box)
    geometry_msgs::msg::Point32 point;
    const auto& min_pt = bbox.min_point();
    const auto& max_pt = bbox.max_point();
    
    // Bottom corners at z = min
    point.x = min_pt.x(); point.y = min_pt.y(); point.z = min_pt.z();
    shape.footprint.points.push_back(point);
    point.x = max_pt.x(); point.y = min_pt.y(); point.z = min_pt.z();
    shape.footprint.points.push_back(point);
    point.x = max_pt.x(); point.y = max_pt.y(); point.z = min_pt.z();
    shape.footprint.points.push_back(point);
    point.x = min_pt.x(); point.y = max_pt.y(); point.z = min_pt.z();
    shape.footprint.points.push_back(point);
    
    detected_obj.shape = shape;
    
    msg.objects.push_back(detected_obj);
  }
  
  // Publish the message
  _publisher->publish(msg);
  
  fprintf(stderr, "INFO: Published %zu detected objects\n", msg.objects.size());
}

}  // namespace depth_clustering

#endif  // CLUSTER_PUBLISHER_ENABLED

