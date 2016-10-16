#include "../include/e2ml.h"

void e2ml::Data::load(const std::string &fileName, IOType rt) {
	std::ifstream ifs (fileName, std::ifstream::in);
	ifStreams.push_back(std::move(ifs));

	switch (rt) {
		case IOType::Text: parse(); break;
		case IOType::Bin: break;
	}
}
