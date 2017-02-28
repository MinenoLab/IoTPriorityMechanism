/*
 * APAdminTable.cpp
 *
 *  Created on: 2016/04/06
 *      Author: tachibana
 */

#include "APAdminTable.h"

namespace IoTPriority {

APAdminTable::APAdminTable():ID_Counter(0) {
	// TODO 自動生成されたコンストラクター・スタブ

}

APAdminTable::~APAdminTable() {
	// TODO Auto-generated destructor stub
}
int APAdminTable::table_insert(AATableEntitiy ent){
	insert(std::make_pair(ID_Counter,ent));
	return ID_Counter++;
}

int APAdminTable::getNextID(){
	return ID_Counter;
}

} /* namespace IoTPriority */
