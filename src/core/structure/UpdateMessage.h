/*
 * InterruptMessage.h
 *
 *  Created on: 2016/11/09
 *      Author: tachibana
 */

#ifndef STRUCTURE_UPDATEMESSAGE_H_
#define STRUCTURE_UPDATEMESSAGE_H_

#include <iostream>
#include <memory>
#include "../utils/BinaryConverter.h"

namespace IoTPriority {

class UpdateMessage {
public:
	UpdateMessage();
	virtual ~UpdateMessage();

	static const int STOP=0x0;
	static const int RESTART=0x1;
	static const int ABS_VALUE=0x2;
	static const int REL_VALUE=0x3;

	bool set(int id);
	bool set(int id,float value);
	int getId();
	float getValue();
	std::shared_ptr<std::string> getMessage();
	void putMessage(std::string data);

private:
	int Id;
	float Value;

	bool isNeedvalue(int id);
};

} /* namespace IoTPriority */

#endif /* STRUCTURE_UPDATEMESSAGE_H_ */
