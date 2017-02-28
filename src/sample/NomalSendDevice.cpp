/*
 * NomalSendDevice.cpp
 *
 *  Created on: 2016/05/06
 *      Author: tachibana
 */

#include <iostream>
#include <memory>
#include <thread>
#include <time.h>
#include <sys/time.h>
#include <fstream>
#include <iterator>
#include "../core/iEntity.h"
#include "../core/NormalIoTDevices.h"

void onrecv_nomal(std::string data, int sesid) {
	std::cerr << "recv!!  :" << data.substr(0, 30) << std::endl;
}

int main(int argc, char *argv[]) {
	std::thread th;
	if (argc != 5 && argc != 6) {
		std::cerr << "usage- hostname deviceID filename Datatype (jsondatafile)" << std::endl;
		return -1;
	}
	std::string hostname = argv[1];
	int deviceID = atoi(argv[2]);
	std::string filename = argv[3];
	std::string datatype = argv[4];
	std::ifstream ifs(filename);
	if (ifs.fail()) {
		std::cerr << "file not found" << std::endl;
		return -1;
	}
	std::string datastr((std::istreambuf_iterator<char>(ifs)),
			std::istreambuf_iterator<char>());
	auto device = std::make_shared<IoTPriority::NormalIoTDevices>();
	device->api_setDeviceID(deviceID);
	device->connectAPServer(hostname, 31400);
	device->initState();
	if (argc == 6) {
		std::string filename_j = argv[5];
		std::ifstream ifsj(filename_j);
		if (ifsj.fail()) {
			std::cerr << "file not found" << std::endl;
			return -1;
		}
		std::string jsondata((std::istreambuf_iterator<char>(ifsj)),
				std::istreambuf_iterator<char>());
		std::cerr << "json read" << std::endl;
		th=std::thread(&IoTPriority::NormalIoTDevices::api_resist, device, 10,jsondata);
	} else {
		th=std::thread(&IoTPriority::NormalIoTDevices::api_resist, device, 10,"");
	}
	std::cerr << "wait 10 sec..." << std::endl;
	sleep(10);
	for (int i = 0; i < 60; i++) {
		device->api_sendData(datatype, std::string(datastr));
		std::cout << IoTPriority::iEntity::strnowtime() << ";Data put;DeviceID;"
				<< deviceID << ";DataID;" << (i + 1) << ";type;" << datatype
				<< std::endl;
		sleep(60);
	}
	th.detach();
}
