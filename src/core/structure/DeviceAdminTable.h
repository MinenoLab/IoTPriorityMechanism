/*
 * DeviceAdminTable.h
 *
 *  Created on: 2016/04/06
 *      Author: tachibana
 */

#ifndef STRUCTURE_DEVICEADMINTABLE_H_
#define STRUCTURE_DEVICEADMINTABLE_H_

#include <time.h>
#include <sys/time.h>
#include <tuple>
#include "DATableEntitity.h"
#include <iostream>
#include <map>

namespace IoTPriority {

class DeviceAdminTable : public std::map<int,DATableEntitity>{
public:
	DeviceAdminTable();
	virtual ~DeviceAdminTable();
	int table_insert(int deviceID,DATableEntitity ent);
};

} /* namespace IoTPriority */

#endif /* STRUCTURE_DEVICEADMINTABLE_H_ */
