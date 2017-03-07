/*
 * SyncServerSockets.cpp
 *
 *  Created on: 2016/04/22
 *      Author: tachibana
 */

#include "SyncServerSockets.h"

namespace IoTPriority {

Sync_ServerSockets::Sync_ServerSockets(boost::asio::io_service& io_service, int port,
		std::function<void(std::string, int)> recvfunc) :
		Io_service(io_service), port(port), sock(io_service), On_recv_func(
				recvfunc), isReady(false), sockIdCounter(0),changefunc(NULL) {
	// TODO 自動生成されたコンストラクター・スタ
	Acceptor = std::make_shared<tcp::acceptor>(Io_service,
			tcp::endpoint(tcp::v4(), port));

}

Sync_ServerSockets::~Sync_ServerSockets() {
	// TODO Auto-generated destructor stub
}

void Sync_ServerSockets::accept(int maxaccept) {
	std::unique_lock<std::mutex> lk(mtx);
	boost::system::error_code error;
	isReady = true;
	for (int i = 0; i < maxaccept; i++) {
		Acceptor->accept(sock, error);
		if (error) {
			std::cout << "accept failed: " << error.message() << std::endl;
		} else {
			auto endpoint = sock.remote_endpoint();
			std::cerr << "accept start!" << std::endl;
			auto ses = std::make_shared<Sync_BaseSockSession>(std::move(sock),
					sockIdCounter, On_recv_func);
			std::cout << "accept correct!" << std::endl;
			std::cerr << "accept correct!" << std::endl;
			std::cout << "sendrecv :" <<sendforever<<" , "<<recvforever<<std::endl;
			if (sendforever || recvforever){
				std::cout << "ac_setforever" << std::endl;
				ses->start_sendrecv_forever(sendforever, recvforever);
			}
			if(changefunc!=NULL){
				ses->setNotifyChangeListner(changefunc);
			}
			acceptedSocks.insert(std::make_pair(sockIdCounter,std::make_pair(std::move(ses), endpoint)));
			sockIdCounter++;
		}
	}
}

void Sync_ServerSockets::setchangeListener(std::function<void(int)> func){
	changefunc=func;
}

bool Sync_ServerSockets::basic_send(std::shared_ptr<std::string> data,
		int sesid) {
	if (!isReady)
		return false;
	return acceptedSocks[sesid].first->basic_send(data);
}

std::string Sync_ServerSockets::basic_recv(int sesid) {
	if (!isReady)
		return "";
	return acceptedSocks[sesid].first->basic_recv();
}

void Sync_ServerSockets::limit_send(std::shared_ptr<std::string> data,
		float rate, int sesid) {
	if (!isReady)
		return;
	acceptedSocks[sesid].first->limit_send(data, rate);
}

void Sync_ServerSockets::start_sendrecv_forever(bool send,
bool recv) {
	std::unique_lock<std::mutex> lk(mtx);
	std::cerr << "change forever"  <<send<<" , "<<recv<<std::endl;
	sendforever = send;
	recvforever = recv;
	std::cerr << "state forever" <<sendforever<<" , "<<recvforever<<std::endl;
	if (!isReady)
		return;
	for (auto it=acceptedSocks.begin();it!=acceptedSocks.end();++it)
		it->second.first->start_sendrecv_forever(send, recv);
}

void Sync_ServerSockets::pause_sendrecv_forever(int sesid) {
	std::unique_lock<std::mutex> lk(mtx);
	acceptedSocks[sesid].first->stop_sendrecv_forever();
}

void Sync_ServerSockets::stop_sendrecv_forever(){
	std::unique_lock<std::mutex> lk(mtx);
	for (auto it=acceptedSocks.begin();it!=acceptedSocks.end();++it)
		it->second.first->stop_sendrecv_forever();
	sendforever = false;
	recvforever = false;
}

Sync_BaseSockSession::SockState Sync_ServerSockets::checkState(int sesid){
	return acceptedSocks[sesid].first->getSockState();
}

void Sync_ServerSockets::closeSession(int sesid){
	std::unique_lock<std::mutex> lk(mtx);
	acceptedSocks[sesid].first->stop_sendrecv_forever();
	acceptedSocks.erase(sesid);
	std::cerr<<"sesid "<<sesid<<" is deleted!"<<std::endl;
}

} /* namespace IoTPriority */
