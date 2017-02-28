/*
 * DATableEntitity.h
 *
 *  Created on: 2016/04/06
 *      Author: tachibana
 */

#ifndef DEVICESTATE_DATABLEENTITITY_H_
#define DEVICESTATE_DATABLEENTITITY_H_

#include <time.h>
#include <sys/time.h>
#include <iostream>
#include <vector>
#include <sstream>

namespace IoTPriority {

class DATableEntitity {
public:
	std::vector<std::string> Datatypes;
	DATableEntitity(std::vector<std::string> types);
	virtual ~DATableEntitity();
	const std::vector<std::string> splitDatatypes(std::string str_types);
	void notifylive();
	struct timeval& getLastLiveTime();
	bool islive();

private:
	struct timeval LastLiveTime;
	bool isLive;
};

} /* namespace IoTPriority */

#endif /* DEVICESTATE_DATABLEENTITITY_H_ */
