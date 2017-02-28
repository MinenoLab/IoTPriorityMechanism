/*
 * IoTDeviceBuffer.h
 *
 *  Created on: 2016/03/06
 *      Author: tachibana
 */

#include <iostream>

#ifndef IOTDEVICEBUFFER_H_
#define IOTDEVICEBUFFER_H_

namespace IoTPriority {
/**
 * @breif IoTデバイスのバッファ構造を記述したクラス
 */
class IoTDeviceBuffer {
public:
	/**
	 * @breif コンストラクタ
	 */
	IoTDeviceBuffer();
	virtual ~IoTDeviceBuffer();
	/**
	 * @breif データの挿入
	 * @param datasize データサイズ(Byte単位)
	 * @param data 送信予定データ
	 */
	void setData(float datasize,std::string data);
	/**
	 * @breif 送信予定データを取得
	 * @return 送信予定データ
	 */
	std::string getData();
	/**
	 * @breif データサイズを取得
	 * @return データサイズ(Byte単位)
	*/
	float getSize();
	/*
	 * @breif 優先度のセット
	 * @param 与えられた優先度
	 */
	void setPriority(int priority);
	/*
	* @breif 優先度の取得
	* @param 優先度
	*/
	int getPriority();

private:
	std::string data;
	float datasize;
	int priority;
};

} /* namespace IoTPriority */
#endif /* IOTDEVICEBUFFER_H_ */
