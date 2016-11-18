#include <assert.h>
#include "../include/e2ml.h"

using namespace e2ml;

void Value::generatePath(Parameter *param) {
	assert(parameter == nullptr);
	path = param->path + "." + name;
	parameter = param;
}

Value *Data::getValue(const std::string &key) {
	if (valuesMap.find(key) == valuesMap.end())
		throw ValueDoesNotExist(key);

	return valuesMap[key];
}

float Data::getNumber(const std::string &key) {
	NumberValue *value = this->getValue<NumberValue*>(key, Value::Type::Number);
	return value->getValue();
}

int Data::getInt(const std::string &key) {
	return static_cast<int>(getNumber(key));
}

std::string Data::getString(const std::string &key) {
	StringValue *value = this->getValue<StringValue*>(key, Value::Type::String);
	return value->getValue();
}

std::u32string Data::getUTFString(const std::string &key) {
	StringValue *value = this->getValue<StringValue*>(key, Value::Type::String);
	return value->getUTFValue();
}

bool Data::getBoolean(const std::string &key) {
	BooleanValue *value = this->getValue<BooleanValue*>(key, Value::Type::Boolean);
	return value->getValue();
}

ArrayValue *Data::getArray(const std::string &key) {
	return this->getValue<ArrayValue*>(key, Value::Type::Array);
}
