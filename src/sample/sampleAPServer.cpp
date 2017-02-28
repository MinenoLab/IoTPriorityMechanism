/*
 * sampleAPServer.cpp
 *
 *  Created on: 2016/05/01
 *      Author: tachibana
 */

#include "../core/AppServer.h"
#include <thread>
#include <iostream>
#include <memory>

int main() {
	auto server = std::make_shared<IoTPriority::AppServer>();
	server->initState();
	server->api_setBroker("127.0.0.1");
	std::thread th(&IoTPriority::AppServer::api_Request, server);
	sleep(64*60);
	server->api_stop();
	std::cout << "api_stop:" << std::endl;
	th.join();
	return 1;
}

