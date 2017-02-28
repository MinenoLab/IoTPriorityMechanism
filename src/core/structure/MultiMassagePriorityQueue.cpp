/*
 * MultiMassagePriorityQueue.cpp
 *
 *  Created on: 2016/04/13
 *      Author: tachibana
 */

#include "MultiMassagePriorityQueue.h"

namespace IoTPriority {

MultiMassagePriorityQueue::MultiMassagePriorityQueue():max(65536) {
	// TODO 自動生成されたコンストラクター・スタブ

}

MultiMassagePriorityQueue::~MultiMassagePriorityQueue() {
	// TODO Auto-generated destructor stub
}

void MultiMassagePriorityQueue::push(int pri, struct timeval tm,
		std::shared_ptr<MetaData> d) {
	std::unique_lock<std::mutex> lk(mtx);
	cv_nofull.wait(lk, [&] {
		return (BaseQueue.size() < max);
	});
	bool sig = BaseQueue.empty();
	BaseQueue.push(std::make_pair(pri, std::make_pair(tm, d)));
	if (sig)
		cv_noempty.notify_one();
}
std::pair<int, std::pair<struct timeval, std::shared_ptr<MetaData>>>MultiMassagePriorityQueue::pop() {
	std::unique_lock<std::mutex> lk(mtx);
	cv_noempty.wait(lk,[&] {
				return !BaseQueue.empty();
			});
	bool sig= (BaseQueue.size() == max);
	auto d =BaseQueue.top();
	BaseQueue.pop();
	if (sig)
	cv_nofull.notify_one();
	return d;
}

std::pair<int, std::pair<struct timeval, std::shared_ptr<MetaData>>>MultiMassagePriorityQueue::read() {
	std::unique_lock<std::mutex> lk(mtx);
	cv_noempty.wait(lk,[&] {
				return !BaseQueue.empty();
			});
	auto d =BaseQueue.top();
	return d;
}

bool MultiMassagePriorityQueue::empty() {
	std::unique_lock<std::mutex> lk(mtx);
	return BaseQueue.empty();
}

} /* namespace IoTPriority */
