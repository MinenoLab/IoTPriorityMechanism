/*
 * iEntity.cpp
 *
 *  Created on: 2016/02/26
 *      Author: tachibana
 */

#include "iEntity.h"

namespace IoTPriority {

iEntity::iEntity() {
	// TODO 自動生成されたコンストラクター・スタブ

}

iEntity::~iEntity() {
	// TODO Auto-generated destructor stub
}

int iEntity::nextState() {
	auto it = statemap.find(curState->getNextStateName());
	if (it == statemap.end())
		return -1;
	curState = it->second;
	return 1;
}

std::shared_ptr<StateSwicher> iEntity::getCurrentState() {
	return curState;
}

std::string iEntity::execState(std::string stateName) {
	auto it = statemap.find(stateName);
	if (it == statemap.end())
		return "";
	curState = it->second;
	auto nextname = curState->state->execute();
	return nextname;
}

std::string iEntity::execFirstState() {
	curState = firstState;
	auto nextname = curState->state->execute();
	return nextname;
}

std::string iEntity::execCurState() {
	auto nextname = curState->state->execute();
	return nextname;
}

int iEntity::registFirstState(std::string stateName) {
	auto it = statemap.find(stateName);
	if (it == statemap.end())
		return -1;
	firstState = it->second;
	return 1;

}

int iEntity::registState(std::string curStateName,
		std::shared_ptr<StateSwicher> curState) {
	statemap.insert(std::make_pair(curStateName, curState));
	return 1;
}

std::string iEntity::strnowtime(){
	std::string str;
	char s[64];
	struct timeval tmv;
	struct tm *time_st;
	gettimeofday(&tmv,NULL);
	time_st=localtime(&tmv.tv_sec);
	sprintf(s,"%d.%03d;%02d:%02d:%02d.%03d;",tmv.tv_sec,tmv.tv_usec/10000,time_st->tm_hour,time_st->tm_min,time_st->tm_sec,tmv.tv_usec/10000);
	str=s;
	return str;
}

} /* namespace IoTPriority */
