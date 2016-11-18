#include <iostream>
#include <iomanip>
#include "include/e2ml.h"

int nesting = 0;

void traverse(e2ml::Node *node) {
	nesting++;

	for (auto &it : node->children) {
		for (int i = 0; i < nesting*4; ++i)
			std::cout << ' ';

		std::cout << it->name << "(" << nesting << "): " << it->path << std::endl;

		for (auto &param : it->parameters) {
			for (int i = 0; i < nesting*4; ++i)
				std::cout << ' ';

			std::cout << "    " << param->name << ": " << param->path << std::endl;

			for (auto &value : param->values) {
				for (int i = 0; i < nesting*8; ++i)
					std::cout << ' ';

				std::cout << "    " << value->getName() << "(" << value->getTypeString() << ")" << ": " << value->getPath() << std::endl;
			}
		}

		traverse(it.get());
	}
	nesting--;
}

int main() {
	e2ml::Data data;

	data.load("/home/dev/dev/e2dit/e2tml/tests/simple.e2t");

	for (auto &it : data.root.children) {
		std::cout << it->name << ": " << it->path << std::endl;

		for (auto &param : it->parameters) {
			std::cout << "    " << param->name << ": " << param->path << std::endl;

			for (auto &value : param->values)
				std::cout << "        " << value->getName() << "(" << value->getTypeString() << ")" << ": " << value->getPath() << std::endl;
		}

		traverse(it.get());
	}

	std::cout << data.getString("Object.param1.0") << std::endl;
//	std::cout << data.get_as<int>         ("Object.param1.0")          << std::endl;
//	std::cout << data.get_as<std::string> ("Object.Child.param1.1")    << std::endl;
//	std::cout << data.get_as<std::string> ("Object.Child__0.param1.0") << std::endl;
}
