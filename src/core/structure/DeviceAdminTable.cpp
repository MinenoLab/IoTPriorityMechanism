/*
 * DeviceAdminTable.cpp
 *
 *  Created on: 2016/04/06
 *      Author: tachibana
 */

#include "DeviceAdminTable.h"

namespace IoTPriority {

DeviceAdminTable::DeviceAdminTable() {
	// TODO 自動生成されたコンストラクター・スタブ
	map();

}

DeviceAdminTable::~DeviceAdminTable() {
	// TODO Auto-generated destructor stub
}
int DeviceAdminTable::table_insert(int deviceID,DATableEntitity ent){
	if(deviceID > 65536 || deviceID < 0)
		return -1;
	map::insert(std::make_pair(deviceID,ent));
	return 1;
}


} /* namespace IoTPriority */
