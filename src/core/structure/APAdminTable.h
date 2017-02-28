/*
 * APAdminTable.h
 *
 *  Created on: 2016/04/06
 *      Author: tachibana
 */

#ifndef STRUCTURE_APADMINTABLE_H_
#define STRUCTURE_APADMINTABLE_H_

#include "AATableEntitiy.h"
#include <iostream>
#include <map>

namespace IoTPriority {

class APAdminTable: public std::map<int,AATableEntitiy> {
public:
	APAdminTable();
	virtual ~APAdminTable();
	int table_insert(AATableEntitiy ent);
	int getNextID();
private:
	int ID_Counter;
};

} /* namespace IoTPriority */

#endif /* STRUCTURE_APADMINTABLE_H_ */
