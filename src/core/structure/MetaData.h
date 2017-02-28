/*
 * metaData.h
 *
 *  Created on: 2016/04/11
 *      Author: tachibana
 */

#ifndef STRUCTURE_METADATA_H_
#define STRUCTURE_METADATA_H_

#include <iostream>
#include "../utils/BinaryConverter.h"

namespace IoTPriority {

class MetaData {
public:
	/**
	 * @brief コンストラクタ
	 */
	MetaData();
	virtual ~MetaData();
	int DataID, DeviceID;
	std::string DataType;
	float DataSize;
	/**
	* @breif メタデータの要素セット
	* @param dataID データID
	* @param deviceID デバイスID
	* @param dataType データタイプ
	* @param dataSize データのサイズ(Byte単位)
	* @return 常に1
	*/
	int set(int dataID, int deviceID, std::string dataType, float dataSize);
	/*
	 * @breif メタデータのデータIDを取得する
	 * @return データID
	 */
	int getDataID();
	/*
	 * @breif メタデータのデバイスIDを取得する
	 * @return デバイスID
	 */
	int getDeviceID();
	/*
	 * @breif メタデータのデータタイプを取得する
	 * @return データタイプ
	 */
	std::string getDataType();
	/*
	 * @breif メタデータのデータサイズを取得する
	 * @return データサイズ(Byte単位)
	 */
	float getDataSize();
	/**
	* @breif ネットワークから流れてきたバイナリ形式のメタデータを登録する
	* @param data バイナリ形式のメタデータ
	* @return 常に1
	* @sa getMetadata()
	*/
	int set_fromMetaData(std::string data);
	/**
	* @breif ネットワークに流すバイナリ形式のメタデータに変換して取得する
	* @return バイナリ形式のメタデータ
	* @sa set_fromMetaData()
	*/
	std::string getMetadata();
};

} /* namespace IoTPriority */

#endif /* STRUCTURE_METADATA_H_ */
