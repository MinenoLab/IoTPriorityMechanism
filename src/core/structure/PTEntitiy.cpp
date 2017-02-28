/*
 * PriorityTable.cpp
 *
 *  Created on: 2016/04/05
 *      Author: tachibana
 */

#include "PTEntitiy.h"
#include <unistd.h>

namespace IoTPriority {

PTEntity::PTEntity(int deviceId, std::string dataType, int dataIntervalSeconds,
		int Priority) :
		isAnything(std::make_tuple<bool, bool, bool>(false, false, false)), initused(
				false) {
	// TODO 自動生成されたコンストラクター・スタブ
	set(deviceId, dataType, dataIntervalSeconds, Priority);
	gettimeofday(&usedtime, NULL);
}

PTEntity::~PTEntity() {
	// TODO Auto-generated destructor stub

}

int PTEntity::set(int deviceId, std::string dataType, int dataIntervalSeconds,
		int priority) {
	int total = set_id(deviceId);
	set_type(dataType);
	set_intervalSeconds(dataIntervalSeconds);
	total += set_priority(priority);
	//2つの関数の戻り値が共に正なら1，そうでなければ-1
	return total == 2 ? 1 : -1;
}

void PTEntity::set_anything(bool any_DeviceID, bool any_Datatype,
		bool any_DataInterval) {
	isAnything = std::make_tuple(any_DeviceID, any_Datatype, any_DataInterval);
}

int PTEntity::get_intervalSeconds() const {
	return DataIntervalSeconds;
}

int PTEntity::set_id(int id) {
	if (id > 65536 || id < 0)
		return -1;
	DeviceID = id;
	return 1;
}

void PTEntity::set_type(std::string type) {
	DataType = type;
}

void PTEntity::set_intervalSeconds(int val) {
	DataIntervalSeconds = val;
}

int PTEntity::set_priority(int pri) {
	if (pri > 15 || pri < 0)
		return -1;
	Priority = pri;
	return 1;
}

std::string PTEntity::get_type() const {
	return DataType;
}

int PTEntity::get_id() const {
	return DeviceID;
}

int PTEntity::get_priority() const {
	return Priority;
}

struct timeval PTEntity::get_usedtime() const {
	return usedtime;
}

void PTEntity::notifyUsed() {
	gettimeofday(&usedtime, NULL);
	initused = true;
}

int PTEntity::matchCondition(int deviceId, std::string dataType,
		struct timeval datatime) {
	if (!std::get<0>(isAnything) && DeviceID != deviceId) {
		return -1;
	}
	if (!std::get<1>(isAnything) && DataType != dataType) {
		return -1;
	}
	if (!std::get<2>(isAnything)
			&& datatime.tv_sec - usedtime.tv_sec < DataIntervalSeconds - 5
			&& initused) {
		return -1;
	}
	return 1;
}

std::string PTEntity::getstr() {
	return "Table: Devid " + std::to_string(DeviceID) + ", Type " +DataType + ", Interval "
			+ std::to_string(DataIntervalSeconds) + ", priority " + std::to_string(Priority);
}

} /* namespace IoTPriority */

//Test codes-------------------------------------------------------------------------
/*

 int main(){
 IoTPriority::PTEntity ent(5,"jpg",20,5);
 std::cout<<"Deviceid: "<<ent.get_id()<<std::endl;
 std::cout<<"Type: "<<ent.get_type()<<std::endl;
 std::cout<<"seconds: "<<ent.get_intervalSeconds()<<std::endl;
 struct timeval tm;
 gettimeofday(&tm,NULL);
 std::cout<<"match(5,\"jpg\",tm): "<<ent.matchCondition(5,"jpg",tm)<<std::endl;
 std::cout<<"match(5,\"jpg\",tm): "<<ent.matchCondition(5,"txt",tm)<<std::endl;
 ent.set_anything(false,true,false);
 std::cout<<"match(5,*,tm): "<<ent.matchCondition(5,"txt",tm)<<std::endl;
 ent.notifyUsed();
 sleep(5);
 gettimeofday(&tm,NULL);
 std::cout<<"match(5,\"jpg\",tm): "<<ent.matchCondition(5,"jpg",tm)<<std::endl;
 sleep(20);
 gettimeofday(&tm,NULL);
 std::cout<<"match(5,\"jpg\",tm): "<<ent.matchCondition(5,"jpg",tm)<<std::endl;

 }
 */
