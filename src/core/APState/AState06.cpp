/*
 * AState06.cpp
 *
 *  Created on: 2016/02/29
 *      Author: tachibana
 */

#include "../AppServer.h"

namespace IoTPriority {

A_State06::A_State06(std::shared_ptr<AppServer> server) :
		Server(server) {
}

A_State06::~A_State06() {
}

std::string A_State06::execute() {
	std::string next;
	struct timeval tm;
	std::cerr << "[AP]State 6" << std::endl;
	do {
		if (Server->getSendStopreq()) {
			std::cerr << "[AP]State 6 end" << std::endl;
			next = "";
			break;
		}
		gettimeofday(&tm, NULL);
		auto thvalue = Server->PrevTime.tv_sec + Server->getRateInterval();
		if (thvalue <= tm.tv_sec) {
			std::cerr << "[AP]State 6:nowtime" << localtime(&tm.tv_sec)->tm_min
					<< " m "<<localtime(&tm.tv_sec)->tm_sec << std::endl;
			next = Server->name_state7;
			Server->PrevTime.tv_sec = tm.tv_sec;
			break;
		}
	} while (1);
	return next;
}

} /* namespace IoTPriority */
