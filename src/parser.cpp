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

	generatePath(&root);
}

std::unique_ptr<Node> Data::parseObject() {
	std::string name = currentToken.string;
	std::string type = "";
	std::vector<std::unique_ptr<Parameter>> parameters;

	isInline = true;
	auto node = std::make_unique<Node>(name, name);

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

			node->addChild(parseObject());
			continue;
		}

		parameters.push_back(parseParameter(paramName));
	}

	lexPrevToken();
	node->parameters = std::move(parameters);

	return node;
}

void Data::parseInclude() {

}

std::unique_ptr<Parameter> Data::parseParameter(const std::string &name) {
	lockIndent();
	std::vector<std::unique_ptr<Value>> values;

	while (true) {
		const std::string index = std::to_string(values.size());
		values.push_back(parseValue(index));
		lexNextToken();

		if (currentToken.code != ',')
			break;
	}

	unlockIndent();
	return std::make_unique<Parameter>(name, name, std::move(values));
}

std::unique_ptr<Value> Data::parseValue(const std::string &name) {
	lexNextToken();

	switch (currentToken.code) {
		case '[':
			return parseArray(name);

		case tok_number:
			return std::make_unique<NumberValue>(name, currentToken.number);

		case tok_string:
			return std::make_unique<StringValue>(name, currentToken.string, currentToken.utfString);

		case tok_id:
			return std::make_unique<StringValue>(name, currentToken.string);

		case tok_boolean:
			return std::make_unique<BooleanValue>(name, currentToken.boolean);

		default:
			throw ParseError("value error", line, pos);
	}
}

std::unique_ptr<Value> Data::parseArray(const std::string &name) {
	std::vector<std::unique_ptr<Value>> values;

	while (currentToken.code != ']' || currentToken.code != tok_eof) {
		const std::string index = std::to_string(values.size());
		auto value = parseValue(index);
		values.push_back(std::move(value));
		lexNextToken();

		if (currentToken.code != ',' && currentToken.code != ']')
			throw ParseError("expected ',' or ']'", line, pos);

		if (currentToken.code == ']')
			break;
	}

	return std::make_unique<ArrayValue>(name, std::move(values));
}
