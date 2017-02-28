/*
 * ANetworkStates.cpp
 *
 *  Created on: 2017/02/13
 *      Author: tachibana
 */

#include "../AppServer.h"

namespace IoTPriority {

A_RecvFunctions::A_RecvFunctions(std::shared_ptr<AppServer> server) {
	// TODO 自動生成されたコンストラクター・スタブ

}

A_RecvFunctions::~A_RecvFunctions() {
	// TODO Auto-generated destructor stub
}

void A_RecvFunctions::mesureLength(std::shared_ptr<PriorityMessage> mess,
		int dataLength) {
	//Deviceからの受信サイズ測定
	std::cerr << "receive pri_calc  length: " << dataLength << std::endl;
	Server->devid_byte_map[mess->getSenderDeviceID()][Server->devid_pri_map[mess->getSenderDeviceID()]] +=
			dataLength;
	std::cerr << "pri,length : "
			<< Server->devid_pri_map[mess->getSenderDeviceID()] << " ,"
			<< Server->devid_byte_map[mess->getSenderDeviceID()][Server->devid_pri_map[mess->getSenderDeviceID()]]
			<< std::endl;
}

std::string A_RecvFunctions::execute_connect(std::shared_ptr<PriorityMessage> mess,int sesid,std::string rdata) {
	auto mit = Server->dvid_sesid_map.left.find(mess->getSenderDeviceID());
	if (mit == Server->dvid_sesid_map.left.end()) {
		Server->dvid_sesid_map.insert(bival(mess->getSenderDeviceID(), sesid));
		return rdata;
	} else if (BinaryConverter::strtoInt8(*mess->getContentData())==1) {//TODO prioritymessageに1=trueという定義を追記
		//もし再接続なら
		int prev_sesid = mit->get_right();
		//既に別のセッションが同じdeviceIDを使っている->マップの置き換え
		Server->dvid_sesid_map.left.replace_data(mit, sesid);
		//切断前のremaindataを連結
		std::string returndata = Server->getDeviceRemainData(prev_sesid) + rdata;
		Server->dv_close(prev_sesid);
		return returndata;
	} else {
		std::cerr << "this ID is already used :" << mess->getSenderDeviceID()
				<< std::endl;
		return rdata;
	}
}

void A_RecvFunctions::execute_data(std::shared_ptr<PriorityMessage> mess,
		std::function<void(std::string, int)> onrecv) {
	//DataIDとデータを切り取り
	std::string strDid = mess->getContentData()->substr(0, 2);
	std::string strData = mess->getContentData()->substr(2);
	//ACK送信
	auto p = std::make_shared<PriorityMessage>();
	p->makeMessage(Server->DeviceID, mess->getSenderDeviceID(),
			PriorityMessage::MESSTYPE_DATA, PriorityMessage::ACK,
			std::make_shared<std::string>(strDid));
	Server->dv_send(mess->getSenderDeviceID(), p->getAllData());
	std::cerr << "ACK sended" << std::endl;

	//ハンドラが登録してある場合にはデータのみハンドラに投げてキューは登録しない
	//データメッセージは他のキューと分離する
	if (onrecv != NULL) {
		onrecv(strData, mess->getSenderDeviceID());
	} else {
		if (strData.length() > 5)
			std::cerr << "recved Datamessage at: " << strData.substr(5, 20)
					<< std::endl;
		else
			std::cerr << "recved Datamessage" << std::endl;

		int idataid = BinaryConverter::strtoInt16(strDid);
		std::cout << iEntity::strnowtime() << ";data recv DeviceID;"
				<< mess->getSenderDeviceID() << ";DataID;" << idataid
				<< std::endl;

		//TODO in real use
		//Server->pushtoUserQueue(mess);
	}
}

void A_RecvFunctions::execute_decdata(std::shared_ptr<PriorityMessage> mess) {
	Server->devid_pri_map[mess->getSenderDeviceID()] =
			BinaryConverter::strtoInt8(*mess->getContentData());
	if (Server->devid_byte_map.find(mess->getSenderDeviceID())
			== Server->devid_byte_map.end()) {
		Server->devid_byte_map[mess->getSenderDeviceID()] = std::vector<int>(16,
				0);
	}
}

} /* namespace IoTPriority */
