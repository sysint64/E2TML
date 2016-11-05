#include <iostream>
#include "../include/e2ml.h"
#include "../include/exceptions.h"

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
			throw ParseError("expected ')'", line, pos);

		lexNextToken();
	}

	parseParameters(name);
}

void Data::parseInclude() {

}

void Data::parseParameters(const std::string &name) {
	std::string paramName = currentToken.string;
	lexNextToken();
	lockIndent();

	if (currentToken.code != ':') {
		lexPrevToken();
		parseObject();
		return;
	}

	while (true) {
		parseValue();
		lexNextToken();

		if (currentToken.code != ',')
			break;
	}

	unlockIndent();
}

std::unique_ptr<Value> Data::parseValue() {
	std::cout << getIndent() << ", " << tabSize << std::endl;
	lexNextToken();

	switch (currentToken.code) {
		case '[':
			return parseArray();

		case tok_number:
			return std::make_unique<NumberValue>(currentToken.number);

		case tok_string:
			return std::make_unique<StringValue>(currentToken.string, currentToken.utfString);

		case tok_boolean:
			return std::make_unique<BooleanValue>(currentToken.boolean);

		default:
			throw ParseError("value error", line, pos);
	}
}

std::unique_ptr<Value> Data::parseArray() {
	std::vector<std::unique_ptr<Value>> values;

	while (currentToken.code != ']' || currentToken.code != tok_eof) {
		auto value = parseValue();
		values.push_back(std::move(value));
		lexNextToken();

		if (currentToken.code != ',' && currentToken.code != ']')
			throw ParseError("expected ',' or ']'", line, pos);

		if (currentToken.code == ']')
			break;
	}

	return std::make_unique<ArrayValue>(std::move(values));
}
