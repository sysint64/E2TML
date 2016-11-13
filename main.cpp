#include <iostream>
#include <iomanip>
#include "include/e2ml.h"

int nesting = 0;

void traverse(e2ml::Node *node) {
	nesting++;

	for (auto &it : node->children) {
		for (int i = 0; i < nesting*4; ++i)
			std::cout << ' ';

		std::cout << it.name << "(" << nesting << ")" << std::endl;

		for (auto &param : it.parameters) {
			for (int i = 0; i < nesting*4; ++i)
				std::cout << ' ';

			std::cout << "    " << param.name << std::endl;
		}

		traverse(&it);
	}
	nesting--;
}

int main() {
	e2ml::Data data;

	data.load("/home/dev/dev/e2dit/e2tml/tests/simple.e2t");

	for (auto &it : data) {
		std::cout << it.name << std::endl;

		for (auto &param : it.parameters) {
			std::cout << "    " << param.name << std::endl;
		}

		traverse(&it);
	}
//	std::cout << data.get_as<int>         ("Object.param1.0")          << std::endl;
//	std::cout << data.get_as<std::string> ("Object.Child.param1.1")    << std::endl;
//	std::cout << data.get_as<std::string> ("Object.Child__0.param1.0") << std::endl;
}
