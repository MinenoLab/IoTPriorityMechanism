/*
 * PriorityTable.h
 *
 *  Created on: 2016/04/05
 *      Author: tachibana
 */

#ifndef UTILS_PRIORITYTABLE_H_
#define UTILS_PRIORITYTABLE_H_

#include <time.h>
#include <sys/time.h>
#include <iostream>
#include <memory>

namespace IoTPriority {
/**
 * @breif 優先度決定テーブルの要素クラス
 */
class PTEntity {
public:
	/**
	* @breif コンストラクタ、同時にset()と同様の機能を行う
	* @sa set()
	 */
	PTEntity(int deviceId,std::string dataType,int dataIntervalSeconds,int priority);
	virtual ~PTEntity();
	/**
	 * @breif テーブルに含める情報のセット
	 * @param deviceID デバイスID
	 * @param dataType データタイプ
	 * @param dataIntervalSeconds データ間隔
	 * @param priority 優先度
	 * @return デバイスIDまたは優先度が範囲外の値を設定した場合-1,それ以外は1
	 */
	int set(int deviceId,std::string dataType,int dataIntervalSeconds,int priority);
	/**
	* @breif 不問項目の設定、trueの項目が条件判定時に不問となる
	* @param any_DeviceID デバイスIDの不問
	* @param any_DataType データタイプの不問
	* @param any_DataIntervalSeconds データ間隔の不問
	*/
	void set_anything(bool any_DeviceID,bool any_Datatype,bool any_DataInterval);
	/**
	 * @breif デバイスIDのセット
	 * @param id デバイスID
	 * @return 範囲外ならば-1,正常なら1
	 */
	int set_id(int id);
	/**
	 * @breif データタイプのセット
	 * @param type データタイプ
	 */
	void set_type(std::string type);
	/**
	 * @breif データ間隔のセット
	 * @param val データ間隔
	 */
	void set_intervalSeconds(int val);
	/**
	 * @breif 優先度のセット
	 * @param pri 優先度
	 * @return 範囲外ならば-1,正常なら1
	 */
	int set_priority(int pri);
	int get_intervalSeconds() const;
	std::string get_type() const;
	int get_id() const;
	int get_priority() const;
	struct timeval get_usedtime() const;
	int matchCondition(int deviceId,std::string dataType,struct timeval datatime);
	void notifyUsed();
	std::string getstr();

private:
	int DeviceID;
	std::string DataType;
	int DataIntervalSeconds;
	int Priority;
	struct timeval usedtime;
	bool initused;
	//条件不問用フラグ
	std::tuple<bool,bool,bool> isAnything;
};

} /* namespace IoTPriority */

#endif /* UTILS_PRIORITYTABLE_H_ */
