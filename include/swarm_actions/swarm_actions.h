/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2013, Unbounded Robotics Inc.
 *  Copyright (c) 2012, Willow Garage, Inc.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the Willow Garage nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

/* Author: Ghufran Ullah */

#include <memory>
#include <actionlib/server/simple_action_server.h>
#include <actionlib/client/simple_action_client.h>
#include <geometry_msgs/PoseStamped.h>
#include <control_toolbox/pid.h>
#include <swarm_actions/SwarmActionsAction.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>
#include <mavros_msgs/PositionTarget.h>
#include <ros/ros.h>
#include <tf/tf.h>
#include <string>

class SwarmActions
{
    typedef actionlib::SimpleActionServer<swarm_actions::SwarmActionsAction> ActionServer;
    typedef swarm_actions::SwarmActionsResult Result;
    typedef swarm_actions::SwarmActionsFeedback Feedback;
    typedef swarm_actions::SwarmActionsGoalConstPtr GoalPtr;
public:
    SwarmActions(const std::string& name); // constructor
    void init(const std::string& ns); // Initiates the action server
    void idle(); // Action server update cycle in idle state
    void executeCb(const GoalPtr &goal); // Execute callback of action server

    // ros callbacks
    void stateCb(const mavros_msgs::State::ConstPtr& msg); // mavros state callback

    void poseCb(const geometry_msgs::PoseStamped::ConstPtr& msg); // mavros pose callback


private:
    // ros
    ros::NodeHandle nh_; // node handle
    ros::Rate rate_ = {ros::Rate(20.0)};  // ros run rate
    float initial_x;
    float initial_y;
    float initial_z;
    float initial_R;
    float initial_P;
    float initial_Y;
    
    // subscribers
    ros::Subscriber local_pose_sub_; // mavros local position subscriber
    ros::Subscriber state_sub_; // mavros state subscriber 

    // publishers
    ros::Publisher local_cmd_pose_pub_; // position commands publisher

    // services
    ros::ServiceClient arming_client_; // mavros service for arming/disarming the robot
    ros::ServiceClient set_mode_client_; // mavros service for setting mode. Position commands are only available in mode OFFBOARD.
    ros::ServiceClient set_offboard_client_; // mavros service for setting mode. Position commands are only available in mode OFFBOARD.

    // action server
    ActionServer action_server_; // simple actionlib server
    Feedback feedback_; // action server feedback
    Result result_; // action server result
    std::string action_name_; // action name

    mavros_msgs::State current_state_; // latest mavros state
    geometry_msgs::PoseStamped current_pose_; // latest robot pose
    double cycle_time_;
    enum class ControlMode {
        position,
        velocity
    };
    ControlMode control_mode_ = {ControlMode::position};
};