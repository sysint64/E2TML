#include <iostream>
#include "../include/e2ml.h"

void e2ml::Data::parse() {
//	while (ifStreams.back().good()) {
//
//	}
	//if (lexNextToken()->code == tok_id) {
	lexNextToken();
	lexNextToken();
	lexNextToken();
	lexNextToken();
	lexNextToken();
	lexNextToken();
	lexNextToken();
	lexNextToken();
	lexNextToken();
	lexNextToken();
	lexNextToken();

	lexNextToken();
	lexNextToken();
	lexNextToken();
	// lexNextToken();

	// lexNextToken();
	// lexNextToken();
	// lexNextToken(); // -4 true


	std::cout << tabSize << std::endl;
	std::cout << indent << std::endl;
	std::cout << int(currentToken.code) << std::endl;
	std::cout << currentToken.identifier << std::endl;
	std::cout << currentToken.string << std::endl;
	std::cout << currentToken.number << std::endl;
	//}

	ifStreams.back().close();
	ifStreams.pop_back();
}
