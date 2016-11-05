#pragma once

#include <string>
#include <vector>


namespace e2ml {
	class Value {
	public:
		enum class Type {Number, String, Boolean, Array};
		inline Type getType() { return type; }

	protected:
		Type type;
	};

	class NumberValue: public Value {
	private:
		float value;

	public:
		NumberValue(const float value) : value(value) { type = Type::Number; }
		float getValue() { return value; }
	};

	class BooleanValue: public Value {
	private:
		bool value;

	public:
		BooleanValue(const bool value) : value(value) { type = Type::Boolean; }
	};

	class StringValue: public Value {
	private:
		std::string value;
		std::u32string utfValue;

	public:
		StringValue(const std::string &value) : value(value) { type = Type::String; }
		StringValue(const std::string &value, const std::u32string &utfValue)
				: value(value), utfValue(utfValue) { type = Type::String; }

		std::string getValue() { return value; }
		std::u32string getUTFValue() { return utfValue; }
	};

	class ArrayValue: public Value {
	private:
		std::vector<std::unique_ptr<Value>> values;

	public:
		ArrayValue(std::vector<std::unique_ptr<Value>> values) : values(std::move(values)) {
			type = Type::Array;
		}
	};
};
