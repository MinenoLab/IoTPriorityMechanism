/*
 * PriorityMessage.h
 *
 *  Created on: 2016/03/03
 *      Author: tachibana
 */

#ifndef PRIORITYMESSAGE_H_
#define PRIORITYMESSAGE_H_

#include <iostream>
#include <memory>
#include <bitset>
#include <stdexcept>
#include "../utils/BinaryConverter.h"

namespace IoTPriority {
/**
 * @breif 優先度制御のすべての通信で利用するTCP上の通信フォーマット
 */
class PriorityMessage {
public:
	/**
	 * @brief コンストラクタ
	 */
	PriorityMessage();
	virtual ~PriorityMessage();
	static const int MESSTYPE_REGIST=0x0;
	static const int MESSTYPE_REQUEST=0x1;
	static const int MESSTYPE_KEEPALIVE=0x2;
	static const int MESSTYPE_OFFER=0x3;
	static const int MESSTYPE_CONNECT=0x4;
	static const int MESSTYPE_REQLIST=0x5;
	static const int MESSTYPE_META=0x10;
	static const int MESSTYPE_PRI=0x11;
	static const int MESSTYPE_DELLIST=0x12;
	static const int MESSTYPE_PERMIT=0x13;
	static const int MESSTYPE_UPDATE=0x14;
	static const int MESSTYPE_COMPLETE=0x15;
	static const int MESSTYPE_DATA=0x16;
	static const int MESSTYPE_RATE=0x17;
	static const int MESSTYPE_DECDATA=0x18;
	static const int MESSTYPE_STOPREQ=0x20;
	static const int MESSTYPE_CALLOFF=0x21;
	static const int MESSTYPE_FIN=0x22;
	static const int NOACK=0x0;
	static const int ACK=0x1;
	static const int NAK=0x2;

	/**
	* @breif メッセージに含める情報のセット
	* @param senderDeviceID 送信元デバイスID
	* @param ReciverDeviceID 送信元デバイスID
	* @param messType メッセージタイプ
	* @param ackType ACKタイプ
	* @param data ペイロードデータ
	* @return 常に１
	*/
	int makeMessage(int senderDeviceID,int ReciverDeviceID,int messType,int ackType,std::shared_ptr<std::string> data);
	/**
	* @breif ネットワークに流すバイナリ形式のメッセージヘッダに変換して取得する
	* @return バイナリ形式のメッセージヘッダ
	* @sa getContentData()
	* @sa getAllData()
	*/
	std::shared_ptr<std::string> getMessageHeader();
	/**
	* @breif ネットワークに流すバイナリ形式のメッセージペイロードに変換して取得する
	* @return バイナリ形式のメッセージペイロード
	* @sa getMessageHeader()
	* @sa getAllData()
	*/
	std::shared_ptr<std::string> getContentData();
	/**
	* @breif ネットワークに流すバイナリ形式のメッセージ全体に変換して取得する
	* @return バイナリ形式のメッセージペイロード
	* @sa getMessageHeader()
	* @sa getAllData()
	* @sa putMessage
	*/
	std::shared_ptr<std::string> getAllData();
	/*
	* @breif メッセージの送信元デバイスIDを取得する
	* @return 送信元デバイスID
	*/
	int getSenderDeviceID();
	/*
	* @breif メッセージの送信先デバイスIDを取得する
	* @return 送信先デバイスID
	*/
	int getReceverDeviceID();
	/*
	* @breif メッセージのメッセージタイプを取得する
	* @return メッセージタイプ
	*/
	int getMessType();
	/*
	* @breif メッセージのACKタイプを取得する
	* @return ACKタイプ
	*/
	int getAckType();
	/**
	 * @breif 内容を全て消去する
	*/
	void cleanall();
	/**
	* @breif ネットワークから流れてきたバイナリ形式のメッセージを登録する
	* @param data バイナリ形式のメッセージ
	* @return メッセージの長さを超えるデータを代入した場合、余りを返す
	* @details メッセージの長さ未満のデータを代入した場合、返り値は空文字である。メッセージの長さ以上のデータを代入した場合isAllDataRecieved()がtrueになる。
	* @sa getAllData()
	* @sa isAllDataRecieved()
	*/
	std::string putMessage(std::string data);
	/**
	* @breif 正しい長さのバイナリデータが登録されているか確認する
	* @return 登録されたメッセージの長さが正しければtrue,足りなければfalse
	* @sa putMessage()
	*/
	bool isAllDataRecieved();


private:
	int SenderDeviceID,ReceverDeviceID;
	int MessType,AckType;
	std::string Header;
	std::shared_ptr<std::string> data;
	bool isOK;

};

} /* namespace IoTPriority */
#endif /* PRIORITYMESSAGE_H_ */
