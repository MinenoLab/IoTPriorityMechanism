/*
 * AState00.cpp
 *
 *  Created on: 2016/04/18
 *      Author: tachibana
 */

#include "../AppServer.h"

namespace IoTPriority {

A_State00::A_State00(std::shared_ptr<AppServer> server) {
	// TODO 自動生成されたコンストラクター・スタブ

}

A_State00::~A_State00() {
	// TODO Auto-generated destructor stub
}

std::string A_State00::execute(){
	std::cerr << "[AP]State 0" << std::endl;
	return "";
}

} /* namespace IoTPriority */
