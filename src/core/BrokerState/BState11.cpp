/*
 * BState11.cpp
 *
 *  Created on: 2016/03/30
 *      Author: tachibana
 */

#include "../BrokerServer.h"

namespace IoTPriority {

B_State11::B_State11(std::shared_ptr<BrokerServer> server) :
		Server(server) {
	// TODO 自動生成されたコンストラクター・スタブ

}

B_State11::~B_State11() {
	// TODO Auto-generated destructor stub
}

std::string B_State11::execute() {
	std::cerr << "[q-BR]State 11" << std::endl;
	int mestype;
	do {
#ifdef CHECK
		std::cerr << "[q-BR]State 11 ... waiting 1 sec." << std::endl;
		sleep(1);
		testinit();
#endif
		bool ret = Server->iContorol->detectIntertupt();
		if (ret)
			return "";

		mestype = Server->recv_and_gettype_fromDevice();
	} while (mestype != PriorityMessage::MESSTYPE_COMPLETE);
	auto d = Server->dv_recv_and_getdata();
	const char *c = d->getContentData()->substr(0, 2).c_str();
	int dataID = (int) (c[0] * 256) + (int) c[1];
	int sclass = Server->getSendingSClass(d->getSenderDeviceID(), dataID);
	if (sclass >= 0)
		Server->prevSClass = sclass;
	int ret = Server->releaseSending(d->getSenderDeviceID(), dataID);
	std::cerr << "[q-BR]State 11 release DataID: " << dataID << " class : "
			<< sclass << std::endl;
	std::cout << iEntity::strnowtime() << ";CompleteDataSend;DeviceID;"
			<< d->getSenderDeviceID() << ";Dataid;" << dataID << std::endl;
	//COMPLETEメッセージとUPDATE(true)メッセージが同時に発信された時の不整合の解消
	if (ret == -1) {
		Server->iContorol->resolutePassing(d,dataID);
		return Server->name_state11;
	}
	return "";
}


void B_State11::testinit() {
	int dataid = 100;
	for (int i = 0; i < 2; i++) {
		std::string s;
		s += (char) (dataid / 256);
		s += (char) (dataid % 256);
		PriorityMessage p;
		p.makeMessage(0x1f, 0xff, PriorityMessage::MESSTYPE_COMPLETE,
				PriorityMessage::NOACK, std::make_shared<std::string>(s));
		Server->putMessage(false, std::make_shared<PriorityMessage>(p));
		dataid++;
	}
}

} /* namespace IoTPriority */
