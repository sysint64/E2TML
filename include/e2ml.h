#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <exception>
#include <sstream>
#include <iostream>

namespace e2ml {
	class LexerError : public std::exception {
	private:
		std::string details;

	public:
		LexerError(const std::string &details) {
			this->details = details;
		}

		virtual const char* what() const noexcept {
			return details.c_str();
		}
	};

	class ParseError : public std::exception {
	private:
		std::string details;

	public:
		ParseError(const std::string &details, const int line, const int pos) {
			std::stringstream stream;
			stream << "line " << line << ", pos " << pos << ": " << details;
			this->details = stream.str();
		}

		virtual const char* what() const noexcept {
			return details.c_str();
		}
	};

	enum {
		tok_eof = -1, tok_tab = -2,
		tok_id = -3, tok_number = -4, tok_string = -5,
		tok_include = -7
	};

	enum {
		op_eof = 0x00
	};

	enum class IOType {Text, Bin};

	template<typename T>
	class Value {
		void *data;
	};

	class Node {
	public:
		std::string name;
		std::string path;
		std::vector<Node> children;
	};

	class Data {
	private:
		struct Token {
			Token() {}
			Token(const char code): code(code) {}
			Token(const char code, const std::string &identifier): code(code), identifier(identifier) {}
			Token(const char code, const float number): code(code), number(number) {}

			char code;
			std::string identifier;
			std::string string;
			float number;
		};

		int  tabSize = 0;
		bool calculateIndent = true;
		int  indent = 0;
		char lostChar = -1;

		Node root;
		std::vector<std::ifstream> ifStreams;
		std::vector<unsigned char> byteCode;
		std::vector<Token> tokenStack;
		size_t stackCursor = 0;

		// Meta information
		Token currentToken;
		char  lastChar  = ' ';
		bool  skipTab   = false;
		int   line      = 1;
		int   pos       = 0;

		// Lexer
		char   lexChar();
		Token *lexToken();
		Token *lexNextToken();
		Token *lexPrevToken();
		Token *lexTokString();
		Token *lexTokNumber();
		Token *lexTokId();
		Token *lexComment();

		// Parser
		void parse();
		void parseObject();
		void parseInclude();

	public:
		void load(const std::string &fileName, IOType rt = IOType::Text);
		void save(const std::string &fileName, IOType rt = IOType::Text);

		template<typename T>
		Value<T> *get_as(const std::string &key) {
			return nullptr;
		}

		inline std::vector<Node>::iterator begin() {
			return root.children.begin();
		}

		inline std::vector<Node>::iterator end() {
			return root.children.end();
		}

		inline Node operator*() {
			return *root.children.data();
		}
	};
}
