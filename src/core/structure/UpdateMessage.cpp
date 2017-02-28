/*
 * InterruptMessage.cpp
 *
 *  Created on: 2016/11/09
 *      Author: tachibana
 */

#include "UpdateMessage.h"

namespace IoTPriority {

UpdateMessage::UpdateMessage(){
	// TODO 自動生成されたコンストラクター・スタブ
}

UpdateMessage::~UpdateMessage() {
	// TODO Auto-generated destructor stub
}

bool UpdateMessage::set(int id,float value){
	if(!isNeedvalue(id))
		return false;
	Id=id;
	Value=value;
	return true;
}

bool UpdateMessage::set(int id){
	if(isNeedvalue(id))
		return false;
	Id=id;
	return true;
}

int UpdateMessage::getId(){
	return Id;
}

float UpdateMessage::getValue(){
	if(isNeedvalue(Id))
		return Value;
	return -1;
}

std::shared_ptr<std::string> UpdateMessage::getMessage() {
	std::string data = "";
	data+=BinaryConverter::int8toStr(Id);
	if(isNeedvalue(Id))
		data+=BinaryConverter::floattoStr(Value);
	return std::make_shared<std::string>(data);
}

void UpdateMessage::putMessage(std::string data){
	Id=BinaryConverter::strtoInt8(data);
	if(isNeedvalue(Id))
		Value=BinaryConverter::strtoFloat(data.substr(1));
}

bool UpdateMessage::isNeedvalue(int id){
	switch(id){
	case ABS_VALUE:
	case REL_VALUE:
		return true;
	default:
		return false;
	}
}


} /* namespace IoTPriority */
