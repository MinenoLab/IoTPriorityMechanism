/*
 * StateSwicher.h
 *
 *  Created on: 2016/02/28
 *      Author: tachibana
 */

#ifndef STATESWICHER_H_
#define STATESWICHER_H_

#include <memory>
#include <iostream>
#include "StateBase.h"

namespace IoTPriority {

class StateSwicher {
public:
	StateSwicher(std::shared_ptr<StateBase> state,std::string nextStateName);
	virtual ~StateSwicher();
	std::string getNextStateName();
	std::shared_ptr<StateBase>  state;

private:
	std::string nextStateName;
};

} /* namespace IoTPriority */
#endif /* STATESWICHER_H_ */
