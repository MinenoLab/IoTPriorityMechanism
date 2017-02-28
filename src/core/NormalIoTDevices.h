/*
 * IoTDevices.h
 *
 *  Created on: 2016/02/29
 *      Author: tachibana
 */

#ifndef NOMALIOTDEVICES_H_
#define NOMALIOTDEVICES_H_

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
#include "utils/SyncClientSocket.h"
#include "StateBase.h"
#include "StateSwicher.h"
#include "structure/PTEntitiy.h"
#include "structure/AATableEntitiy.h"
#include "utils/BinaryConverter.h"

namespace asio = boost::asio;

namespace IoTPriority {

class NormalIoTDevices: public iEntity,
		public std::enable_shared_from_this<NormalIoTDevices> {
public:
	NormalIoTDevices();
	virtual ~NormalIoTDevices();
	int api_setDeviceID(int deviceID);
	int api_setDataType(std::string dataType);
	int api_setBroker(std::string hostname);
	int api_resist(int keepalive,std::string jsondata);
	int api_fin();
	int api_sendData(std::string dataType, std::string data);
	void api_setRecvCallback(std::function<void(std::string, int)> onrecv); //基本送信制御なので使わないはず
	void run_Entitiy();
	void initState();
	void connectAPServer(std::string hostname, int port);

	std::shared_ptr<std::map<int, std::shared_ptr<IoTDeviceBuffer>>>getBuffer();
	void putRemoveQueue(int priority,struct timeval tm,int dataid);
	int getRemovedata();

	bool emptyDeviceQueue();
	std::shared_ptr<std::pair<std::string,std::string>> readDeviceQueue();
	std::shared_ptr<std::pair<std::string,std::string>> putDeviceQueue();
	int getDeviceID();

	//メインスレッドから呼び出し、キューからデータを取り出して処理(次の状態も呼び出す)
	std::shared_ptr<PriorityMessage> br_recv_and_getdata();
	std::shared_ptr<PriorityMessage> ap_recv_and_getdata();
	void ap_recvqueue_repush(std::shared_ptr<PriorityMessage> pdata);
	int recv_and_gettype();
	int recv_and_gettype_single();
	int br_send(std::shared_ptr<std::string> data);
	int ap_send(std::shared_ptr<std::string> data,int rate);
	bool br_queue_empty();

	//ネットワークスレッドから呼び出し、キューへ突っ込む
	void onRecvFromBroker(std::string data, int sesid);
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
	int DeviceID;
	long Buffersize;//Bufferサイズは32MB仮定
	std::map<int,std::shared_ptr<IoTDeviceBuffer>> Buffer;

	//Test Function
	void putMessage(bool isserver,std::shared_ptr<PriorityMessage> p);
	int popPriority();

	//出力用
	long OtherMessCount;
	long MessCount=0;

	//優先度テーブルと照合
	int getPriorityValueFromTable(std::shared_ptr<MetaData> mdata,struct timeval aliveTime);

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

	std::vector<PTEntity> pTable;

	//TODO 現状バッファはメモリ上に実装、二次記憶への移行も検討
	//std::map<int,std::shared_ptr<IoTDeviceBuffer>> Buffer;
	BrokerPriorityQueue<int,int,std::less<int>>::PriorityQueue bufferRemoveQueue;

	//メタデータ送信前の待避所<データ種類、データ>
	Multi_MessageQueue<std::pair<std::string,std::string>> DeviceDataqueue;
	Multi_MessageQueue<int> DevicePriorityValuequeue;
	void onRecv_base(std::string data, Multi_MessageQueue<PriorityMessage>* recvqueue,std::string* remaindata);

	int dataID_f;
};
/*
 class D_State00: public StateBase {
 public:
 D_State00(std::shared_ptr<IoTDevices> device);
 virtual ~D_State00();
 std::string execute();
 private:
 std::shared_ptr<IoTDevices> Device;
 };

 class D_State05: public IoTPriority::StateBase {
 public:
 D_State05(std::shared_ptr<IoTDevices> device);
 virtual ~D_State05();
 std::string execute();
 private:
 std::shared_ptr<IoTDevices> device;
 };

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
 static const int BROKER_DEVICEID = 0xff;
 };

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

 class D_State08: public IoTPriority::StateBase {
 public:
 D_State08(std::shared_ptr<IoTDevices> device);
 virtual ~D_State08();
 std::string execute();
 private:
 std::shared_ptr<IoTDevices> device;

 };

 class D_State09: public StateBase {
 public:
 D_State09(std::shared_ptr<IoTDevices> device);
 virtual ~D_State09();
 std::string execute();
 private:
 std::shared_ptr<IoTDevices> device;
 };*/

class D_State50: public StateBase {
public:
	D_State50(std::shared_ptr<NormalIoTDevices> device);
	virtual ~D_State50();
	std::string execute();
private:
	std::shared_ptr<NormalIoTDevices> Device;
};

class D_State51: public StateBase {
public:
	D_State51(std::shared_ptr<NormalIoTDevices> device);
	virtual ~D_State51();
	std::string execute();
private:
	std::shared_ptr<NormalIoTDevices> Device;
	int DataID;
	int getDataID();
	static const int BROKER_DEVICEID = 0xff;
};

}
/* namespace IoTPriority */
#endif /* NOMALIOTDEVICES_H_ */
