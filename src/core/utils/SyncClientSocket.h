/*
 * SyncClientSocket.h
 *
 *  Created on: 2016/04/22
 *      Author: tachibana
 */

#ifndef UTILS_SYNCCLIENTSOCKET_H_
#define UTILS_SYNCCLIENTSOCKET_H_

#include <iostream>
#include <thread>
#include <memory>
#include <mutex>
#include <unistd.h>
#include <boost/asio.hpp>
#include "SyncBaseSockSession.h"

namespace asio = boost::asio;
using asio::ip::tcp;

namespace IoTPriority {

class Sync_ClientSocket {
public:
	Sync_ClientSocket(asio::io_service& io_service, std::string hostname, int port,
			std::function<void(std::string, int)> recvfunc);
	virtual ~Sync_ClientSocket();
	bool basic_send(std::shared_ptr<std::string> data);
	std::string basic_recv();
	void start_sendrecv_forever(bool send,bool recv);
	void stop_sendrecv_forever();
	void pause_sendrecv_forever();
	void limit_send(std::shared_ptr<std::string> data,float rate);
	void resumSend();
	void changeSendData();
	void changeSendRate(float rate);
	void changeSendRateAbs(float rate);
	Sync_BaseSockSession::SockState checkState();
	bool reconnect();

private:

	std::shared_ptr<Sync_BaseSockSession> Session;
	tcp::socket sock;
	asio::io_service& Io_service;
	std::string hostname;
	std::function<void(std::string, int)> On_recv_func;
	int port;
	bool isReady;
	bool sendforever,recvforever;
	std::mutex rcmtx;

};

} /* namespace IoTPriority */

#endif /* UTILS_SYNCCLIENTSOCKET_H_ */
