/*
 * MPQueue.h
 *
 *  Created on: 2016/04/08
 *      Author: tachibana
 */

#ifndef UTILS_MULTIMESSAGEQUEUE_H_
#define UTILS_MULTIMESSAGEQUEUE_H_

#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <queue>
#include <chrono>

namespace chrono = std::chrono;

namespace IoTPriority {
/**
 * マルチスレッド対応キュー
 */
template<typename T>
class Multi_MessageQueue {
public:
	/**
	 * @breif コンストラクタ
	 */
	Multi_MessageQueue();
	virtual ~Multi_MessageQueue();
	/**
	 * @breif データの挿入
	 * @param p 挿入データ
	 */
	void push(std::shared_ptr<T> p);
	/**
	 * @breif 先頭データの取り出し
	 * @return 先頭データ
	 * @details 先頭データをキューから削除したくない場合はread()を使用する
	 * @sa read()
	 * @sa read_withtimeout()
	 */
	std::shared_ptr<T> pop();
	/**
	 * @breif 先頭データの読み取り(永続的ブロッキング)
	 * @return 先頭データ
	 * @details 先頭データをキューから削除したい場合はpop()を使用する。データが入っていない場合はデータが来るまでブロッキングするため、データがなければ終了する場合、read_withtimeout()
	 * @sa pop()
	 * @sa read_withtimeout()
	 */
	std::shared_ptr<T> read();
	/**
	 * @breif 先頭データの読み取り(タイムアウト付きブロッキング)
	 * @return 先頭データ、存在しなければnull
	 * @details 先頭データをキューから削除したい場合はpop()を使用する。データが来るまでブロッキングしたい場合、read()を使用する
	 * @sa read()
	 * @sa pop()
	 */
	std::shared_ptr<T> read_withtimeout();
	/**
	 * @breif キューを次に進める
	 */
	void next();
	/**
	 * @breif キューのサイズを取得する
	 * @return キューサイズ
	 */
	int size();
	/**
	* @breif キューが空か確認する
	* @return 空ならばtrue
	*/
	bool empty();
private:
	std::queue<std::shared_ptr<T>> BaseQueue;
	std::mutex mtx;
	std::condition_variable cv_noempty, cv_nofull;
	int max;
};

template<typename T>
Multi_MessageQueue<T>::Multi_MessageQueue() :
		max(65536) {
	// TODO 自動生成されたコンストラクター・スタブ

}

template<typename T>
Multi_MessageQueue<T>::~Multi_MessageQueue() {
	// TODO Auto-generated destructor stub
}

template<typename T>
bool Multi_MessageQueue<T>::empty() {
	std::unique_lock<std::mutex> lk(mtx);
	return BaseQueue.empty();
}

template<typename T>
void Multi_MessageQueue<T>::push(std::shared_ptr<T> p) {
	std::unique_lock<std::mutex> lk(mtx);
	cv_nofull.wait(lk, [&] {
		return (BaseQueue.size()< max);
	});
	bool sig = BaseQueue.empty();
	BaseQueue.push(p);
	if (sig)
		cv_noempty.notify_one();
}

template<typename T>
std::shared_ptr<T> Multi_MessageQueue<T>::pop() {
	std::unique_lock<std::mutex> lk(mtx);
	cv_noempty.wait(lk, [&] {
		return !BaseQueue.empty();
	});
	bool sig = (BaseQueue.size() == max);
	auto d = BaseQueue.front();
	BaseQueue.pop();
	if (sig)
		cv_nofull.notify_one();
	return d;
}

template<typename T>
void Multi_MessageQueue<T>::next() {
	std::unique_lock<std::mutex> lk(mtx);
	cv_noempty.wait(lk, [&] {
		return !BaseQueue.empty();
	});
	bool sig = (BaseQueue.size() == max);
	BaseQueue.pop();
	if (sig)
		cv_nofull.notify_one();
}

template<typename T>
int Multi_MessageQueue<T>::size() {
	std::unique_lock<std::mutex> lk(mtx);
	return BaseQueue.size();
}

template<typename T>
std::shared_ptr<T> Multi_MessageQueue<T>::read() {
	std::unique_lock<std::mutex> lk(mtx);
	cv_noempty.wait(lk, [&] {
		return !BaseQueue.empty();
	});
	auto d = BaseQueue.front();
	return d;
}

template<typename T>
std::shared_ptr<T> Multi_MessageQueue<T>::read_withtimeout() {
	std::unique_lock<std::mutex> lk(mtx);
	bool ok = cv_noempty.wait_for(lk, chrono::milliseconds(50), [&] {
		return !BaseQueue.empty();
	});
	if (ok) {
		auto d = BaseQueue.front();
		return d;
	}
	return NULL;
}

} /* namespace IoTPriority */

#endif /* UTILS_MULTIMESSAGEQUEUE_H_ */
