/*
 * BrokerServer.cpp
 *
 *  Created on: 2016/03/30
 *      Author: tachibana
 */

#include "BrokerServer.h"

namespace IoTPriority {

BrokerServer::BrokerServer(int apport, int dvport) :
		apport(apport), dvport(dvport), curCommRate_kiro(128) {
	std::cerr << "[BR]init end" << std::endl;
	//優先度通信フェーズのみ動かすとき
	server_remaindata = "";
	SendingIDmap = std::make_shared<
			std::unordered_map<int,
					std::shared_ptr<
							std::unordered_map<int, std::pair<int, int>>>>>();
	for (int i = 0; i < 256; i++) {
		device_remaindata[i] = "";
	}
}

BrokerServer::~BrokerServer() {
	if (queuethread.joinable())
		queuethread.join();
}

void BrokerServer::run_Entitiy() {
	init_onlypriority();

	iContorol = std::make_shared<InterruptController>(shared_from_this());
	//状態遷移の作成:
	//state0は仮の終了用状態
	auto state0 = std::make_shared<StateSwicher>(
			std::make_shared<B_State00>(shared_from_this()), "");
	std::cerr << "[BR]state0init end" << std::endl;
	auto state4 = std::make_shared<StateSwicher>(
			std::make_shared<B_State04>(shared_from_this()), name_state5);
	auto state5 = std::make_shared<StateSwicher>(
			std::make_shared<B_State05>(shared_from_this()), name_state6);
	auto state6 = std::make_shared<StateSwicher>(
			std::make_shared<B_State06>(shared_from_this()), name_state5);
	auto state7 = std::make_shared<StateSwicher>(
			std::make_shared<B_State07>(shared_from_this()), name_state6);
	std::cerr << "[BR]state7init end" << std::endl;
	auto state10 = std::make_shared<StateSwicher>(
			std::make_shared<B_State10>(shared_from_this()), name_state11);
	auto state11 = std::make_shared<StateSwicher>(
			std::make_shared<B_State11>(shared_from_this()), name_state10);
	std::cerr << "[BR]state11init end" << std::endl;

	//状態登録(状態名、状態)

	registState(name_state0, state0);
	std::cerr << "[BR]state0 registend" << std::endl;
	registState(name_state4, state4);
	registState(name_state5, state5);
	registState(name_state6, state6);
	registState(name_state7, state7);
	std::cerr << "[BR]state7 registend" << std::endl;
	registState(name_state10, state10);
	registState(name_state11, state11);
	std::cerr << "[BR]state11 registend" << std::endl;
#ifndef CHECK
	//ソケット起動
	toAPServerSocket = std::make_shared<Sync_ServerSockets>(io_service_AP,
			apport,
			std::bind(&BrokerServer::onRecvfromAPServer, shared_from_this(),
					std::placeholders::_1, std::placeholders::_2));
	toIotDeviceSocket = std::make_shared<Sync_ServerSockets>(io_service_DV,
			dvport,
			std::bind(&BrokerServer::onRecvFromDevices, shared_from_this(),
					std::placeholders::_1, std::placeholders::_2));
	std::cerr << "[BR]setforever" << std::endl;
	toAPServerSocket->start_sendrecv_forever(false, true);
	toIotDeviceSocket->start_sendrecv_forever(false, true);
	accept_ap_th = std::thread(&Sync_ServerSockets::accept, toAPServerSocket,
			1);
	accept_dv_th = std::thread(&Sync_ServerSockets::accept, toIotDeviceSocket,
			30);	//仮

	std::cerr << "[BR]Accept OK" << std::endl;
#endif
	//受信スタート
	int ret = registFirstState(name_state4);
	if (ret < 0)
		std::cerr << "[BR]State not found!!" << std::endl;
	std::cerr << "[BR]init OK" << std::endl;

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

void BrokerServer::run_state_secondthread(std::string startState) {
	auto it = statemap.find(startState);
	if (it == statemap.end())
		return;
	auto sState = it->second;
	auto next = sState->state->execute();
	while (1) {
		if (next == "") {
			it = statemap.find(sState->getNextStateName());
		} else {
			it = statemap.find(next);
		}
		if (it == statemap.end())
			break;
		sState = it->second;
		next = sState->state->execute();
	}
	std::cerr << "[q-BR] thread end" << std::endl;
}

void BrokerServer::pushMainPriorityQueue(int pri, struct timeval tm,
		std::shared_ptr<MetaData> d) {
	main_priority_queue.push(pri, tm, d);
}

std::pair<int, std::pair<struct timeval, std::shared_ptr<MetaData>>>BrokerServer::popMainPriorityQueue() {
	return main_priority_queue.pop();
}

std::pair<int, std::pair<struct timeval, std::shared_ptr<MetaData>>>BrokerServer::readMainPriorityQueue() {
	return main_priority_queue.read();
}

bool BrokerServer::emptyMainPriorityQueue() {
	return main_priority_queue.empty();
}

void BrokerServer::flushPriorityqueue() {
	std::cerr << "end" << std::endl;
	std::cout << "end" << std::endl;
	while (!main_priority_queue.empty()) {
		char s[64];
		auto popdata = main_priority_queue.pop();
		struct tm *time_st;
		time_st = localtime(&popdata.second.first.tv_sec);
		sprintf(s, " %02d:%02d:%02d", time_st->tm_hour, time_st->tm_min,
				time_st->tm_sec);
		std::cerr
				<< "[q-BR]State 10 popPriQueuefirst DeviceID,Dataid,Priority,Time"
				<< popdata.second.second->getDeviceID() << " , "
				<< popdata.second.second->getDataID() << " , " << popdata.first
				<< s << std::endl;
		std::cout << iEntity::strnowtime() << ";popPriQueuefirst;DeviceID;"
				<< popdata.second.second->getDeviceID() << ";Dataid;"
				<< popdata.second.second->getDataID() << " ;Priority;"
				<< popdata.first << ";time;" << s << std::endl;
	}
}

float BrokerServer::getCurCommRate() {
	std::unique_lock<std::mutex> lk(mtx);
	return curCommRate_kiro;
}
void BrokerServer::putCurCommRate(float rate) {
	std::unique_lock<std::mutex> lk(mtx);
	curCommRate_kiro = rate;
}

//優先度通信フェーズだけ作ったとき用の仮初期化メソッド
void BrokerServer::init_onlypriority() {
	if (!setpriority) {
		PTEntity e(101, "jpg", 10 * 60, 2);
		PTEntity e2(101, "jpg", 1 * 60, 5);
		PTEntity e3(102, "jpg", 10 * 60, 2);
		PTEntity e4(102, "jpg", 1 * 60, 5);
		PTEntity e5(103, "txt", 5 * 60, 6);
		PTEntity e6(103, "txt", 1 * 60, 9);
		PTEntity e7(104, "txt", 5 * 60, 6);
		PTEntity e8(104, "txt", 1 * 60, 9);
		PTEntity e9(101, "spg", 5 * 60, 1);
		PTEntity e10(101, "spg", 1 * 60, 4);
		PTEntity e11(102, "spg", 5 * 60, 1);
		PTEntity e12(102, "spg", 1 * 60, 4);
		pTable.push_back(e);
		pTable.push_back(e2);
		pTable.push_back(e3);
		pTable.push_back(e4);
		pTable.push_back(e5);
		pTable.push_back(e6);
		pTable.push_back(e7);
		pTable.push_back(e8);
		pTable.push_back(e9);
		pTable.push_back(e10);
		pTable.push_back(e11);
		pTable.push_back(e12);
	}
	for (auto p : pTable) {
		std::cout << p.getstr() << std::endl;
		std::cerr << p.getstr() << std::endl;
	}
}

void BrokerServer::putpriority(std::string json) {
	AATableEntitiy at("127.0.0.1", 35200, json);
	auto data = at.getrequire();
	for (auto d : data) {
		pTable.push_back(d);
	}
	setpriority = true;
}

//------Test Function(ダミーデータ挿入)
void BrokerServer::putMessage(bool isServer,
		std::shared_ptr<PriorityMessage> p) {
	if (isServer) {
		server_recvqueue.push(p);
	} else {
		device_recvqueue.push(p);
	}
}

int BrokerServer::recv_and_gettype() {
	std::shared_ptr<PriorityMessage> mess;
	while (1) {
		mess = server_recvqueue.read_withtimeout();
		if (mess != NULL)
			break;
		mess = device_recvqueue.read_withtimeout();
		if (mess != NULL)
			break;
	}
	return mess->getMessType();
}

int BrokerServer::recv_and_gettype_fromDevice() {
	std::shared_ptr<PriorityMessage> mess;
	while (1) {
		mess = device_recvqueue.read_withtimeout();
		if (mess != NULL)
			break;
	}
	return mess->getMessType();
}

void BrokerServer::registSending(int deviceID, int dataID, int priority,
		int sizeClass) {
	auto minimap = SendingIDmap->find(deviceID);
	if (minimap == SendingIDmap->end()) {
		auto m = std::make_shared<std::unordered_map<int, std::pair<int, int>>>();
		m->insert(std::make_pair(dataID, std::make_pair(priority, sizeClass)));
		SendingIDmap->insert(std::make_pair(deviceID,m));
	} else {
		minimap->second->insert(
		std::make_pair(dataID, std::make_pair(priority, sizeClass)));
	}
}

int BrokerServer::getSendingSClass(int deviceID, int DataID) {
	auto minimap = SendingIDmap->find(deviceID);
	if (minimap != SendingIDmap->end()) {
		auto data = minimap->second->find(DataID);
		int sclass;
		if (data != minimap->second->end()) {
			sclass = data->second.second;
			return sclass;
		}
	}
	return -1;
}

int BrokerServer::releaseSending(int deviceID, int dataID) {
	auto minimap = SendingIDmap->find(deviceID);
	if (minimap != SendingIDmap->end()) {
		auto data = minimap->second->find(dataID);
		int pri;
		if (data != minimap->second->end()) {
			pri = data->second.first;
			minimap->second->erase(dataID);
			return pri;
		}
	}
	return -1;
}

bool BrokerServer::isemptysending() {
	if (SendingIDmap->empty()) {
		return true;
	}
	for (auto m : *SendingIDmap) {
		if (!m.second->empty())
			return false;
	}
	return true;
}

std::vector<std::tuple<int, int, int, int>> BrokerServer::getallsendingData() {
	std::vector<std::tuple<int, int, int, int>> retvalue;
	for (auto minimap : *SendingIDmap) {
		int deviceID = minimap.first;
		for (auto m : *(minimap.second)) {
			int dataID = m.first;
			int priority = m.second.first;
			int sClass = m.second.second;
			retvalue.push_back(
					std::make_tuple(deviceID, dataID, priority, sClass));
		}
	}
	return retvalue;
}

std::shared_ptr<PriorityMessage> BrokerServer::ap_recv_and_getdata() {
	return server_recvqueue.pop();
}

std::shared_ptr<PriorityMessage> BrokerServer::dv_recv_and_getdata() {
	return device_recvqueue.pop();
}

int BrokerServer::ap_send(int RecieverDeviceID,
		std::shared_ptr<std::string> data) {
	auto it = ap_session_id_map.left.find(RecieverDeviceID);
	if (it == ap_session_id_map.left.end())
		return -1;
	int sesid = it->second;
	toAPServerSocket->basic_send(data, sesid);
	return 1;
}

//TODO レート制御用ができるオーバーロードメソッド書かなきゃね
int BrokerServer::dv_send(int RecieverDeviceID,
		std::shared_ptr<std::string> data) {
	auto it = dv_session_id_map.left.find(RecieverDeviceID);
	if (it == dv_session_id_map.left.end()) {
		std::cerr << "[BR] deviceid not found" << RecieverDeviceID << std::endl;
		return -1;
	}
	int sesid = it->second;
	toIotDeviceSocket->basic_send(data, sesid);
	return 1;
}

void BrokerServer::ap_close(int sesid) {
	toAPServerSocket->closeSession(sesid);
}

void BrokerServer::dv_close(int sesid) {
	toIotDeviceSocket->closeSession(sesid);
}

int BrokerServer::getPriorityValueFromTable(std::shared_ptr<MetaData> mdata,
		struct timeval aliveTime) {
	std::cerr << "[BR]State getPriority" << std::endl;
	for (auto it = pTable.begin(); it != pTable.end(); ++it) {
		int ret = it->matchCondition(mdata->getDeviceID(), mdata->getDataType(),
				aliveTime);
		if (ret == 1) {
			it->notifyUsed();
			return it->get_priority();
		}
	}
	return 15;	//最も低い優先度
}

//-----------------------Exec on NetworkThreads-------------------------
void BrokerServer::onRecv_base(std::string data, int sesid,
		Multi_MessageQueue<PriorityMessage>* recvqueue,
		bimaps::bimap<int, int>* sesidmap, std::string* remaindata,
		std::shared_ptr<Sync_ServerSockets> sock) {
	auto pushdata = *remaindata + data;
	while (1) {
		auto mess = std::make_shared<PriorityMessage>();
		std::string rdata = mess->putMessage(pushdata);
		if (!mess->isAllDataRecieved())
			break;
		if (mess->getMessType() == PriorityMessage::MESSTYPE_CONNECT)
			onExecute_connect(mess, sesidmap, sesid, sock);
		recvqueue->push(mess);
		std::cerr << "recived ID : " << mess->getSenderDeviceID()
				<< " messtype : " << mess->getMessType() << std::endl;
		pushdata = rdata;
	}
	*remaindata = pushdata;
}

void BrokerServer::onRecvFromDevices(std::string data, int sesid) {
	std::cerr << "receive from devices" << std::endl;
	onRecv_base(data, sesid, &device_recvqueue, &dv_session_id_map,
			&device_remaindata[sesid], toIotDeviceSocket);
}

void BrokerServer::onRecvfromAPServer(std::string data, int sesid) {
	std::cerr << "receive from server  " << std::endl;
	onRecv_base(data, sesid, &server_recvqueue, &ap_session_id_map,
			&server_remaindata, toAPServerSocket);
}

void BrokerServer::onExecute_connect(std::shared_ptr<PriorityMessage> mess,
		bimaps::bimap<int, int>* sesidmap, int sesid,
		std::shared_ptr<Sync_ServerSockets> sock) {

	auto mit = sesidmap->left.find(mess->getSenderDeviceID());
	if (mit == sesidmap->left.end()) {
		sesidmap->left.insert(std::make_pair(mess->getSenderDeviceID(), sesid));
		std::cerr << "[BR] insert " << mess->getSenderDeviceID() << " , "
				<< sesid << std::endl;
	} else if (BinaryConverter::strtoInt8(*mess->getContentData()) == 1) {//TODO prioritymessageに1=trueという定義を追記
		//もし再接続なら
		int prev_sesid = mit->get_right();
		//既に別のセッションが同じdeviceIDを使っている->マップの置き換え
		sesidmap->left.replace_data(mit, sesid);
		sock->closeSession(sesid);
	} else {
		std::cerr << "this ID is already used :" << mess->getSenderDeviceID()
				<< std::endl;
	}
}

} /* namespace IoTPriority *
 /
 /*
 int main() {
 auto broker = std::make_shared<IoTPriority::BrokerServer>(35200, 35225);
 broker->run_Entitiy();
 }
 */
