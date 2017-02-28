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
	if (argc != 7) {
		std::cerr
				<< "usage- hostname deviceID filename1 Datatype1 filename2 Datatype2"
				<< std::endl;
		return -1;
	}
	std::string hostname = argv[1];
	int deviceID = atoi(argv[2]);
	std::string filename1 = argv[3];
	std::string datatype1 = argv[4];
	std::string filename2 = argv[5];
	std::string datatype2 = argv[6];
	auto device = std::make_shared<IoTPriority::IoTDevices>();
	device->api_setBroker(hostname);
	device->api_setDeviceID(deviceID);
	device->connectAPServer(hostname, 31400);

	std::ifstream ifs1(filename1), ifs2(filename2);
	if (ifs1.fail()) {
		std::cerr << "file1 not found" << std::endl;
		return -1;
	}
	if (ifs2.fail()) {
		std::cerr << "file2 not found" << std::endl;
		return -1;
	}
	std::string datastr1((std::istreambuf_iterator<char>(ifs1)),
			std::istreambuf_iterator<char>());
	std::string datastr2((std::istreambuf_iterator<char>(ifs2)),
			std::istreambuf_iterator<char>());
	std::cerr << "wait 10 sec..." << std::endl;
	sleep(10);
	device->initState();
	std::cerr << "conecting brserver" << std::endl;
	std::thread th(&IoTPriority::IoTDevices::api_resist, device, 10);
	for (int i = 0; i <= 120; i += 2) {
		int did = i + 1 <= 100 ? i + 1 : i + 201;
		device->api_sendData(datatype1, std::string(datastr1));
		std::cout << IoTPriority::iEntity::strnowtime() << ";Data put;DeviceID;"
				<< deviceID << ";DataID;" << did << ";type;" << datatype1
				<< std::endl;
		did++;
		device->api_sendData(datatype2, std::string(datastr2));
		std::cout << IoTPriority::iEntity::strnowtime() << ";Data put;DeviceID;"
				<< deviceID << ";DataID;" << did << ";type;" << datatype2
				<< std::endl;
		sleep(60);
	}
	std::cerr << "end!!" << std::endl;
	th.detach();

	return 1;
}

