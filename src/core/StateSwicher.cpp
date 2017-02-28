/*
 * StateSwicher.cpp
 *
 *  Created on: 2016/02/28
 *      Author: tachibana
 */

#include "StateSwicher.h"

namespace IoTPriority {

StateSwicher::StateSwicher(std::shared_ptr<StateBase> state, std::string nextStateName):
		state(state),nextStateName(nextStateName)
	{
	}

StateSwicher::~StateSwicher() {
	// TODO Auto-generated destructor stub
}

std::string StateSwicher::getNextStateName(){
	return nextStateName;
}

} /* namespace IoTPriority */
