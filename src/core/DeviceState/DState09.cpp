/*
 * DState09.cpp
 *
 *  Created on: 2016/02/29
 *      Author: tachibana
 */

#include "../IoTDevices.h"
#include "../utils/BinaryConverter.h"

namespace IoTPriority {

D_State09::D_State09(std::shared_ptr<IoTDevices> device) :
		device(device) {

}

D_State09::~D_State09() {
}

std::string D_State09::execute() {
	std::cerr << "[DV] State9" << std::endl;
	std::vector<std::shared_ptr<PriorityMessage>> buf;
	std::shared_ptr<PriorityMessage> data;

#ifdef CHECK
	auto p = std::make_shared<PriorityMessage>();
	p->makeMessage(0x00, device->DeviceID, PriorityMessage::MESSTYPE_DATA,
			PriorityMessage::ACK,
			std::make_shared<std::string>(std::to_string(1)));
	device->putMessage(true,p);
#endif

	while (1) {
		auto d = device->ap_recv_and_getdata();
		std::cerr << "[DV] State9 messtype : " << d->getMessType() << " ,ack "
				<< d->getAckType() << std::endl;
		if (d->getMessType() == PriorityMessage::MESSTYPE_DATA
				&& d->getAckType() == PriorityMessage::ACK) {
			data = d;
			break;
		}
		buf.push_back(d);
	}
	device->OtherMessCount += data->getAllData()->length();
	std::cout << iEntity::strnowtime() << "other packet(recv ackdata) ;"
			<< data->getAllData()->length() << ";total otherData;"
			<< device->OtherMessCount << std::endl;

	PriorityMessage compmess;
	compmess.makeMessage(device->DeviceID, 0xff,
			PriorityMessage::MESSTYPE_COMPLETE, PriorityMessage::NOACK,
			data->getContentData());

	int dataID = BinaryConverter::strtoInt16(*(data->getContentData()));
	device->Buffer.erase(dataID);
	std::cout << iEntity::strnowtime() << ";Data complete;DeviceID;"
			<< device->getDeviceID() << ";DataID;" << dataID << std::endl;
	device->OtherMessCount += compmess.getAllData()->length();
	std::cout << iEntity::strnowtime() << "other packet(send complete) ;"
			<< data->getAllData()->length() << ";total otherData;"
			<< device->OtherMessCount << std::endl;
	std::cerr << "[DV] State9 compDataID  bufsize : :" << dataID << " , "
			<< device->getBuffer()->size() << std::endl;
	bool sendstatus=device->br_send(compmess.getAllData());
	if(!sendstatus)
			device->br_startReconnect();

	for (auto it : buf)
		device->ap_recvqueue_repush(it);
	return "";
}

} /* namespace IoTPriority */
