/*
 * AState07.cpp
 *
 *  Created on: 2016/02/29
 *      Author: tachibana
 */

#include "../AppServer.h"

namespace IoTPriority {

A_State07::A_State07(std::shared_ptr<AppServer> server) :
		Server(server) {
	// TODO 自動生成されたコンストラクター・スタブ
#ifdef CHECK
std::cout<<"called state7 const"<<std::endl;
#endif

}

A_State07::~A_State07() {
	// TODO Auto-generated destructor stub
}

std::string A_State07::execute() {
	std::cerr << "[AP]State 7" << std::endl;
#ifdef CHECK
	float rate = 30;
#else
	float rate = Server->CalcCurRate();
	if(rate<8*1024)
		rate=8*1024;
#endif
	std::cout <<iEntity::strnowtime()<< "[AP]State 7 rate: " <<(rate/1024)<<"Kbps"<<std::endl;
	std::cout<<iEntity::strnowtime()<<";curRate;"<<(rate/1024)<<"Kbps"<<std::endl;
	for(auto it=Server->devid_byte_map.begin();it!=Server->devid_byte_map.end();++it){
		std::cout <<iEntity::strnowtime()<< "DeviceID ; " <<it->first;
		for(int i=1;i<it->second.size();i++){
			std::cout << "; priority ;" <<i<<"; bytes ;"<<it->second[i];
			it->second[i]=0;
		}
		std::cout<<std::endl;
	}
	auto mess = std::make_shared<PriorityMessage>();
	std::string strrate;
	//一向に増えなくなるのでました値を送信する
	if(rate<(100*1024)){
		strrate=BinaryConverter::floattoStr(rate*2.5);
	}else{
		strrate=BinaryConverter::floattoStr(rate*1.5);
	}
	mess->makeMessage(Server->DeviceID, 0xff, PriorityMessage::MESSTYPE_RATE,
			PriorityMessage::NOACK, std::make_shared<std::string>(strrate));

#ifdef CHECK
	std::cout<<"[AP]State 7:mess:"<<mess->getSenderDeviceID()<<std::endl;
#else
	std::cerr << "[AP]State 7 sending...:"<< std::endl;
	bool sendret=Server->br_send(mess->getAllData());
	if(!sendret)
		return Server->name_state8;
#endif
	return "";
}

} /* namespace IoTPriority */
