/*
 * InterruptController.cpp
 *
 *  Created on: 2016/10/17
 *      Author: tachibana
 */

#include "../BrokerServer.h"

namespace IoTPriority {

BrokerServer::InterruptController::InterruptController(
		std::shared_ptr<BrokerServer> server) :
		Server(server) {
	// TODO 自動生成されたコンストラクター・スタブ
}

BrokerServer::InterruptController::~InterruptController() {
	// TODO Auto-generated destructor stub
}

//State10
void BrokerServer::InterruptController::interruptHighPriority() {
	auto d = Server->readMainPriorityQueue();
	int priority = d.first;
	if (priority < Server->prevpriority) {
		std::unique_lock<std::mutex> lk(stackmtx);
		std::cerr << "[q-BR]State 10 stackAct" << std::endl;
		if (!Server->isemptysending()) {
			std::cerr << "[q-BR]State 10 stack_send" << std::endl;
			notifyALLsendingDevices(false);
			pushcurIDmap();
		}
		(stackAct) = false;
		Server->prevpriority = priority;
	}
}

bool BrokerServer::InterruptController::returnInterruptHighPriority() {
	int pri = readMapPriority();
	int nextpri;

	if (!Server->emptyMainPriorityQueue()) {
		auto d = Server->readMainPriorityQueue();
		nextpri = d.first;
		//メインの優先度キューよりスタックの中の方優先度が高ければpop
		if (pri > 0 && pri <= nextpri) {
			std::cerr << "[q-BR]State 10 stackBack" << std::endl;
			popprevIDmap();
			notifyALLsendingDevices(true);
			return true;
		}
	}
	return false;
}

//State11
bool BrokerServer::InterruptController::detectIntertupt() {
	std::unique_lock<std::mutex> lk(stackmtx);
	//高優先度による割り込みの検知->State10へ
	if (stackAct > 0) {
		std::cerr << "[q-BR]State 11 Data Change: " << std::endl;
		return true;
	}
	return false;
}

void BrokerServer::InterruptController::resolutePassing(
		std::shared_ptr<PriorityMessage> d, int dataID) {
	std::cerr << "[q-BR]State 11 rerease prev " << std::endl;
	bool resum = releaseprevIDmap(d->getSenderDeviceID(), dataID);
	//該当デバイスのスタックされたマップ中にデータがなくなったと判断した場合、デバイスのスタックをPOPする
	if (resum) {
		PriorityMessage mess;
		std::string data;
		data = "r";
		mess.makeMessage(Server->DeviceID, d->getSenderDeviceID(),
				PriorityMessage::MESSTYPE_UPDATE, PriorityMessage::NOACK,
				std::make_shared<std::string>(data));
		Server->dv_send(d->getSenderDeviceID(), mess.getAllData());
		std::cerr << "[q-BR]State 11 resum Stack at DeviceID : "
				<< d->getSenderDeviceID() << std::endl;
	}
}

//すれ違いでcompパケットを受信した用
bool BrokerServer::InterruptController::releaseprevIDmap(int deviceID,
		int DataID) {
	if (IDmapStack.empty())
		return false;
	Multi_MessageStack<
			std::pair<int,
					std::shared_ptr<
							std::unordered_map<int,
									std::shared_ptr<
											std::unordered_map<int,
													std::pair<int, int>>>>>>> sbuf;
	Multi_MessageStack<std::tuple<float, float, float>> ratebuf;
	bool ret = false;
	while (1) {
		auto idmap = IDmapStack.read()->second;
		auto minimap = idmap->find(deviceID);
		if (minimap != idmap->end()) {
			auto data = minimap->second->find(DataID);
			int pri;
			if (data != minimap->second->end()) {
				ret = false;
				pri = data->second.first;
				minimap->second->erase(DataID);
				std::cerr << "prev erase OK" << std::endl;
				//同じデバイスIDに要素がなければtrue
				if (minimap->second->empty())
					ret = true;
				//他に要素がなければ消してしまう
				bool isremove = true;
				for (auto m : *idmap) {
					if (!m.second->empty()) {
						isremove = false;
						break;
					}
				}
				if (isremove) {
					IDmapStack.next();
					sendrateStack.next();
				}
				break;
			}
		}
		//なければ深堀り
		sbuf.push(IDmapStack.pop());
		ratebuf.push(sendrateStack.pop());
		if (IDmapStack.empty()) {
			std::cerr << "prev not found" << std::endl;
			break;
		}
	}
	while (!sbuf.empty()) {
		IDmapStack.push(sbuf.pop());
		sendrateStack.push(ratebuf.pop());
	}
	return ret;
}

void BrokerServer::InterruptController::reAllocateRate(std::vector<std::tuple<int,int,int,int>> datamap) {
	for (auto k : datamap) {
		float sendrate;
		switch (std::get<3>(k)) {
		case 0:
			sendrate = std::get<0>(Server->sendrate);
			break;
		case 1:
			sendrate = std::get<1>(Server->sendrate);
			break;
		case 2:
			sendrate = std::get<2>(Server->sendrate);
			break;
		default:
			sendrate = -1;
		}
		//メッセージ作成
		std::cerr << "[q-BR] resend id, data, rare" << std::get<0>(k) << " , "
				<< std::get<1>(k) << " , " << sendrate << std::endl;
		UpdateMessage um;
		PriorityMessage primess;
		um.set(UpdateMessage::ABS_VALUE, sendrate);
		primess.makeMessage(0xff, std::get<0>(k),
				PriorityMessage::MESSTYPE_UPDATE, PriorityMessage::NOACK,
				um.getMessage());
		//送信
		Server->dv_send(primess.getReceverDeviceID(), primess.getAllData());
	}
}

void BrokerServer::InterruptController::notifyALLsendingDevices(bool state) {
	int id; //Restart or Stop
	id = state ? UpdateMessage::RESTART : UpdateMessage::STOP;
	UpdateMessage um;
	um.set(id);
	baseNotifyALLsendingDevices(PriorityMessage::MESSTYPE_UPDATE,
			um.getMessage());
}

void BrokerServer::InterruptController::notifyALLsendingDevices(float value) {
	UpdateMessage um;
	um.set(UpdateMessage::REL_VALUE, value);
	baseNotifyALLsendingDevices(PriorityMessage::MESSTYPE_UPDATE,
			um.getMessage());
}

void BrokerServer::InterruptController::baseNotifyALLsendingDevices(
		int messType, std::shared_ptr<std::string> data) {
	for (auto minimap : *Server->SendingIDmap) {
		if (!minimap.second->empty()) {
			PriorityMessage mess;
			//Restart or Stop
			mess.makeMessage(Server->DeviceID, minimap.first, messType,
					PriorityMessage::NOACK, data);
			std::cerr << "notify data :" << data << " devid : " << minimap.first
					<< std::endl;
			Server->dv_send(minimap.first, mess.getAllData());
		}
	}
}

void BrokerServer::InterruptController::pushcurIDmap() {
	auto p = std::make_pair(Server->prevpriority, Server->SendingIDmap);
	IDmapStack.push(
			std::make_shared<
					std::pair<int,
							std::shared_ptr<
									std::unordered_map<int,
											std::shared_ptr<
													std::unordered_map<int,
															std::pair<int, int>>>>>>>(p));
	Server->SendingIDmap = std::make_shared<
			std::unordered_map<int,
					std::shared_ptr<
							std::unordered_map<int, std::pair<int, int>>>>>();
	sendrateStack.push(
			std::make_shared<std::tuple<float, float, float>>(
					Server->sendrate));
	std::cerr << "push stack size : " << IDmapStack.size() << std::endl;
}
void BrokerServer::InterruptController::popprevIDmap() {
	if (IDmapStack.empty())
		return;
	auto map = IDmapStack.pop();
	auto ratetuple = sendrateStack.pop();
	Server->SendingIDmap = map->second;
	Server->prevpriority = map->first;
	Server->sendrate = *ratetuple;
	std::cerr << "pop stack size : " << IDmapStack.size() << std::endl;
	std::cerr << "poped sendrate : " << ";rate S,M,L ; "
			<< std::get<0>(Server->sendrate) << ";"
			<< std::get<1>(Server->sendrate) << ";"
			<< std::get<2>(Server->sendrate) << std::endl;
}

int BrokerServer::InterruptController::readMapPriority() {
	//スタックが空なら-1で返却
	if (IDmapStack.empty())
		return -1;
	std::cerr << "stack not empty" << std::endl;
	return IDmapStack.read()->first;
}

}

