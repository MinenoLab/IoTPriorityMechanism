/*
 * sampleDevice.cpp
 *
 *  Created on: 2016/05/01
 *      Author: tachibana
 */

#include "../core/IoTDevices.h"
#include "../core/iEntity.h"
#include <thread>
#include <iostream>
#include <memory>
#include <fstream>
#include <iterator>

int main(int argc, char *argv[]) {
	if (argc != 5) {
		std::cerr << "usage- hostname deviceID filename Datatype" << std::endl;
		return -1;
	}
	std::string hostname = argv[1];
	int deviceID = atoi(argv[2]);
	std::string filename = argv[3];
	std::string datatype = argv[4];
	auto device = std::make_shared<IoTPriority::IoTDevices>();
	device->api_setBroker(hostname);
	device->api_setDeviceID(deviceID);
	device->connectAPServer(hostname, 31400);

	std::ifstream ifs(filename);
	if (ifs.fail()) {
		std::cerr << "file not found" << std::endl;
		return -1;
	}
	std::string datastr((std::istreambuf_iterator<char>(ifs)),
			std::istreambuf_iterator<char>());
	std::cerr << "wait 10 sec..." << std::endl;
	sleep(10);
	device->initState();
	std::cerr << "conecting brserver" << std::endl;
	std::thread th(&IoTPriority::IoTDevices::api_resist, device, 10);
	for (int i = 0; i < 60; i++) {
		device->api_sendData(datatype, std::string(datastr));
		std::cout << IoTPriority::iEntity::strnowtime() << ";Data put;DeviceID;"
				<< deviceID << ";DataID;" << (i + 1) << ";type;" << datatype
				<< std::endl;
		sleep(60);
	}
	std::cerr << "end!!" << std::endl;
	th.detach();

	return 1;
}
//メモ代わり
/*
 void settings(){
 //初期設定
 auto device = std::make_shared<IoTPriority::IoTDevices>();
 device->api_setBroker(hostname);
 device->api_setDeviceID(deviceID);
 device->connectAPServer(hostname, 31400);

 std::thread th(&IoTPriority::IoTDevices::api_resist, device, 10);
 //データ送信
 device->api_sendData(datatype, std::string(datastr));
 //終了
 th.detach();
 }
 */

