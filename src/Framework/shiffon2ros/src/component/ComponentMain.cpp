
/*
 * ComponentMain.cpp
 *
 *  Created on: Monday, 07. September 2015 03:23PM
 *      Author: autogenerated
 */
#include "ComponentMain.h"
#include "ros/time.h"
#include "tf/LinearMath/Matrix3x3.h"

#include <ros/ros.h>
#include <std_msgs/String.h>
#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>
#include "ParameterHandler.h"
#include <tf/transform_broadcaster.h>
#include <tf/transform_listener.h>

const double PI = 3.14159;
const double MILS_2_DEG = 0.056250000; // (360/6400)
const double MILS_2_RAD = 2*PI/6400;
const double PI_2_DEG = 180; //

//#define TEST_HEARTBEAT


ComponentMain::ComponentMain(int argc,char** argv)
: _inited(init(argc, argv))
{
	if ( argc == 2 )
	{
		//if IP received from command line (as parameter), change the default (localhost - 127.0.0.1)
		IPADDR = argv[1];
	}
	else
		IPADDR = "127.0.0.1";

	_pub_GPS=ros::Publisher(_nh.advertise<config::SHIFFON2ROS::pub::GPS>(fetchParam(&_nh,"SHIFFON2ROS","GPS","pub"),10));
	_pub_INS=ros::Publisher(_nh.advertise<config::SHIFFON2ROS::pub::INS>(fetchParam(&_nh,"SHIFFON2ROS","INS","pub"),10));
	_pub_INS2=ros::Publisher(_nh.advertise<std_msgs::Float64>("SENSORS/INSSPEED",10));
	_pub_GpsSpeed=ros::Publisher(_nh.advertise<config::SHIFFON2ROS::pub::GpsSpeed>(fetchParam(&_nh,"SHIFFON2ROS","GpsSpeed","pub"),10));
	_pub_GpsSpeed2=ros::Publisher(_nh.advertise<std_msgs::Float64>("SENSORS/GPSSPEED2",10));
	_pub_diagnostic=ros::Publisher(_nh.advertise<diagnostic_msgs::DiagnosticArray>("/diagnostics",100));
//	_maintains.add_thread(new boost::thread(boost::bind(&ComponentMain::heartbeat,this)));
    //Replace the thread group with a simple pthread because there is a SIGEV otherwise
	// and I didn't find the reason.
	// With pthread, it seems to work.
#ifdef TEST_HEARTBEAT
	_myHeartbeatThread = (pthread_t) NULL;
	pthread_create(&_myHeartbeatThread, NULL, &callHeartbeat, this);
#endif

}


void ComponentMain::InitShiphonConection() {

	ROS_INFO("Initializing Shiphon Connection");
	char ipAddr[16];
	//string tmpStr = "132.4.6.60";
	strcpy (ipAddr, IPADDR.c_str());
	int lPort = 2010;
	int rPort = 4997;

	_shiphonCtrl = new Shiphon_Ctrl ();
	_shiphonCtrl->Init(ipAddr, lPort, rPort);

	struct timeval start, end;
	gettimeofday(&start, NULL);
}


ComponentMain::~ComponentMain() {
#ifdef TEST_HEARTBEAT
	if (_myHeartbeatThread) {
		pthread_cancel(_myHeartbeatThread);
		_myHeartbeatThread = (pthread_t)NULL;
	}
#endif

}

bool ComponentMain::init(int argc,char** argv){
	ros::init(argc,argv,"SHIFFON2ROS_node");
	return true;
}

void ComponentMain::ReadAndPub_ShiphonGPS(){
	config::SHIFFON2ROS::pub::GPS GPS_msg;

	GPS_msg.latitude  =  (_shiphonCtrl->get_PERIODIC100HZMESSAGE()).LAT_Egi * PI_2_DEG;
	GPS_msg.longitude =  (_shiphonCtrl->get_PERIODIC100HZMESSAGE()).LONG_Egi * PI_2_DEG;
	GPS_msg.altitude  =  (_shiphonCtrl->get_PERIODIC100HZMESSAGE()).Altitude_MSL_EGI;

	GPS_msg.header.stamp = ros::Time::now();

	publishGPS(GPS_msg);
}

void ComponentMain::publishGPS(config::SHIFFON2ROS::pub::GPS& msg) {
	_pub_GPS.publish(msg);
}


