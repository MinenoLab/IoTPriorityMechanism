/*
 * IoTDeviceBuffer.cpp
 *
 *  Created on: 2016/03/06
 *      Author: tachibana
 */

#include "IoTDeviceBuffer.h"

namespace IoTPriority {

IoTDeviceBuffer::IoTDeviceBuffer():priority(0),datasize(0){
	// TODO 自動生成されたコンストラクター・スタブ

}

IoTDeviceBuffer::~IoTDeviceBuffer() {
	// TODO Auto-generated destructor stub
}

void IoTDeviceBuffer::setData(float datasize,std::string data){
	this->datasize=datasize;
	this->data=data;
}
std::string IoTDeviceBuffer::getData(){
	return data;
}
float IoTDeviceBuffer::getSize(){
	return datasize;
}

void IoTDeviceBuffer::setPriority(int priority){
	this->priority=priority;
}
int IoTDeviceBuffer::getPriority(){
	return priority;
}

} /* namespace IoTPriority */
