/*
 * BinaryConverter.h
 *
 *  Created on: 2016/05/25
 *      Author: tachibana
 */

#ifndef UTILS_BINARYCONVERTER_H_
#define UTILS_BINARYCONVERTER_H_

#include <iostream>
#include <memory>

namespace IoTPriority {

class BinaryConverter {
public:
	BinaryConverter();
	virtual ~BinaryConverter();
	static std::string floattoStr(float f);
	static float strtoFloat(std::string fstr);
	static std::string int16toStr(int i);
	static int strtoInt16(std::string istr);
	static std::string int8toStr(int i);
	static int strtoInt8(std::string istr);

private:
	union fconvert {
		float f;
		uint32_t u;
	};
};

} /* namespace IoTPriority */

#endif /* UTILS_BINARYCONVERTER_H_ */
