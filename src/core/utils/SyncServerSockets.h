/*
 * SyncServerSockets.h
 *
 *  Created on: 2016/04/22
 *      Author: tachibana
 */

#ifndef UTILS_SYNCSERVERSOCKETS_H_
#define UTILS_SYNCSERVERSOCKETS_H_

#include <iostream>
#include <thread>
#include <memory>
#include <mutex>
#include <unistd.h>
#include <boost/asio.hpp>
#include "SyncBaseSockSession.h"


using boost::asio::ip::tcp;

namespace IoTPriority {

class Sync_ServerSockets {
public:
	Sync_ServerSockets(boost::asio::io_service& io_service, int port,
			std::function<void(std::string, int)> recvfunc);
	virtual ~Sync_ServerSockets();
	void accept(int maxaccept);
	bool basic_send(std::shared_ptr<std::string> data,int sesid);
	std::string basic_recv(int sesid);
	void start_sendrecv_forever(bool send,bool recv);
	void pause_sendrecv_forever(int sesid);
	void stop_sendrecv_forever();
	void limit_send(std::shared_ptr<std::string> data, float rate,int sesid);
	void setchangeListener(std::function<void(int)> func);
	std::map<int,std::pair<std::shared_ptr<Sync_BaseSockSession>, tcp::endpoint>> acceptedSocks;
	Sync_BaseSockSession::SockState checkState(int sesid);
	void closeSession(int sesid);

private:
	std::shared_ptr<tcp::acceptor> Acceptor;
	tcp::socket sock;
	int sockIdCounter;
	boost::asio::io_service& Io_service;
	int port;
	bool isReady;
	std::function<void(std::string, int)> On_recv_func;
	std::function<void(int)> changefunc;
	bool sendforever,recvforever;
	std::mutex mtx;

};

} /* namespace IoTPriority */

#endif /* UTILS_SYNCSERVERSOCKETS_H_ */
