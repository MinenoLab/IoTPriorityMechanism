/*
 * AATableEntitiy.cpp
 *
 *  Created on: 2016/04/06
 *      Author: tachibana
 */
#include "AATableEntitiy.h"
#include <fstream>

namespace IoTPriority {

AATableEntitiy::AATableEntitiy(std::string hostname, int port,
		std::string json_require) :
		hostname(hostname), port(port) {
	// TODO 自動生成されたコンストラクター・スタブ
	set_require(json_require);
}

AATableEntitiy::~AATableEntitiy() {
	// TODO Auto-generated destructor stub
}

void AATableEntitiy::set_require(std::string req) {
	json_require = req;
	const char* c_jreq = req.c_str();
	picojson::value v;
	std::string err;
	picojson::parse(v, c_jreq, c_jreq + strlen(c_jreq), &err);
	if (err.empty()) {
		picojson::array array = v.get<picojson::array>();
		for (picojson::array::iterator it = array.begin(); it != array.end();
				it++) {
			std::map<std::string, std::string> m;
			picojson::object& o = it->get<picojson::object>();
			//intervalだけは計算式(掛け算)の対処が必要
			int interval= calcInterval(o["DataInterval"].get<std::string>());
			PTEntity pe(std::stoi(o["DeviceID"].get<std::string>(), nullptr, 0),
					o["DataType"].get<std::string>(), interval,
					std::stoi(o["Priority"].get<std::string>()));
			jsonvalue.push_back(pe);
		}
	} else {
		std::cout << err << std::endl;
	}

}
std::string& AATableEntitiy::getrequire_str() {
	return json_require;
}
std::vector<PTEntity>& AATableEntitiy::getrequire() {
	return jsonvalue;
}

int AATableEntitiy::calcInterval(std::string strinterval) {
	int interval = 1;
	int prevpos = 0;
	while (strinterval.find("*",prevpos) != std::string::npos) {
		int pos = strinterval.find("*",prevpos);
		interval *= std::stoi(strinterval.substr(prevpos, pos - prevpos));
		prevpos = pos+1;
	}
	interval *= std::stoi(strinterval.substr(prevpos));
	return interval;
}
}
/* namespace IoTPriority */
/*
int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "usage jsonfilename" << std::endl;
		exit(1);
	}
	std::string filename = argv[1];
	std::ifstream ifs(filename);
	if (ifs.fail()) {
		std::cerr << "file not found" << std::endl;
		return -1;
	}
	std::string jsondata((std::istreambuf_iterator<char>(ifs)),
			std::istreambuf_iterator<char>());
	std::cerr << "json :" << jsondata << std::endl;
	IoTPriority::AATableEntitiy at("127.0.0.1", 35255, jsondata);
	auto data = at.getrequire();
	for (auto d : data) {
		std::cout << "DeviceID: " << d.get_id() << std::endl;
		std::cout << "DataType: " << d.get_type() << std::endl;
		std::cout << "DataInterval: " << d.get_intervalSeconds() << std::endl;
		std::cout << "priority: " << d.get_priority() << std::endl;
	}
	return 0;
}
*/
