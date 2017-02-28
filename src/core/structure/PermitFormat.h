/*
 * PermitFormat.h
 *
 *  Created on: 2016/04/20
 *      Author: tachibana
 */

#ifndef STRUCTURE_PERMITFORMAT_H_
#define STRUCTURE_PERMITFORMAT_H_

#include <iostream>
#include <memory>
#include "../utils/BinaryConverter.h"

namespace IoTPriority {
/**
 * @breif PERMITメッセージのペイロードフォーマット
 */
class PermitFormat {
public:
	/**
	 * @breif コンストラクタ
	 */
	PermitFormat();
	virtual ~PermitFormat();
	/**
	 * @breif メッセージに含める情報のセット
	 * @param dataid 対象のデータID
	 * @param rate 許可レート
	 */
	void set(int dataid,float rate);
	/**
	 * @breif メッセージのデータIDの取得
	 * @return データID
	 */
	int getDataId();
	/**
	 * @breif メッセージのレートの取得
	 * @return 通信レート
	 */
	float getRelativeRate();
	/**
	* @breif ネットワークに流すバイナリ形式のメッセージに変換して取得する
	* @return バイナリ形式のメッセージ
	* @sa putMessage()
	*/
	std::shared_ptr<std::string> getMessage();
	/**
	* @breif ネットワークから流れてきたバイナリ形式のメッセージを登録する
	* @param data バイナリ形式のメッセージ
	* @sa getMessage()
	*/
	void putMessage(std::string data);

private:
	int Dataid;
	float RelativeRate;
	union fconvert {
		float f;
		uint32_t u;
	};
};

} /* namespace IoTPriority */

#endif /* STRUCTURE_PERMITFORMAT_H_ */
