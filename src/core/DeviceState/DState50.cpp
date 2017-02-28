/*
 * DState50.cpp
 *
 *  Created on: 2016/05/07
 *      Author: tachibana
 */

#include "../NormalIoTDevices.h"

namespace IoTPriority {

D_State50::D_State50(std::shared_ptr<NormalIoTDevices> device):Device(device) {
	// TODO 自動生成されたコンストラクター・スタブ

}

D_State50::~D_State50() {
	// TODO Auto-generated destructor stub
}

std::string D_State50::execute(){
	std::cerr << "[DV] State0" << std::endl;
	return "";
}
} /* namespace IoTPriority */
