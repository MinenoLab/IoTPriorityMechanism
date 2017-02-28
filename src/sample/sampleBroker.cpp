/*
 * sampleBroker.cpp
 *
 *  Created on: 2016/05/01
 *      Author: tachibana
 */

#include "../core/BrokerServer.h"
#include <thread>
#include <iostream>
#include <memory>
#include <fstream>
#include <iterator>

int main(int argc, char *argv[]) {
	auto broker = std::make_shared<IoTPriority::BrokerServer>(35200, 35225);
	if (argc == 2) {
		std::string filename = argv[1];
		std::ifstream ifs(filename);
		if (ifs.fail()) {
			std::cerr << "file not found" << std::endl;
			return -1;
		}
		std::string jsondata((std::istreambuf_iterator<char>(ifs)),
				std::istreambuf_iterator<char>());
		broker->putpriority(jsondata);
		std::cerr<<"json read"<<std::endl;
	}else{
		std::cerr<<"no json"<<std::endl;
	}
	std::thread th(&IoTPriority::BrokerServer::run_Entitiy, broker);
	sleep((60 + 5) * 60);
	//終了処理
	broker->flushPriorityqueue();
	th.detach();
	return 1;
}
