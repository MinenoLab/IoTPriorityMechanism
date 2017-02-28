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
	if (argc != 7 && argc != 8) {
		std::cerr << "usage- hostname deviceID filename1 Datatype1 filename2 Datatype2 (jsonfilename)" << std::endl;
		return -1;
	}
	std::string hostname = argv[1];
	int deviceID = atoi(argv[2]);
	std::string filename1 = argv[3];
	std::string datatype1 = argv[4];
	std::string filename2 = argv[5];
	std::string datatype2 = argv[6];
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
	auto device = std::make_shared<IoTPriority::NormalIoTDevices>();
	device->api_setDeviceID(deviceID);
	device->connectAPServer(hostname, 31400);
	device->initState();
	if (argc == 8) {
		std::string filename_j = argv[7];
		std::ifstream ifsj(filename_j);
		if (ifsj.fail()) {
			std::cerr << "file not found" << std::endl;
			return -1;
		}
		std::string jsondata((std::istreambuf_iterator<char>(ifsj)),
				std::istreambuf_iterator<char>());
		std::cerr << "json read" << std::endl;
		th=std::thread(&IoTPriority::NormalIoTDevices::api_resist, device, 10, jsondata);
	} else {
		th=std::thread(&IoTPriority::NormalIoTDevices::api_resist, device, 10,"");
	}
	std::cerr << "wait 10 sec..." << std::endl;
	sleep(10);
	for (int i = 0; i < 120; i += 2) {
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
	th.detach();
}
