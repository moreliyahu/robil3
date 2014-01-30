
/*
 * ComponentMain.cpp
 *
 *  Created on: Thursday, 30. January 2014 03:54PM
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

void ComponentMain::handleMissionStatus(config::SSM::sub::MissionStatus& msg)
{
	std::cout<< "SSM say:" << msg.data << std::endl;
}
	

void ComponentMain::publishStatusData(config::SSM::pub::StatusData& msg)
{
	_roscomm->publishStatusData(msg);
}
	