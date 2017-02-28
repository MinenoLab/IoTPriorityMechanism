/*
 * NormalIoTDevices.cpp
 *
 *  Created on: 2016/02/29
 *      Author: tachibana
 */

#include "NormalIoTDevices.h"

namespace IoTPriority {

NormalIoTDevices::NormalIoTDevices() :
		onRecv_Func(NULL), apport(31400), OtherMessCount(0), dataID_f(1) {
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
	BROKERPORTNUM = 35225;
	Buffersize = 32 * 1024 * 1024; //Bufferサイズは32MB仮定

}

NormalIoTDevices::~NormalIoTDevices() {
	// TODO Auto-generated destructor stub
}

void NormalIoTDevices::initState() {
	//StateSwicherクラスにて状態遷移を作成(状態、次状態名)
	auto state0 = std::make_shared<StateSwicher>(
			std::make_shared<D_State50>(shared_from_this()), "");
	auto state5 = std::make_shared<StateSwicher>(
			std::make_shared<D_State51>(shared_from_this()), name_state0);
	/*auto state6 = std::make_shared<StateSwicher>(
	 std::make_shared<D_State06>(shared_from_this()), name_state5);
	 auto state7 = std::make_shared<StateSwicher>(
	 std::make_shared<D_State07>(shared_from_this()), name_state5);
	 auto state8 = std::make_shared<StateSwicher>(
	 std::make_shared<D_State08>(shared_from_this()), name_state9);
	 auto state9 = std::make_shared<StateSwicher>(
	 std::make_shared<D_State09>(shared_from_this()), name_state5);*/
	//状態登録(状態名、状態)
	registState(name_state0, state0);
	registState(name_state5, state5);
	/*registState(name_state6, state6);
	 registState(name_state7, state7);
	 registState(name_state8, state8);
	 registState(name_state9, state9);*/

	init_onlypriority();
}

void NormalIoTDevices::run_Entitiy() {
	int ret = registFirstState(name_state5);
	if (ret < 0)
		std::cout << "State not found!!" << std::endl;
	std::cerr << "Device:init OK" << std::endl;

	//受信スタート
	toAPServerSocket->start_sendrecv_forever(true, true);
	//toBrokerSocket->start_sendrecv_forever(false, true);

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

//------Test Function(ダミーデータ挿入)
void NormalIoTDevices::putMessage(bool isServer,
		std::shared_ptr<PriorityMessage> p) {
	if (isServer) {
		server_recvqueue.push(p);
	} else {
		broker_recvqueue.push(p);
	}
}

void NormalIoTDevices::connectAPServer(std::string hostname, int port) {
	ServerHostName = hostname;
	apport = port;
	//ソケット起動
	toAPServerSocket = std::make_shared<Sync_ClientSocket>(io_service_AP,
			ServerHostName, apport,
			std::bind(&NormalIoTDevices::onRecvFromAPServer, shared_from_this(),
					std::placeholders::_1, std::placeholders::_2));
}

int NormalIoTDevices::api_setDeviceID(int deviceID) {
	this->DeviceID = deviceID;
	return 1;
}

int NormalIoTDevices::api_setDataType(std::string dataType) {
	this->DataType.push_back(dataType);
	return 1;
}

void NormalIoTDevices::putRemoveQueue(int priority, struct timeval tm,
		int dataid) {
	bufferRemoveQueue.push(
			std::make_pair(priority, std::make_pair(tm, dataid)));
}
int NormalIoTDevices::getRemovedata() {
	auto d = bufferRemoveQueue.top();
	bufferRemoveQueue.pop();
	return d.second.second;
}

std::shared_ptr<std::map<int, std::shared_ptr<IoTDeviceBuffer>>>NormalIoTDevices::getBuffer() {
	return std::make_shared<std::map<int, std::shared_ptr<IoTDeviceBuffer>>>(Buffer);
}

bool NormalIoTDevices::emptyDeviceQueue() {
	return DeviceDataqueue.empty();
}

std::shared_ptr<std::pair<std::string, std::string>> NormalIoTDevices::readDeviceQueue() {
	return DeviceDataqueue.read_withtimeout();
}

std::shared_ptr<std::pair<std::string, std::string>> NormalIoTDevices::putDeviceQueue() {
	return DeviceDataqueue.pop();
}

int NormalIoTDevices::getDeviceID() {
	return DeviceID;
}

int NormalIoTDevices::api_setBroker(std::string hostname) {
	this->BrokerHostName = hostname;
	return 1;
}

int NormalIoTDevices::api_resist(int keepalive,std::string jsondata) {

	/*--------全体作成後---------
	 if (curState->getNextStateName() == name_state1) {
	 std::cerr << "AP: resist accept" << std::endl;
	 nextState();
	 return 1;
	 } else {
	 std::cerr << "AP: resist reject" << std::endl;
	 return -1;
	 }----------------------*/
	if (jsondata == "") {
		PTEntity e(101, "jpg", 10 * 60, 2);
		PTEntity e2(101, "jpg", 1 * 60, 7);
		PTEntity e3(102, "jpg", 10 * 60, 2);
		PTEntity e4(102, "jpg", 1 * 60, 7);
		PTEntity e5(103, "txt", 5 * 60, 4);
		PTEntity e6(103, "txt", 1 * 60, 9);
		PTEntity e7(104, "txt", 5 * 60, 4);
		PTEntity e8(104, "txt", 1 * 60, 9);
		PTEntity e9(101, "spg", 5 * 60, 1);
		PTEntity e10(101, "spg", 1 * 60, 6);
		PTEntity e11(102, "spg", 5 * 60, 1);
		PTEntity e12(102, "spg", 1 * 60, 6);
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
	} else {
		AATableEntitiy at("127.0.0.1", 35200, jsondata);
		auto data = at.getrequire();
		for (auto d : data) {
			pTable.push_back(d);
		}
	}
	for(auto p:pTable){
		std::cout<<p.getstr()<<std::endl;
	}
	run_Entitiy();
}

int NormalIoTDevices::getPriorityValueFromTable(std::shared_ptr<MetaData> mdata,
		struct timeval aliveTime) {
	std::cerr << "[BR]State getPriority" << std::endl;
	for (auto it = pTable.begin(); it != pTable.end(); ++it) {
		std::cerr << "[BR]State getPriority-loop" << std::endl;
		int ret = it->matchCondition(mdata->getDeviceID(), mdata->getDataType(),
				aliveTime);
		if (ret == 1) {
			it->notifyUsed();
			return it->get_priority();
		}
	}
	return 15;	//最も低い優先度
}

int NormalIoTDevices::api_fin() {
	return 0;
}

int NormalIoTDevices::api_sendData(std::string dataType, std::string data) {
	DeviceDataqueue.push(
			std::make_shared<std::pair<std::string, std::string>>(
					std::make_pair(dataType, data)));
	//優先度の決定
	MetaData meta;
	meta.set(dataID_f, getDeviceID(), dataType, (float) data.length());
	dataID_f++;
	if (dataID_f == 101) {
		dataID_f = 301;
	}
	struct timeval nowtime;
	auto ptr_mdata = std::make_shared<MetaData>(meta);
	gettimeofday(&nowtime, NULL);
	int priority = getPriorityValueFromTable(ptr_mdata, nowtime);
	DevicePriorityValuequeue.push(std::make_shared<int>(priority));
	std::cout << iEntity::strnowtime() << ";recv priority;DeviceID;"
			<< getDeviceID() << ";DataID;" << meta.getDataID() << ";;;priority;"
			<< priority << std::endl;
	return 1;
}

int NormalIoTDevices::popPriority() {
	auto ret = DevicePriorityValuequeue.pop();
	return *ret;
}

void NormalIoTDevices::api_setRecvCallback(
		std::function<void(std::string, int)> onrecv) {
	onRecv_Func = onrecv;
}

std::shared_ptr<PriorityMessage> NormalIoTDevices::br_recv_and_getdata() {
	return broker_recvqueue.pop();
}
std::shared_ptr<PriorityMessage> NormalIoTDevices::ap_recv_and_getdata() {
	return server_recvqueue.pop();
}

void NormalIoTDevices::ap_recvqueue_repush(
		std::shared_ptr<PriorityMessage> pdata) {
	server_recvqueue.push(pdata);
}

int NormalIoTDevices::recv_and_gettype() {
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

int NormalIoTDevices::recv_and_gettype_single() {
	std::shared_ptr<PriorityMessage> mess;
	mess = broker_recvqueue.read_withtimeout();
	if (mess != NULL)
		return mess->getMessType();
	mess = server_recvqueue.read_withtimeout();
	if (mess != NULL)
		return mess->getMessType();
	return -1;
}

/*int NormalIoTDevices::br_send(std::shared_ptr<std::string> data) {
 toBrokerSocket->basic_send(data);
 return 1;
 }*/
int NormalIoTDevices::ap_send(std::shared_ptr<std::string> data, int rate) {
	if (rate < 0) {
		toAPServerSocket->basic_send(data);
	} else {
		toAPServerSocket->limit_send(data, rate);
	}
	return 1;
}

bool NormalIoTDevices::br_queue_empty() {
	return broker_recvqueue.empty();
}

void NormalIoTDevices::init_onlypriority() {
	apport = 31400;
	ServerHostName = "127.0.0.1";
}

//-------------------------Exec on NetoworkThread-----------------------------
void NormalIoTDevices::onRecv_base(std::string data,
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

/*void NormalIoTDevices::onRecvFromBroker(std::string data, int sesid) {
 std::cerr << "receive from broker : " << data << std::endl;
 onRecv_base(data, &broker_recvqueue, &broker_remaindata);
 }*/
void NormalIoTDevices::onRecvFromAPServer(std::string data, int sesid) {
	std::cerr << "receive from server : " << data << std::endl;
	onRecv_base(data, &server_recvqueue, &server_remaindata);
}

} /* namespace IoTPriority */

/*--------------------------
 int main() {
 auto device = std::make_shared<IoTPriority::NormalIoTDevices>();
 device->api_setBroker("127.0.0.1");
 device->api_setDeviceID(101);
 device->connectAPServer("127.0.0.1",31400);
 std::string dummy =
 "ddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd";
 std::string a;
 std::cin>>a;
 device->initState();
 std::cerr<<"conecting brserver"<<std::endl;
 std::thread th(&IoTPriority::NormalIoTDevices::api_resist,device,10);
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
