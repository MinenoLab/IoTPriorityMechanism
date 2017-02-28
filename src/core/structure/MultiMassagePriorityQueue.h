/*
 * MultiMassagePriorityQueue.h
 *
 *  Created on: 2016/04/13
 *      Author: tachibana
 */

#ifndef STRUCTURE_MULTIMASSAGEPRIORITYQUEUE_H_
#define STRUCTURE_MULTIMASSAGEPRIORITYQUEUE_H_

#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>
#include "BrokerPriorityQueue.h"
#include "MetaData.h"

namespace IoTPriority {
/*
 * 複数スレッドからのアクセスに対応したBrokerPriorityQueue
 */
class MultiMassagePriorityQueue {
public:
	/**
	 * @breif　コンストラクタ
	 */
	MultiMassagePriorityQueue();
	virtual ~MultiMassagePriorityQueue();
	/**
	 * @breif　データのプッシュ
	 * @param pri 優先度
	 * @param tm 到着時刻
	 * @param d メタデータ
	 */
	void push(int pri,struct timeval tm,std::shared_ptr<MetaData> d);
	/**
	 * @breif キューの先頭データを取り出す
	 * @return キューの先頭データ<優先度<到着時刻、メタデータ>>
	 * @details キューから取り出したデータを削除したくない場合はread()を使う
	 * @sa read()
	 */
	std::pair<int,std::pair<struct timeval,std::shared_ptr<MetaData>>>  pop();
	/**
	* @breif キューの先頭データを読み取る
	* @return キューの先頭データ<優先度<到着時刻、メタデータ>>
	* @details キューから取り出したデータを削除したい場合はpop()を使う
	* @sa pop()
	*/
	std::pair<int,std::pair<struct timeval,std::shared_ptr<MetaData>>>  read();
	/**
	 * @breif キューが空か確認する
	 * @return 空ならtrue
	 */
	bool empty();
private:
	BrokerPriorityQueue<int,std::shared_ptr<MetaData>,std::greater<int>>::PriorityQueue BaseQueue;
	std::mutex mtx;
	std::condition_variable cv_noempty,cv_nofull;
	int max;

};

} /* namespace IoTPriority */

#endif /* STRUCTURE_MULTIMASSAGEPRIORITYQUEUE_H_ */
