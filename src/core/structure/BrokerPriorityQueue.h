/*
 * BrokerPriorityQueue.h
 *
 *  Created on: 2016/04/13
 *      Author: tachibana
 */

#ifndef STRUCTURE_BROKERPRIORITYQUEUE_H_
#define STRUCTURE_BROKERPRIORITYQUEUE_H_

#include <time.h>
#include <sys/time.h>
#include <iostream>
#include <memory>
#include <queue>

namespace IoTPriority {

template<typename T, typename V, typename Compare>
struct stable_greater_compare {
	bool operator()(const std::pair<T, std::pair<struct timeval, V>>& x,
			const std::pair<T, std::pair<struct timeval, V>>& y) const {
		if (comp_(x.first, y.first))
			return true;
		if (comp_(y.first, x.first))
			return false;
		return timercmp(&(x.second.first), &(y.second.first), >) != 0 ?
				true : false; // 優先順位が等しいときは時刻の早い方を選ぶ
	}
	Compare comp_;
};
/*
 * ブローカー用PriorityQueue(時間を第二優先度とし、時刻の早い順に並び替える)
 */
template<typename T, typename V, typename Compare>
struct BrokerPriorityQueue {
	typedef std::pair<T, std::pair<struct timeval, V>> value_type;
	typedef std::vector<value_type> container_type;
	typedef stable_greater_compare<T, V, Compare> compare_type;
	typedef std::priority_queue<value_type, container_type, compare_type> PriorityQueue;
};

} /* namespace IoTPriority */

#endif /* STRUCTURE_BROKERPRIORITYQUEUE_H_ */
