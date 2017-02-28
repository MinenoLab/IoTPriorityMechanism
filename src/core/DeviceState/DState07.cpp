/*
 * DState07.cpp
 *
 *  Created on: 2016/04/06
 *      Author: tachibana
 */

#include <memory>
#include "../IoTDevices.h"

namespace IoTPriority {

D_State07::D_State07(std::shared_ptr<IoTDevices> device) :
		Device(device) {
}

D_State07::~D_State07() {
}

std::string D_State07::execute() {
	std::cerr << "[DV] State7" << std::endl;
	auto d = Device->br_recv_and_getdata();
	PriReturnValue value;
	value.putMessage(*(d->getContentData()));
	std::cerr << "[DV] State7 DataID : " << value.getdataID() << std::endl;
	//帰ってきた優先度をバッファ中の該当データと関連付け;
	auto it = Device->Buffer.find(value.getdataID());
	if (it == Device->Buffer.end())
		return "";
	it->second->setPriority(value.getPriority());
	std::cerr << "[DV] State7 priority= " << value.getPriority() << std::endl;
	Device->OtherMessCount += d->getAllData()->length();
	std::cout << iEntity::strnowtime() << ";recv priority;DeviceID;"
			<< Device->getDeviceID() << ";DataID;" << value.getdataID()
			<< ";;;priority;" << value.getPriority() << std::endl;
	std::cout << iEntity::strnowtime() << "other packet(recv priority) ;"
			<< d->getAllData()->length() << ";total otherData;"
			<< Device->OtherMessCount << std::endl;
	//バッファ空き容量整理
	struct timeval tm;
	gettimeofday(&tm, NULL);
	Device->putRemoveQueue(value.getPriority(), tm, value.getdataID());
	long sub = calcBufferSize() - (Device->Buffersize * 0.8);
	std::cerr << "[DV] State7 buffersub = " << sub << std::endl;
	if (sub >= 0) {
		auto rbuf = removeBufferData(sub);
		for (auto r : rbuf) {
			std::cerr << "removed at DataID: " << r << std::endl;
			std::cout << iEntity::strnowtime() << ";remove Data;DeviceID;"
					<< Device->getDeviceID() << ";DataID;" << r<< std::endl;
		}
	}
	return "";
}

long D_State07::calcBufferSize() {
	long total = 0;
	for (auto it : *(Device->getBuffer()))
		total += (long) it.second->getSize();
	std::cerr<<"buffer total : "<<total<<std::endl;
	return total;
}

std::vector<int> D_State07::removeBufferData(long sub) {
	long total = 0;
	std::vector<int> removeIDs;
	std::cerr<<"remove start"<<std::endl;
	while (total < sub) {
		int dataid = Device->getRemovedata();
		std::cerr<<"buffer removed id : "<<dataid<<std::endl;
		auto it = Device->Buffer.find(dataid);
		if (it !=Device->Buffer.end()) {
			total += it->second->getSize();
			Device->Buffer.erase(dataid);
			removeIDs.push_back(dataid);
		}
		std::cerr<<"total remove bytes : "<<total<<std::endl;
	}
	return removeIDs;
}

} /* namespace IoTPriority */
