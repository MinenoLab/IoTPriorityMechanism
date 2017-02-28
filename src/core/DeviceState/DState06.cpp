/*
 * DState06.cpp
 *
 *  Created on: 2016/04/06
 *      Author: tachibana
 */

#include "../IoTDevices.h"

namespace IoTPriority {
//一時バグ回避として31番スタート
//TODO 全データのunsigned化
D_State06::D_State06(std::shared_ptr<IoTDevices> device) :
		Device(device), DataID(1) {
	// TODO 自動生成されたコンストラクター・スタブ
}

D_State06::~D_State06() {
	// TODO Auto-generated destructor stub
}
std::string D_State06::execute() {
	std::cerr << "[DV] State6" << std::endl;
	PriorityMessage mess;
	MetaData meta;
	//コンテンツデータ取得
	auto content = Device->putDeviceQueue();
	//メタデータ生成
	int dId = getDataID();
	meta.set(dId, Device->getDeviceID(), content->first,
			(float) content->second.size());
	//バッファにデータ退避
	auto buf = std::make_shared<IoTDeviceBuffer>();
	buf->setData((float) content->second.size(), content->second);
	auto ret =Device->Buffer.insert(std::make_pair(dId, buf));
	std::cerr<<"[DV] State6 metaData id size: "<<dId<<" , "<<content->second.length()<<std::endl;
#ifdef CHECK
	std::cerr << "[DV] State6 haskey: " <<Device->getBuffer()->count(dId)<<std::endl;
#endif
	//送信
	mess.makeMessage(Device->getDeviceID(), iEntity::BROKER_DEVICEID,
			PriorityMessage::MESSTYPE_META, PriorityMessage::NOACK,
			std::make_shared<std::string>(meta.getMetadata()));
	std::cerr<<"[DV] State6 metaData id,type : "<<meta.getDataID()<<" , "<<meta.getDataType()<<std::endl;
	Device->OtherMessCount+=mess.getAllData()->length();
	std::cout<<iEntity::strnowtime()<<"other packet(send Metadata) ;"<<mess.getAllData()->length()<<";total otherData;"<<Device->OtherMessCount<<std::endl;
	#ifdef CHECK
	testinit();
#else
	bool sendstatus = Device->br_send(mess.getAllData());
	if(!sendstatus)
		Device->br_startReconnect();
#endif
	return "";
}

void D_State06::testinit() {
	PriReturnValue v;
	v.set(1, PriReturnValue::CLASS_M, 5);
	auto p = std::make_shared<PriorityMessage>();
	p->makeMessage(0xff, Device->DeviceID, PriorityMessage::MESSTYPE_PRI,
			PriorityMessage::NOACK, v.getMessage());
	PermitFormat pf;
	pf.set(1, 15);
	auto p2 = std::make_shared<PriorityMessage>();
	p2->makeMessage(0xff, Device->DeviceID, PriorityMessage::MESSTYPE_PERMIT,
			PriorityMessage::NOACK, pf.getMessage());
	std::string n = "";
	auto p3 = std::make_shared<PriorityMessage>();
	p3->makeMessage(0xff, Device->DeviceID, PriorityMessage::MESSTYPE_CALLOFF,
			PriorityMessage::NOACK, std::make_shared<std::string>(n));
	Device->putMessage(false, p);
	Device->putMessage(false, p2);
	Device->putMessage(false, p3);
}

int D_State06::getDataID() {
	return DataID++;
}

} /* namespace IoTPriority */
