#include <iostream>
#include "include/e2ml.h"

int main() {
	e2ml::Data data;

	data.load("tests/simple.e2t");

	std::cout << data.get_as<int>         ("Object.param1.0")          << std::endl;
	std::cout << data.get_as<std::string> ("Object.Child.param1.1")    << std::endl;
	std::cout << data.get_as<std::string> ("Object.Child__0.param1.0") << std::endl;
}
