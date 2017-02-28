/*
 * IoTDevices.cpp
 *
 *  Created on: 2016/02/29
 *      Author: tachibana
 */

#include "IoTDevices.h"

namespace IoTPriority {

IoTDevices::IoTDevices() :
		onRecv_Func(NULL), apport(31400), OtherMessCount(0) {
	// TODO 自動生成されたコンストラクター・スタブ
	name_state0 = "State0";
	name_state1 = "State1";
	name_state2 = "State2";
	name_state3 = "State3";
	name_state4 = "State4";
	name_state5 = "State5";
	name_state6 = "State6";
	name_state7 = "State7";
	name_state8 = "State8";
	name_state9 = "State9";
	name_state10 = "State10";
	name_state11 = "State11";
	BROKERPORTNUM = 35225;
	Buffersize = 32 * 1024 * 1024; //Bufferサイズは32MB仮定

}

IoTDevices::IoTDevices(int tobrport, int toapport) :
		onRecv_Func(NULL), apport(toapport), OtherMessCount(0) {
	// TODO 自動生成されたコンストラクター・スタブ
	name_state0 = "State0";
	name_state1 = "State1";
	name_state2 = "State2";
	name_state3 = "State3";
	name_state4 = "State4";
	name_state5 = "State5";
	name_state6 = "State6";
	name_state7 = "State7";
	name_state8 = "State8";
	name_state9 = "State9";
	name_state10 = "State10";
	name_state11 = "State11";
	BROKERPORTNUM = tobrport;
	Buffersize = 32 * 1024 * 1024; //Bufferサイズは32MB仮定

}

IoTDevices::~IoTDevices() {
	// TODO Auto-generated destructor stub
}

void IoTDevices::initState() {
	//StateSwicherクラスにて状態遷移を作成(状態、次状態名)
	auto state0 = std::make_shared<StateSwicher>(
			std::make_shared<D_State00>(shared_from_this()), "");
	auto state5 = std::make_shared<StateSwicher>(
			std::make_shared<D_State05>(shared_from_this()), name_state6);
	auto state6 = std::make_shared<StateSwicher>(
			std::make_shared<D_State06>(shared_from_this()), name_state5);
	auto state7 = std::make_shared<StateSwicher>(
			std::make_shared<D_State07>(shared_from_this()), name_state5);
	auto state8 = std::make_shared<StateSwicher>(
			std::make_shared<D_State08>(shared_from_this()), name_state9);
	auto state9 = std::make_shared<StateSwicher>(
			std::make_shared<D_State09>(shared_from_this()), name_state5);
	auto state10 = std::make_shared<StateSwicher>(
			std::make_shared<D_State10>(shared_from_this()), name_state5);
	auto state11=std::make_shared<StateSwicher>(
			std::make_shared<D_State11>(shared_from_this()), name_state5);
	//状態登録(状態名、状態)
	registState(name_state0, state0);
	registState(name_state5, state5);
	registState(name_state6, state6);
	registState(name_state7, state7);
	registState(name_state8, state8);
	registState(name_state9, state9);
	registState(name_state10, state10);
	registState(name_state11, state11);

	init_onlypriority();
}

void IoTDevices::run_Entitiy() {
	int ret = registFirstState(name_state5);
	if (ret < 0)
		std::cout << "State not found!!" << std::endl;
	std::cerr << "Device:init OK" << std::endl;

	//受信スタート
	toAPServerSocket->start_sendrecv_forever(true, true);
	toBrokerSocket->start_sendrecv_forever(false, true);

	std::cerr << "Device:start OK" << std::endl;
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

bool IoTDevices::br_startReconnect() {
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
	//再接続フラグを建てる
	std::string payload = BinaryConverter::int8toStr(1);
	mess.makeMessage(getDeviceID(), iEntity::BROKER_DEVICEID,
			PriorityMessage::MESSTYPE_CONNECT, PriorityMessage::NOACK,
			std::make_shared<std::string>(payload));
	br_send(mess.getAllData());

	return true;
}

bool IoTDevices::ap_startReconnect() {
	bool ret;
	ret = toAPServerSocket->reconnect();
	int fallcount = 0;
	while (!ret) {
		fallcount++;
		if (fallcount > 10) {
			std::cout << "recconect failed" << std::endl;
			return false;
		}
		sleep(5);
		std::cout << "retry recconecting..." << std::endl;
		ret = toAPServerSocket->reconnect();
	}
	PriorityMessage mess;
	//再接続フラグを建てる
	std::string payload = BinaryConverter::int8toStr(1);
	mess.makeMessage(getDeviceID(), iEntity::APPSERVER_DEVICEID,
			PriorityMessage::MESSTYPE_CONNECT, PriorityMessage::NOACK,
			std::make_shared<std::string>(payload));
	ap_send(mess.getAllData(), -1);
	return true;
}

//------Test Function(ダミーデータ挿入)
void IoTDevices::putMessage(bool isServer, std::shared_ptr<PriorityMessage> p) {
	if (isServer) {
		server_recvqueue.push(p);
	} else {
		broker_recvqueue.push(p);
	}
}

void IoTDevices::connectAPServer(std::string hostname, int port) {
	ServerHostName = hostname;
	apport = port;
	//ソケット起動
	toAPServerSocket = std::make_shared<Sync_ClientSocket>(io_service_AP,
			ServerHostName, apport,
			std::bind(&IoTDevices::onRecvFromAPServer, shared_from_this(),
					std::placeholders::_1, std::placeholders::_2));
}

int IoTDevices::api_setDeviceID(int deviceID) {
	this->DeviceID = deviceID;
	return 1;
}

int IoTDevices::api_setDataType(std::string dataType) {
	this->DataType.push_back(dataType);
	return 1;
}

void IoTDevices::putRemoveQueue(int priority, struct timeval tm, int dataid) {
	bufferRemoveQueue.push(
			std::make_pair(priority, std::make_pair(tm, dataid)));
}
int IoTDevices::getRemovedata() {
	auto d = bufferRemoveQueue.top();
	bufferRemoveQueue.pop();
	return d.second.second;
}

std::shared_ptr<std::map<int, std::shared_ptr<IoTDeviceBuffer>>>IoTDevices::getBuffer() {
	return std::make_shared<std::map<int, std::shared_ptr<IoTDeviceBuffer>>>(Buffer);
}

bool IoTDevices::emptyDeviceQueue() {
	return DeviceDataqueue.empty();
}

std::shared_ptr<std::pair<std::string, std::string>> IoTDevices::readDeviceQueue() {
	return DeviceDataqueue.read_withtimeout();
}

std::shared_ptr<std::pair<std::string, std::string>> IoTDevices::putDeviceQueue() {
	return DeviceDataqueue.pop();
}

int IoTDevices::getDeviceID() {
	return DeviceID;
}

int IoTDevices::api_setBroker(std::string hostname) {
	this->BrokerHostName = hostname;
	return 1;
}

int IoTDevices::api_resist(int keepalive) {
	/*--------全体作成後---------
	 if (curState->getNextStateName() == name_state1) {
	 std::cerr << "AP: resist accept" << std::endl;
	 nextState();
	 return 1;
	 } else {
	 std::cerr << "AP: resist reject" << std::endl;
	 return -1;
	 }----------------------*/
	//ソケット起動
	toBrokerSocket = std::make_shared<Sync_ClientSocket>(io_service_BR,
			BrokerHostName, BROKERPORTNUM,
			std::bind(&IoTDevices::onRecvFromBroker, shared_from_this(),
					std::placeholders::_1, std::placeholders::_2));
	run_Entitiy();
}

int IoTDevices::api_fin() {
	return 0;
}

int IoTDevices::api_sendData(std::string dataType, std::string data) {
	DeviceDataqueue.push(
			std::make_shared<std::pair<std::string, std::string>>(
					std::make_pair(dataType, data)));
	return 1;
}

void IoTDevices::api_setRecvCallback(
		std::function<void(std::string, int)> onrecv) {
	onRecv_Func = onrecv;
}

std::shared_ptr<PriorityMessage> IoTDevices::br_recv_and_getdata() {
	return broker_recvqueue.pop();
}
std::shared_ptr<PriorityMessage> IoTDevices::ap_recv_and_getdata() {
	return server_recvqueue.pop();
}

void IoTDevices::ap_recvqueue_repush(std::shared_ptr<PriorityMessage> pdata) {
	server_recvqueue.push(pdata);
}

int IoTDevices::recv_and_gettype() {
	std::shared_ptr<PriorityMessage> mess;
	while (1) {
		mess = broker_recvqueue.read_withtimeout();
		if (mess != NULL)
			break;
		mess = server_recvqueue.read_withtimeout();
		if (mess != NULL)
			break;
	}
	return mess->getMessType();
}

int IoTDevices::recv_and_gettype_single() {
	std::shared_ptr<PriorityMessage> mess;
	mess = server_recvqueue.read_withtimeout();
	if (mess != NULL && mess->getMessType() == PriorityMessage::MESSTYPE_DATA)
		return mess->getMessType();
	mess = broker_recvqueue.read_withtimeout();
	if (mess != NULL)
		return mess->getMessType();
	mess = server_recvqueue.read_withtimeout();
	if (mess != NULL)
		return mess->getMessType();

	return -1;
}

void IoTDevices::ap_sendResum() {
	toAPServerSocket->resumSend();
}
void IoTDevices::ap_changeSendData() {
	toAPServerSocket->changeSendData();
}
void IoTDevices::ap_changeSendRate(float rate) {
	toAPServerSocket->changeSendRate(rate);
}
void IoTDevices::ap_changeSendRateAbs(float rate) {
	toAPServerSocket->changeSendRateAbs(rate);
}

bool IoTDevices::br_send(std::shared_ptr<std::string> data) {
	return toBrokerSocket->basic_send(data);
}

bool IoTDevices::ap_send(std::shared_ptr<std::string> data, int rate) {
	if (rate < 0) {
		return toAPServerSocket->basic_send(data);
	} else {
		toAPServerSocket->limit_send(data, rate);//TODO bpsは仮
		return true;
	}
}

Sync_BaseSockSession::SockState IoTDevices::br_checkState() {
	return toBrokerSocket->checkState();
}

Sync_BaseSockSession::SockState IoTDevices::ap_checkState() {
	return toBrokerSocket->checkState();
}

bool IoTDevices::br_queue_empty() {
	return broker_recvqueue.empty();
}

void IoTDevices::init_onlypriority() {
	apport = 31400;
	ServerHostName = "127.0.0.1";
}

//-------------------------Exec on NetoworkThread-----------------------------
void IoTDevices::onRecv_base(std::string data,
		Multi_MessageQueue<PriorityMessage>* recvqueue,
		std::string* remaindata) {
	auto pushdata = *remaindata + data;
	while (1) {
		auto mess = std::make_shared<PriorityMessage>();
		std::string rdata = mess->putMessage(pushdata);
		if (!mess->isAllDataRecieved())
			break;
		recvqueue->push(mess);
		pushdata = rdata;
	}
	broker_remaindata = pushdata;
}

void IoTDevices::onRecvFromBroker(std::string data, int sesid) {
	std::cerr << "receive from broker : " << data << std::endl;
	onRecv_base(data, &broker_recvqueue, &broker_remaindata);
}
void IoTDevices::onRecvFromAPServer(std::string data, int sesid) {
	std::cerr << "receive from server : " << data << std::endl;
	onRecv_base(data, &server_recvqueue, &server_remaindata);
}

} /* namespace IoTPriority */

/*--------------------------
 int main() {
 auto device = std::make_shared<IoTPriority::IoTDevices>();
 device->api_setBroker("127.0.0.1");
 device->api_setDeviceID(101);
 device->connectAPServer("127.0.0.1",31400);
 std::string dummy =
 "ddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd";
 std::string a;
 std::cin>>a;
 device->initState();
 std::cerr<<"conecting brserver"<<std::endl;
 std::thread th(&IoTPriority::IoTDevices::api_resist,device,10);
 std::cerr<<"datasending"<<std::endl;
 device->api_sendData("jpg", std::string(dummy+dummy));
 device->api_sendData("txt", std::string(dummy));
 device->api_sendData("jpg", std::string(dummy+dummy+dummy));
 device->api_sendData("txt",  std::string(dummy));
 device->api_sendData("txt",  std::string(dummy));
 std::cerr<<"size"<<dummy.length()<<std::endl;
 th.join();
 return 0;
 }

 */
