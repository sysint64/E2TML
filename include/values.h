#pragma once

#include <string>
#include <vector>
#include "e2ml.h"


namespace e2ml {
	class Parameter;
	class Value {
	public:
		enum class Type {Number, String, Boolean, Array};
		inline Type getType() { return type; }
		virtual void generatePath(Parameter *param);

		inline std::string getName() { return name; }
		inline std::string getPath() { return path; }
		inline std::string getTypeString() {
			return Value::typeToString(type);
		}

		static std::string typeToString(Type type) {
			switch (type) {
				case Type::Number: return "number";
				case Type::String: return "string";
				case Type::Boolean: return "boolean";
				case Type::Array: return "array";
			}
		}

	protected:
		Type type;
		std::string path;
		std::string name;
		Parameter *parameter = nullptr;
	};

	class NumberValue: public Value {
	private:
		float value;

	public:
		NumberValue(const std::string &name, const float value)
				: value(value)
		{
			this->name = name;
			type = Type::Number;
		}

		float getValue() { return value; }
	};

	class BooleanValue: public Value {
	private:
		bool value;

	public:
		BooleanValue(const std::string &name,const bool value)
				: value(value)
		{
			this->name = name;
			type = Type::Boolean;
		}

		bool getValue() { return value; }
	};

	class StringValue: public Value {
	private:
		std::string value;
		std::u32string utfValue;

	public:
		StringValue(const std::string &name,const std::string &value)
				: value(value)
		{
			this->name = name;
			type = Type::String;
		}
		StringValue(const std::string &name, const std::string &value,
		            const std::u32string &utfValue)
				: value(value), utfValue(utfValue)
		{
			this->name = name;
			type = Type::String;
		}

		std::string getValue() { return value; }
		std::u32string getUTFValue() { return utfValue; }
	};

	class ArrayValue: public Value {
	private:
		std::vector<std::unique_ptr<Value>> values;

	public:
		ArrayValue(const std::string &name, std::vector<std::unique_ptr<Value>> values)
				: values(std::move(values))
		{
			this->name = name;
			type = Type::Array;
		}
	};
};
