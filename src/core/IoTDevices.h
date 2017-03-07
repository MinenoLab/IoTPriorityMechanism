/*
 * IoTDevices.h
 *
 *  Created on: 2016/02/29
 *      Author: tachibana
 */

#ifndef IOTDEVICES_H_
#define IOTDEVICES_H_

#include "iEntity.h"
#include <iostream>
#include <memory>
#include <thread>
#include <mutex>
#include <time.h>
#include <sys/time.h>
#include "boost/system/error_code.hpp"
#include <boost/asio.hpp>

#include "structure/IoTDeviceBuffer.h"
#include "structure/BrokerPriorityQueue.h"
#include "structure/MultiMessageQueue.h"
#include "structure/PermitFormat.h"
#include "structure/PriReturnValue.h"
#include "structure/PriorityMessage.h"
#include "structure/MetaData.h"
#include "structure/UpdateMessage.h"
#include "utils/SyncClientSocket.h"
#include "utils/SyncBaseSockSession.h"
#include "StateBase.h"
#include "StateSwicher.h"

namespace asio = boost::asio;

namespace IoTPriority {

class IoTDevices: public IoTPriority::iEntity,
		public std::enable_shared_from_this<IoTDevices> {
public:
	/**
	* @breif コンストラクタ、デフォルトのポートでAppServerを起動する
	*/
	IoTDevices();
	/**
	* @breif コンストラクタ、指定したポートでAppServerを起動する
	* @param tobrport ブローカーサーバ接続先ポート
	* @param toapport APサーバ接続先ポート
	*/
	IoTDevices(int tobrport, int toapport);
	virtual ~IoTDevices();
	/**
	* @breif デバイスIDの設定
	* @param deviceID 設定したいデバイスID
	* @return 常に1
	*/
	int api_setDeviceID(int deviceID);
	/**
	* @breif API,デフォルトデータタイプの設定
	* @param deviceID 設定したいデータタイプ
	* @return 常に1
	* @detail 現状不使用
	*/
	int api_setDataType(std::string dataType);
	/**
	* @breif API,ブローカーサーバのホスト名を設定
	* @param hostname ブローカーサーバのホスト名
	* @return 常に1
	*/
	int api_setBroker(std::string hostname);
	/**
	* @breif API,IoTデバイスの開始
	* @param keepalive 不使用
	* @return 不定(意味なし)
	*/
	int api_resist(int keepalive);
	/**
	* @breif API,Appサーバのストップ要求(未実装)
	* @return 常に0
	*/
	int api_fin();
	/**
	* @breif API,優先度制御を使ったデータ送信
	* @param dataType 送信データのデータタイプ
	* @param data 送信データ
	* @return 常に1
	*/
	int api_sendData(std::string dataType, std::string data);

	void api_setRecvCallback(std::function<void(std::string, int)> onrecv); //基本送信制御なので使わないはず

	/**
	* @breif 状態遷移の動作開始
	*/
	void run_Entitiy();
	/**
	* @breif 状態遷移の初期化
	* @details 必ずインスタンス化直後に呼び出すこと
	*/
	void initState();
	/**
	* @breif APサーバへの接続
	* @param hostname APサーバホスト名
	* @port　ポート番号
	*/
	void connectAPServer(std::string hostname, int port);
	/**
	* @breif ブローカーへの再接続要求
	* @return 成功ならtrue
	*/
	bool br_startReconnect();
	/**
	* @breif APサーバへの再接続要求
	* @return 成功ならtrue
	*/
	bool ap_startReconnect();
	/**
	* @breif デバイス内バッファへのポインタを取得
	* @return デバイス内バッファへのポインタ
	*/
	std::shared_ptr<std::map<int, std::shared_ptr<IoTDeviceBuffer>>>getBuffer();
	/**
	* @breif 削除候補剪定用キューに情報を登録
	* @param priority 優先度
	* @param tm 到着時刻
	* @param dataid データID
	*/
	void putRemoveQueue(int priority,struct timeval tm,int dataid);
	/**
	* @breif 削除候補剪定用キューから次に消すべきデータのデータIDを取得
	* @return 次に消すべきデータID
	*/
	int getRemovedata();
	/**
	* @breif メタデータ送信前の送信データを保持するキューが空か確認
	* @return 空ならばtrue
	*/
	bool emptyDeviceQueue();
	/**
	* @breif メタデータ送信前の送信データを保持するキューからデータを確認する
	* @return <dataType,data>のpair
	* @details 優先度キューから先頭データを削除したい場合はputDeviceQueue()を使う
	* @sa putDeviceQueue()
	*/
	std::shared_ptr<std::pair<std::string,std::string>> readDeviceQueue();
	/**
	* @breif メタデータ送信前の送信データを保持するキューからデータを取り出す
	* @return <dataType,data>のpair
	* @details 優先度キューから先頭データを削除したい場合はreadDeviceQueue()を使う
	* @sa readDeviceQueue()
	*/
	std::shared_ptr<std::pair<std::string,std::string>> putDeviceQueue();
	/**
	* @breif デバイスIDを取得
	* @return デバイスID
	*/
	int getDeviceID();

	//メインスレッドから呼び出し、キューからデータを取り出して処理(次の状態も呼び出す)
	/**
	* @breif ブローカーからの受信データを取り出す
	* @return PriorityMessage形式のデータ
	* @details 受信データを処理する場合にはこの関数でデータを取得する
	*/
	std::shared_ptr<PriorityMessage> br_recv_and_getdata();
	/**
	* @breif APサーバからの受信データを取り出す
	* @return PriorityMessage形式のデータ
	* @details 受信データを処理する場合にはこの関数でデータを取得する
	*/
	std::shared_ptr<PriorityMessage> ap_recv_and_getdata();
	/**
	* @breif APサーバからの受信データキューにデータをpushする
	* @param pdata PriorityMessage形式のデータ
	*/
	void ap_recvqueue_repush(std::shared_ptr<PriorityMessage> pdata);
	/**
	* @breif ブローカーまたはAPサーバからの受信データのタイプを取得する
	* @return PriorityMessage形式のデータタイプ
	* @details ブローカーからのデータが優先される、いずれかのデータが到着するまで待ち続ける
	* @sa recv_and_gettype_single()
	*/
	int recv_and_gettype();
	/**
	* @breif 現在ブローカーまたはAPサーバからの受信データがする場合、そのタイプを取得する
	* @return PriorityMessage形式のデータタイプ、どちらにも受信データが存在しなければ-1
	* @details ブローカーからのデータが優先される。いずれのデータもなければ-1を返して終了する
	* @sa recv_and_gettype()
	*/
	int recv_and_gettype_single();
	/**
	 * @breif 割り込み通信からの復帰を通知
	 */
	void ap_sendResum();
	/**
	 * @breif 割り込み通信の開始を通知
	 */
	void ap_changeSendData();
	/**
	 * @breif 送信レートの変更を相対値で通知(現在不使用)
	 * @param rate 変更後の通信レート(相対値)
	 * @sa ap_changeSendRateAbs
	 */
	void ap_changeSendRate(float rate);
	/**
	 * @breif 送信レートの変更を絶対値で通知(現在不使用)
	 * @param rate 変更後の通信レート(絶対値)
	 * @sa ap_changeSendRate()
	 */
	void ap_changeSendRateAbs(float rate);
	/**
	* @breif ブローカーへのデータ送信
	* @param data 送信データ
	* @return 成功ならtrue
	*/
	bool br_send(std::shared_ptr<std::string> data);
	/**
	* @breif APサーバへのデータ送信
	* @param data 送信データ
	* @param rate　送信レート、0未満ならば通常通りの送信
	* @return 成功ならtrue
	* @details rateの指定機能は現在不使用、コンテンツデータの送信などはデータを分割して送信するためrateに正の値を指定する
	*/
	bool ap_send(std::shared_ptr<std::string> data,int rate);
	/**
	* @breif ブローカーサーバとの接続状態確認
	* @return 現在の状態
	*/
	Sync_BaseSockSession::SockState br_checkState();
	/**
	* @breif APサーバとの接続状態確認
	* @return 現在の状態
	*/
	Sync_BaseSockSession::SockState ap_checkState();
	/**
	 * @breif ブローカからの受信データのキューが空か確認する
	 * @return 空ならばtrue
	 */
	bool br_queue_empty();

	//ネットワークスレッドから呼び出し、キューへ突っ込む
	/**
	* @breif ブローカーからのデータを受け取る
	* @param data ブローカーからのデータ
	* @param sesid ソケットのセッションID(ファイルディスクリプタ)
	* @details データは長さ不完全、ネットワークスレッドから呼び出す
	*/
	void onRecvFromBroker(std::string data, int sesid);
	/**
	* @breif APサーバからのデータを受け取る
	* @param data デバイスからのデータ
	* @param sesid ソケットのセッションID(ファイルディスクリプタ)
	* @details データは長さ不完全、ネットワークスレッドから呼び出す
	*/
	void onRecvFromAPServer(std::string data,int sesid);

	std::string name_state0;
	std::string name_state1;
	std::string name_state2;
	std::string name_state3;
	std::string name_state4;
	std::string name_state5;
	std::string name_state6;
	std::string name_state7;
	std::string name_state8;
	std::string name_state9;
	std::string name_state10;
	std::string name_state11;

	int DeviceID;
	long Buffersize;//Bufferサイズは32MB仮定
	std::map<int,std::shared_ptr<IoTDeviceBuffer>> Buffer;

	//Test Function
	void putMessage(bool isserver,std::shared_ptr<PriorityMessage> p);

	//出力用
	long OtherMessCount;
	long MessCount;

private:
	std::string BrokerHostName,ServerHostName;
	int apport;
	int BROKERPORTNUM;
	std::vector<std::string> DataType;

	bool keepalive;
	std::shared_ptr<Sync_ClientSocket> toBrokerSocket,toAPServerSocket;
	asio::io_service io_service_BR, io_service_AP;
	std::string server_remaindata, broker_remaindata;
	std::function<void(std::string, int)> onRecv_Func;
	Multi_MessageQueue<PriorityMessage> server_recvqueue, broker_recvqueue;
	void init_onlypriority();

	//TODO 現状バッファはメモリ上に実装、二次記憶への移行も検討
	//std::map<int,std::shared_ptr<IoTDeviceBuffer>> Buffer;
	BrokerPriorityQueue<int,int,std::less<int>>::PriorityQueue bufferRemoveQueue;

	//メタデータ送信前の待避所<データ種類、データ>
	Multi_MessageQueue<std::pair<std::string,std::string>> DeviceDataqueue;
	void onRecv_base(std::string data, Multi_MessageQueue<PriorityMessage>* recvqueue,std::string* remaindata);

};

/**
 * @breif 終了状態
 */
class D_State00: public StateBase {
public:
	D_State00(std::shared_ptr<IoTDevices> device);
	virtual ~D_State00();
	std::string execute();
private:
	std::shared_ptr<IoTDevices> Device;
};

/**
 * @breif 待機状態
 */
class D_State05: public IoTPriority::StateBase {
public:
	D_State05(std::shared_ptr<IoTDevices> device);
	virtual ~D_State05();
	std::string execute();
private:
	std::shared_ptr<IoTDevices> device;
	struct timeval PrevTime;
	static const int INTERVAL=60;
};

/**
 * @breif データを受け取理メタデータを送信する状態
 */
class D_State06: public StateBase {
public:
	D_State06(std::shared_ptr<IoTDevices> device);
	virtual ~D_State06();
	std::string execute();
private:
	std::shared_ptr<IoTDevices> Device;
	void testinit();
	int DataID;
	int getDataID();
};

/**
 * @breif 優先度を受信する状態
 */
class D_State07: public StateBase {
public:
	D_State07(std::shared_ptr<IoTDevices> device);
	virtual ~D_State07();
	std::string execute();
private:
	std::shared_ptr<IoTDevices> Device;
	long calcBufferSize();
	std::vector<int> removeBufferData(long sub);

};

/**
 * @breif ブローカーの許可メッセージに従い送信を始める状態
 */
class D_State08: public IoTPriority::StateBase {
public:
	D_State08(std::shared_ptr<IoTDevices> device);
	virtual ~D_State08();
	std::string execute();
private:
	std::shared_ptr<IoTDevices> device;

};

/**
 * @breif ブローカーに送信完了を伝える状態
 */
class D_State09: public StateBase {
public:
	D_State09(std::shared_ptr<IoTDevices> device);
	virtual ~D_State09();
	std::string execute();
private:
	std::shared_ptr<IoTDevices> device;
};

/**
 * @breif ブローカーから割り込み関係の通知を受信した状態
 */
class D_State10: public StateBase {
public:
	D_State10(std::shared_ptr<IoTDevices> device);
	virtual ~D_State10();
	std::string execute();
private:
	std::shared_ptr<IoTDevices> device;
};

/**
 * @breif 接続確認を行う状態
 */
class D_State11: public StateBase {
public:
	D_State11(std::shared_ptr<IoTDevices> device);
	virtual ~D_State11();
	std::string execute();
private:
	std::shared_ptr<IoTDevices> device;
};

}
/* namespace IoTPriority */
#endif /* IOTDEVICES_H_ */
