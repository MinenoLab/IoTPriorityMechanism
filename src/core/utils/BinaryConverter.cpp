/*
 * BinaryConverter.cpp
 *
 *  Created on: 2016/05/25
 *      Author: tachibana
 */

#include "BinaryConverter.h"

namespace IoTPriority {

BinaryConverter::BinaryConverter() {
	// TODO 自動生成されたコンストラクター・スタブ

}

BinaryConverter::~BinaryConverter() {
	// TODO Auto-generated destructor stub
}

std::string BinaryConverter::floattoStr(float f) {
	union fconvert frate;
	std::string data = "";
	frate.f = f;
	for (int i = 0; i < 4; i++) {
		unsigned char c;
		c = (unsigned char) (frate.u % 256);
		data += c;
		frate.u = frate.u >> 8;
	}
	return data;
}

float BinaryConverter::strtoFloat(std::string fstr) {
	union fconvert frate;
	const char* fch = fstr.c_str();
	frate.u = 0;
	for (int i = 3; i >= 0; i--) {
		unsigned char c = (unsigned char) fch[i];
		frate.u = (frate.u << 8) + (uint32_t) c;
	}
	return frate.f;
}

std::string BinaryConverter::int16toStr(int i) {
	uint32_t ui = (uint32_t) i;
	unsigned char ci;
	std::string istr;
	ci = (unsigned char) (ui >> 8);
	istr += ci;
	ci = (unsigned char) (ui % 256);
	istr += ci;
	return istr;
}

int BinaryConverter::strtoInt16(std::string istr) {
	const char* ich = istr.c_str();
	unsigned char c = (unsigned char) ich[0];
	uint32_t reti = (uint32_t) c;
	c = (unsigned char) ich[1];
	reti = (reti << 8) + (uint32_t) c;
	return reti;
}

std::string BinaryConverter::int8toStr(int i) {
	uint32_t ui = (uint32_t) i;
	unsigned char ci;
	ci = (unsigned char) (ui % 256);
	std::string istr;
	istr += ci;
	return istr;
}

int BinaryConverter::strtoInt8(std::string istr) {
	const char* ich = istr.c_str();
	unsigned char c = (unsigned char) ich[0];
	uint32_t reti = (uint32_t) c;
	return reti;
}

} /* namespace IoTPriority */

//Test functions-------------------------------------
/*
int main() {

	for (int i = 0; i < 65540; i++) {
		double rad=i*M_PI/180;
		std::cout << "input : " << i<<" , "<<(float)rad<< std::endl;
		auto s = IoTPriority::BinaryConverter::int16toStr(i);
		s += IoTPriority::BinaryConverter::floattoStr((float)rad);
		int reti = IoTPriority::BinaryConverter::strtoInt16(s);
		float retj = IoTPriority::BinaryConverter::strtoFloat(s.substr(2));
		std::cout << "output : " << reti<<" , "<<retj << std::endl;
	}

	return 0;
}
*/

