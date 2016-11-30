
/*
 * ComponentMain.cpp
 *
 *  Created on: Thursday, 27. February 2014 12:29PM
 *      Author: autogenerated
 */
#include "ComponentMain.h"
#include "boost/bind.hpp"
#include <ros/ros.h>
#include <std_msgs/String.h>

#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>
#include "ParameterHandler.h"
#include <tf/transform_broadcaster.h>
#include <tf/transform_listener.h>
#include "userHeader.h"



ComponentMain *ComponentMain::_this;

ComponentMain::ComponentMain(int argc,char** argv)	: _inited(init(argc, argv))
{
	_sub_PositionUpdate=ros::Subscriber(_nh.subscribe("/OCU/PositionUpdate", 10, &ComponentMain::handlePositionUpdate,this));
	_sub_GPS=ros::Subscriber(_nh.subscribe("/SENSORS/GPS", 10, &ComponentMain::handleGPS,this));
	_sub_INS=ros::Subscriber(_nh.subscribe("/SENSORS/INS", 10, &ComponentMain::handleINS,this));
	_sub_GpsSpeed=ros::Subscriber(_nh.subscribe("/SENSORS/GPS/Speed", 10, &ComponentMain::handleGpsSpeed,this));
	_pub_Location=ros::Publisher(_nh.advertise<geometry_msgs::PoseWithCovarianceStamped>("/LOC/Pose",10));
	_pub_PerVelocity=ros::Publisher(_nh.advertise<geometry_msgs::TwistStamped>("/LOC/Velocity",10));
	_pub_diagnostic=ros::Publisher(_nh.advertise<diagnostic_msgs::DiagnosticArray>("/diagnostics",100));
	_maintains.add_thread(new boost::thread(boost::bind(&ComponentMain::heartbeat,this)));

	ComponentMain::_this = this;
	_estimation_thread = new boost::thread(&ComponentMain::performEstimation);
}
ComponentMain::~ComponentMain() {

	_estimation_thread->interrupt();
	delete _estimation_thread;
}

bool ComponentMain::init(int argc,char** argv){
	ros::init(argc,argv,"LOC_node");
	return true;
}

void ComponentMain::performEstimation()
{
  
        geometry_msgs::PoseWithCovarianceStamped msg1;
        geometry_msgs::TwistStamped msg2;
    while (ros::ok())
	{
		try
        {
            if (_this->dyn_conf.init)
            {
                if (_this->dyn_conf.calMeas > 100)
                    _this->_estimator.calibrate(10);
                else
                    _this->_estimator.calibrate(_this->dyn_conf.calMeas);
                _this->dyn_conf.init= false;
            }
			/* Perform the estimatior process*/
            if(_this->dyn_conf.noise)
			{
				_this->_estimator.estimator();
				msg1 = _this->_estimator.getEstimatedPose();
				msg2 = _this->_estimator.getEstimatedSpeed();
			}
			else
			{
				_this->_observer.estimator();
				msg1 = _this->_observer.getEstimatedPose();
				msg2 = _this->_observer.getEstimatedSpeed();
			}
			_this->publishLocation(msg1);
			_this->publishPerVelocity(msg2);
            _this->_estimator.broadcastTF();
			boost::this_thread::sleep(boost::posix_time::milliseconds(100));
		}
		catch(boost::thread_interrupted&)
		{
			std::cout << "Thread has stopped. No estimation is published to LOC!!!!" << std::endl;
			return;
		}
	}
}
void ComponentMain::handlePositionUpdate(const geometry_msgs::PoseStamped& msg)
{
    if(dyn_conf.noise)
	  _this->_estimator.positionUpdate(msg);
	else
	  std::cout << "LOC says: position update is not activated if noise is not added" << std::endl;
}
	

void ComponentMain::handleGPS(const sensor_msgs::NavSatFix& msg)
{
        geometry_msgs::PoseWithCovarianceStamped msg1;
    geometry_msgs::TwistStamped msg2;
    if(dyn_conf.noise)
		_estimator.setGPSMeasurement(msg);
	else
		_observer.setGPSMeasurement(msg);
}
	

void ComponentMain::handleINS(const sensor_msgs::Imu& msg)
{
    if(dyn_conf.noise)
		_estimator.setIMUMeasurement(msg);
	else
		_observer.setIMUMeasurement(msg);
}
	

void ComponentMain::handleVOOdometry(const nav_msgs::Odometry& msg)
{
	//std::cout<< "LOC say:" << msg << std::endl;
}

void ComponentMain::handleGpsSpeed(const sensor_msgs::NavSatFix& msg)
{

    _estimator.setGPSSpeedMeasurement(msg);
}	

void ComponentMain::publishLocation(geometry_msgs::PoseWithCovarianceStamped& msg)
{
	_pub_Location.publish(msg);
}
	

void ComponentMain::publishPerVelocity(geometry_msgs::TwistStamped& msg)
{
	_pub_PerVelocity.publish(msg);
}
	
void ComponentMain::publishTransform(const tf::Transform& _tf, std::string srcFrame, std::string distFrame){
	static tf::TransformBroadcaster br;
	br.sendTransform(tf::StampedTransform(_tf, ros::Time::now(), srcFrame, distFrame));
}
tf::StampedTransform ComponentMain::getLastTransform(std::string srcFrame, std::string distFrame){
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
void ComponentMain::publishDiagnostic(const diagnostic_msgs::DiagnosticStatus& _report){
	diagnostic_msgs::DiagnosticArray msg;
	msg.status.push_back(_report);
	_pub_diagnostic.publish(msg);
}
void ComponentMain::publishDiagnostic(const std_msgs::Header& header, const diagnostic_msgs::DiagnosticStatus& _report){
	diagnostic_msgs::DiagnosticArray msg;
	msg.header = header;
	msg.status.push_back(_report);
	_pub_diagnostic.publish(msg);
}
void ComponentMain::heartbeat(){
	using namespace boost::posix_time;
	ros::Publisher _pub = _nh.advertise<std_msgs::String>("/heartbeat", 10);
	double hz = HEARTBEAT_FREQUANCY;
	while(ros::ok()){
		boost::system_time stop_time = boost::get_system_time() + milliseconds((1/hz)*1000);
		std_msgs::String msg;
		msg.data = "LOC";
		_pub.publish(msg);
	    boost::this_thread::sleep(stop_time);
	}
}
void ComponentMain::setSteeringInput(double msg)
{
    if(dyn_conf.noise) _estimator.setSteeringInput(msg);
}
void ComponentMain::setThrottleInput(double msg)
{
    if(dyn_conf.noise) _estimator.setThrottleInput(msg);
}
void ComponentMain::configCallback(loc::configConfig &config, uint32_t level)
{
  // Set class variables to new values. They should match what is input at the dynamic reconfigure GUI.
    dyn_conf = config;
    _estimator._gps_height = config.height;
    _estimator._dyn = config;
    _estimator.modify_Q(config.Q);
}
