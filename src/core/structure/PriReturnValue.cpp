/*
 * PriReturnValue.cpp
 *
 *  Created on: 2016/04/20
 *      Author: tachibana
 */

#include "PriReturnValue.h"

namespace IoTPriority {

PriReturnValue::PriReturnValue() {
	// TODO 自動生成されたコンストラクター・スタブ
}

PriReturnValue::~PriReturnValue() {
	// TODO Auto-generated destructor stub
}

int PriReturnValue::set(int dataID, int sclass, int priority) {
	DataID = dataID;
	if (sclass < 0x0 || sclass > 0x2)
		return -1;
	Sclass = sclass;
	Priority = priority;
	return 1;
}

std::shared_ptr<std::string> PriReturnValue::getMessage() {
	std::string mess = "";
	mess +=BinaryConverter::int16toStr(DataID);
	unsigned char c = (unsigned char) (((uint32_t)Sclass << 4) + ((uint32_t)Priority & 0x0f));
	mess += c;
	return std::make_shared<std::string>(mess);
}

int PriReturnValue::calcSclass(float datasize) {
	if (datasize >= th_L)
		return CLASS_L;
	if (datasize >= th_M)
		return CLASS_M;
	return CLASS_S;
}

int PriReturnValue::getdataID() {
	return DataID;
}

int PriReturnValue::getSclass() {
	return Sclass;
}

int PriReturnValue::getPriority() {
	return Priority;
}

void PriReturnValue::putMessage(std::string mess) {
	DataID=BinaryConverter::strtoInt16(mess);
	const char*	d = mess.substr(2, 1).c_str();
	Sclass = (uint32_t) ((unsigned char)d[0] >> 4);
	Priority = (uint32_t) ((unsigned char)d[0] & 0x0f);
}
} /* namespace IoTPriority */
