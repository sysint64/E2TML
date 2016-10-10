#include "../include/e2ml.h"

void e2ml::Data::parse() {
	while (ifStreams.back().good()) {

	}

	ifStreams.back().close();
	ifStreams.pop_back();
}
