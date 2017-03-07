/*
 * BrokerServer.h
 *
 *  Created on: 2016/03/30
 *      Author: tachibana
 */

#ifndef BROKERSERVER_H_
#define BROKERSERVER_H_

#include "iEntity.h"

#include <memory>
#include <iostream>
#include <tuple>
#include "boost/system/error_code.hpp"
#include <boost/asio.hpp>
#include <thread>
#include <mutex>
#include <time.h>
#include <sys/time.h>
#include <tuple>
#include <unordered_map>
#include "structure/APAdminTable.h"
#include "structure/AATableEntitiy.h"
#include "structure/IoTDeviceBuffer.h"
#include "structure/PTEntitiy.h"
#include "structure/DeviceAdminTable.h"
#include "structure/MultiMassagePriorityQueue.h"
#include "structure/MultiMessageStack.h"
#include "structure/MultiMessageQueue.h"
#include "structure/PermitFormat.h"
#include "structure/PriorityMessage.h"
#include "structure/PriReturnValue.h"
#include "structure/MetaData.h"
#include "utils/SyncServerSockets.h"
#include "StateBase.h"
#include "structure/UpdateMessage.h"

namespace IoTPriority {

class BrokerServer: public iEntity, public std::enable_shared_from_this<
		BrokerServer> {
public:
	/**
	* @breif コンストラクタ
	* @param apport APサーバListenポート
	* @param dvport IoTデバイスListenポート
	*/
	BrokerServer(int apport, int dvport);
	virtual ~BrokerServer();
	//ネットワークスレッドから呼び出し、キューへ突っ込む
	/**
	* @breif デバイスからのデータを受け取る
	* @param data デバイスからのデータ
	* @param sesid ソケットのセッションID(ファイルディスクリプタ)
	* @details データは長さ不完全、ネットワークスレッドから呼び出す
	*/
	void onRecvFromDevices(std::string data, int sesid);
	/**
	* @breif APサーバからのデータを受け取る
	* @param data APサーバからのデータ
	* @param sesid ソケットのセッションID(ファイルディスクリプタ)
	* @details データは長さ不完全、ネットワークスレッドから呼び出す
	*/
	void onRecvfromAPServer(std::string data, int sesid);
	//メインスレッドから呼び出し、キューからデータを
	/**
	* @breif APサーバからの受信データを取り出す
	* @return PriorityMessage形式のデータ
	* @details 受信データを処理する場合にはこの関数でデータを取得する
	*/
	std::shared_ptr<PriorityMessage> ap_recv_and_getdata();
	/**
	* @breif デバイスからの受信データを取り出す
	* @return PriorityMessage形式のデータ
	* @details 受信データを処理する場合にはこの関数でデータを取得する
	*/
	std::shared_ptr<PriorityMessage> dv_recv_and_getdata();
	/**
	* @breif 現在の送信レートを参照
	* @return 現在の送信レート
	*/
	float getCurCommRate();
	/**
	* @breif 現在の送信レートを登録
	* @param rate 現在の送信レート
	*/
	void putCurCommRate(float rate);
	/**
	* @breif APサーバまたはデバイスからの受信データのタイプを取得する
	* @return PriorityMessage形式のデータタイプ
	* @details  APサーバからのデータが優先される
	*/
	int recv_and_gettype();
	/**
	* @breif デバイスからの受信データのタイプを取得する
	* @return PriorityMessage形式のデータタイプ
	* @details 必ずデバイスからのデータをチェックしたい場合はこちらを使う
	*/
	int recv_and_gettype_fromDevice();
	/**
	* @breif デバイスへのデータ送信
	* @param data 送信データ
	* @param RecieverDeviceID 送信先deviceID
	* @return 成功なら1,不正なdeviceIDなら-1,その他のエラーは-2
	*/
	int ap_send(int RecieverDeviceID, std::shared_ptr<std::string> data);
	/**
	* @breif デバイスへのデータ送信
	* @param data 送信データ
	* @param RecieverDeviceID 送信先deviceID
	* @return 成功なら1,不正なdeviceIDなら-1,その他のエラーは-2
	*/
	int dv_send(int RecieverDeviceID, std::shared_ptr<std::string> data);
	/**
	* @breif APサーバセッションのクローズ
	* @param sesid ソケットのセッションID(ファイルディスクリプタ)
	*/
	void ap_close(int sesid);
	/**
	* @breif デバイスセッションのクローズ
	* @param sesid ソケットのセッションID(ファイルディスクリプタ)
	*/
	void dv_close(int sesid);
	/**
	* @breif 送信中データの情報登録
	* @param deviceID データ発信元のデバイスID
	* @param dataID 該当データのデータID
	* @param Priority　該当データの優先度
	* @param sizeClass 該当データのサイズクラス , PriReturnValueクラスを参照
	*/
	void registSending(int deviceID, int dataID, int Priority, int sizeClass);
	/**
	* @breif 送信中データの登録解除
	* @param deviceID データ発信元のデバイスID
	* @param dataID 該当データのデータID
	* @return 成功すれば優先度、該当データがない場合は-1
	*/
	int releaseSending(int deviceID, int DataID);
	/**
	* @breif 送信中データのサイズクラス取得
	* @param deviceID データ発信元のデバイスID
	* @param dataID 該当データのデータID
	* @return 成功すればサイズクラス、該当データがない場合は-1
	*/
	int getSendingSClass(int deviceID, int DataID);
	//DeviceID,DataID,priority,SClass
	/**
	* @breif すべての送信中データの情報取得
	* @return <DeviceID,DataID,priority,SClass>を要素としたVector
	* @sa registSending()
	*/
	std::vector<std::tuple<int,int,int,int>>getallsendingData();
	/**
	* @breif 別スレッドでの状態遷移動作開始
	* @param name 初期StateのState名
	*/
	void run_state_secondthread(std::string name);
	/**
	* @breif 優先度キューへのメタデータ登録
	* @param pri 優先度
	* @param tm メタデータ到着時刻
	* @param d メタデータ
	*/
	void pushMainPriorityQueue(int pri, struct timeval tm,
			std::shared_ptr<MetaData> d);
	/**
	* @breif 優先度キューから先頭データを取り出す
	* @return <優先度, <到着時刻, メタデータ>>形式の優先度が最も高いメタデータ
	* @details 優先度キューから先頭データを削除したくない場合はreadMainPriorityQueue()を使う
	* @sa readMainPriorityQueue()
	*/
	std::pair<int, std::pair<struct timeval, std::shared_ptr<MetaData>>>popMainPriorityQueue();
	/**
	* @breif 優先度キューから先頭データの情報を確認する
	* @return <優先度, <到着時刻, メタデータ>>形式の優先度が最も高いメタデータ
	* @details 優先度キューから先頭データを削除したい場合はpopMainPriorityQueue()を使う
	* @sa popMainPriorityQueue()
	*/
	std::pair<int, std::pair<struct timeval, std::shared_ptr<MetaData>>>readMainPriorityQueue();
	/**
	* @breif 優先度キューが空か確認する
	* @return 空ならtrue
	*/
	bool emptyMainPriorityQueue();
	/**
	* @breif 状態遷移の動作開始
	*/
	void run_Entitiy();
	/**
	* @breif 優先度決定要求登録
	* @details フォーマットはsampleディレクトリのjsonファイルを参照
	*/
	void putpriority(std::string json);
	//優先度テーブルと照合
	/**
	* @breif メタデータの優先度を決定する
	* @param mdata メタデータ
	* @param aliveTime 到着時刻
	* @return 優先度
	*/
	int getPriorityValueFromTable(std::shared_ptr<MetaData> mdata,struct timeval aliveTime);
	std::mutex mtx;
	bool endthread=false;
	const int DeviceID = 0xff;
	//Test Function
	void putMessage(bool isserver,std::shared_ptr<PriorityMessage> p);

	std::thread queuethread,accept_ap_th,accept_dv_th;

	//<did,sesid>
	boost::bimaps::bimap<int, int> ap_session_id_map;
	boost::bimaps::bimap<int, int> dv_session_id_map;

	//---仮変数関数
	float prevrate;
	int prevpriority;
	bool isemptysending();
	std::tuple<float,float,float> sendrate;
	int prevSClass;

	void flushPriorityqueue();

	const std::string name_state0 = "State0";
	const std::string name_state1 = "State1";
	const std::string name_state2 = "State2";
	const std::string name_state3 = "State3";
	const std::string name_state4 = "State4";
	const std::string name_state5 = "State5";
	const std::string name_state6 = "State6";
	const std::string name_state7 = "State7";
	const std::string name_state8 = "State8";
	const std::string name_state9 = "State9";
	const std::string name_state10 = "State10";
	const std::string name_state11 = "State11";

private:
	std::shared_ptr<Sync_ServerSockets> toAPServerSocket;
	std::shared_ptr<Sync_ServerSockets> toIotDeviceSocket;
	boost::asio::io_service io_service_AP, io_service_DV;
	std::vector<PTEntity> pTable;
	APAdminTable apTable;
	DeviceAdminTable dvTable;
	const int apport, dvport;
	bool setpriority=false;

	Multi_MessageQueue<PriorityMessage> device_recvqueue, server_recvqueue;
	std::string device_remaindata[256];
	std::string server_remaindata;
	float curCommRate_kiro;//Caution キュースレッド、メインスレッド双方から参照
	MultiMassagePriorityQueue main_priority_queue;
	//<DeviceID,<DataID,<Priority,sizeclass>>>
	std::shared_ptr<std::unordered_map <int,std::shared_ptr<std::unordered_map<int,std::pair<int,int>>>>> SendingIDmap;
	void init_onlypriority();
	void onRecv_base(std::string data, int sesid, Multi_MessageQueue<PriorityMessage>* recvqueue,
			boost::bimaps::bimap<int, int>* sesidmap, std::string* remaindata, std::shared_ptr<Sync_ServerSockets> sock);
	void onExecute_connect(std::shared_ptr<PriorityMessage> mess, boost::bimaps::bimap<int, int>* sesidmap,int sesid, std::shared_ptr<Sync_ServerSockets> sock);

	/**
	 * @breif 割り込み処理を記述するクラスbaseNotifyALLsendingDevice
	 */
	class InterruptController{
	public:
		/**
		 * @breif コンストラクタ
		 * @param server ブローカーインスタンス
		 */
		InterruptController(std::shared_ptr<BrokerServer> server);
		virtual ~InterruptController();
		/*
		 * @breif 高優先度による割り込みの開始処理
		 */
		void interruptHighPriority();
		/*
		 * @breif 高優先度による割り込みの終了判定と終了処理
		 * @return 終了ならtrue
		 */
		bool returnInterruptHighPriority();
		/*
		 * @breif 高優先度による割り込みの開始判定
		 * @return 開始ならtrue
		 */
		bool detectIntertupt();
		/*
		 * @breif COMPLETEメッセージ受信とUPDATE(true)メッセージ発信が同時に起こった時の不整合の解消
		 * @param d 受信メッセージ
		 * @param dataID データID
		 */
		void resolutePassing(std::shared_ptr<PriorityMessage> d,int dataID);
		/*
		 * @breif データ送信中の全デバイスに送信停止・再開要求を送る
		 * @param state trueなら再開、falseなら停止
		 */
		void notifyALLsendingDevices(bool state);
		/*
		 * @breif データ送信中の全デバイスに送信レートの更新を通知
		 * @param value 更新後のレート、更新前のレートを1とした相対表記
		 */
		void notifyALLsendingDevices(float value);
		/*
		 * @breif データ送信中の全デバイスに送信レートの更新を通知
		 * @param data 送信中データのリスト、<DeviceID,DataID,Priority,sizeclass>形式
		 */
		void reAllocateRate(std::vector<std::tuple<int,int,int,int>> datamap);
		bool stackAct=false;
		std::mutex stackmtx;
	private:
		std::shared_ptr<BrokerServer> Server;
		Multi_MessageStack<std::pair<int,std::shared_ptr<std::unordered_map <int,std::shared_ptr<std::unordered_map<int,std::pair<int,int>>>>>>> IDmapStack;
		Multi_MessageStack<std::tuple<float,float,float>> sendrateStack;
		void pushcurIDmap();
		void popprevIDmap();
		bool releaseprevIDmap(int deviceID, int DataID);
		int readMapPriority();
		void baseNotifyALLsendingDevices(int messType,std::shared_ptr<std::string> data);
	};

public:
	std::shared_ptr<InterruptController> iContorol;
};



/**
 * @breif 終了状態
 */
class B_State00: public StateBase {
public:
	B_State00(std::shared_ptr<BrokerServer> server);
	virtual ~B_State00();
	std::string execute();
private:
	std::shared_ptr<BrokerServer> Server;
};

/*
 class B_State01: public StateBase {
 public:
 B_State01();
 virtual ~B_State01();
 };

 class B_State02: public StateBase {
 public:
 B_State02();
 virtual ~B_State02();
 };

 class B_State03: public StateBase {
 public:
 B_State03();
 virtual ~B_State03();
 };
 */

/**
 * @breif 待機状態
 */
class B_State04: public StateBase {
public:
	B_State04(std::shared_ptr<BrokerServer> server);
	virtual ~B_State04();;
	std::string execute();
private:
	std::shared_ptr<BrokerServer> Server;
};

/**
 * @breif METAメッセージから優先度を求め、優先度キューに挿入する状態
 */
class B_State05: public StateBase {
public:
	B_State05(std::shared_ptr<BrokerServer> server);
	virtual ~B_State05();
	std::string execute();
private:
	std::shared_ptr<BrokerServer> Server;
};
/**
 * @breif METAメッセージから優先度を求め、優先度キューに挿入する状態
 */
class B_State06: public StateBase {
public:
	B_State06(std::shared_ptr<BrokerServer> server);
	virtual ~B_State06();
	std::string execute();
private:
	std::shared_ptr<BrokerServer> Server;
	void testinit();
};

/**
 * @breif APサーバからのレート状況を反映する状態
 */
class B_State07: public StateBase {
public:
	B_State07(std::shared_ptr<BrokerServer> server);
	virtual ~B_State07();
	std::string execute();
private:
	std::shared_ptr<BrokerServer> Server;
	bool firstchanged=false;
	float firstrate=-1;
	union fconvert {
		float f;
		uint32_t u;
	};
};

/**
 * @breif 優先度キューから送信データを決定し、指示を出す状態
 */
class B_State10: public StateBase {
public:
	B_State10(std::shared_ptr<BrokerServer> server);
	virtual ~B_State10();
	std::string execute();
private:
	std::shared_ptr<BrokerServer> Server;
	std::tuple<float, float, float> calcpermitRate();
	int NS, NM, NL;
	void countclass(float size);
	void countclass_base(int sclass);
	int decountclass(float size);
	float calcaddrate(float size);
	void printPopDebugStr(
			std::pair<int, std::pair<struct timeval, std::shared_ptr<MetaData>>>& popdata);
	void printCountRateStr(int priority);
	void printSendStr(int sendrate,PriorityMessage& primess, std::pair<int, std::pair<struct timeval, std::shared_ptr<MetaData>>>& senddata);
};

/**
 * @breif 送信データの終了を待機する状態
 */
class B_State11: public StateBase {
public:
	B_State11(std::shared_ptr<BrokerServer> server);
	virtual ~B_State11();
	std::string execute();
private:
	std::shared_ptr<BrokerServer> Server;
	void testinit();
};


} /* namespace IoTPriority */

#endif /* BROKERSERVER_H_ */
