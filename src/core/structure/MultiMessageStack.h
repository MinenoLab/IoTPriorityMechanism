/*
 * MultiMessageStack.h
 *
 *  Created on: 2016/06/27
 *      Author: tachibana
 */

#ifndef STRUCTURE_MULTIMESSAGESTACK_H_
#define STRUCTURE_MULTIMESSAGESTACK_H_

#include <stack>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <chrono>

namespace chrono = std::chrono;

namespace IoTPriority {
/**
 * マルチスレッド対応スタック
 */
template<typename T>
class Multi_MessageStack {
public:
	/**
	 * @breif コンストラクタ
	*/
	Multi_MessageStack();
	virtual ~Multi_MessageStack();
	/**
	* @breif データの挿入
	* @param p 挿入データ
	*/
	void push(std::shared_ptr<T> p);
	/**
	 * @breif 先頭データの取り出し
	 * @return 先頭データ
	 * @details 先頭データをスタックから削除したくない場合はread()を使用する
	 * @sa read()
	 * @sa read_withtimeout()
	 */
	std::shared_ptr<T> pop();
	/**
	* @breif 先頭データの読み取り(永続的ブロッキング)
	* @return 先頭データ
	* @details 先頭データをスタックから削除したい場合はpop()を使用する。データが入っていない場合はデータが来るまでブロッキングするため、データがなければ終了する場合、read_withtimeout()
	* @sa pop()
	* @sa read_withtimeout()
	*/
	std::shared_ptr<T> read();
	/**
	* @breif 先頭データの読み取り(タイムアウト付きブロッキング)
	* @return 先頭データ、存在しなければnull
	* @details 先頭データをスタックから削除したい場合はpop()を使用する。データが来るまでブロッキングしたい場合、read()を使用する
	* @sa read()
	* @sa pop()
	*/
	std::shared_ptr<T> read_withtimeout();
	/**
	* @breif スタックを次に進める
	*/
	void next();
	/**
	* @breif スタックのサイズを取得する
	* @return スタックサイズ
	*/
	int size();
	/**
	* @breif スタックが空か確認する
	* @return 空ならばtrue
	*/
	bool empty();
private:
	std::stack<std::shared_ptr<T>> BaseStack;
	std::mutex mtx;
	std::condition_variable cv_noempty, cv_nofull;
	int max;

};

template<typename T>
Multi_MessageStack<T>::Multi_MessageStack():max(65536) {
	// TODO 自動生成されたコンストラクター・スタブ

}

template<typename T>
Multi_MessageStack<T>::~Multi_MessageStack() {
	// TODO Auto-generated destructor stub
}

template<typename T>
bool Multi_MessageStack<T>::empty() {
	std::unique_lock<std::mutex> lk(mtx);
	return BaseStack.empty();
}

template<typename T>
void Multi_MessageStack<T>::push(std::shared_ptr<T> p) {
	std::unique_lock<std::mutex> lk(mtx);
	cv_nofull.wait(lk, [&] {
		return (BaseStack.size()< max);
	});
	bool sig = BaseStack.empty();
	BaseStack.push(p);
	if (sig)
		cv_noempty.notify_one();
}

template<typename T>
std::shared_ptr<T> Multi_MessageStack<T>::pop() {
	std::unique_lock<std::mutex> lk(mtx);
	cv_noempty.wait(lk, [&] {
		return !BaseStack.empty();
	});
	bool sig = (BaseStack.size() == max);
	auto d = BaseStack.top();
	BaseStack.pop();
	if (sig)
		cv_nofull.notify_one();
	return d;
}

template<typename T>
void Multi_MessageStack<T>::next() {
	std::unique_lock<std::mutex> lk(mtx);
	cv_noempty.wait(lk, [&] {
		return !BaseStack.empty();
	});
	bool sig = (BaseStack.size() == max);
	BaseStack.pop();
	if (sig)
		cv_nofull.notify_one();
}

template<typename T>
int Multi_MessageStack<T>::size() {
	std::unique_lock<std::mutex> lk(mtx);
	return BaseStack.size();
}

template<typename T>
std::shared_ptr<T> Multi_MessageStack<T>::read() {
	std::unique_lock<std::mutex> lk(mtx);
	cv_noempty.wait(lk, [&] {
		return !BaseStack.empty();
	});
	auto d = BaseStack.top();
	return d;
}

template<typename T>
std::shared_ptr<T> Multi_MessageStack<T>::read_withtimeout() {
	std::unique_lock<std::mutex> lk(mtx);
	bool ok = cv_noempty.wait_for(lk, chrono::milliseconds(50), [&] {
		return !BaseStack.empty();
	});
	if (ok) {
		auto d = BaseStack.top();
		return d;
	}
	return NULL;
}

} /* namespace IoTPriority */

#endif /* STRUCTURE_MULTIMESSAGESTACK_H_ */
