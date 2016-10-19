#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <exception>

namespace e2ml {
	class LexerError : public std::exception {
	private:
		std::string details;

	public:
		LexerError(const std::string &details) {
			this->details = details;
		}

		virtual const char* what() const throw() {
			return ("Lexer error: "+details).c_str();
		}
	};

	enum {
		tok_eof = -1, tok_tab = -2,
		tok_id = -3, tok_number = -4, tok_string = -5,
		tok_include = -7
	};

	enum {
		op_eof  = 0x00
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
			std::string name;
			float number;
		} typedef tok;

		Node root;
		std::vector<std::ifstream> ifStreams;
		std::vector<unsigned char> byteCode;
		std::vector<Token> tokenStack;
		size_t stackCursor = 0;

		// Meta information
		char lastChar = ' ';
		Token currentToken;
		bool skipTab = false;

		// Lexer
		char lexChar();
		Token *lexToken();
		Token *lexNextToken();
		Token *lexPrevToken();
		Token *lexTokString();
		Token *lexTokNumber();
		Token *lexTokId();
		Token *lexComment();

		// Parser
		void parse();

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
