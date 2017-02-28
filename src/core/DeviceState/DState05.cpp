/*
 * DState05.cpp
 *
 *  Created on: 2016/02/29
 *      Author: tachibana
 */


#include "../IoTDevices.h"

namespace IoTPriority {

D_State05::D_State05(std::shared_ptr<IoTDevices> device) :
		device(device),{
	// TODO 自動生成されたコンストラクター・スタブ

}

D_State05::~D_State05() {
	// TODO Auto-generated destructor stub
}

std::string D_State05::execute() {
	struct timeval tm;
	std::cerr<<"[DV] State5"<<std::endl;
	do {
		auto ret=device->readDeviceQueue();
		if(ret!=NULL)
			return "";
		auto intret=device->recv_and_gettype_single();
		if (intret>=0){
			//std::cerr<<"[DV] State5 ok"<<std::endl;
			switch(intret){
			case PriorityMessage::MESSTYPE_PRI:
				return device->name_state7;
			case PriorityMessage::MESSTYPE_PERMIT:
				return device->name_state8;
			case PriorityMessage::MESSTYPE_CALLOFF:
				return device->name_state0;
			case PriorityMessage::MESSTYPE_DATA:
				return device->name_state9;
			case PriorityMessage::MESSTYPE_UPDATE:
				return device->name_state10;
			default:
				break;
			}
		}
		gettimeofday(&tm, NULL);
		auto thvalue = PrevTime.tv_sec + INTERVAL;
		if(thvalue <= tm.tv_sec){
			PrevTime.tv_sec = tm.tv_sec;
			return device->name_state11;
		}
	} while (1);
	return "";
}

} /* namespace IoTPriority */
