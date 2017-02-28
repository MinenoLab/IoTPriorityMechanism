/*
 * BState00.cpp
 *
 *  Created on: 2016/03/30
 *      Author: tachibana
 */

#include "../BrokerServer.h"

namespace IoTPriority {

B_State00::B_State00(std::shared_ptr<BrokerServer> server):Server(server) {
	// TODO 自動生成されたコンストラクター・スタブ

}

B_State00::~B_State00() {
	// TODO Auto-generated destructor stub
}

std::string B_State00::execute(){
	std::cerr << "[BR]State 0" << std::endl;
	{
		std::unique_lock<std::mutex> lk(Server->mtx);
		Server->endthread=true;
	}
	return "";
}

} /* namespace IoTPriority */
