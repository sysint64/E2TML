#include <iostream>
#include "../include/e2ml.h"
#include "../include/exceptions.h"

using namespace e2ml;

void Data::parse() {
	lexNextToken();
	bool read = ifStreams.back().good();

	while (read) {
		switch (currentToken.code) {
			case tok_eof    : return;
			case tok_include: parseInclude(); break;
			case tok_id     : root.addChild(parseObject()); break;
			default:
				read = false;
		}
	}

	ifStreams.back().close();
	ifStreams.pop_back();
}

Node Data::parseObject() {
	std::string name = currentToken.string;
	std::string type = "";
	std::vector<Parameter> parameters;

	isInline = true;
	Node node(name, name);

	int objectIndent = getIndent();
	lexNextToken();

	if (currentToken.code == '(') {
		lexNextToken();
		type = currentToken.string;

		lexNextToken();

		if (currentToken.code != ')')
			throw ParseError("expected ')'", line, pos);

		lexNextToken();
	}

	while (currentToken.code != tok_eof) {
		std::string paramName = currentToken.string;
		lexNextToken();
		int targetIndent = isInline ? getIndent() : getIndent()-1;

		if (objectIndent != targetIndent) {
			break;
		}

		if (currentToken.code != ':') {
			lexPrevToken();

			node.addChild(parseObject());
			continue;
		}

		parameters.push_back(parseParameter(paramName));
	}

	lexPrevToken();
	node.parameters = std::move(parameters);

	return node;
	//nodes.push_back(std::move(node));
	//nesting.pop_back();
}

void Data::parseInclude() {

}

Parameter Data::parseParameter(const std::string &name) {
	lockIndent();
	std::vector<std::unique_ptr<Value>> values;

	while (true) {
		values.push_back(parseValue());
		lexNextToken();

		if (currentToken.code != ',')
			break;
	}

	unlockIndent();
	return Parameter(name, name, std::move(values));
}

std::unique_ptr<Value> Data::parseValue() {
	lexNextToken();

	switch (currentToken.code) {
		case '[':
			return parseArray();

		case tok_number:
			return std::make_unique<NumberValue>(currentToken.number);

		case tok_string:
			return std::make_unique<StringValue>(currentToken.string, currentToken.utfString);

		case tok_id:
			return std::make_unique<StringValue>(currentToken.string);

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
