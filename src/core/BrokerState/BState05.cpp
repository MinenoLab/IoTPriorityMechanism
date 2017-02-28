/*
 * BState05.cpp
 *
 *  Created on: 2016/03/30
 *      Author: tachibana
 */

#include "../BrokerServer.h"

namespace IoTPriority {

B_State05::B_State05(std::shared_ptr<BrokerServer> server) :
		Server(server) {
	// TODO 自動生成されたコンストラクター・スタブ

}

B_State05::~B_State05() {
	// TODO Auto-generated destructor stub
}

std::string B_State05::execute() {
	std::cerr << iEntity::strnowtime()<<"[BR]State 5" << std::endl;
	do {
		std::cerr << "[BR]State 5 startpop" << std::endl;
		auto d = Server->dv_recv_and_getdata();
		std::cerr << "[BR]State 5 endpop" << std::endl;
		auto contdata = d->getContentData();
		//メタデータの整形
		MetaData mdata;
		mdata.set_fromMetaData(*contdata);
		struct timeval nowtime;
		auto ptr_mdata = std::make_shared<MetaData>(mdata);
		std::cerr << "[BR]State 5 endmakemeta" << std::endl;
		//時刻取得
		gettimeofday(&nowtime, NULL);
		int priority = Server->getPriorityValueFromTable(ptr_mdata, nowtime);
		std::cerr << "[BR]State 5 endgetpri" << std::endl;
		//メッセージ作成
		PriorityMessage primess;
		PriReturnValue value;
		value.set(mdata.getDataID(), PriReturnValue::calcSclass(mdata.DataSize),
				priority);
		primess.makeMessage(0xff, d->getSenderDeviceID(),
				PriorityMessage::MESSTYPE_PRI, PriorityMessage::NOACK,
				value.getMessage());
		std::cerr << "[BR]State 5 endmakemess" << std::endl;
		//送信
		std::cerr << "[BR]State5 sendID : " << primess.getReceverDeviceID()
				<< std::endl;
		std::cerr << "[BR]State5 dataID,priority : " << value.getdataID()
				<< " , " << value.getPriority() << std::endl;
		Server->dv_send(primess.getReceverDeviceID(), primess.getAllData());
		//優先度キューに挿入
		Server->pushMainPriorityQueue(priority, nowtime, ptr_mdata);
		//現在送信中のデータより優先度が高かったら割り込み信号を送る
		{
			std::unique_lock<std::mutex> lk(Server->iContorol->stackmtx);
			if (priority < Server->prevpriority)
				(Server->iContorol->stackAct) = true;
		}

	} while (Server->recv_and_gettype() == PriorityMessage::MESSTYPE_META);
	return "";
}

} /* namespace IoTPriority */
