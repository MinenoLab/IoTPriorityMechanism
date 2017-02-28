/*
 * DState51.cpp
 *
 *  Created on: 2016/05/07
 *      Author: tachibana
 */

#include "../NormalIoTDevices.h"

namespace IoTPriority {

D_State51::D_State51(std::shared_ptr<NormalIoTDevices> device) :
		Device(device), DataID(1) {
	// TODO 自動生成されたコンストラクター・スタブ

}

D_State51::~D_State51() {
	// TODO Auto-generated destructor stub
}

std::string D_State51::execute() {
	//コンテンツデータ取得
	while (1) {
		auto content = Device->putDeviceQueue();
		int dId = getDataID();
		//優先度取得
		int pri = Device->popPriority();
		//データIDを先頭に付与
		std::string strdid;
		strdid += (char) (dId / 256);
		strdid += (char) (dId % 256);
		std::string senddata = strdid + content->second;
		PriorityMessage mess, premess;
		premess.makeMessage(Device->getDeviceID(), 0x00,
				PriorityMessage::MESSTYPE_DECDATA, PriorityMessage::NOACK,
				std::make_shared<std::string>(BinaryConverter::int8toStr(pri)));
		mess.makeMessage(Device->getDeviceID(), 0x00,
				PriorityMessage::MESSTYPE_DATA, PriorityMessage::NOACK,
				std::make_shared<std::string>(senddata));

		std::cerr << "[DV] State8 id : " << dId << std::endl;
		std::cerr << "[DV] State8 contentsize : " << content->second.length()
				<< std::endl;
		std::cout << iEntity::strnowtime() << ";Data send;DeviceID;"
				<< Device->getDeviceID() << ";DataID;" << dId << ";;;priority;"
				<< pri << std::endl;
		Device->MessCount += mess.getContentData()->length();
		std::cout << iEntity::strnowtime() << "packet(send data) ;DataID;"
				<< dId << ";bytes;" << mess.getContentData()->length()
				<< ";total MessData;" << Device->MessCount << std::endl;

		Device->ap_send(premess.getAllData(), -1);
		Device->ap_send(mess.getAllData(), -1);
		std::cout << iEntity::strnowtime() << ";Data complete;DeviceID;"
				<< Device->getDeviceID() << ";DataID;" <<dId<< std::endl;
	}
}

int D_State51::getDataID() {
	return DataID++;
}
} /* namespace IoTPriority */
