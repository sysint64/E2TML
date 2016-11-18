#include <assert.h>
#include "../include/e2ml.h"

using namespace e2ml;

void Data::load(const std::string &fileName, IOType rt) {
	std::ifstream ifs (fileName, std::ifstream::in);
	ifStreams.push_back(std::move(ifs));

	switch (rt) {
		case IOType::Text: parse(); break;
		case IOType::Bin: break;
	}
}

void Parameter::generatePath(Node *parent) {
	assert(this->parent == nullptr);

	this->parent = parent;
	path = parent->path + "." + name;
}

void Data::generateValuesPath(Parameter *parameter) {
	assert(parameter != nullptr);

	for (const auto &value : parameter->values) {
		value->generatePath(parameter);
		valuesMap[value->getPath()] = value.get();
	}
}

void Data::generateParamsPath(Node *node) {
	assert(node != nullptr);

	for (const auto &param : node->parameters) {
		param->generatePath(node);
		parametersMap[node->path] = param.get();

		generateValuesPath(param.get());
	}
};

void Data::generatePath(Node *parent) {
	assert(parent != nullptr);

	for (const auto &node : parent->children) {
		if (parent != &root) {
			node->generatePath(parent);
			nodesMap[node->path] = node.get();
		}

		generateParamsPath(node.get());
		generatePath(node.get());
	}
}
