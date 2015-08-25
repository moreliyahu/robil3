
/*
 * ComponentMain.h
 *
 *  Created on: Thursday, 04. June 2015 09:56AM
 *      Author: autogenerated
 */
#ifndef COMPONENTMAIN_H_
#define COMPONENTMAIN_H_
#include "ros/ros.h"
#include <std_msgs/String.h>
#include <ParameterTypes.h>
#include <tf/tf.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include "../QinetiQ_IO/LLICtrl.h"
#include <boost/thread.hpp>

using namespace std;

class RosComm;
class ComponentMain {
	RosComm*    _roscomm;
	boost::thread* _driver_thread;
	timer_t     gTimerid;
public:
	ComponentMain(int argc,char** argv);
	virtual ~ComponentMain();
	void handleEffortsTh(const config::LLI::sub::EffortsTh& msg);
	void handleEffortsSt(const config::LLI::sub::EffortsSt& msg);
	void handleEffortsJn(const config::LLI::sub::EffortsJn& msg);


	void publishTransform(const tf::Transform& _tf,  std::string srcFrame, std::string distFrame);
	tf::StampedTransform getLastTrasform(std::string srcFrame, std::string distFrame);
	void publishDiagnostic(const diagnostic_msgs::DiagnosticStatus& _report);
	void publishDiagnostic(const std_msgs::Header& header, const diagnostic_msgs::DiagnosticStatus& _report);
	static void        lliCtrlLoop();
	//void Init ();
	//void StartTimer(void);
	//void TimerCallback (int sig);
//	void TimerCallback (const ros::TimerEvent&);

};
#endif /* COMPONENTMAIN_H_ */