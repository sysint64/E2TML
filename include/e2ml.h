#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <bits/unique_ptr.h>
#include <map>
#include "values.h"
#include "exceptions.h"
#include <assert.h>


namespace e2ml {
	enum {
		tok_eof = -1, tok_tab = -2,
		tok_id = -3, tok_number = -4, tok_string = -5, tok_boolean = -6,
		tok_include = -7
	};

	enum {
		op_eof = 0x00
	};

	enum class IOType {Text, Bin};

	class Node;
	class Parameter {
	private:
		Node *parent = nullptr;

	public:
		std::string name;
		std::string path;
		std::vector<std::unique_ptr<Value>> values;

		Parameter(const std::string &name, const std::string &path,
		          std::vector<std::unique_ptr<Value>> values)
				: name(name), path(path), values(std::move(values)) {}

		void generatePath(Node *parent);
//		inline void generatePath(Node *parent) {
//			assert(this->parent == nullptr);
//
//			this->parent = parent;
//			path = parent->path + "." + name;
//		}
	};

	class Node {
	private:
		Node *parent = nullptr;

	public:
		std::string name;
		std::string path;
		std::vector<std::unique_ptr<Parameter>> parameters;
		std::vector<std::unique_ptr<Node>> children;

		Node() {}
		Node(const std::string &name, const std::string &path) : name(name), path(path) {}
		Node(const std::string &name, const std::string &path, std::vector<std::unique_ptr<Parameter>> parameters)
				: name(name), path(path), parameters(std::move(parameters)) {}

		inline void addChild(std::unique_ptr<Node> node) {
			children.push_back(std::move(node));
		}

		inline void generatePath(Node *parent) {
			assert(this->parent == nullptr);

			this->parent = parent;
			path = parent->path + "." + name;
		}
	};

	class Data {
	private:
		struct Token {
			Token() {}
			Token(const char code): code(code) { }
			Token(const char code, const std::string &identifier)
					: code(code), identifier(identifier) {}

			int indent;
			char code;
			std::string identifier;
			std::string string;
			std::u32string utfString;
			bool boolean;
			float number;
		};

		int  tabSize = 0;
		bool calculateIndent = true;
		bool calculateTabSize = true;
		int  indent = 0;
		int  lastIndent = 0;
		int  lockedIndent = 0;
		char lostChar = -1;

		std::vector<std::ifstream> ifStreams;
		std::vector<unsigned char> byteCode;
		std::vector<Token> tokenStack;
		std::map<std::string, Node*> nodesMap;
		std::map<std::string, Parameter*> parametersMap;
		std::map<std::string, Value*> valuesMap;

		size_t stackCursor = 0;

		// Meta information
		Token currentToken;
		char  lastChar     = ' ';
		bool  isLockIndent = false;
		bool  skipTab      = false;
		int   line         = 1;
		int   pos          = 0;
		bool  isInline     = false;

		bool lockRN = false; // If true block skipping newline in lexer and just return newline char

		// Lexer
		char   lexChar();
		Token *lexToken();
		Token *lexNextToken();
		Token *lexPrevToken();
		Token *lexTokString();
		Token *lexTokNumber();
		Token *lexTokId();
		Token *lexComment();

		inline void lockIndent() {
			lockedIndent = indent;
			isLockIndent = true;
		}

		inline void unlockIndent() {
			isLockIndent = false;
		}

		inline int getIndent() {
			return isLockIndent ? lockedIndent : indent;
		}

		// Parser
		void parse();
		std::unique_ptr<Node> parseObject();
		std::unique_ptr<Parameter> parseParameter(const std::string &name);
		std::unique_ptr<Value> parseValue(const std::string &name);
		std::unique_ptr<Value> parseArray(const std::string &name);
		void parseInclude();

		void generatePath(Node *parent = nullptr);
		void generateParamsPath(Node *node = nullptr);
		void generateValuesPath(Parameter *parameter = nullptr);

		template<typename T>
		T getValue(const std::string &key, const Value::Type &valueType) {
			Value *value = getValue(key);

			if (value->getType() != valueType)
				throw BadValueType(Value::typeToString(valueType), value->getTypeString());

			//return value;
			return dynamic_cast<T>(value);
		}

	public:
		void load(const std::string &fileName, IOType rt = IOType::Text);
		void save(const std::string &fileName, IOType rt = IOType::Text);

		Node root;

		template<typename T>
		Value *get_as(const std::string &key) {
			return nullptr;
		}

		Value *getValue(const std::string &key);
		float getNumber(const std::string &key);
		int getInt(const std::string &key);
		std::string getString(const std::string &key);
		std::u32string getUTFString(const std::string &key);
		bool getBoolean(const std::string &key);
		ArrayValue *getArray(const std::string &key);

//		inline std::vector<Node>::iterator begin() { return root.children.begin(); }
//		inline std::vector<Node>::iterator end()   { return root.children.end(); }
//		inline Node *operator*()                   { return root.children.data(); }
	};
}
