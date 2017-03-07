/*
 * DState11.cpp
 *
 *  Created on: 2017/01/30
 *      Author: tachibana
 */

#include "../IoTDevices.h"

namespace IoTPriority {

D_State11::D_State11(std::shared_ptr<IoTDevices> device) :
		device(device) {
	// TODO 自動生成されたコンストラクター・スタブ

}

D_State11::~D_State11() {
	// TODO Auto-generated destructor stub
}

std::string D_State11::execute() {
	std::cerr<<"[DV] State11"<<std::endl;
	if (device->br_checkState() == Sync_BaseSockSession::SockState::NONACTIVE) {
		bool ret = device->br_startReconnect();
		if (!ret) {
			std::cout << "BR recconect failed" << std::endl;
			return device->name_state0;
		}
	}

	if (device->ap_checkState() == Sync_BaseSockSession::SockState::NONACTIVE) {
		bool ret = device->ap_startReconnect();
		if (!ret) {
			std::cout << "AP recconect failed" << std::endl;
			return device->name_state0;
		}
	}
	return "";
}

} /* namespace IoTPriority */
