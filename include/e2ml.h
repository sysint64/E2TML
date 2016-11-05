#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <bits/unique_ptr.h>
#include "values.h"


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

	class Node {
	public:
		std::string name;
		std::string path;
		std::vector<std::unique_ptr<Value>> values;
		std::vector<std::unique_ptr<Node>> children;
	};

	class Data {
	private:
		struct Token {
			Token() {}
			Token(const char code): code(code) {}
			Token(const char code, const std::string &identifier): code(code), identifier(identifier) {}

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
		int  lockedIndent = 0;
		char lostChar = -1;

		Node root;
		std::vector<std::ifstream> ifStreams;
		std::vector<unsigned char> byteCode;
		std::vector<Token> tokenStack;
		size_t stackCursor = 0;

		// Meta information
		Token currentToken;
		char  lastChar    = ' ';
		bool  isLockIndent = false;
		bool  skipTab      = false;
		int   line         = 1;
		int   pos          = 0;

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
		void parseObject();
		void parseParameters(const std::string &name);
		std::unique_ptr<Value> parseValue();
		std::unique_ptr<Value> parseArray();
		void parseInclude();

	public:
		void load(const std::string &fileName, IOType rt = IOType::Text);
		void save(const std::string &fileName, IOType rt = IOType::Text);

		template<typename T>
		Value *get_as(const std::string &key) {
			return nullptr;
		}

		inline std::vector<std::unique_ptr<Node>>::iterator begin() {
			return root.children.begin();
		}

		inline std::vector<std::unique_ptr<Node>>::iterator end() {
			return root.children.end();
		}

		inline std::unique_ptr<Node> operator*() {
//			return *root.children.data();
			return nullptr;
		}
	};
}
