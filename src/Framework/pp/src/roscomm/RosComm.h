
/*
 * RosComm.h
 *
 *  Created on: Tuesday, 25. February 2014 11:19AM
 *      Author: autogenerated
 */
#ifndef ROSCOMM_H_
#define ROSCOMM_H_
#include <ros/ros.h>
#include <std_msgs/String.h>
#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>
#include <ParameterTypes.h>
#include <tf/tf.h>
#include <boost/thread.hpp>
class ComponentMain;
class RosComm {
  bool _inited;
  ComponentMain   * _comp;
  ros::NodeHandle _nh;
  ros::Publisher _pub_diagnostic;
  boost::thread_group _maintains;
	ros::Subscriber _sub_GlobalPath;
	ros::Subscriber _sub_BladePosition;
	ros::Subscriber _sub_Map;
	ros::Publisher  _pub_LocalPath;

  bool init(int argc,char** argv);
public:
	RosComm(ComponentMain* comp,int argc,char** argv);
	virtual ~RosComm();
	void GlobalPathCallback(const config::PP::sub::GlobalPath::ConstPtr &msg);
	void BladePositionCallback(const config::PP::sub::BladePosition::ConstPtr &msg);
	void MapCallback(const config::PP::sub::Map::ConstPtr &msg);
	void publishLocalPath( config::PP::pub::LocalPath &msg);

	void publishTransform(const tf::Transform& _tf, std::string srcFrame, std::string distFrame);
	tf::StampedTransform getLastTrasform(std::string srcFrame, std::string distFrame);
	void publishDiagnostic(const diagnostic_msgs::DiagnosticStatus& _report);
	void publishDiagnostic(const std_msgs::Header& header, const diagnostic_msgs::DiagnosticStatus& _report);
	void heartbeat();
};
#endif /* ROSCOMM_H_ */
