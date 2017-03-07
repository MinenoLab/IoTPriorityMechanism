/*
 * AppServer.cpp
 *
 *  Created on: 2016/02/29
 *      Author: tachibana
 */

#include "AppServer.h"
#include "utils/BinaryConverter.h"

namespace IoTPriority {

AppServer::AppServer() :
		onRecv_Func(NULL), SendStopreq(false), TotalRecvBufferSize(0), TotalRecvSec(
				0), prevRate(10 * 1024 * 1024), asyncrecvCountor(0) {
	acceptIotDevices = false;

}

AppServer::AppServer(int tobrport, int todvport) :
		onRecv_Func(NULL), SendStopreq(false), TotalRecvBufferSize(0), TotalRecvSec(
				0), prevRate(10 * 1024 * 1024), asyncrecvCountor(0), BROKERPORTNUM(
				tobrport), PORTNUM(todvport) {
	acceptIotDevices = false;

}

void AppServer::initState() {
#ifdef CHECK
	std::cerr<<"ok-"<<std::endl;
#endif
	//状態遷移の作成:
	//StateSwicherクラスにて状態遷移を作成(状態、次状態名)
	auto state0 = std::make_shared<StateSwicher>(
			std::make_shared<A_State00>(shared_from_this()), "");
	/*auto state1 = std::make_shared<StateSwicher>(
	 std::make_shared<A_State01>(shared_from_this()), name_state2);
	 auto state2 = std::make_shared<StateSwicher>(
	 std::make_shared<A_State02>(shared_from_this()), name_state3);
	 auto state3 = std::make_shared<StateSwicher>(
	 std::make_shared<A_State03>(shared_from_this()), name_state4);
	 auto state4 = std::make_shared<StateSwicher>(
	 std::make_shared<A_State04>(shared_from_this()), name_state5);
	 auto state5 = std::make_shared<StateSwicher>(
	 std::make_shared<A_State05>(shared_from_this()), name_state6);*/
	auto state6 = std::make_shared<StateSwicher>(
			std::make_shared<A_State06>(shared_from_this()), name_state0);
	auto state7 = std::make_shared<StateSwicher>(
			std::make_shared<A_State07>(shared_from_this()), name_state6);
	auto state8 = std::make_shared<StateSwicher>(
			std::make_shared<A_State08>(shared_from_this()), name_state6);

#ifdef CHECK
	std::cerr<<"ok--"<<std::endl;
#endif

	//状態登録(状態名、状態)
	registState(name_state0, state0);
	registState(name_state6, state6);
	registState(name_state7, state7);
	registState(name_state8, state8);

	//受信スレット用の動作メソッド群クラスの初期化
	recvFunctions = std::make_shared<A_RecvFunctions>(shared_from_this());
}

void AppServer::run_Entitiy() {
	int ret = registFirstState(name_state6);
	if (ret < 0)
		std::cerr << "AP:State not found!!" << std::endl;
	std::cerr << "AP:init OK" << std::endl;
	toIotDeviceSocket->setchangeListener(
			std::bind(&AppServer::changeRecvData, shared_from_this(),
					std::placeholders::_1));
	toIotDeviceSocket->start_sendrecv_forever(false, true);
	toBrokerSocket->start_sendrecv_forever(false, true);
	accept_dv_th = std::thread(&Sync_ServerSockets::accept, toIotDeviceSocket,
			30);	//仮

	//connectメッセージ送信
	PriorityMessage mess;
	std::string payload = BinaryConverter::int8toStr(0);
	mess.makeMessage(iEntity::APPSERVER_DEVICEID, iEntity::BROKER_DEVICEID,
			PriorityMessage::MESSTYPE_CONNECT, PriorityMessage::NOACK,
			std::make_shared<std::string>(payload));
	br_send(mess.getAllData());
	//受信スタート
	//実行
	auto next = execFirstState();
	while (1) {
		if (next == "") {
			if (nextState() < 0)
				break;
			next = execCurState();
		} else {
			next = execState(next);
		}
	}
}

AppServer::~AppServer() {
}

int AppServer::api_registReqTable(std::string table) {
	this->prioritytable = table;
	std::cerr << "AP: table= " << table << std::endl;
	return 1;
}

int AppServer::api_Request() {
	/*--------全体作成後---------
	 if (curState->getNextStateName() == name_state1 && prioritytable != "") {
	 std::cerr << "AP: reqest accept" << std::endl;
	 nextState();
	 return 1;
	 } else {
	 std::cerr << "AP: reqest reject" << std::endl;
	 return -1;
	 }
	 ---------------------------*/
	//ソケット起動
	toBrokerSocket = std::make_shared<Sync_ClientSocket>(io_service_BR,
			brokerHostName, BROKERPORTNUM,
			std::bind(&AppServer::onRecvFromBroker, shared_from_this(),
					std::placeholders::_1, std::placeholders::_2));
	toIotDeviceSocket = std::make_shared<Sync_ServerSockets>(io_service_DV,
			PORTNUM,
			std::bind(&AppServer::onRecvFromDevices, shared_from_this(),
					std::placeholders::_1, std::placeholders::_2));
	std::cerr << "dvportnum : " << PORTNUM << std::endl;
	run_Entitiy();
}

bool AppServer::startReconnect() {
	bool ret;
	ret = toBrokerSocket->reconnect();
	int fallcount = 0;
	while (!ret) {
		fallcount++;
		if (fallcount > 10) {
			std::cout << "recconect failed" << std::endl;
			return false;
		}
		sleep(5);
		std::cout << "retry recconecting..." << std::endl;
		ret = toBrokerSocket->reconnect();
	}
	PriorityMessage mess;
	std::string payload = BinaryConverter::int8toStr(1);
	mess.makeMessage(DeviceID, iEntity::BROKER_DEVICEID,
			PriorityMessage::MESSTYPE_CONNECT, PriorityMessage::NOACK,
			std::make_shared<std::string>(payload));
	br_send(mess.getAllData());
}

void AppServer::api_setRecvCallback(
		std::function<void(std::string, int)> onrecv) {
	onRecv_Func = onrecv;
}

std::pair<int, std::string> AppServer::api_getrecvdata() {
	auto d = device_touserrecvqueue.pop();
	return std::make_pair(d->getSenderDeviceID(), *(d->getContentData()));
}

int AppServer::api_setBroker(std::string hostname) {
	brokerHostName = hostname;
	std::cerr << "AP: brokerHostName= " << brokerHostName << std::endl;
	return 1;
}

int AppServer::api_stop() {
	std::unique_lock<std::mutex> lk(mtx);
	if (curState->getNextStateName() == name_state0) {
		std::cerr << "AP: stop accept" << std::endl;
		SendStopreq = true;
		return 1;
	} else {
		return -1;
	}
}

std::shared_ptr<PriorityMessage> AppServer::br_recv_and_getdata() {
	return broker_recvqueue.pop();
}

std::shared_ptr<PriorityMessage> AppServer::dv_recv_and_getdata() {
	return device_recvqueue.pop();
}

int AppServer::recv_and_gettype() {
	std::shared_ptr<PriorityMessage> mess;
	while (1) {
		mess = broker_recvqueue.read_withtimeout();
		if (mess != NULL)
			break;
		mess = device_recvqueue.read_withtimeout();
		if (mess != NULL)
			break;
	}
	return mess->getMessType();
}

bool AppServer::br_send(std::shared_ptr<std::string> data) {
	return toBrokerSocket->basic_send(data);
}

int AppServer::dv_send(int RecieverDeviceID,
		std::shared_ptr<std::string> data) {
	auto it = dvid_sesid_map.left.find(RecieverDeviceID);
	if (it == dvid_sesid_map.left.end()) {
		std::cerr << "devid not found" << RecieverDeviceID << std::endl;
		return -1;	//deviceid not found
	}
	int sesid = it->second;
	bool ret = toIotDeviceSocket->basic_send(data, sesid);
	if (!ret)
		return -2;	//connection error
	return 0;
}

void AppServer::dv_close(int sesid) {
	toIotDeviceSocket->closeSession(sesid);
}

bool AppServer::getSendStopreq() {
	std::unique_lock<std::mutex> lk(mtx);
	return SendStopreq;
}

const int AppServer::getRateInterval() {
	return Rateinterval;
}

//result unit is "bps"
float AppServer::CalcCurRate() {
	float rate = prevRate;
	if (asyncrecvCountor > 0) {
		std::unique_lock<std::mutex> lk(recvmtx);
		struct timeval subtime;
		gettimeofday(&endtime, NULL);
		timersub(&endtime, &starttime, &subtime);
		TotalRecvSec += subtime.tv_sec + (double) subtime.tv_usec / 1000000;
		gettimeofday(&starttime, NULL);
	}
	std::cerr << iEntity::strnowtime() << "TotalRecvSec ;" << TotalRecvSec
			<< std::endl;
	std::cout << iEntity::strnowtime() << "TotalRecvSec ;" << TotalRecvSec
			<< std::endl;
	if (TotalRecvSec > 10 && TotalRecvBufferSize > 200000) {	//右側は無理やり決め打ち
		rate = (TotalRecvBufferSize * 8) / TotalRecvSec;
		prevRate = rate;
	}
	TotalRecvBufferSize = 0;
	TotalRecvSec = 0;
	return rate;
}

Sync_BaseSockSession::SockState AppServer::br_checkState() {
	return toBrokerSocket->checkState();
}

//-----------------------Exec on NetworkThreads-------------------------

void AppServer::onRecvFromDevices(std::string data, int sesid) {
	std::cerr << "receive from devices sesid : " << sesid << std::endl;
	auto pushdata = device_remaindata[sesid] + data;
	if (pushdata.length() > 5)
		std::cerr << "push sesid,data:" << sesid << " , "
				<< pushdata.substr(5, 20) << std::endl;
	//Deviceからの受信サイズ測定
	TotalRecvBufferSize += data.length();
	while (1) {
		if (pushdata.length() < 4)
			break;
		auto mess = std::make_shared<PriorityMessage>();
		std::string rdata = mess->putMessage(pushdata);

		//サイズ測定#研究結果取得用
		if (mess->getMessType() == PriorityMessage::MESSTYPE_DATA)
			recvFunctions->mesureLength(mess, data.length());

		if (!mess->isAllDataRecieved())
			break;

		std::cerr<<"messType is "<<mess->getMessType()<<std::endl;

		switch (mess->getMessType()) {
		case PriorityMessage::MESSTYPE_CONNECT:
			rdata = recvFunctions->execute_connect(mess, sesid, rdata);
			break;
		case PriorityMessage::MESSTYPE_DATA:
			decrementRecvCounter();
			recvFunctions->execute_data(mess, onRecv_Func);
			//受信完了したらスタックを一つ戻す
			if (!remainstack[sesid].empty()) {
				incrementRecvCounter();
				auto sd = remainstack[sesid].pop();
				devid_pri_map[mess->getSenderDeviceID()] = sd->first;
				std::string resumdata = sd->second;
				rdata = resumdata + rdata;
			}
			break;
		case PriorityMessage::MESSTYPE_DECDATA:
			recvFunctions->execute_decdata(mess);
			incrementRecvCounter();
			break;
		default:
			device_recvqueue.push(mess);
			std::cerr << "start recvData_queue " << std::endl;
		}

		pushdata = rdata;
	}
	std::cerr << "flagment mess length : " << pushdata.length() << std::endl;
	device_remaindata[sesid] = pushdata;
}

void AppServer::onRecvFromBroker(std::string data, int sesid) {
	std::cerr << "receive from broker" << std::endl;
	auto pushdata = broker_remaindata + data;
	while (1) {
		auto mess = std::make_shared<PriorityMessage>();
		std::string rdata = mess->putMessage(pushdata);
		if (!mess->isAllDataRecieved())
			break;
		broker_recvqueue.push(mess);
		pushdata = rdata;
	}
	broker_remaindata = pushdata;
}

void AppServer::changeRecvData(int sesid) {
	std::cerr << "App_data_change" << std::endl;
	int pri = devid_pri_map[dvid_sesid_map.right.at(sesid)];
	auto p = std::make_pair(pri, device_remaindata[sesid]);
	remainstack[sesid].push(std::make_shared<std::pair<int, std::string>>(p));
	device_remaindata[sesid] = "";
	decrementRecvCounter();
}

void AppServer::pushtoUserQueue(std::shared_ptr<PriorityMessage> mess) {
	device_touserrecvqueue.push(mess);
}

std::string AppServer::getDeviceRemainData(int sesid) {
	return device_remaindata[sesid];
}

//受信中データの数を示すカウンタのインクリメント
void AppServer::incrementRecvCounter() {
	std::unique_lock<std::mutex> lk(recvmtx);
	if (asyncrecvCountor == 0) {
		gettimeofday(&(starttime), NULL);
		std::cerr << "start recvDatamessage " << std::endl;
	}
	asyncrecvCountor++;
	std::cerr << "recvcount ; " << asyncrecvCountor << std::endl;
}

//受信中データの数を示すカウンタのデクリメントと、受信中になっていた時間の計測
void AppServer::decrementRecvCounter() {
	std::unique_lock<std::mutex> lk(recvmtx);
	asyncrecvCountor--;
	std::cerr << "recvcount ; " << asyncrecvCountor << std::endl;
	if (asyncrecvCountor == 0) {
		struct timeval subtime;
		gettimeofday(&endtime, NULL);
		timersub(&endtime, &starttime, &subtime);
		TotalRecvSec += subtime.tv_sec + (double) subtime.tv_usec / 1000000;
		std::cerr << "recvtime : " << TotalRecvSec << std::endl;
	}
}

}/* namespace IoTPriority */

/*--------------------------
 int main() {
 auto server = std::make_shared<IoTPriority::AppServer>();
 server->initState();
 server->api_setBroker("127.0.0.1");
 std::thread th(&IoTPriority::AppServer::api_Request, server);
 sleep(120);
 server->api_stop();
 std::cout << "api_stop:" << std::endl;
 th.join();
 }
 */
