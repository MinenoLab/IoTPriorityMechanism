/*
 * PermitFormat.cpp
 *
 *  Created on: 2016/04/20
 *      Author: tachibana
 */

#include "PermitFormat.h"

namespace IoTPriority {

PermitFormat::PermitFormat() {
	// TODO 自動生成されたコンストラクター・スタブ

}

PermitFormat::~PermitFormat() {
	// TODO Auto-generated destructor stub
}

void PermitFormat::set(int dataid, float relativeRate) {
	Dataid = dataid;
	RelativeRate = relativeRate;
}
int PermitFormat::getDataId() {
	return Dataid;
}
float PermitFormat::getRelativeRate() {
	return RelativeRate;
}
std::shared_ptr<std::string> PermitFormat::getMessage() {
	std::string data = "";
	data+=BinaryConverter::int16toStr(Dataid);
	data+=BinaryConverter::floattoStr(RelativeRate);
	return std::make_shared<std::string>(data);
}
void PermitFormat::putMessage(std::string data) {
	Dataid=BinaryConverter::strtoInt16(data);
	RelativeRate=BinaryConverter::strtoFloat(data.substr(2));
	std::cerr << "Trate : " << RelativeRate<< std::endl;
}

} /* namespace IoTPriority */
