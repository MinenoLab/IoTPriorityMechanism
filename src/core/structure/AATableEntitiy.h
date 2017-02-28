/*
 * AATableEntitiy.h
 *
 *  Created on: 2016/04/06
 *      Author: tachibana
 */

#ifndef STRUCTURE_AATABLEENTITIY_H_
#define STRUCTURE_AATABLEENTITIY_H_

#include "../utils/picojson.h"
#include "PTEntitiy.h"
#include <tuple>
#include <iostream>
#include <memory>

namespace IoTPriority {
/*
 * @breif 管理テーブルのカラム
 */
class AATableEntitiy {
public:
	/**
	* @breif コンストラクタ
	* @param hostname ブローカーのホスト名(現在不使用)
	* @param port ブローカーのポート番号(現在不使用)
	* @param json_require JSONデータ
	*/
	AATableEntitiy(std::string hostname,int port,std::string json_require);
	virtual ~AATableEntitiy();
	std::string hostname;
	int port;
	/**
	* @breif JSONデータの登録
	* @param req JSONデータ
	*/
	void set_require(std::string req);
	/**
	* @breif JSONデータの取得
	* @return JSONデータ
	*/
	std::string& getrequire_str();
	/**
	* @breif 優先度決定テーブルの取得
	* @return 優先度決定テーブル
	*/
	std::vector<PTEntity>& getrequire();
private:
	std::string json_require;
	std::vector<PTEntity> jsonvalue;
	int calcInterval(std::string strinterval);
};

} /* namespace IoTPriority */

#endif /* STRUCTURE_AATABLEENTITIY_H_ */
