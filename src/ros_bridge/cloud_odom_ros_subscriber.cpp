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

#include "ros_bridge/cloud_odom_ros_subscriber.h"
#include <tf2_eigen/tf2_eigen.hpp>

#include <vector>
#include <string>
#include <algorithm>

#include "utils/pose.h"

namespace depth_clustering {

using message_filters::Subscriber;
using message_filters::Synchronizer;
using message_filters::sync_policies::ApproximateTime;
using nav_msgs::msg::Odometry;
using sensor_msgs::msg::PointCloud2;

using std::vector;
using std::string;
using std::map;

template <class T>
T BytesTo(const vector<uint8_t>& data, uint32_t start_idx) {
  const size_t kNumberOfBytes = sizeof(T);
  uint8_t byte_array[kNumberOfBytes];
  // forward bit order (it is a HACK. We do not account for bigendianes)
  for (size_t i = 0; i < kNumberOfBytes; ++i) {
    byte_array[i] = data[start_idx + i];
  }
  T result;
  std::copy(reinterpret_cast<const uint8_t*>(&byte_array[0]),
            reinterpret_cast<const uint8_t*>(&byte_array[kNumberOfBytes]),
            reinterpret_cast<uint8_t*>(&result));
  return result;
}

void PrintMsgStats(const sensor_msgs::msg::PointCloud2::ConstSharedPtr& msg) {
  fprintf(stderr, "<<<<<<<<<<<<<<< new cloud >>>>>>>>>>>>>>>\n");
  fprintf(stderr, "received timestamp: %d.%d\n", msg->header.stamp.sec, msg->header.stamp.nanosec);
  fprintf(stderr, "height:        %d\n", msg->height);
  fprintf(stderr, "width:         %d\n", msg->width);
  fprintf(stderr, "num of fields: %lu\n", msg->fields.size());
  fprintf(stderr, "fields of each point:\n");
  for (auto const& pointField : msg->fields) {
    fprintf(stderr, "\tname:     %s\n", pointField.name.c_str());
    fprintf(stderr, "\toffset:   %d\n", pointField.offset);
    fprintf(stderr, "\tdatatype: %d\n", pointField.datatype);
    fprintf(stderr, "\tcount:    %d\n", pointField.count);
    fprintf(stderr, "\n");
  }
  fprintf(stderr, "is bigendian:  %s\n", msg->is_bigendian ? "true" : "false");
  fprintf(stderr, "point step:    %d\n", msg->point_step);
  fprintf(stderr, "row step:      %d\n", msg->row_step);
  fprintf(stderr, "data size:     %lu\n", msg->data.size() * sizeof(msg->data));
  fprintf(stderr, "is dense:      %s\n", msg->is_dense ? "true" : "false");
  fprintf(stderr, "=========================================\n");
}

CloudOdomRosSubscriber::CloudOdomRosSubscriber(rclcpp::Node::SharedPtr node,
                                               const ProjectionParams& params,
                                               const string& topic_clouds,
                                               const string& topic_odom)
    : AbstractSender{SenderType::STREAMER}, _params{params} {
  _node = node;
  _topic_clouds = topic_clouds;
  _topic_odom = topic_odom;
  _msg_queue_size = 100;

  _subscriber_clouds = nullptr;
  _subscriber_odom = nullptr;
  _sync = nullptr;
}

void CloudOdomRosSubscriber::StartListeningToRos() {
  if (!_topic_odom.empty()) {
    _subscriber_clouds = std::make_shared<Subscriber<PointCloud2, rclcpp::Node>>(
        _node, _topic_clouds, rmw_qos_profile_sensor_data);
    _subscriber_odom = std::make_shared<Subscriber<Odometry, rclcpp::Node>>(
        _node, _topic_odom, rmw_qos_profile_default);
    _sync = std::make_shared<Synchronizer<ApproximateTimePolicy>>(
        ApproximateTimePolicy(100), *_subscriber_clouds, *_subscriber_odom);
    _sync->registerCallback(
        std::bind(&CloudOdomRosSubscriber::Callback, this, 
                  std::placeholders::_1, std::placeholders::_2));
  } else {
    _subscriber_clouds = std::make_shared<Subscriber<PointCloud2, rclcpp::Node>>(
        _node, _topic_clouds, rmw_qos_profile_sensor_data);
    _subscriber_clouds->registerCallback(
        std::bind(&CloudOdomRosSubscriber::CallbackVelodyne, this, 
                  std::placeholders::_1));
  }
}

void CloudOdomRosSubscriber::Callback(const PointCloud2::ConstSharedPtr& msg_cloud,
                                      const Odometry::ConstSharedPtr& msg_odom) {
  // PrintMsgStats(msg_cloud);
  Cloud::Ptr cloud_ptr = RosCloudToCloud(msg_cloud);
  cloud_ptr->SetPose(RosOdomToPose(msg_odom));
  cloud_ptr->InitProjection(_params);
  ShareDataWithAllClients(*cloud_ptr);
}

void CloudOdomRosSubscriber::CallbackVelodyne(
    const PointCloud2::ConstSharedPtr& msg_cloud) {
  // PrintMsgStats(msg_cloud);
  Cloud::Ptr cloud_ptr = RosCloudToCloud(msg_cloud);
  cloud_ptr->InitProjection(_params);
  ShareDataWithAllClients(*cloud_ptr);
}

Pose CloudOdomRosSubscriber::RosOdomToPose(const Odometry::ConstSharedPtr& msg) {
  Pose pose;
  // we want float, so some casting is needed
  Eigen::Affine3d pose_double;
  pose_double = tf2::transformToEigen(msg->pose.pose);
  pose = pose_double.cast<float>();
  return pose;
}

Cloud::Ptr CloudOdomRosSubscriber::RosCloudToCloud(
    const PointCloud2::ConstSharedPtr& msg) {
  uint32_t x_offset = msg->fields[0].offset;
  uint32_t y_offset = msg->fields[1].offset;
  uint32_t z_offset = msg->fields[2].offset;
  uint32_t ring_offset = msg->fields[4].offset;

  Cloud cloud;
  for (uint32_t point_start_byte = 0, counter = 0;
       point_start_byte < msg->data.size();
       point_start_byte += msg->point_step, ++counter) {
    RichPoint point;
    point.x() = BytesTo<float>(msg->data, point_start_byte + x_offset);
    point.y() = BytesTo<float>(msg->data, point_start_byte + y_offset);
    point.z() = BytesTo<float>(msg->data, point_start_byte + z_offset);
    point.ring() = BytesTo<uint16_t>(msg->data, point_start_byte + ring_offset);
    // point.z *= -1;  // hack
    cloud.push_back(point);
  }

  return make_shared<Cloud>(cloud);
}

}  // namespace depth_clustering
