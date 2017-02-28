/*
 * metaData.cpp
 *
 *  Created on: 2016/04/11
 *      Author: tachibana
 */

#include "MetaData.h"
#include <iostream>

namespace IoTPriority {

MetaData::MetaData() {
	// TODO 自動生成されたコンストラクター・スタブ
}

MetaData::~MetaData() {
	// TODO Auto-generated destructor stub
}

int MetaData::set(int dataID, int deviceID, std::string dataType,
		float dataSize) {
	DataID = dataID;
	DeviceID = deviceID;
	DataType = dataType;
	DataSize = dataSize;
	return 1;
}

int MetaData::getDataID() {
	std::cerr << "metadata ID : " << DataID << std::endl;
	return DataID;
}
int MetaData::getDeviceID() {
	return DeviceID;
}
std::string MetaData::getDataType() {
	return DataType;
}
float MetaData::getDataSize() {
	return DataSize;
}
int MetaData::set_fromMetaData(std::string data) {
	DataID = BinaryConverter::strtoInt16(data);
	DeviceID = BinaryConverter::strtoInt8(data.substr(2));
	DataSize = BinaryConverter::strtoFloat(data.substr(3));
	DataType = data.substr(7);
	return 1;
}
std::string MetaData::getMetadata() {
	std::string data = "";
	data += BinaryConverter::int16toStr(DataID);
	data += BinaryConverter::int8toStr(DeviceID);
	data += BinaryConverter::floattoStr(DataSize);
	for (auto c : DataType)
		data += c;
	return data;
}
} /* namespace IoTPriority */

//----------------Test Codes----------------------
/*
 int main() {
 IoTPriority::MetaData mt1, mt2;
 mt1.set(5, 10, "txt", 25.257);
 std::cout << "DataSize : " << mt1.getDataSize() << std::endl;
 auto data = mt1.getMetadata();
 mt2.set_fromMetaData(data);
 std::cout << "dataID " << mt2.getDataID() << std::endl;
 std::cout << "deviceID " << mt2.getDeviceID() << std::endl;
 std::cout << "Datatype " << mt2.getDataType() << std::endl;
 std::cout << "DataSize :" <<mt2.getDataSize() << std::endl;
 }
*/

