/*
 * DState00.cpp
 *
 *  Created on: 2016/04/19
 *      Author: tachibana
 */
#include "../IoTDevices.h"

namespace IoTPriority {

D_State00::D_State00(std::shared_ptr<IoTDevices> device) {
	// TODO 自動生成されたコンストラクター・スタブ

}

D_State00::~D_State00() {
	// TODO Auto-generated destructor stub
}

std::string D_State00::execute(){
	std::cerr << "[DV] State0" << std::endl;
	return "";
}

} /* namespace IoTPriority */
