/*
 * AppServer.h
 *
 *  Created on: 2016/02/29
 *      Author: tachibana
 */

#ifndef APPSERVER_H_
#define APPSERVER_H_

#include "iEntity.h"
#include <memory>
#include <iostream>
#include <boost/system/error_code.hpp>
#include <thread>
#include <mutex>
#include <sys/time.h>
#include <time.h>

#include "structure/MultiMessageQueue.h"
#include "structure/PriorityMessage.h"
#include "structure/MetaData.h"
#include "utils/SyncBaseSockSession.h"
#include "utils/SyncServerSockets.h"
#include "utils/SyncClientSocket.h"
#include "StateSwicher.h"
#include "StateBase.h"



namespace IoTPriority {

class AppServer: public IoTPriority::iEntity,
		public std::enable_shared_from_this<AppServer> {
public:
	/**
	* @breif コンストラクタ、デフォルトのポートでAppServerを起動する
	*/
	AppServer();
	/**
	* @breif コンストラクタ、指定したポートでAppServerを起動する
	* @param tobrport ブローカーサーバ接続先ポート
	* @param todvport IoTデバイスListenポート
	*/
	AppServer(int tobrport,int todvport);
	virtual ~AppServer();
	//----必ずインスタンス化直後に呼び出すこと
	/**
	* @breif 状態遷移の初期化
	* @details 必ずインスタンス化直後に呼び出すこと
	*/
	void initState();
	/**
	* @breif API,Appサーバ機能の開始
	* @return 不定(意味なし)
	*/
	int api_Request();
	/**
	* @breif API,Brokerサーバへの優先度要求登録(未実装)
	* @return 常に1
	* @details 未実装のため特に動作しない
	*/
	int api_registReqTable(std::string table);
	/**
	* @breif API,Brokerサーバのホスト名設定
	* @return 常に1
	*/
	int api_setBroker(std::string hostname);
	/**
	* @breif API,Appサーバのストップ要求
	* @return 成功したら１，そうでなければ-1
	*/
	int api_stop();
	//前者はコールバック形式でデータを利用、後者はキューからの呼び出し形式でデータを利用
	/**
	* @breif API,データ受信時のコールバック関数登録
	* @param onrecv 登録するコールバック関数
	*/
	void api_setRecvCallback(std::function<void(std::string, int)> onrecv);
	/**
	* @breif API,Appサーバのキュー内に溜まっている受信データの取り出し
	* @return <デバイスID,データ>形式の受信データ
	* @details api_setRecvCallback()でコールバックを登録した場合、このメソッドでデータは取得できない
	* @sa api_setRecvCallback()
	*/
	std::pair<int, std::string> api_getrecvdata();

	/**
	* @breif 状態遷移の動作開始
	*/
	void run_Entitiy();

	//ネットワークスレッドから呼び出し、キューへ突っ込む
	/**
	* @breif デバイスからのデータを受け取る
	* @param data デバイスからのデータ
	* @param sesid ソケットのセッションID(ファイルディスクリプタ)
	* @details データは長さ不完全、ネットワークスレッドから呼び出す
	*/
	void onRecvFromDevices(std::string data, int sesid);
	/**
	* @breif ブローカーからのデータを受け取る
	* @param data ブローカーからのデータ
	* @param sesid ソケットのセッションID(ファイルディスクリプタ)
	* @details データは長さ不完全、ネットワークスレッドから呼び出す
	*/
	void onRecvFromBroker(std::string data, int sesid);
	//ネットワークスレッドから呼び出し、状態変更を通知
	/**
	* @breif 通信中に別のデータの割り込みが入ったことを通知する
	* @param sesid ソケットのセッションID(ファイルディスクリプタ)
	* @details ネットワークスレッドから呼び出す
	*/
	void changeRecvData(int sesid);
	//メインスレッドから呼び出し、キューからデータを取り出して処理(次の状態も呼び出す)
	/**
	* @breif ブローカーからの受信データを取り出す
	* @return PriorityMessage形式のデータ
	* @details 受信データを処理する場合にはこの関数でデータを取得する
	*/
	std::shared_ptr<PriorityMessage> br_recv_and_getdata();
	/**
	* @breif デバイスからの受信データを取り出す
	* @return PriorityMessage形式のデータ
	* @details 受信データを処理する場合にはこの関数でデータを取得する
	*/
	std::shared_ptr<PriorityMessage> dv_recv_and_getdata();
	/**
	* @breif ブローカーまたはデバイスからの受信データのタイプを取得する
	* @return PriorityMessage形式のデータタイプ
	* @details ブローカーからのデータが優先される
	*/
	int recv_and_gettype();
	/**
	* @breif ブローカーへのデータ送信
	* @param data 送信データ
	* @return 成功ならtrue
	*/
	bool br_send(std::shared_ptr<std::string> data);
	/**
	* @breif デバイスへのデータ送信
	* @param data 送信データ
	* @param RecieverDeviceID 送信先deviceID
	* @return 成功なら1,不正なdeviceIDなら-1,その他のエラーは-2
	*/
	int  dv_send(int RecieverDeviceID, std::shared_ptr<std::string> data);bool getSendStopreq();
	/**
	* @breif ブローカーへのデータ送信
	* @param data 送信データ
	* @return 成功ならtrue
	*/
	const int getRateInterval();
	/**
	* @breif 現在の送信レート計算
	* @return 現在の送信レート
	*/
	float CalcCurRate();
	/**
	* @breif ブローカーへの再接続要求
	* @return 成功ならtrue
	*/
	bool startReconnect();
	/**
	* @breif デバイスセッションのクローズ
	* @param sesid ソケットのセッションID(ファイルディスクリプタ)
	*/
	void dv_close(int sesid);
	/**
	* @breif ブローカーサーバとの接続状態確認
	* @return 現在の状態
	*/
	Sync_BaseSockSession::SockState br_checkState();

	struct timeval PrevTime;
	const int DeviceID = 0x00;

	//測定用map 優先度記録用とバイト数記録用
	std::map<int,int> devid_pri_map;
	std::map<int,std::vector<int>> devid_byte_map;

	//ネットワークスレッド用
	void pushtoUserQueue(std::shared_ptr<PriorityMessage> mess);
	std::string getDeviceRemainData(int sesid);
	//<did,sesid>
	bimaps::bimap<int,int> dvid_sesid_map;

	const std::string name_state0 = "State0";
	const std::string name_state1 = "State1";
	const std::string name_state2 = "State2";
	const std::string name_state3 = "State3";
	const std::string name_state4 = "State4";
	const std::string name_state5 = "State5";
	const std::string name_state6 = "State6";
	const std::string name_state7 = "State7";
	const std::string name_state8 = "State8";

private:
	std::shared_ptr<Sync_ServerSockets> toIotDeviceSocket;
	std::shared_ptr<Sync_ClientSocket> toBrokerSocket;
	asio::io_service io_service_BR, io_service_DV;

	std::shared_ptr<A_RecvFunctions> recvFunctions;

	int PORTNUM = 31400;
	int BROKERPORTNUM = 35200;
	long long TotalRecvBufferSize;

	std::string prioritytable;
	std::string brokerHostName;
	bool acceptIotDevices;

	Multi_MessageQueue<PriorityMessage> device_recvqueue,
			device_touserrecvqueue, broker_recvqueue;
	std::string device_remaindata[256];
	std::string broker_remaindata;
	Multi_MessageStack<std::pair<int,std::string>> remainstack[256];
	std::function<void(std::string, int)> onRecv_Func;

	float prevRate;


	//停止用フラグ
	std::mutex mtx;
	bool SendStopreq;

	std::thread accept_dv_th;

	const int Rateinterval = 20;

	//研究の評価用関数・変数
	int asyncrecvCountor;
	struct timeval starttime,endtime;
	std::mutex recvmtx;
	double TotalRecvSec;
	void incrementRecvCounter();
	void decrementRecvCounter();
};

/**
 * @breif 終了状態
 */
class A_State00: public StateBase {
public:
	A_State00(std::shared_ptr<AppServer> server);
	virtual ~A_State00();
	std::string execute();
};
/*
class A_State01: public StateBase {
public:
	A_State01(std::shared_ptr<AppServer> server);
	virtual ~A_State01();
};

class A_State02: public StateBase {
public:
	A_State02(std::shared_ptr<AppServer> server);
	virtual ~A_State02();
};

class A_State03: public StateBase {
public:
	A_State03(std::shared_ptr<AppServer> server);
	virtual ~A_State03();
};

class A_State04: public StateBase {
public:
	A_State04(std::shared_ptr<AppServer> server);
	virtual ~A_State04();
};

class A_State05: public StateBase {
public:
	A_State05(std::shared_ptr<AppServer> server);
	virtual ~A_State05();
private:

};
*/

/**
 * @breif 終了状態
 */
class A_State06: public StateBase {
public:
	A_State06(std::shared_ptr<AppServer> server);
	virtual ~A_State06();
	std::string execute();
private:
	std::shared_ptr<AppServer> Server;
};

/**
 * @breif 待機状態
 */
class A_State07: public StateBase {
public:
	A_State07(std::shared_ptr<AppServer> server);
	virtual ~A_State07();
	std::string execute();
private:
	std::shared_ptr<AppServer> Server;
};

/**
 * @breif レート計算状態
 */
class A_State08: public StateBase {
public:
	A_State08(std::shared_ptr<AppServer> server);
	virtual ~A_State08();
	std::string execute();
private:
	std::shared_ptr<AppServer> Server;
};

/**
 * @breif IoTデバイスからデータが来た時の処理を行うクラス
 */
class A_RecvFunctions {
public:
	/**
	 * @breif コンストラクタ
	 * @param server APサーバインスタンス
	 */
	A_RecvFunctions(std::shared_ptr<AppServer> server);
	virtual ~A_RecvFunctions();
	/**
	 * @breif 受信サイズの計算を行う
	 * @param mess 受信メッセージ
	 * @param dataLength 受信メッセージ長
	 */
	void mesureLength(std::shared_ptr<PriorityMessage> mess,int dataLength);
	/**
	 * @breif CONNECTメッセージが到着した際の処理を行う
	 * @param mess 受信メッセージ
	 * @param sesid ソケットのセッションID
	 * @param rdata 前回受信時の余りデータ
	 */
	std::string execute_connect(std::shared_ptr<PriorityMessage> mess,int sesid,std::string rdata);
	/**
	 * @breif DATAメッセージが到着した際の処理を行う
	 * @param mess 受信メッセージ
	 * @param onrecv コールバック関数
	 */
	void execute_data(std::shared_ptr<PriorityMessage> mess,std::function<void(std::string, int)> onrecv);
	/**
	 * @breif DECDATAメッセージが到着した際の処理を行う
	 * @param mess 受信メッセージ
	 */
	void execute_decdata(std::shared_ptr<PriorityMessage> mess);
private:
	std::shared_ptr<AppServer> Server;
};


} /* namespace IoTPriority */
#endif /* APPSERVER_H_ */
