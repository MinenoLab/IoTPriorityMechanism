/*
 * BState06.cpp
 *
 *  Created on: 2016/03/30
 *      Author: tachibana
 */

#include "../BrokerServer.h"

namespace IoTPriority {

B_State06::B_State06(std::shared_ptr<BrokerServer> server) :
		Server(server) {
}

B_State06::~B_State06() {
	// TODO Auto-generated destructor stub
}

std::string B_State06::execute() {
	std::cerr<<"[BR]State 6"<<std::endl;
	std::string next;

#ifdef CHECK
	testinit();
	std::cerr<<"[BR]State 6-messPut"<<std::endl;
#endif
	bool getnext = false;
	do {
		switch (Server->recv_and_gettype()) {
		case PriorityMessage::MESSTYPE_META:
			next = "";
			getnext = true;
			break;
		case PriorityMessage::MESSTYPE_STOPREQ:
			next = Server->name_state0;
			getnext = true;
			break;
		case PriorityMessage::MESSTYPE_RATE:
			next = Server->name_state7;
			getnext = true;
			break;
		default:
			break;
		}
	} while (!getnext);
	return next;
}

void B_State06::testinit(){
	union fconvert {
			float f;
			uint32_t u;
		};
	union fconvert frate;
	std::string strrate;
		frate.f=40.3;
		for(int i=0;i<4;i++){
			unsigned char c;
			c=(unsigned char)(frate.u%256);
			strrate+=c;
			frate.u=frate.u>>8;
		}
		PriorityMessage p,p2,p3;
		p.makeMessage(Server->DeviceID, 0xff, PriorityMessage::MESSTYPE_RATE,
				PriorityMessage::NOACK, std::make_shared<std::string>(strrate));
		Server->putMessage(true ,std::make_shared<PriorityMessage>(p));
		std::string n="";
		p2.makeMessage(Server->DeviceID, 0xff, PriorityMessage::MESSTYPE_STOPREQ,
						PriorityMessage::NOACK, std::make_shared<std::string>(n));
		Server->putMessage(true ,std::make_shared<PriorityMessage>(p2));
}

} /* namespace IoTPriority */
