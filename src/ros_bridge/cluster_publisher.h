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

#ifndef SRC_ROS_BRIDGE_CLUSTER_PUBLISHER_H_
#define SRC_ROS_BRIDGE_CLUSTER_PUBLISHER_H_

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/header.hpp>

#ifdef AUTOWARE_PERCEPTION_MSGS_FOUND
#include <autoware_perception_msgs/msg/detected_objects.hpp>
#define CLUSTER_PUBLISHER_ENABLED
#endif

#include <memory>
#include <string>
#include <unordered_map>

#include "communication/abstract_client.h"
#include "utils/cloud.h"
#include "utils/bbox.h"

namespace depth_clustering {

#ifdef CLUSTER_PUBLISHER_ENABLED
/**
 * @brief      Publishes clusters as Autoware DetectedObjects messages
 */
class ClusterPublisher : public AbstractClient<std::unordered_map<uint16_t, Cloud>> {
 public:
  ClusterPublisher(rclcpp::Node::SharedPtr node, const std::string& topic_name = "/detection/objects");
  virtual ~ClusterPublisher() = default;

  /**
   * @brief      Called when new clusters are received
   *
   * @param[in]  clusters  The clusters
   * @param[in]  sender_id The sender identifier
   */
  void OnNewObjectReceived(const std::unordered_map<uint16_t, Cloud>& clusters,
                           const int sender_id) override;

 private:
  rclcpp::Node::SharedPtr _node;
  rclcpp::Publisher<autoware_perception_msgs::msg::DetectedObjects>::SharedPtr _publisher;
  std::string _frame_id;
};
#endif  // CLUSTER_PUBLISHER_ENABLED

}  // namespace depth_clustering

#endif  // SRC_ROS_BRIDGE_CLUSTER_PUBLISHER_H_

