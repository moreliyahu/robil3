
/*
 * RosComm.cpp
 *
 *  Created on: Tuesday, 25. February 2014 11:19AM
 *      Author: autogenerated
 */
#include <ros/ros.h>
#include <std_msgs/String.h>
#include "RosComm.h"
#include "../component/ComponentMain.h"
#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>
#include "ParameterHandler.h"
#include <tf/transform_broadcaster.h>
#include <tf/transform_listener.h>
RosComm::RosComm(ComponentMain* comp,int argc,char** argv)
	: _inited(init(argc, argv)), _comp(comp)
{
	_sub_IEDLocation=ros::Subscriber(_nh.subscribe(fetchParam(&_nh,"SMME","IEDLocation","sub"), 10, &RosComm::IEDLocationCallback,this));
	_sub_AssignNavTask=ros::Subscriber(_nh.subscribe(fetchParam(&_nh,"SMME","AssignNavTask","sub"), 10, &RosComm::AssignNavTaskCallback,this));
	_sub_AssignManTask=ros::Subscriber(_nh.subscribe(fetchParam(&_nh,"SMME","AssignManTask","sub"), 10, &RosComm::AssignManTaskCallback,this));
	_sub_AssignMission=ros::Subscriber(_nh.subscribe(fetchParam(&_nh,"SMME","AssignMission","sub"), 10, &RosComm::AssignMissionCallback,this));
	_sub_BladePosition=ros::Subscriber(_nh.subscribe(fetchParam(&_nh,"SMME","BladePosition","sub"), 10, &RosComm::BladePositionCallback,this));
	_pub_GlobalPath=ros::Publisher(_nh.advertise<config::SMME::pub::GlobalPath>(fetchParam(&_nh,"SMME","GlobalPath","pub"),10));
	_pub_WorkSeqData=ros::Publisher(_nh.advertise<config::SMME::pub::WorkSeqData>(fetchParam(&_nh,"SMME","WorkSeqData","pub"),10));
	_pub_MissionAcceptance=ros::Publisher(_nh.advertise<config::SMME::pub::MissionAcceptance>(fetchParam(&_nh,"SMME","MissionAcceptance","pub"),10));
	_pub_diagnostic=ros::Publisher(_nh.advertise<diagnostic_msgs::DiagnosticArray>("/diagnostics",100));
	_maintains.add_thread(new boost::thread(boost::bind(&RosComm::heartbeat,this)));
}
RosComm::~RosComm()
{
}
bool RosComm::init(int argc,char** argv){
	ros::init(argc,argv,"SMME_node");
	return true;
}


void RosComm::IEDLocationCallback(const config::SMME::sub::IEDLocation::ConstPtr &msg)
{
	_comp->handleIEDLocation(*msg);
}
	

void RosComm::AssignNavTaskCallback(const config::SMME::sub::AssignNavTask::ConstPtr &msg)
{
	_comp->handleAssignNavTask(*msg);
}
	

void RosComm::AssignManTaskCallback(const config::SMME::sub::AssignManTask::ConstPtr &msg)
{
	_comp->handleAssignManTask(*msg);
}
	

void RosComm::AssignMissionCallback(const config::SMME::sub::AssignMission::ConstPtr &msg)
{
	_comp->handleAssignMission(*msg);
}
	

void RosComm::BladePositionCallback(const config::SMME::sub::BladePosition::ConstPtr &msg)
{
	_comp->handleBladePosition(*msg);
}
	

void RosComm::publishGlobalPath( config::SMME::pub::GlobalPath &msg)
{
	_pub_GlobalPath.publish(msg);
}
	

void RosComm::publishWorkSeqData( config::SMME::pub::WorkSeqData &msg)
{
	_pub_WorkSeqData.publish(msg);
}
	

void RosComm::publishMissionAcceptance( config::SMME::pub::MissionAcceptance &msg)
{
	_pub_MissionAcceptance.publish(msg);
}
	
void RosComm::publishTransform(const tf::Transform& _tf, std::string srcFrame, std::string distFrame){
	static tf::TransformBroadcaster br;
	br.sendTransform(tf::StampedTransform(_tf, ros::Time::now(), srcFrame, distFrame));
}
tf::StampedTransform RosComm::getLastTrasform(std::string srcFrame, std::string distFrame){
	tf::StampedTransform _tf;
	static tf::TransformListener listener;
	try {
	    listener.waitForTransform(distFrame, srcFrame, ros::Time(0), ros::Duration(10.0) );
	    listener.lookupTransform(distFrame, srcFrame, ros::Time(0), _tf);
	} catch (tf::TransformException& ex) {
	    ROS_ERROR("%s",ex.what());
	}
	return _tf;
}
void RosComm::publishDiagnostic(const diagnostic_msgs::DiagnosticStatus& _report){
	diagnostic_msgs::DiagnosticArray msg;
	msg.status.push_back(_report);
	_pub_diagnostic.publish(msg);
}
void RosComm::publishDiagnostic(const std_msgs::Header& header, const diagnostic_msgs::DiagnosticStatus& _report){
	diagnostic_msgs::DiagnosticArray msg;
	msg.header = header;
	msg.status.push_back(_report);
	_pub_diagnostic.publish(msg);
}
void RosComm::heartbeat(){
	using namespace boost::posix_time;
	ros::Publisher _pub = _nh.advertise<std_msgs::String>("/heartbeat", 10);
	double hz = HEARTBEAT_FREQUANCY;
	while(ros::ok()){
		boost::system_time stop_time = boost::get_system_time() + milliseconds((1/hz)*1000);
		std_msgs::String msg;
		msg.data = "SMME";
		_pub.publish(msg);
	    boost::this_thread::sleep(stop_time);
	}
}
