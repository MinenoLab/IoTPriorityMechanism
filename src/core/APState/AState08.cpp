/*
 * AState08.cpp
 *
 *  Created on: 2016/04/06
 *      Author: tachibana
 */

#include "../AppServer.h"

namespace IoTPriority {

A_State08::A_State08(std::shared_ptr<AppServer> server):
				Server(server) {
	// TODO 自動生成されたコンストラクター・スタブ

}

A_State08::~A_State08() {
	// TODO Auto-generated destructor stub
}

std::string A_State08::execute(){
	if(Server->br_checkState()==Sync_BaseSockSession::SockState::NONACTIVE){
		bool ret=Server->startReconnect();
		if(!ret){
			std::cout << "BR recconect failed" << std::endl;
			return Server->name_state0;
		}
	}
	return "";
}

} /* namespace IoTPriority */
