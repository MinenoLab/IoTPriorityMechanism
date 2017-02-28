/*
 * BState04.cpp
 *
 *  Created on: 2016/03/30
 *      Author: tachibana
 */

#include "../BrokerServer.h"

namespace IoTPriority {

B_State04::B_State04(std::shared_ptr<BrokerServer> server) :
		Server(server) {
	// TODO 自動生成されたコンストラクター・スタブ
}

B_State04::~B_State04() {
	// TODO Auto-generated destructor stub
}

std::string B_State04::execute() {
	std::cerr << "[BR]State 4" << std::endl;
	Server->queuethread = std::thread(&BrokerServer::run_state_secondthread,
			Server, Server->name_state10);
	std::cerr << "[BR]State 4 thread_create" << std::endl;
	std::string next;
#ifdef CHECK
	MetaData m,m2,m3;
	PriorityMessage p,p2,p3;
	m.set(100,0x1f,"jpg",2*1024*1024);
	p.makeMessage(0x1f,0xff,PriorityMessage::MESSTYPE_META,PriorityMessage::NOACK,std::make_shared<std::string>(m.getMetadata()));
	Server->putMessage(false ,std::make_shared<PriorityMessage>(p));
	std::cerr<<"[BR]State 4-messPut id : "<<m.getDataID()<<" dataType:"<<m.getDataType()<<std::endl;
	m2.set(101,0x1f,"jpg",2*1024*1024);
	p2.makeMessage(0x1f,0xff,PriorityMessage::MESSTYPE_META,PriorityMessage::NOACK,std::make_shared<std::string>(m2.getMetadata()));
	Server->putMessage(false ,std::make_shared<PriorityMessage>(p2));
	std::cerr<<"[BR]State 4-messPut id : "<<m2.getDataID()<<" dataType:"<<m2.getDataType()<<std::endl;
	m3.set(102,0x1f,"txt",85);
	p3.makeMessage(0x1f,0xff,PriorityMessage::MESSTYPE_META,PriorityMessage::NOACK,std::make_shared<std::string>(m3.getMetadata()));
	Server->putMessage(false ,std::make_shared<PriorityMessage>(p3));
	std::cerr<<"[BR]State 4-messPut id : "<<m3.getDataID()<<" dataType:"<<m3.getDataType()<<std::endl;
#endif
	switch (Server->recv_and_gettype()) {
	case PriorityMessage::MESSTYPE_META:
		next = "";
		break;
	case PriorityMessage::MESSTYPE_STOPREQ:
	case PriorityMessage::MESSTYPE_RATE:
		next = Server->name_state6;
		break;
	default:
		break;
	}
	return next;
}

} /* namespace IoTPriority */
