
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
#include <pthread.h>

class RosComm;

class ComponentMain {

public:
	ComponentMain(int argc,char** argv);
	virtual ~ComponentMain();
	void   lliCtrlLoop();
	static void * callPThread(void *pThis);
	void workerFunc();
	void handleEffortsTh(const config::LLI::sub::EffortsTh& msg);
	void handleEffortsSt(const config::LLI::sub::EffortsSt& msg);
	void handleEffortsJn(const config::LLI::sub::EffortsJn& msg);


	void publishTransform(const tf::Transform& _tf,  std::string srcFrame, std::string distFrame);
	tf::StampedTransform getLastTrasform(std::string srcFrame, std::string distFrame);
	void publishDiagnostic(const diagnostic_msgs::DiagnosticStatus& _report);
	void publishDiagnostic(const std_msgs::Header& header, const diagnostic_msgs::DiagnosticStatus& _report);
    void setReady() {is_ready=true;}
    void setNotReady(){is_ready=false;}

private:
	RosComm*    _roscomm;
	boost::thread* _driver_thread;
	CLLI_Ctrl *_clli;
	static ComponentMain *_this;
	pthread_t _mythread;
	bool is_ready;
};
#endif /* COMPONENTMAIN_H_ */
