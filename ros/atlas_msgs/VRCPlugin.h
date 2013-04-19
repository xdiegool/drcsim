/*
 * Copyright 2012 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#ifndef GAZEBO_VRC_PLUGIN_HH
#define GAZEBO_VRC_PLUGIN_HH

#include <map>
#include <string>
#include <vector>

#include <ros/ros.h>
#include <ros/callback_queue.h>
#include <ros/advertise_options.h>
#include <ros/subscribe_options.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Pose.h>
#include <std_msgs/String.h>
#include <sensor_msgs/JointState.h>

#include <atlas_msgs/AtlasCommand.h>
#include <atlas_msgs/AtlasSimInterfaceCommand.h>

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

#include <gazebo/math/Vector3.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/transport/TransportTypes.hh>
#include <gazebo/common/Time.hh>
#include <gazebo/common/Plugin.hh>
#include <gazebo/common/Events.hh>

namespace gazebo
{
  class VRCPlugin : public WorldPlugin
  {
    /// \brief Constructor
    public: VRCPlugin();

    /// \brief Destructor
    public: virtual ~VRCPlugin();

    /// \brief Load the plugin.
    /// \param[in] _parent Pointer to parent world.
    /// \param[in] _sdf Pointer to sdf element.
    public: void Load(physics::WorldPtr _parent, sdf::ElementPtr _sdf);

    /// \brief Update the controller on every World::Update
    private: void UpdateStates();

    ////////////////////////////////////////////////////////////////////////////
    //                                                                        //
    //   List of available actions                                            //
    //                                                                        //
    ////////////////////////////////////////////////////////////////////////////
    /// \brief Sets Atlas planar navigational command velocity
    /// \param[in] _cmd A Vector3, where:
    ///   - x is the desired forward linear velocity, positive is robot-forward
    ///     and negative is robot-back.
    ///   - y is the desired lateral linear velocity, positive is robot-left
    ///     and negative is robot-right.
    ///   - z is the desired heading angular velocity, positive makes
    ///     the robot turn left, and negative makes the robot turn right
    public: void SetRobotCmdVel(const geometry_msgs::Twist::ConstPtr &_cmd);

    /// \brief sets robot's absolute world pose
    /// \param[in] _cmd Pose command for the robot
    public: void SetRobotPose(const geometry_msgs::Pose::ConstPtr &_cmd);

    /// \brief sets robot's joint positions
    /// \param[in] _cmd configuration made of sensor_msgs::JointState message
    /// \todo: not yet implemented
    public: void SetRobotConfiguration(const sensor_msgs::JointState::ConstPtr
                                       &/*_cmd*/);

    /// \brief sets robot mode via ros topic
    /// \sa SetRobotMode(const std::string &_str)
    public: void SetRobotModeTopic(const std_msgs::String::ConstPtr &_str);

    /// \brief sets robot mode
    /// \param[in] _str sets robot mode by a string.  Supported modes are:
    ///  - "no_gravity" Gravity disabled for the robot.
    ///  - "nominal" Nominal "normal" physics.
    ///  - "pinned" Robot is pinned to inertial world by the pelvis.
    ///  - "feet" same as no_gravity except for r_foot and l_foot links.
    public: void SetRobotMode(const std::string &_str);


    /// \brief Robot Vehicle Interaction, put robot in driver's seat.
    /// \param[in] _pose Relative pose offset, Pose()::Zero provides default
    ///                 behavior.
    public: void RobotEnterCar(const geometry_msgs::Pose::ConstPtr &_pose);

    /// \brief Robot Vehicle Interaction, put robot outside driver's side door.
    /// \param[in] _pose Relative pose offset, Pose()::Zero provides default
    ///                 behavior.
    public: void RobotExitCar(const geometry_msgs::Pose::ConstPtr &_pose);

    /// \brief Cheats to teleport fire hose to hand and make a fixed joint
    /// \param[in] _cmd Relative pose offset between the link and the hand.
    public: void RobotGrabFireHose(const geometry_msgs::Pose::ConstPtr &_cmd);

    /// \brief remove the fixed joint between robot hand link and fire hose.
    /// \param[in] _cmd not used.
    public: void RobotReleaseLink(const geometry_msgs::Pose::ConstPtr &_cmd);


    ////////////////////////////////////////////////////////////////////////////
    //                                                                        //
    //   Generic tools for manipulating models                                //
    //                                                                        //
    ////////////////////////////////////////////////////////////////////////////
    /// \brief sets robot's absolute world pose
    /// \param[in] _pinLink Link to pin to the world
    /// \param[in] _pose sets the _pinLink world pose before pinning
    /// \param[in] _jp joint and positions for model configuration (TODO)
    /// \param[out] _pinJoint a pin Joint is created
    private: void Teleport(const physics::LinkPtr &_pinLink,
                          physics::JointPtr &_pinJoint,
                          const math::Pose &_pose,
                          const std::map<std::string, double> &/*_jp*/);

    /// \brief sets robot's absolute world pose
    /// \param[in] _pinLink Link to pin to the world
    /// \param[in] _pose sets the _pinLink world pose before pinning
    /// \param[out] _pinJoint a pin Joint is created
    private: void Teleport(const physics::LinkPtr &_pinLink,
                          physics::JointPtr &_pinJoint,
                          const math::Pose &_pose);

    /// \brief add a constraint between 2 links
    /// \param[in] _world a pointer to the current World
    /// \param[in] _model a pointer to the Model the new Joint will be under
    /// \param[in] _link1 parent link in the new Joint
    /// \param[in] _link2 child link in the new Joint
    /// \param[in] _type string specifying joint type
    /// \param[in] _anchor a Vector3 anchor offset of the new joint
    /// \param[in] _axis Vector3 containing xyz axis of the new joint
    /// \param[in] _upper upper linit of the new joint
    /// \param[in] _lower lower linit of the new joint
    /// \return Joint created between _link1 and _link2 under _model.
    private: physics::JointPtr AddJoint(physics::WorldPtr _world,
                                        physics::ModelPtr _model,
                                        physics::LinkPtr _link1,
                                        physics::LinkPtr _link2,
                                        std::string _type,
                                        math::Vector3 _anchor,
                                        math::Vector3 _axis,
                                        double _upper, double _lower);

    /// \brief Remove a joint.
    /// \param[in] _joint Joint to remove.
    private: void RemoveJoint(physics::JointPtr &_joint);

    /// \brief setup Robot ROS publication and sbuscriptions for the Robot
    /// These ros api describes Robot only actions
    private: void LoadRobotROSAPI();

    /// \brief setup ROS publication and sbuscriptions for VRC
    /// These ros api describes interactions between different models
    /// /atlas/cmd_vel - in pinned mode, the robot teleports based on
    ///                      messages from the cmd_vel
    private: void LoadVRCROSAPI();

    /// \brief check and spawn screw joint to simulate threads
    /// if links are aligned
    private: void CheckThreadStart();

    /// \brief: thread out Load function with
    /// with anything that might be blocking.
    private: void DeferredLoad();

    /// \brief ROS callback queue thread
    private: void ROSQueueThread();

    ////////////////////////////////////////////////////////////////////////////
    //                                                                        //
    //   Atlas properties and states                                          //
    //                                                                        //
    ////////////////////////////////////////////////////////////////////////////
    private: class Robot
    {
      /// \brief Load the atlas portion of plugin.
      /// \param[in] _parent Pointer to parent world.
      /// \param[in] _sdf Pointer to sdf element.
      private: void Load(physics::WorldPtr _parent, sdf::ElementPtr _sdf);

      private: physics::ModelPtr model;
      private: physics::LinkPtr pinLink;
      private: physics::JointPtr pinJoint;

      /// \brief keep initial pose of robot to prevent z-drifting when
      /// teleporting the robot.
      private: math::Pose initialPose;

      /// \brief Pose of robot relative to vehicle.
      private: math::Pose vehicleRelPose;

      /// \brief Robot configuration when inside of vehicle.
      private: std::map<std::string, double> inVehicleConfiguration;

      /// \brief Flag to keep track of start-up 'harness' on the robot.
      private: bool startupHarness;

      /// \brief Duration in StandPrep before going into Stand
      private: double startupStandPrepDuration;

      /// \brief Flag to keep track of start-up 'bdi_stand' on the robot.
      private: bool startupBDIStand;

      /// \brief allow user to set startup mode as bdi_stand or pinned
      private: std::string startupMode;

      /// \brief flag for successful initialization of atlas
      private: bool isInitialized;

      private: double startupHarnessDuration;

      private: ros::Subscriber subTrajectory;
      private: ros::Subscriber subPose;
      private: ros::Subscriber subConfiguration;
      private: ros::Subscriber subMode;

      friend class VRCPlugin;
    } atlas;

    ////////////////////////////////////////////////////////////////////////////
    //                                                                        //
    //   DRC Vehicle properties and states                                    //
    //                                                                        //
    ////////////////////////////////////////////////////////////////////////////
    private: class Vehicle
    {
      /// \brief Load the drc vehicle portion of plugin.
      /// \param[in] _parent Pointer to parent world.
      /// \param[in] _sdf Pointer to sdf element.
      private: void Load(physics::WorldPtr _parent, sdf::ElementPtr _sdf);

      private: physics::ModelPtr model;
      private: math::Pose initialPose;
      private: physics::LinkPtr seatLink;

      /// \brief flag for successful initialization of vehicle
      private: bool isInitialized;

      friend class VRCPlugin;
    } drcVehicle;

    ////////////////////////////////////////////////////////////////////////////
    //                                                                        //
    //   DRC Fire Hose (and Standpipe)                                        //
    //                                                                        //
    ////////////////////////////////////////////////////////////////////////////
    private: class FireHose
    {
      /// \brief set initial configuration of the fire hose link
      private: void SetInitialConfiguration()
      {
        // for (unsigned int i = 0; i < this->fireHoseJoints.size(); ++i)
        //   gzerr << "joint [" << this->fireHoseJoints[i]->GetName() << "]\n";
        // for (unsigned int i = 0; i < this->links.size(); ++i)
        //   gzerr << "link [" << this->links[i]->GetName() << "]\n";
        this->fireHoseModel->SetWorldPose(this->initialFireHosePose);
        this->fireHoseJoints[fireHoseJoints.size()-4]->SetAngle(0, -M_PI/4.0);
        this->fireHoseJoints[fireHoseJoints.size()-2]->SetAngle(0, -M_PI/4.0);
      }

      /// \brief Load the drc_fire_hose portion of plugin.
      /// \param[in] _parent Pointer to parent world.
      /// \param[in] _sdf Pointer to sdf element.
      private: void Load(physics::WorldPtr _parent, sdf::ElementPtr _sdf);

      private: physics::ModelPtr fireHoseModel;
      private: physics::ModelPtr standpipeModel;

      /// joint for pinning a link to the world
      private: physics::JointPtr fixedJoint;

      /// joints and links
      private: physics::Joint_V fireHoseJoints;
      private: physics::Link_V fireHoseLinks;
      /// screw joint
      private: physics::JointPtr screwJoint;
      private: double threadPitch;

      /// Pointer to the update event connection
      private: event::ConnectionPtr updateConnection;

      private: physics::LinkPtr couplingLink;
      private: physics::LinkPtr spoutLink;
      private: math::Pose couplingRelativePose;
      private: math::Pose initialFireHosePose;

      /// \brief flag for successful initialization of fire hose, standpipe
      private: bool isInitialized;

      friend class VRCPlugin;
    } drcFireHose;

    ////////////////////////////////////////////////////////////////////////////
    //                                                                        //
    //   Joint Trajectory Controller                                          //
    //                                                                        //
    ////////////////////////////////////////////////////////////////////////////
    private: class AtlasCommandController
    {
      /// \brief Constructor, note atlas_controller is the name
      /// of the controller loaded from yaml
      private: AtlasCommandController();

      /// \brief Destructor
      private: ~AtlasCommandController();

      /// \brief subscriber to joint_states of the atlas robot
      private: void GetJointStates(
        const sensor_msgs::JointState::ConstPtr &_js);

      /// \brief stand configuration with PID controller
      /// \param[in] pointer to atlas model
      private: void SetPIDStand(physics::ModelPtr atlasModel);

      /// \brief switch to StandPrep Mode
      private: void SetBDIStandPrep();

      /// \brief switch to Stand Mode
      private: void SetBDIStand();

      /// \brief sitting configuration of the robot when it enters the vehicle.
      /// \param[in] pointer to atlas model
      private: void SetSeatingConfiguration(physics::ModelPtr atlasModel);

      /// \brief standing configuration of the robot when it exits the vehicle.
      /// \param[in] pointer to atlas model
      private: void SetStandingConfiguration(physics::ModelPtr atlasModel);

      /// \brief subscriber to joint_states
      private: ros::Subscriber subJointStates;

      /// \brief publisher of joint_commands
      private: ros::Publisher pubAtlasCommand;

      /// \brief publisher of AtlasSimInterfaceCommand
      private: ros::Publisher pubAtlasSimInterfaceCommand;

      /// \brief ros node handle
      private: ros::NodeHandle* rosNode;

      /// \brief local copy of AtlasCommand message
      private: atlas_msgs::AtlasCommand jc;

      /// \brief hardcoded joint names for atlas
      private: std::vector<std::string> jointNames;

      friend class VRCPlugin;
    } atlasCommandController;

    ////////////////////////////////////////////////////////////////////////////
    //                                                                        //
    //   Private variables                                                    //
    //                                                                        //
    ////////////////////////////////////////////////////////////////////////////
    private: bool warpRobotWithCmdVel;
    private: double lastUpdateTime;
    private: geometry_msgs::Twist robotCmdVel;

    /// \brief fix robot butt to vehicle for efficiency
    // public: std::pair<physics::LinkPtr, physics::LinkPtr> vehicleRobot;
    public: physics::JointPtr vehicleRobotJoint;

    /// \brief Pointer to parent world.
    private: physics::WorldPtr world;

    /// \brief Pointer to the update event connection
    private: event::ConnectionPtr updateConnection;

    // default ros stuff
    private: ros::NodeHandle* rosNode;
    private: ros::CallbackQueue rosQueue;
    private: boost::thread callbackQueueThread;

    // ros subscribers for robot actions
    private: ros::Subscriber subRobotGrab;
    private: ros::Subscriber subRobotRelease;
    private: ros::Subscriber subRobotEnterCar;
    private: ros::Subscriber subRobotExitCar;
    private: physics::JointPtr grabJoint;

    // items below are used for deferred load in case ros is blocking
    private: sdf::ElementPtr sdf;
    private: boost::thread deferredLoadThread;

    // mode flag to indicate StandPrep mode has already been called once.
    private: bool bdiStandPrep;
  };
/** \} */
/// @}
}
#endif
