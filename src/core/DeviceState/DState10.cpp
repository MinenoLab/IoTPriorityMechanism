/*
 * DState10.cpp
 *
 *  Created on: 2016/07/03
 *      Author: tachibana
 */

#include "../IoTDevices.h"

namespace IoTPriority {

D_State10::D_State10(std::shared_ptr<IoTDevices> device) :
		device(device) {
	// TODO 自動生成されたコンストラクター・スタブ

}

D_State10::~D_State10() {
	// TODO Auto-generated destructor stub
}

std::string D_State10::execute() {
	float rate;
	std::cerr << "[DV] State10" << std::endl;
	auto d = device->br_recv_and_getdata();
	UpdateMessage um;
	um.putMessage(*d->getContentData());
	switch (um.getId()) {
	case UpdateMessage::RESTART:
		std::cerr << "[DV] State10 Restart" << std::endl;
		device->ap_sendResum();
		break;
	case UpdateMessage::STOP:
		std::cerr << "[DV] State10 Stop" << std::endl;
		device->ap_changeSendData();
		break;
	case UpdateMessage::ABS_VALUE:
		std::cerr << "[DV] State10 changeAbsRate" << std::endl;
		rate=um.getValue();
		device->ap_changeSendRateAbs(rate);
		break;
	case UpdateMessage::REL_VALUE:
		std::cerr << "[DV] State10 changeRate" << std::endl;
		rate = um.getValue();
		device->ap_changeSendRate(rate);
		break;
	}
	return "";
}

} /* namespace IoTPriority */