void ComponentMain::ReadAndPub_ShiphonINS(){
	config::SHIFFON2ROS::pub::INS INS_msg;

	INS_msg.linear_acceleration.x  =  (_shiphonCtrl->get_PERIODIC100HZMESSAGE()).Acc_X_Egi;
	INS_msg.linear_acceleration.y  =  (_shiphonCtrl->get_PERIODIC100HZMESSAGE()).Acc_Y_Egi;
	INS_msg.linear_acceleration.z  =  (_shiphonCtrl->get_PERIODIC100HZMESSAGE()).Acc_Z_Egi;
	
	INS_msg.angular_velocity.x  =  (_shiphonCtrl->get_PERIODIC100HZMESSAGE()).Roll_rate_X_PD_Egi * MILS_2_RAD;
	INS_msg.angular_velocity.y  =  (_shiphonCtrl->get_PERIODIC100HZMESSAGE()).Pitch_rate_Y_PD_Egi * MILS_2_RAD;
	INS_msg.angular_velocity.z  =  (_shiphonCtrl->get_PERIODIC100HZMESSAGE()).Azimuth_rate_Z_PD_Egi * MILS_2_RAD;

	float Roll = (_shiphonCtrl->get_PERIODIC100HZMESSAGE()).Roll_PD_Egi * MILS_2_RAD;
	float Pitch = (_shiphonCtrl->get_PERIODIC100HZMESSAGE()).Pitch_PD_Egi * MILS_2_RAD;
	float Yaw = (_shiphonCtrl->get_PERIODIC100HZMESSAGE()).Azimuth_PD_geographic * MILS_2_RAD;

	tf::Matrix3x3 obs_mat;
	obs_mat.setEulerYPR(Yaw,Pitch,Roll);

	tf::Quaternion q_tf;
	obs_mat.getRotation(q_tf);

	INS_msg.orientation.x = q_tf.getX();
	INS_msg.orientation.y = q_tf.getY();
	INS_msg.orientation.z = q_tf.getZ();
	INS_msg.orientation.w = q_tf.getW();


	INS_msg.header.stamp = ros::Time::now();

	publishINS(INS_msg);

	std_msgs::Float64 INS_msg2;
	INS_msg2.data = (float)((_shiphonCtrl->get_PERIODIC100HZMESSAGE()).Azimuth_rate_Z_PD_Egi * MILS_2_RAD);
	publishINS2(INS_msg2);

}

void ComponentMain::publishINS(config::SHIFFON2ROS::pub::INS& msg) {
	_pub_INS.publish(msg);
}

void ComponentMain::publishINS2(std_msgs::Float64& msg) {
	_pub_INS2.publish(msg);
}

void ComponentMain::ReadAndPub_ShiphonGpsSpeed() {
	config::SHIFFON2ROS::pub::GpsSpeed GpsSpeed_msg;
	double East_vel = (_shiphonCtrl->get_PERIODIC100HZMESSAGE()).Velocity_East_Egi;
	double North_vel = (_shiphonCtrl->get_PERIODIC100HZMESSAGE()).Velocity_north_Egi;
	double Down_vel = (_shiphonCtrl->get_PERIODIC100HZMESSAGE()).Velocity_down_Egi;

	GpsSpeed_msg.speed = sqrt(East_vel*East_vel + North_vel*North_vel + Down_vel*Down_vel);
	GpsSpeed_msg.header.stamp = ros::Time::now();

	publishGpsSpeed(GpsSpeed_msg);


	//Temporary
	std_msgs::Float64 GpsSpeed2_msg;
	GpsSpeed2_msg.data = GpsSpeed_msg.speed;
	publishGpsSpeed2(GpsSpeed2_msg);

}
void ComponentMain::publishGpsSpeed2(std_msgs::Float64& msg)
{
	_pub_GpsSpeed2.publish(msg);
}

void ComponentMain::publishGpsSpeed(config::SHIFFON2ROS::pub::GpsSpeed& msg)
{
	_pub_GpsSpeed.publish(msg);
}
	


void ComponentMain::publishTransform(const tf::Transform& _tf, std::string srcFrame, std::string distFrame){
	//static tf::TransformBroadcaster br;
	//	br.sendTransform(tf::StampedTransform(_tf, ros::Time::now(), srcFrame, distFrame));
}

tf::StampedTransform ComponentMain::getLastTransform(std::string srcFrame, std::string distFrame){
/*	tf::StampedTransform _tf;
	static tf::TransformListener listener;
	try {
	    listener.waitForTransform(distFrame, srcFrame, ros::Time(0), ros::Duration(10.0) );
	    listener.lookupTransform(distFrame, srcFrame, ros::Time(0), _tf);
	} catch (tf::TransformException& ex) {
	    ROS_ERROR("%s",ex.what());
	}
	return _tf;
*/
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
	//using namespace boost::posix_time;
	ros::Publisher _pub = _nh.advertise<std_msgs::String>("/heartbeat", 10);
	double hz = HEARTBEAT_FREQUENCY;
	double cycle = (1/hz);
	ros::Duration oneSec(cycle);

	while(ros::ok()){
		//boost::system_time stop_time = boost::get_system_time() + milliseconds((1/hz)*1000);
		std_msgs::String msg;
		msg.data = "SHIFFON2ROS";
		_pub.publish(msg);
		oneSec.sleep();
	   // boost::this_thread::sleep(stop_time);
	}
}

void *ComponentMain::callHeartbeat(void * pParam)
{
	ComponentMain *myHandle = (ComponentMain *) (pParam);

	myHandle->heartbeat();

}
