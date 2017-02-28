/*
 * SyncClientSocket.cpp
 *
 *  Created on: 2016/04/22
 *      Author: tachibana
 */

#include "SyncClientSocket.h"

namespace IoTPriority {

Sync_ClientSocket::Sync_ClientSocket(asio::io_service& io_service,
		std::string hostname, int port,
		std::function<void(std::string, int)> recvfunc) :
		Io_service(io_service), hostname(hostname), port(port), sock(
				io_service), On_recv_func(recvfunc), isReady(false) {
	boost::system::error_code error;
	sock.connect(tcp::endpoint(asio::ip::address::from_string(hostname), port),
			error);
	if (error) {
		std::cout << "connect failed : " << error.message() << std::endl;
	} else {
		std::cout << "connected" << std::endl;
		Session = std::make_shared<Sync_BaseSockSession>(std::move(sock),
				On_recv_func);
		isReady = true;
	}
	std::cout << "connect end" << std::endl;
}

Sync_ClientSocket::~Sync_ClientSocket() {
	// TODO Auto-generated destructor stub

}

bool Sync_ClientSocket::reconnect() {
	std::unique_lock<std::mutex> lk(rcmtx);
	std::cerr<<"recconect start"<<std::endl;
	boost::system::error_code error;
	pause_sendrecv_forever();
	Session->notifyRecconect();
	sock = asio::basic_stream_socket<tcp>(Io_service);
	sock.connect(tcp::endpoint(asio::ip::address::from_string(hostname), port),
			error);
	if (error) {
		std::cout << "reconnect failed : " << error.message() << std::endl;
		std::cerr << "reconnect failed : " << error.message() << std::endl;
	} else {
		std::cout << "reconnect success : " << error.message() << std::endl;
		std::cerr << "reconnect success : " << error.message() << std::endl;
		Session->recconect_init(std::move(sock));
		start_sendrecv_forever(sendforever,recvforever);
	}
	return true;
}

bool Sync_ClientSocket::basic_send(std::shared_ptr<std::string> data) {
	if (!isReady)
		return false;
	return Session->basic_send(data);
}

std::string Sync_ClientSocket::basic_recv() {
	if (!isReady)
		return "";
	return Session->basic_recv();
}

void Sync_ClientSocket::limit_send(std::shared_ptr<std::string> data,
		float rate) {
	if (!isReady)
		return;
	Session->limit_send(data, rate);
}

void Sync_ClientSocket::start_sendrecv_forever(bool send, bool recv) {
	if (!isReady)
		return;
	Session->start_sendrecv_forever(send, recv);
	sendforever = send;
	recvforever = recv;
}

void Sync_ClientSocket::stop_sendrecv_forever() {
	Session->stop_sendrecv_forever();
}

void Sync_ClientSocket::stop_sendrecv_forever() {
	Session->stop_sendrecv_forever();
	sendforever = false;
	recvforever = false;
}

void Sync_ClientSocket::resumSend() {
	if (!isReady)
		return;
	Session->resumSend();
}
void Sync_ClientSocket::changeSendData() {
	if (!isReady)
		return;
	Session->changeSendData();
}

void Sync_ClientSocket::changeSendRate(float rate) {
	if (!isReady)
		return;
	Session->changeSendRate(rate);
}

void Sync_ClientSocket::changeSendRateAbs(float rate) {
	if (!isReady)
		return;
	Session->changeSendRateAbs(rate);
}

Sync_BaseSockSession::SockState Sync_ClientSocket::checkState() {
	return Session->getSockState();
}

} /* namespace IoTPriority */
