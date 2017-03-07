/**
 * @file SyncBaseSockSession.h
 * @brief TCP送受信用クラス
 *  Created on: 2016/04/24
 *      Author: tachibana
 */

#ifndef UTILS_SYNCBASESOCKSESSION_H_
#define UTILS_SYNCBASESOCKSESSION_H_

#include <iostream>
#include <thread>
#include <memory>
#include <mutex>
#include <unistd.h>
#include <boost/asio.hpp>
#include "../structure/MultiMessageQueue.h"
#include "../structure/MultiMessageStack.h"
#include "../iEntity.h"

using boost::asio::ip::tcp;

namespace IoTPriority {

/**
 *@breif クライアント、サーバ側問わずのTCP送受信クラス
 */
class Sync_BaseSockSession :public std::enable_shared_from_this<Sync_BaseSockSession>{
public:
	/**
	 * @enum SockState
	 * 通信状態を定義
	 */
	enum SockState{
		ACTIVE=0,
		NONACTIVE=1,
		RECONNECTING=2,
		CLOSE=3,
	};

	/**
	 * @breif コンストラクタ、クライアント側からの呼び出し
	 * @param sock TCPのソケット
	 * @param recvfunc データ受信時のコールバック関数
	*/
	Sync_BaseSockSession(tcp::socket sock,
			std::function<void(std::string, int)> recvfunc);
	/**
	 * @breif コンストラクタ、サーバ側からの呼び出し
	 * @param sock TCPのソケット
	 * @param sesid セッションID
	 * @param recvfunc データ受信時のコールバック関数
	*/
	Sync_BaseSockSession(tcp::socket sock,int sesid,
				std::function<void(std::string, int)> recvfunc);

	virtual ~Sync_BaseSockSession();

	/**
	 * @breif 通常の送信
	 * @param data 送信データ
	 * @return 成功したらTrue
	 * @details 一般的なTCPソケットでのブロッキング送信を行う
	 */
	bool basic_send(std::shared_ptr<std::string> data);

	/**
	 * @breif 通常の受信
	 * @return 受信データ
	 * @details 一般的なTCPソケットでのブロッキング受信を行う
	*/
	std::string basic_recv();

	/**
	 * @breif 別スレッドを用いた送受信の開始設定
	 * @param send 送信に適用
	 * @param recv 受信に適用
	 * @sa send_forever()
	 * @sa recv_forever()
	 * @sa limit_Send()
	*/
	void start_sendrecv_forever(bool send,bool recv);

	/**
		 * @breif 別スレッドを用いた送受信の停止
		 * @sa start_sendrecv_forever()
	*/
	void stop_sendrecv_forever();
	/**
	　* @breif 別スレッドを用いたデータ送信
	 * @param data 送信データ
	 * @param rate 送信レート(現在不使用)
	 * @details 別スレッドにてデータを分割した送信を行う。ここで送信するデータは優先度制御の割り込み送信なども適用される
	 * @sa send_forever()
	*/
	void limit_send(std::shared_ptr<std::string> data, float rate);
	/**
	 * @breif スタック内の送信データの復帰
	 */
	void resumSend();
	/**
	 * @breif 送信中のデータをスタックへ退避
	 */
	void changeSendData();
	void changeSendRate(float rate);
	void changeSendRateAbs(float rate);
	/**
	* @breif 送信中のデータをスタックへ退避
	 */
	void setNotifyChangeListner(std::function<void(int)> func);
	/**
	* @breif ソケットの状態を取得
	* @return 現在のソケットの状態
	 */
	SockState getSockState();
	/**
	* @breif ソケットを再接続状態に変更
	 */
	void notifyRecconect();
	/**
	* @breif 再接続後のソケット移譲と初期化
	 */
	//only client use
	void recconect_init(tcp::socket sock);

private:

	int sesid;
	tcp::socket sock;
	std::function<void(std::string, int)> On_recv_func;
	std::function<void(int)> changefunc;
	Multi_MessageQueue<std::pair<std::shared_ptr<std::string>, float>> sendqueue;
	Multi_MessageStack<std::pair<std::shared_ptr<std::string>, float>> sendstack;
	bool flag_forever;
	bool isServer;
	bool used_basicSend, used_basicRecv;
	bool flag_sendChange,flag_rateChange,flag_rateChangeAbs;
	float changeRateValue,changeAbsRateValue;
	boost::asio::streambuf receive_buffer;
	std::mutex sendmtx, recvmtx, gmtx,changemtx;
	SockState sstate;
	/**
	* @breif 別スレッドを用いた送信
	* @details 別スレッドでデータを4096バイトに分割しながら送信する。changeSendData()が呼び出された場合は送信中のデータはスタックに待避する
	* @sa changeSendData()
	* @sa resumSend()
	*/
	void send_forever();
	void recv_forever();
	void sendSeq();
	void checkSameSeq(std::shared_ptr<std::string> data);
	int checkreadSeq(std::shared_ptr<std::string> data);
	std::thread sendth, recvth;

};

}/* namespace IoTPriority */

#endif /* UTILS_SYNCBASESOCKSESSION_H_ */
