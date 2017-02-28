/*
 * StateBase.h
 *
 *  Created on: 2016/02/27
 *      Author: tachibana
 */

#ifndef STATEBASE_H_
#define STATEBASE_H_


#include <iostream>
#include <memory>

namespace IoTPriority {

class StateBase {
public:
	StateBase();
	virtual ~StateBase();
	/**
	* @breif 状態で実行すべき処理の開始
	* @return 次に遷移すべき状態名、空文字の場合はデフォルトの遷移
	*/
	virtual std::string execute()=0;
};

} /* namespace IoTPriority */
#endif /* STATEBASE_H_ */
