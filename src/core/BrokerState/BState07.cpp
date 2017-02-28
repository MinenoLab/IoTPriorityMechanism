/*
 * BState07.cpp
 *
 *  Created on: 2016/03/30
 *      Author: tachibana
 */

#include "../BrokerServer.h"

namespace IoTPriority {

B_State07::B_State07(std::shared_ptr<BrokerServer> server) :
		Server(server) {
}

B_State07::~B_State07() {
	// TODO Auto-generated destructor stub
}

std::string B_State07::execute() {
	std::cerr << "[BR]State 7" << std::endl;
	auto d = Server->ap_recv_and_getdata();
	float rate = BinaryConverter::strtoFloat(*(d->getContentData()));
	float prevrate = Server->getCurCommRate();
	Server->putCurCommRate(rate);
	std::cerr << "[BR]State 7  Rate:" << (Server->getCurCommRate() / 1024)
			<< "Kbps" << std::endl;
	//初期設定値との乖離によるガタ落ちを防ぐため初期値のみ除く
	if (firstchanged) {
		float changePercent = rate / prevrate;
		Server->iContorol->notifyALLsendingDevices(changePercent);
	} else {
		if (firstrate < 0)
			firstrate = rate;
		if (firstrate != rate)
			firstchanged = true;
	}
	return "";
}
} /* namespace IoTPriority */
