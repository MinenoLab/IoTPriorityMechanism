/*
 * PriorityMessage.cpp
 *
 *  Created on: 2016/03/03
 *      Author: tachibana
 */

#include "PriorityMessage.h"

namespace IoTPriority {

PriorityMessage::PriorityMessage() :
		SenderDeviceID(-1), ReceverDeviceID(-1), Header(""), isOK(false) {
	// TODO 自動生成されたコンストラクター・スタブ

}

PriorityMessage::~PriorityMessage() {
	// TODO Auto-generated destructor stub
}

int PriorityMessage::makeMessage(int senderDeviceID, int ReciverDeviceID,
		int messType, int ackType, std::shared_ptr<std::string> data) {
	cleanall();
	this->SenderDeviceID = senderDeviceID;
	this->ReceverDeviceID = ReciverDeviceID;
	this->MessType = messType;
	this->AckType = ackType;

	unsigned char mT = (unsigned char) messType;
	unsigned char aT = (unsigned char) ackType;
	unsigned char type = (mT << 2) | aT;
	uint32_t d = data->size();
	uint32_t s = 0, c = 0;
	std::cerr << "payload length : " << d << std::endl;
	do {
		c++;
		uint32_t digit = d % 128;
		//std::cout << "digit : " << digit << std::endl;
		d /= 128;
		// if there are more digits to encode, set the top bit of this digit
		if (d > 0)
			digit = digit | 0x80;
		s = (s << 8) + digit;
	} while (d > 0);
	unsigned char datasize[c];
	for (int i = c - 1; i >= 0; i--) {
		datasize[i] = (unsigned char) (s % 256);
		s /= 256;
	}
	Header += BinaryConverter::int8toStr(this->SenderDeviceID);
	Header += BinaryConverter::int8toStr(this->ReceverDeviceID);
	Header += type;
	for (auto c : datasize)
		Header += c;
	this->data = data;
	return 1;
}

std::shared_ptr<std::string> PriorityMessage::getMessageHeader() {
	return std::make_shared<std::string>(Header);
}

std::shared_ptr<std::string> PriorityMessage::getContentData() {
	return data;
}

std::shared_ptr<std::string> PriorityMessage::getAllData() {
	return std::make_shared<std::string>(Header + *data);
}

int PriorityMessage::getSenderDeviceID() {
	return SenderDeviceID;
}
int PriorityMessage::getReceverDeviceID() {
	return ReceverDeviceID;
}
int PriorityMessage::getMessType() {
	return MessType;
}
int PriorityMessage::getAckType() {
	return AckType;
}

void PriorityMessage::cleanall() {
	SenderDeviceID = -1;
	ReceverDeviceID = -1;
	Header = "";
	data.reset();
}

//Headerには情報は入らない
std::string PriorityMessage::putMessage(std::string data) {
	if (data == "") {
		std::cerr << "ok " << std::endl;
		isOK = false;
		return "";
	}
	try {
		std::string strhead = data.substr(0, 3);
		SenderDeviceID = BinaryConverter::strtoInt8(strhead);
		ReceverDeviceID = BinaryConverter::strtoInt8(strhead.substr(1));
		const char *head = strhead.substr(2, 1).c_str();
		MessType = (int) (((unsigned char) head[0] & 0xfc) >> 2);
		AckType = (int) ((unsigned char) head[0] & 0x03);
	} catch (const std::out_of_range & e) {
		isOK = false;
		return "";
	}
	int mult = 1, length = 0, c = 0;
	uint32_t digit;
	do {
		try {
			const char *v = data.substr(3 + c, 1).c_str();
			unsigned char uv = (unsigned char) v[0];
			digit = (uint32_t) uv;
			length += (digit & 0x7f) * mult;
			mult *= 128;
			c++;
		} catch (const std::out_of_range & e) {
			isOK = false;
			return "";
		}
	} while ((digit & 0x80) != 0);
	//もし他のメッセージがくっついてたらその部分を返す
	std::cerr << "payload length : " << length << std::endl;
	if (length == 0) {
		try {
			isOK = true;
			return std::string(data.substr(4));
		} catch (const std::out_of_range & e) {
			isOK = false;
			return "";
		}
	}
	try {
		this->data = std::make_shared<std::string>(data.substr(3 + c, length));
		isOK = true;
		return std::string(data.substr(3 + c + length));
	} catch (const std::out_of_range & e) {
		isOK = false;
		return "";
	}
}

bool PriorityMessage::isAllDataRecieved() {
	return isOK;
}

}/* namespace IoTPriority */

//Test codes-------------------------------------------------------------------------
/*
 int main() {
 IoTPriority::PriorityMessage ps, pr, ps2, pr2, pr3, pr4;
 auto dummydata = std::make_shared<std::string>("dummydummydummy");
 ps.makeMessage(15, 20, IoTPriority::PriorityMessage::MESSTYPE_META,
 IoTPriority::PriorityMessage::NOACK, dummydata);
 std::cout << "header: " << *(ps.getMessageHeader()) << " , "
 << ps.getMessageHeader()->length() << std::endl;
 auto mess = ps.getAllData();
 std::cout << mess->length() << std::endl;
 pr.putMessage(*mess);
 std::cout << "sender: " << pr.getSenderDeviceID() << std::endl;
 std::cout << "reciver: " << pr.getReceverDeviceID() << std::endl;
 std::cout << "messtype: " << pr.getMessType() << std::endl;
 std::cout << "data: " << *(pr.getContentData()) << std::endl;

 auto dummydata2 = std::make_shared<std::string>("dummy---dummy");
 ps2.makeMessage(7, 20, IoTPriority::PriorityMessage::MESSTYPE_PRI,
 IoTPriority::PriorityMessage::NOACK, dummydata2);
 auto mess2 = std::make_shared<std::string>(
 *(ps.getAllData()) + *(ps2.getAllData()));
 auto mess3 = pr2.putMessage(*mess2);
 std::cout << "sender2: " << pr2.getSenderDeviceID() << std::endl;
 std::cout << "reciver2: " << pr2.getReceverDeviceID() << std::endl;
 std::cout << "messtype2: " << pr2.getMessType() << std::endl;
 std::cout << "data2: " << *(pr2.getContentData()) << std::endl;
 pr3.putMessage(mess3);
 std::cout << "sender3: " << pr3.getSenderDeviceID() << std::endl;
 std::cout << "reciver3: " << pr3.getReceverDeviceID() << std::endl;
 std::cout << "messtype3: " << pr3.getMessType() << std::endl;
 std::cout << "data3: " << *(pr3.getContentData()) << std::endl;
 auto mf = mess->substr(0, 5);
 auto mf2 = mess->substr(5);
 if (pr4.putMessage(mf) == "")
 std::cout << "clear" << std::endl;
 mf += mf2;
 pr4.cleanall();
 pr4.putMessage(mf);
 std::cout << "sender4: " << pr4.getSenderDeviceID() << std::endl;
 std::cout << "reciver4: " << pr4.getReceverDeviceID() << std::endl;
 std::cout << "messtype4: " << pr4.getMessType() << std::endl;
 std::cout << "data4: " << *(pr4.getContentData()) << std::endl;

 }
*/

