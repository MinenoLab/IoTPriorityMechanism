/*
 * DState08.cpp
 *
 *  Created on: 2016/02/29
 *      Author: tachibana
 */

#include "../IoTDevices.h"
#include "../utils/BinaryConverter.h"

namespace IoTPriority {

D_State08::D_State08(std::shared_ptr<IoTDevices> device) :
		device(device) {
	// TODO 自動生成されたコンストラクター・スタブ
}

D_State08::~D_State08() {
	// TODO Auto-generated destructor stub
}

std::string D_State08::execute() {
	std::cerr << "[DV] State8" << std::endl;
	auto d = device->br_recv_and_getdata();
	PermitFormat p;
	p.putMessage(*(d->getContentData()));

	//データIDを先頭に付与
	std::string did;
	did=BinaryConverter::int16toStr(p.getDataId());
	auto it = device->Buffer.find(p.getDataId());
	if (it == device->Buffer.end()) {
		std::cerr << "data not found!! " << p.getDataId() << std::endl;
		std::cout <<iEntity::strnowtime()<< ";data not found!! id; " << p.getDataId() << std::endl;
		//ないので終了通知をすぐ送る
		PriorityMessage compmess;
		compmess.makeMessage(device->DeviceID, 0xff,
				PriorityMessage::MESSTYPE_COMPLETE, PriorityMessage::NOACK,
				std::make_shared<std::string>(did));
		device->br_send(compmess.getAllData());
		return device->name_state5;
	}
	device->OtherMessCount += d->getAllData()->length();
	std::cout << iEntity::strnowtime() << "other packet(recv permit) ;"
			<< d->getAllData()->length() << ";total otherData;"
			<< device->OtherMessCount << std::endl;

	PriorityMessage mess, premess;
	std::string content = did + it->second->getData();
	premess.makeMessage(device->getDeviceID(), 0x00,
			PriorityMessage::MESSTYPE_DECDATA, PriorityMessage::NOACK,
			std::make_shared<std::string>(BinaryConverter::int8toStr(it->second->getPriority())));
	mess.makeMessage(device->getDeviceID(), 0x00,
			PriorityMessage::MESSTYPE_DATA, PriorityMessage::NOACK,
			std::make_shared<std::string>(content));

	std::cerr << "[DV] State8 id : " << p.getDataId() << std::endl;
	std::cerr << "[DV] State8 contentsize : " << content.length() << std::endl;
	std::cerr << "[DV] State8 rate : " << p.getRelativeRate() << std::endl;
	std::cout << iEntity::strnowtime() << ";Data send;DeviceID;"
			<< device->getDeviceID() << ";DataID;" << p.getDataId()
			<< ";;;priority;" << it->second->getPriority() << ";rate;"
			<< p.getRelativeRate() << std::endl;
	device->OtherMessCount += premess.getAllData()->length();
	device->MessCount += mess.getContentData()->length();
	std::cout << iEntity::strnowtime() << "other packet(send decdata) ;"
			<< premess.getAllData()->length() << ";total otherData;"
			<< device->OtherMessCount << std::endl;
	std::cout << iEntity::strnowtime() << "packet(send data) ;DataID;"
			<< p.getDataId() << ";bytes;" << mess.getContentData()->length()
			<< ";total MessData;" << device->MessCount << std::endl;

	bool sendstatus = device->ap_send(premess.getAllData(), -1);
	if(!sendstatus)
		device->ap_startReconnect();
	device->ap_send(mess.getAllData(), p.getRelativeRate());
	return device->name_state5;
}

} /* namespace IoTPriority */
