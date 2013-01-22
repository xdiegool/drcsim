#include <ros/ros.h>
#include <trajectory_msgs/JointTrajectory.h>
#include <gazebo/math/Quaternion.hh>
#include <math.h>
#include "osrf_msgs/JointCommands.h"

int main(int argc, char** argv)
{

  ros::init(argc, argv, "pub_joint_trajectory_test");

  ros::NodeHandle rosnode;

  bool wait = true;
  while (wait)
  {
    ros::Time t = ros::Time::now();
    if (t.toSec() > 0)
      wait = false;
  }

  ros::Publisher pub_ = rosnode.advertise<osrf_msgs::JointCommands>("/atlas/joint_commands",1, true);

  osrf_msgs::JointCommands jc;

  jc.header.stamp = ros::Time::now();

  jc.name.push_back("atlas::back_lbz" );
  jc.name.push_back("atlas::back_mby" );
  jc.name.push_back("atlas::back_ubx" );
  jc.name.push_back("atlas::neck_ay"  );
  jc.name.push_back("atlas::l_leg_uhz");
  jc.name.push_back("atlas::l_leg_mhx");
  jc.name.push_back("atlas::l_leg_lhy");
  jc.name.push_back("atlas::l_leg_kny");
  jc.name.push_back("atlas::l_leg_uay");
  jc.name.push_back("atlas::l_leg_lax");
  jc.name.push_back("atlas::r_leg_lax");
  jc.name.push_back("atlas::r_leg_uay");
  jc.name.push_back("atlas::r_leg_kny");
  jc.name.push_back("atlas::r_leg_lhy");
  jc.name.push_back("atlas::r_leg_mhx");
  jc.name.push_back("atlas::r_leg_uhz");
  jc.name.push_back("atlas::l_arm_elx");
  jc.name.push_back("atlas::l_arm_ely");
  jc.name.push_back("atlas::l_arm_mwx");
  jc.name.push_back("atlas::l_arm_shx");
  jc.name.push_back("atlas::l_arm_usy");
  jc.name.push_back("atlas::l_arm_uwy");
  jc.name.push_back("atlas::r_arm_elx");
  jc.name.push_back("atlas::r_arm_ely");
  jc.name.push_back("atlas::r_arm_mwx");
  jc.name.push_back("atlas::r_arm_shx");
  jc.name.push_back("atlas::r_arm_usy");
  jc.name.push_back("atlas::r_arm_uwy");

  int n = jc.name.size();
  jc.position.resize(n);
  jc.velocity.resize(n);
  jc.effort.resize(n);
  jc.kp_position.resize(n);
  jc.ki_position.resize(n);
  jc.kd_position.resize(n);
  jc.kp_velocity.resize(n);
  jc.i_effort_min.resize(n);
  jc.i_effort_max.resize(n);

  double dt = 1.0;
  double rps = 0.05;

  for (int i = 0; i < n; i++)
  {
    double theta = rps*2.0*M_PI*i*dt;
    double x1 = -0.5*sin(2*theta);
    double x2 =  0.5*sin(1*theta);

    jc.position[i]     = ros::Time::now().toSec();
    jc.velocity[i]     = 0;
    jc.effort[i]       = 0;
    jc.kp_position[i]  = 10;
    jc.ki_position[i]  = 0;
    jc.kd_position[i]  = 0;
    jc.kp_velocity[i]  = 0;
    jc.i_effort_min[i] = 0;
    jc.i_effort_max[i] = 0;
  }

  pub_.publish(jc); // use publisher
  ros::spin();

  return 0;
}
