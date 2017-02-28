/*
 * PriReturnValue.h
 *
 *  Created on: 2016/04/20
 *      Author: tachibana
 */

#ifndef STRUCTURE_PRIRETURNVALUE_H_
#define STRUCTURE_PRIRETURNVALUE_H_

#include <iostream>
#include <memory>
#include "../utils/BinaryConverter.h"

namespace IoTPriority {
/**
 * @breif PRIメッセージのペイロードフォーマット
 */
class PriReturnValue {
public:

	static const int CLASS_S=0x0;
	static const int CLASS_M=0x1;
	static const int CLASS_L=0x2;
	/**
	* @brief コンストラクタ
	*/
	PriReturnValue();
	virtual ~PriReturnValue();
	/**
	* @breif メッセージに含める情報のセット
	* @param dataID データID
	* @param sclass サイズクラス(このクラスで定義されたもののみ利用可能)
	* @param priority 優先度
	* @return sclassが定義外だった場合-1、それ以外は1
	*/
	int set(int dataID,int sclass,int priority);
	/**
	* @breif ネットワークに流すバイナリ形式のメッセージに変換して取得する
	* @return バイナリ形式のメッセージ
	* @sa putMessage()
	*/
	std::shared_ptr<std::string> getMessage();
	/**
	* @breif サイズクラス計算
	* @param datasize データサイズ(Byte単位)
	* @return サイズクラス
	*/
	static int calcSclass(float datasize);
	/**
	 * @breif メッセージのデータIDの取得
	 * @return データID
	 */
	int getdataID();
	/**
	 * @breif メッセージのサイズクラスの取得
	 * @return サイズクラス
	 */
	int getSclass();
	/**
	 * @breif メッセージの優先度の取得
	 * @return 優先度
	 */
	int getPriority();
	/**
	* @breif ネットワークから流れてきたバイナリ形式のメッセージを登録する
	* @param data バイナリ形式のメッセージ
	* @sa getMessage()
	*/
	void putMessage(std::string);

private:
	int DataID;
	int Sclass;
	int Priority;
	static const int th_M=1*1024;
	static const int th_L=220*1024;
};

} /* namespace IoTPriority */

#endif /* STRUCTURE_PRIRETURNVALUE_H_ */
