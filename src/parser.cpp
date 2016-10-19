#include <iostream>
#include "../include/e2ml.h"

using namespace e2ml;

void Data::parse() {
	lexNextToken();
	bool read = ifStreams.back().good();

	//while (read) {
		switch (currentToken.code) {
			case tok_eof    : return;
			case tok_include: parseInclude(); break;
			case tok_id     : parseObject (); break;
			default:
				read = false;
		}
	//}

//	std::cout << tabSize << std::endl;
//	std::cout << indent << std::endl;
//	std::cout << int(currentToken.code) << std::endl;
//	std::cout << currentToken.identifier << std::endl;
//	std::cout << currentToken.string << std::endl;
//	std::cout << currentToken.number << std::endl;
	//}

	ifStreams.back().close();
	ifStreams.pop_back();
}

void Data::parseObject() {
	std::string name = currentToken.string;
	std::string type = "";

	lexNextToken();

	if (currentToken.code == '(') {
		lexNextToken();
		type = currentToken.string;

		lexNextToken();

		if (currentToken.code != ')')
			throw ParseError("excepted ')'", line, pos);

		lexNextToken();
	}

	std::cout << name << "(" << type << ")" << std::endl;
}

void Data::parseInclude() {

}
