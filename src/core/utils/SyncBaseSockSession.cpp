/*
 * SyncBaseSockSession.cpp
 *
 *  Created on: 2016/04/24
 *      Author: tachibana
 */

#include "SyncBaseSockSession.h"
#include <istream>

namespace IoTPriority {

Sync_BaseSockSession::Sync_BaseSockSession(tcp::socket sock,
		std::function<void(std::string, int)> recvfunc) :
		sock(std::move(sock)), On_recv_func(recvfunc), used_basicRecv(false), used_basicSend(
		false), flag_forever(false), sesid(0), flag_sendChange(false), flag_rateChange(
		false), flag_rateChangeAbs(false), changefunc(
		NULL), isServer(false), sstate(SockState::ACTIVE) {
	// TODO 自動生成されたコンストラクター・スタブ
	std::cerr << "init_end_1" << std::endl;

}
Sync_BaseSockSession::Sync_BaseSockSession(tcp::socket sock, int sesid,
		std::function<void(std::string, int)> recvfunc) :
		sock(std::move(sock)), On_recv_func(recvfunc), used_basicRecv(false), used_basicSend(
		false), flag_forever(false), sesid(sesid), flag_sendChange(false), flag_rateChange(
		false), flag_rateChangeAbs(false), changefunc(
		NULL), isServer(true), sstate(SockState::ACTIVE) {
	// TODO 自動生成されたコンストラクター・スタブ
	std::cerr << "init_end_2" << std::endl;
}

Sync_BaseSockSession::~Sync_BaseSockSession() {
	// TODO Auto-generated destructor stub
	if (recvth.joinable())
		recvth.join();
	if (sendth.joinable())
		sendth.join();
}

void Sync_BaseSockSession::recconect_init(tcp::socket s) {
	if (isServer) {
		std::cerr << "only use client" << std::endl;
		return;
	}
	if (sstate == SockState::RECONNECTING) {
		std::cerr << "session initializing..." << std::endl;
		sock = std::move(s);
		sstate = SockState::ACTIVE;
		std::cerr << "new socket accepted!" << std::endl;
	}
}

bool Sync_BaseSockSession::basic_send(std::shared_ptr<std::string> data) {
	bool ret;
	if (sstate != SockState::ACTIVE)
		return false;
	{
		std::unique_lock<std::mutex> lk(gmtx);
		used_basicSend = true;
	}
	std::unique_lock<std::mutex> lk(sendmtx);
	boost::system::error_code error;
	checkSameSeq(data);

	//スタックに退避しているデータがあれば先頭にシーケンスをつける
	//↑割り込み時の送信前のpresendメッセージの時だけtrueになる
	if (!sendstack.empty())
		sendSeq();

	asio::write(sock, asio::buffer(*data), error);
	if (error) {
		std::cerr << "send failed : " << error.message() << std::endl;
		if (isServer) {
			sstate = SockState::CLOSE;
		} else {
			sstate = SockState::NONACTIVE;
		}
		ret = false;
	} else {
		std::cerr << "sended!  size: " << data->length() << std::endl;
		ret = true;
	}
	{
		std::unique_lock<std::mutex> lk(gmtx);
		used_basicSend = false;
	}

	return ret;
}

std::string Sync_BaseSockSession::basic_recv() {
	if (sstate != SockState::ACTIVE)
		return "";
	{
		std::unique_lock<std::mutex> lk(gmtx);
		used_basicRecv = true;
	}
	std::string data;
	unsigned char buf[4096];
	{
		std::unique_lock<std::mutex> lk(recvmtx);
		boost::system::error_code error;

		size_t bytes = asio::read(sock, asio::buffer(buf, 4096),
				asio::transfer_at_least(4), error);
		if (error && error != asio::error::eof) {
			std::cout << "receive failed: " << error.message() << std::endl;
			if (isServer) {
				sstate = SockState::CLOSE;
			} else {
				sstate = SockState::NONACTIVE;
			}
			return "";
		}
		/*data = asio::buffer_cast<const char*>(receive_buffer.data());
		 std::cout << receive_buffer.size() << std::endl;
		 data = data.substr(0, receive_buffer.size());
		 receive_buffer.consume(receive_buffer.size());*/
		/*bool isfirst = true;
		 while (receive_buffer.size() > 0) {
		 std::string databuf, data;
		 std::istream is(&receive_buffer);
		 is >> databuf;
		 if (isfirst) {
		 data = databuf;
		 isfirst = false;
		 } else {
		 data = (char) 0x0a + databuf;
		 }
		 }*/

		for (int i = 0; i < bytes; i++)
			data += buf[i];
		std::cerr << "receive correct :" << std::endl;
		checkreadSeq(std::make_shared<std::string>(data));
	}
	{
		std::unique_lock<std::mutex> lk(gmtx);
		used_basicRecv = false;
	}
	return data;
}

void Sync_BaseSockSession::limit_send(std::shared_ptr<std::string> data,
		float rate) {
	if (flag_forever) {
		auto pair = std::make_shared<
				std::pair<std::shared_ptr<std::string>, float>>(
				std::make_pair(data, rate));
		sendqueue.push(pair);
	}
}

void Sync_BaseSockSession::send_forever() {
	if (sstate != SockState::ACTIVE)
		return;

	bool state;
	bool flag_stack;
	float curRate;
	while (flag_forever || !sendqueue.empty()) {
		auto data = sendqueue.read();
		{
			std::unique_lock<std::mutex> lk(gmtx);
			state = used_basicSend;
		}
		//データ送信完了とBrokerサーバからの退避命令が同時だった場合無効化
		{
			std::unique_lock<std::mutex> lk(changemtx);
			flag_sendChange = false;
		}
		std::cerr << "send start (forever)" << std::endl;
		//basicSendが呼び出されていない時のみ実行
		if (!state) {
			std::unique_lock<std::mutex> lk(sendmtx);
			boost::system::error_code error;
			std::string senddata = *(data->first);
			int depbytes = 4096;

			//帯域制御の初期化
			{
				std::cerr << "rate : " << data->second << std::endl;
				curRate = data->second;
				float deptimes =
						((curRate) / 8) / 4096; //4096バイトを送信する間隔を決める
#ifdef CHECK
								std::cerr << "depbytes" <<depbytes << std::endl;
								std::cerr << "data len" << senddata.length() << std::endl;
#endif
				std::cerr << "deptimes : " << deptimes << std::endl;
			}
			while (senddata.length() > 0) {
				if (sstate == SockState::ACTIVE) {
					std::string depdata = senddata.substr(0, depbytes);
					checkSameSeq(std::make_shared<std::string>(depdata));
					asio::write(sock, asio::buffer(depdata), error);
					if (error) {
						std::cout << "send failed : " << error.message()
								<< std::endl;
						if (isServer) {
							sstate = SockState::CLOSE;
						} else {
							sstate = SockState::NONACTIVE;
						}

					}
				}
				if (sstate != SockState::ACTIVE) {
					//一旦送信中断
					//現状のデータを新たにpushし、今のデータを消す
					sendqueue.push(
							std::make_shared<
									std::pair<std::shared_ptr<std::string>,
											float>>(
									std::make_pair(senddata, curRate)));
					sendqueue.next();
					return;
				}

				if (senddata.length() <= depbytes)
					break;
				senddata = senddata.substr(depbytes);
				//上位優先度の割り込みがあるか確認、あったら退避
				{
					std::unique_lock<std::mutex> lk(changemtx);
					flag_stack = flag_sendChange;
					if (flag_sendChange) {
						flag_sendChange = false;
						flag_rateChange = false;
					}
				}
				if (flag_stack) {
					auto stackdata = std::make_shared<std::string>(
							std::string(senddata));
					sendstack.push(
							std::make_shared<
									std::pair<std::shared_ptr<std::string>,
											float>>(
									std::make_pair(stackdata, curRate)));
					std::cerr << "changing data" << std::endl;
					break;
				}
				//brokerサーバからのレート調整要求
				{
					std::unique_lock<std::mutex> lk(changemtx);
					if (flag_rateChange) {
						flag_rateChange = false;
						curRate *= changeRateValue;
						std::cerr << "changerate : " << data->second
								<< "changevalue : " << changeRateValue
								<< std::endl;
					}
					if (flag_rateChangeAbs) {
						flag_rateChangeAbs = false;
						curRate = changeAbsRateValue;
						std::cerr << "changerate : " << data->second
								<< "changevalue(kbps) : " << changeAbsRateValue
								<< std::endl;
					}
				}
				//帯域制御をするために待ち時間を挿入
				//usleep(1000000 / deptimes); //マイクロ秒
			}

			std::cerr << "sended!!(forever)" << std::endl;
			//割り込みがなければキューはempty
			//emptyの場合変化率をリセット
			if (sendqueue.empty()) {
				std::cerr << "rate empty reset" << std::endl;
			}
			sendqueue.next();
		}
	}
}

void Sync_BaseSockSession::recv_forever() {
	if (sstate != SockState::ACTIVE)
		return;

	bool state;
	unsigned char buf[4096];
	while (flag_forever) {
		{
			std::unique_lock<std::mutex> lk(gmtx);
			state = used_basicRecv;
		}
		std::cerr << "receive ever..." << std::endl;
		//basicRecvが呼び出されていない時のみ実行
		if (!state) {
			if (sstate != SockState::ACTIVE)
					return;
			std::unique_lock<std::mutex> lk(recvmtx);
			boost::system::error_code error;
			size_t bytes = asio::read(sock, asio::buffer(buf, 4096),
					asio::transfer_at_least(4), error);
			if (error && error != asio::error::eof) {
				std::cout << "receive failed: " << error.message() << std::endl;
				if (isServer) {
					sstate = SockState::CLOSE;
				} else {
					sstate = SockState::NONACTIVE;
				}
				return;
			}
			if (bytes <= 0)
				return;
			//std::string data = asio::buffer_cast<const char*>(
			//	receive_buffer.data());
			//data = data.substr(0, receive_buffer.size());
			//receive_buffer.consume(receive_buffer.size());

			//2回目以降は先頭に0x0a(改行)を付加(読み飛ばしされるため)
			/*bool isfirst = true;
			 while (receive_buffer.size() > 0) {
			 std::string databuf, data;
			 std::istream is(&receive_buffer);
			 is >> databuf;
			 if (isfirst) {
			 data = databuf;
			 isfirst = false;
			 } else {
			 data = (char) 0x0a + databuf;
			 }

			 std::cerr << "receive correct!!!:  " << data.length()
			 << std::endl;
			 On_recv_func(data, sesid);
			 std::cerr << "ok-r " << std::endl;
			 }*/
			std::string data = "";
			for (int i = 0; i < bytes; i++)
				data += buf[i];
			std::cerr << "receive correct!!!:  " << data.length() << std::endl;
			//フラグシーケンスを確認し、あったら分割＆上位に知らせる
			int change = checkreadSeq(std::make_shared<std::string>(data));
			if (change != std::string::npos) {
				std::string cdata = std::string(data);
				while (1) {
					std::cerr << "receive seqence!!:  " << std::endl;
					On_recv_func(cdata.substr(0, change), sesid);
					if (changefunc != NULL)
						changefunc(sesid);
					try {
						std::cerr << "change CData" << std::endl;
						cdata = cdata.substr(change + 6);
					} catch (const std::out_of_range & e) {
						break;
					}
					change = checkreadSeq(std::make_shared<std::string>(cdata));
					if (change == std::string::npos) {
						std::cerr << "Next recv data" << std::endl;
						On_recv_func(cdata, sesid);
						break;
					}
				}
			} else {
				On_recv_func(data, sesid);
			}
			std::cerr << "ok-r " << std::endl;

		}
	}
}

void Sync_BaseSockSession::start_sendrecv_forever(bool send,
bool recv) {
	std::unique_lock<std::mutex> lk(gmtx);
	flag_forever = true;
	if (send) {
		sendth = std::thread(&Sync_BaseSockSession::send_forever,
				shared_from_this());
		std::cerr << "START send forever" << std::endl;
	}
	if (recv) {
		std::cerr << "STARTing... recv forever" << std::endl;
		recvth = std::thread(&Sync_BaseSockSession::recv_forever,
				shared_from_this());
		std::cerr << "START recv forever" << std::endl;
	}
}

void Sync_BaseSockSession::stop_sendrecv_forever() {
	if (!flag_forever)
		return;
	std::unique_lock<std::mutex> lk(gmtx);
	flag_forever = false;
	if (sendth.joinable())
		sendth.join();
	if (recvth.joinable())
		recvth.join();
}

void Sync_BaseSockSession::changeSendData() {
	std::unique_lock<std::mutex> lk(changemtx);
	flag_sendChange = true;
	std::cerr << "Send Data Changed!! " << std::endl;
}

void Sync_BaseSockSession::changeSendRate(float rate) {
	std::unique_lock<std::mutex> lk(changemtx);
	flag_rateChange = true;
	changeRateValue = rate;
	std::cerr << "Send Rate Changed!! " << std::endl;
}

void Sync_BaseSockSession::changeSendRateAbs(float rate) {
	std::unique_lock<std::mutex> lk(changemtx);
	flag_rateChangeAbs = true;
	changeAbsRateValue = rate;
	std::cerr << "Send Rate Changed(Abs)!! " << std::endl;
}

void Sync_BaseSockSession::setNotifyChangeListner(
		std::function<void(int)> func) {
	changefunc = func;
}

//TODO rateは準備工事
void Sync_BaseSockSession::resumSend() {
	std::unique_lock<std::mutex> lk(changemtx);
	std::cerr << "resum Send start!! " << std::endl;
	flag_sendChange = false;
	if (sendstack.empty())
		return;
	auto data = sendstack.pop();
	if (data == NULL)
		return;
	sendqueue.push(data);
}

void Sync_BaseSockSession::sendSeq() {
	boost::system::error_code error;
	std::string data;
	for (int i = 0; i < 6; i++) {
		data += '1';
	}
	asio::write(sock, asio::buffer(data), error);
	if (error) {
		std::cerr << "send failed : " << error.message() << std::endl;
	} else {
		std::cerr << "send ChangeSeq!! " << std::endl;
	}
}

void Sync_BaseSockSession::checkSameSeq(std::shared_ptr<std::string> data) {
	int pos = -1;
	while (1) {
		pos = data->find("11111", pos + 1);
		if (pos == std::string::npos)
			break;
		data->insert(pos + 5, "0");
	}
}

int Sync_BaseSockSession::checkreadSeq(std::shared_ptr<std::string> data) {
	int pos = -1;
	while (1) {
		pos = data->find("111110", pos + 1);
		if (pos == std::string::npos)
			break;
		data->erase(pos + 5, 1);
	}
	return data->find("111111");
}

Sync_BaseSockSession::SockState Sync_BaseSockSession::getSockState() {
	return sstate;
}

void Sync_BaseSockSession::notifyRecconect() {
	if (sstate != SockState::CLOSE)
		sstate = SockState::RECONNECTING;
}

} /* namespace IoTPriority */
