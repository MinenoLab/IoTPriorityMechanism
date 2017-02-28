/*
 * iEntity.h
 *
 *  Created on: 2016/02/26
 *      Author: tachibana
 */

#ifndef IENTITY_H_
#define IENTITY_H_

#include <iostream>
#include <memory>
#include <unordered_map>
#include <time.h>
#include <sys/time.h>
#include <boost/bimap/bimap.hpp>
#include "StateSwicher.h"

namespace IoTPriority {

using namespace boost;
typedef bimaps::bimap<int,int>::value_type bival;

/**
 * @breif 各エンティティの基底クラス
 */
class iEntity{
public:
	iEntity();
	virtual ~iEntity();
	int nextState();
	static const int Maxpriority=1;
	static const int Minpriority=15;
	static const int BROKER_DEVICEID = 0xff;
	static const int APPSERVER_DEVICEID = 0xff;
	/**
	* @breif 現在のStateを返す
	* @return 現在のStateが含まれたStateSwticher
	*/
	std::shared_ptr<StateSwicher> getCurrentState();
	/**
	* @breif 指定したStateのexecute()を実行する
	* @param stateName State名
	* @return 次に遷移すべきState名、空文字の場合は指定なし
	*/
	std::string execState(std::string stateName);
	/**
	* @breif 初期Stateのexecute()を実行する
	* @return 次に遷移すべきState名、空文字の場合は指定なし
	*/
	std::string execFirstState();
	/**
	* @breif 現在のStateのexecute()を実行する
	* @return 次に遷移すべきState名、空文字の場合は指定なし
	*/
	std::string execCurState();
	/**
	* @breif 初期Stateを登録する
	* @param stateName State名
	* @return statemap内に存在しないStateならば-1,それ以外は1
	*/
	int registFirstState(std::string stateName);
	/**
	* @breif StateとState名を登録する
	* @param curStateName State名
	* @param curState 登録するState
	* @return 常に1
	*/
	int registState(std::string curStateName,std::shared_ptr<StateSwicher> curState);
	std::unordered_map<std::string,std::shared_ptr<StateSwicher> > statemap;
	//TODO 将来的にunique_ptrへ
	std::shared_ptr<StateSwicher> curState;
	std::shared_ptr<StateSwicher> firstState;
	/**
	* @breif 現在の日時をデバッグ用フォーマットの文字列で出力
	* @return 日時を示す文字列
	*/
	static std::string strnowtime();

};



} /* namespace IoTPriority */
#endif /* IENTITY_H_ */
