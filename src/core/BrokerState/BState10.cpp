/*
 * BState10.cpp
 *
 *  Created on: 2016/03/30
 *      Author: tachibana
 */

#include "../BrokerServer.h"

namespace IoTPriority {

B_State10::B_State10(std::shared_ptr<BrokerServer> server) :
		Server(server) {
}

B_State10::~B_State10() {
}

std::string B_State10::execute() {
	std::cerr << "[q-BR]State 10" << std::endl;
	{
		std::unique_lock<std::mutex> lk(Server->mtx);
		if (Server->endthread) {
			std::cerr << "[q-BR]State 10 release" << std::endl;
			return "end";
		}
	}
	//スタックからの復帰
	if (Server->isemptysending()) {
		std::cerr << "[q-BR]State 10 empty" << std::endl;
		bool ret = Server->iContorol->returnInterruptHighPriority();
		if (ret) {
			auto dm = Server->getallsendingData();
			for (auto d : dm)
				countclass_base(std::get<3>(d));
			Server->sendrate = calcpermitRate();
			printCountRateStr(Server->prevpriority);
			Server->iContorol->reAllocateRate(dm);
			return "";
		}
		std::cerr << "[q-BR]State 10 not stack_Back" << std::endl;
	}

	if (!Server->emptyMainPriorityQueue())
		//高優先度による割り込みか判断、割り込みならばその処置をする
		Server->iContorol->interruptHighPriority();

	//キューからのデータ取り出しと送信
	std::map<int,
			std::pair<int, std::pair<struct timeval, std::shared_ptr<MetaData>>> >priDeviceMap;
	std::vector<int> priDeviceMapIDs;
	std::vector<
			std::pair<int, std::pair<struct timeval, std::shared_ptr<MetaData>>> >bufpopdata;
	bool emptySending = Server->isemptysending();
	std::cerr << "[q-BR]State 10 popPriQueue_start" << std::endl;
	if (emptySending) {
		NS = 0, NM = 0, NL = 0;
		std::cerr << "[q-BR]State 10 start_first" << std::endl;
		//(emptyの場合)最初のデータ取りだし
		auto popdata = Server->popMainPriorityQueue();
		{
			std::unique_lock<std::mutex> lk(Server->iContorol->stackmtx);
			(Server->iContorol->stackAct) = false;
		}
		priDeviceMap.insert(
				std::make_pair(popdata.second.second->getDeviceID(), popdata));
		priDeviceMapIDs.push_back(popdata.second.second->getDeviceID());
		countclass(popdata.second.second->getDataSize());
		printPopDebugStr(popdata);
		Server->prevpriority = popdata.first;
	}
	//2つ目以降のデータ取り出し
	while (1) {
		if (Server->emptyMainPriorityQueue())
			break;
		auto popdata = Server->popMainPriorityQueue();
		//優先度が異なったら(低かったら)、終了
		if (popdata.first != Server->prevpriority) {
			bufpopdata.push_back(popdata);
			std::cerr
					<< "[q-BR]State 10 popPriQueuefirst Priority Different. : "
					<< popdata.first << std::endl;
			break;
		}
		//同じデバイスで別のデータがすでに登録されていたらやめる
		if (priDeviceMap.find(popdata.second.second->getDeviceID())
				!= priDeviceMap.end()) {
			std::cerr
					<< "[q-BR]State 10 popPriQueuefirst thisDeviceid was exist. : "
					<< popdata.second.second->getDeviceID() << std::endl;
			bufpopdata.push_back(popdata);
		} else {
			printPopDebugStr(popdata);
			priDeviceMap.insert(
					std::make_pair(popdata.second.second->getDeviceID(),
							popdata));
			priDeviceMapIDs.push_back(popdata.second.second->getDeviceID());
			//TODO emptyじゃなかった時の対応を考える(優先度が異なる可能性がある)
			if (emptySending)
				countclass(popdata.second.second->getDataSize());
			//emptyじゃない場合は1つしか選択しない
			if (!emptySending)
				break;
		}
	}
	for (auto d : bufpopdata) {
		Server->pushMainPriorityQueue(d.first, d.second.first, d.second.second);
	}
	std::cerr << "[q-BR]State 10 popPriQueue_end" << std::endl;

	//許可レート計算
	if (emptySending) {
		Server->sendrate = calcpermitRate();
		printCountRateStr(Server->prevpriority);
	}
	if (priDeviceMapIDs.empty()) {
		auto dm = Server->getallsendingData();
		for (auto d : dm)
			countclass_base(std::get<3>(d));
		Server->sendrate = calcpermitRate();
		printCountRateStr(Server->prevpriority);
		Server->iContorol->reAllocateRate(dm);
	}

	//レート割り当てと送信許可
	for (auto k : priDeviceMapIDs) {
		float sendrate;
		auto it = priDeviceMap.find(k);
		auto senddata = it->second;
		//クラス計算
		float size = senddata.second.second->getDataSize();
		int sclass = PriReturnValue::calcSclass(size);
		//送信対象か確認
		if (emptySending) {
			int ret = decountclass(size);
			switch (ret) {
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
		} else {
			sendrate = calcaddrate(size);
		}
		if (sendrate < 0) {
			//送信できなければ戻す
			Server->pushMainPriorityQueue(senddata.first, senddata.second.first,
					senddata.second.second);
			std::cerr << "[q-BR]State 10 no enugh rate..." << std::endl;
			continue;
		}
		//メッセージ作成
		PermitFormat p;
		PriorityMessage primess;
		p.set(senddata.second.second->getDataID(), sendrate); //TODO emptyじゃないときのレートは仮
		primess.makeMessage(0xff, senddata.second.second->getDeviceID(),
				PriorityMessage::MESSTYPE_PERMIT, PriorityMessage::NOACK,
				p.getMessage());
		//送信中データの登録
		Server->registSending(primess.getReceverDeviceID(),
				senddata.second.second->getDataID(), senddata.first, sclass);
		//送信
		Server->dv_send(primess.getReceverDeviceID(), primess.getAllData());
		printSendStr(sendrate, primess, senddata);
	}
	return "";
}

//TODO　タプル<floatfloatfloat>
std::tuple<float, float, float> B_State10::calcpermitRate() {
	const float SDEF = 16 * 1024, MDEF = 32 * 1024;
	//auto datavector=Server->getallsendingData(priority);
	float ws, wm, wl;
	float wd = Server->getCurCommRate();
	Server->prevrate = wd;
	if (NM == 0 && NL == 0)
		return std::make_tuple(wd / NS, -1, -1);
	ws = SDEF;
	if (ws * NS > wd - ws && NS > 0)
		return std::make_tuple(wd / NS, -1, -1);
	wd -= ws * NS;
	wm = SDEF;
	if (wm * NM > wd) {
		//一番小さいデータがこれならしょうがない
		while (wm * NM > wd && NM > 1) {
			NM--;
		}
		wm = wd / NM;
		return std::make_tuple(ws, wm, -1);
	}
	wm = wd / NM;
	if (NL == 0 || wm <= MDEF)
		return std::make_tuple(ws, wm, -1);
	wm = MDEF;
	wd -= wm * NM;
	wl = MDEF;
	if (wl * NL > wd) {
		//一番小さいデータがこれならしょうがない
		while (wl * NL > wd && NL > 1) {
			NL--;
		}
	}
	wl = wd / NL;
	return std::make_tuple(ws, wm, wl);
}

float B_State10::calcaddrate(float size) {
	const float SDEF = 16 * 1024, MDEF = 32 * 1024;
	int sclass = PriReturnValue::calcSclass(size);
	float nouserate;
	switch (Server->prevSClass) {
	case PriReturnValue::CLASS_S:
		nouserate = std::get<0>(Server->sendrate);
		break;
	case PriReturnValue::CLASS_M:
		nouserate = std::get<1>(Server->sendrate);
		break;
	case PriReturnValue::CLASS_L:
		nouserate = std::get<2>(Server->sendrate);
		break;
	}
	float r = Server->getCurCommRate() - Server->prevrate + nouserate;
	std::cerr << "[q-BR]State 10  rate,cur,prev,val " << r << " , "
			<< Server->getCurCommRate() << " , " << Server->prevrate << " , "
			<< nouserate << std::endl;
	switch (sclass) {
	case PriReturnValue::CLASS_S:
		return r;
	case PriReturnValue::CLASS_M:
		if (r > SDEF) {
			return r;
		}
		return -1;
	case PriReturnValue::CLASS_L:
		if (r > MDEF) {
			return r;
		}
		return -1;
	default:
		break;
	}
	return -1;
}

void B_State10::countclass(float size) {
	int sclass = PriReturnValue::calcSclass(size);
	countclass_base(sclass);
}

void B_State10::countclass_base(int sclass) {
	switch (sclass) {
	case PriReturnValue::CLASS_S:
		NS++;
		break;
	case PriReturnValue::CLASS_M:
		NM++;
		break;
	case PriReturnValue::CLASS_L:
		NL++;
		break;
	default:
		break;
	}
}

//TODO NS...が以下だった時の対応を今後書いて試す
int B_State10::decountclass(float size) {
	int sclass = PriReturnValue::calcSclass(size);
	switch (sclass) {
	case PriReturnValue::CLASS_S:
		if (std::get<0>(Server->sendrate) > 0) {
			NS--;
			return 0;
		}
		break;
	case PriReturnValue::CLASS_M:
		if (std::get<1>(Server->sendrate) > 0) {
			NM--;
			return 1;
		}
		break;
	case PriReturnValue::CLASS_L:
		if (std::get<2>(Server->sendrate) > 0) {
			NL--;
			return 2;
		}
		break;
	default:
		break;
	}
	return -1;
}

void B_State10::printPopDebugStr(
		std::pair<int, std::pair<struct timeval, std::shared_ptr<MetaData>>>& popdata) {
	std::stringstream debugstr;
	debugstr << "popPriQueue DeviceID,Dataid,Priority"
	<< popdata.second.second->getDeviceID() << " , "
	<< popdata.second.second->getDataID() << " , " << popdata.first;
	std::cerr << "[q-BR]State 10 "<<debugstr.str()<< std::endl;
	std::cout<< iEntity::strnowtime() <<debugstr.str()<< std::endl;
}

void B_State10::printCountRateStr(int priority) {
	std::stringstream debugstr, debugstr2;
	debugstr << ";put priority;" << priority << ";Datacount S,M,L ; " << NS
			<< ";" << NM << ";" << NL;
	debugstr2 << ";put priority;" << priority << ";rate S,M,L ; "
			<< std::get<0>(Server->sendrate) << ";"
			<< std::get<1>(Server->sendrate) << ";"
			<< std::get<2>(Server->sendrate);
	std::cerr << "[q-BR]State 10 " << debugstr.str() << std::endl;
	std::cerr << "[q-BR]State 10 " << debugstr2.str() << std::endl;
	std::cout << iEntity::strnowtime() << debugstr.str() << std::endl;
	std::cout << iEntity::strnowtime() << debugstr2.str() << std::endl;
}

void B_State10::printSendStr(int sendrate, PriorityMessage& primess,
		std::pair<int, std::pair<struct timeval, std::shared_ptr<MetaData>>>&senddata) {
			std::stringstream debugstr;
			debugstr <<";popPriQueue;DeviceID;"
			<< senddata.second.second->getDeviceID() << " ;Dataid;"
			<< senddata.second.second->getDataID() << " ;Priority;"
			<< senddata.first <<" ;rate;"<<sendrate;
			std::cerr << "[q-BR]State 10 "<<debugstr.str()<< std::endl;
			std::cout << iEntity::strnowtime()<<debugstr.str()<< std::endl;
		}

	}
	/* namespace IoTPriority */
