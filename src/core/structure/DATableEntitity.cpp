/*
 * DATableEntitity.cpp
 *
 *  Created on: 2016/04/06
 *      Author: tachibana
 */

#include "DATableEntitity.h"

namespace IoTPriority {

DATableEntitity::DATableEntitity(std::vector<std::string> types) :Datatypes(types){
	// TODO 自動生成されたコンストラクター・スタブ
	isLive=true;
	gettimeofday(&LastLiveTime, NULL);
}

DATableEntitity::~DATableEntitity() {
	// TODO Auto-generated destructor stub
}

const std::vector<std::string> splitDatatypes(std::string str_types){
	std::vector<std::string> v;
	std::stringstream ss(str_types);
	std::string buffer;
	while (std::getline(ss, buffer, ',')) {
		v.push_back(buffer);
	}
	return v;
}

void DATableEntitity::notifylive(){
	isLive=true;
	gettimeofday(&LastLiveTime, NULL);
}

struct timeval& DATableEntitity::getLastLiveTime(){
	return LastLiveTime;
}

bool DATableEntitity::islive(){
	//TODO liveの仕組みをきちんとつくる
	return isLive;
}

} /* namespace IoTPriority */
