
/*
 * ComponentMain.cpp
 *
 *  Created on: Tuesday, 25. February 2014 11:19AM
 *      Author: autogenerated
 */
#include "ComponentMain.h"
#include "../roscomm/RosComm.h"
ComponentMain::ComponentMain(int argc,char** argv)
{
	_roscomm = new RosComm(this,argc, argv);
}
ComponentMain::~ComponentMain() {
	if(_roscomm) delete _roscomm; _roscomm=0;
}

void ComponentMain::handleGlobalPath(const config::PP::sub::GlobalPath& msg)
{
	std::cout<< "PP say:" << msg << std::endl;
}
	

void ComponentMain::handleBladePosition(const config::PP::sub::BladePosition& msg)
{
	std::cout<< "PP say:" << msg << std::endl;
}
	

void ComponentMain::handleMap(const config::PP::sub::Map& msg)
{
	std::cout<< "PP say:" << msg << std::endl;
}
	

void ComponentMain::publishLocalPath(config::PP::pub::LocalPath& msg)
{
	_roscomm->publishLocalPath(msg);
}
	
void ComponentMain::publishTransform(const tf::Transform& _tf, std::string srcFrame, std::string distFrame){
	_roscomm->publishTransform(_tf, srcFrame, distFrame);
}
tf::StampedTransform ComponentMain::getLastTrasform(std::string srcFrame, std::string distFrame){
	return _roscomm->getLastTrasform(srcFrame, distFrame);
}
void ComponentMain::publishDiagnostic(const diagnostic_msgs::DiagnosticStatus& _report){
	_roscomm->publishDiagnostic(_report);
}
void ComponentMain::publishDiagnostic(const std_msgs::Header& header, const diagnostic_msgs::DiagnosticStatus& _report){
	_roscomm->publishDiagnostic(header, _report);
}
