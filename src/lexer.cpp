#include <algorithm>
#include <iostream>
#include "../include/e2ml.h"

using namespace e2ml;

char Data::lexChar() {
	if (!ifStreams.back().good())
		return tok_eof;

	char ch = lostChar != -1 ? lostChar : (char) ifStreams.back().get();
	lostChar = -1;

	// Calculate indents
	if (ch == ' ' && tabSize == 0 && calculateIndent) {
		while (ch == ' ') {
			++tabSize;
			ch = (char) ifStreams.back().get();
		}

		lostChar = ch;
		return '\t';
	} else if (ch == ' ' && tabSize > 0) {
		int spaces = 0;

		while (ch == ' ') {
			++spaces;
			ch = (char) ifStreams.back().get();

			if (spaces == tabSize) {
				lostChar = ch;
				return '\t';
			}
		}
	} else if (ch == '\r' || ch == '\n') {
		indent = 0;
		calculateIndent = true;
	} else calculateIndent = false;

	return ch;
}

Data::Token *Data::lexNextToken() {
	if (stackCursor < tokenStack.size()) {
		currentToken = tokenStack[stackCursor];
		return &tokenStack[stackCursor++];
	} else {
		lexToken();
		tokenStack.push_back(currentToken);
		stackCursor = tokenStack.size();
		return &currentToken;
	}
}

Data::Token *Data::lexPrevToken() {
	--stackCursor;

	if (skipTab)
		while (tokenStack[stackCursor-1].code == tok_tab)
			--stackCursor;

	currentToken = tokenStack[stackCursor-1];
	return &tokenStack[stackCursor-1];
}

Data::Token *Data::lexToken() {
	// Skip spaces
	while (isspace(lastChar) && (lastChar != '\t' || skipTab))
		lastChar = lexChar();

	// Tab
	if (!skipTab && lastChar == '\t') {
		++indent;
		lastChar = lexChar();
		currentToken = Token(tok_tab);
		return &currentToken;
	}

	// String
	if (lastChar == '\"')
		return lexTokString();

	// Identifier: [a-zA-Z_][a-zA-Z0-9_]*
	if (isalpha (lastChar) || lastChar == '_')
		return lexTokId();

	// Number Float or Integer: [0-9] (.[0-9]+)? (E[+-]? [0-9]+)?
	if (isdigit(lastChar) || lastChar == '-' || lastChar == '+')
		return lexTokNumber();

	// Comment
	if (lastChar == '#')
		return lexComment();

	// return as ASCII
	currentToken = Token(lastChar);
	lastChar     = lexChar();

	return &currentToken;
}

Data::Token *Data::lexTokString() {
	std::string value;
	char state = 0;

	while (state != 2) {
		switch (state) {
			case 0:
				do {
					lastChar = lexChar();

					if (lastChar == '\\') {
						state = 1;
						break;
					}

					if (lastChar != '\"')
						value += lastChar;
				} while (lastChar != '\"' && lastChar != tok_eof);

				if (lastChar == tok_eof)
					throw LexerError("lex string: end of file");
				else
					lastChar = lexChar();

				if (state == 0)
					state = 2;

				break;

			case 1:
				switch (lastChar) {
					case 'n' : value += "\n"; break;
					case 'r' : value += "\r"; break;
					case '\\': value += "\\"; break;
					case '\"': value += "\""; break;
					default  : break;
				}

				state = 0;
				break;

			default:
				state = 2;
				break;
		}
	}

	currentToken = Token(tok_string);
	currentToken.string = value;
	return &currentToken;
}

Data::Token *Data::lexTokNumber() {
	char state = 0;
	std::string numStr;
	const int finalState = 9;

	while (state < 8) {
		switch (state) {
			case 0: // Digit main
				numStr += lastChar;
				lastChar = lexChar();

				if (isdigit(lastChar)) {
					state = 0;
				} else if (lastChar == '.') {
					state = 1;
				} else {
					state = finalState;
				}

				break;

			case 1: // Comma
				numStr += lastChar;
				lastChar = lexChar();

				if (isdigit(lastChar)) {
					state = 2;
				} else {
					throw LexerError("lex number: not digit after comma");
				}

				break;

			case 2: // Digit after comma
				numStr += lastChar;
				lastChar = lexChar();

				if (isdigit(lastChar)) {
					state = 2;
				} else {
					state = finalState;
				}

				break;

			default:
				throw LexerError("lex number: unknown error");
		}
	}

	if (numStr == "-" || numStr == "+") {
		currentToken = Token(numStr[0]);
		return &currentToken;
	}

	currentToken = Token(tok_number);
	currentToken.number = std::stof(numStr);

	return &currentToken;
}

Data::Token *Data::lexTokId() {
	currentToken = Token(tok_id);

	while (isdigit(lastChar) || isalpha(lastChar) || lastChar == '_') {
		currentToken.identifier += lastChar;
		lastChar = lexChar();
	}

	currentToken.string = currentToken.identifier;
	std::transform(currentToken.identifier.begin(), currentToken.identifier.end(),
	               currentToken.identifier.begin(), ::tolower);

	if (currentToken.identifier == "include") {
		currentToken.code = tok_include;
	}

	if (currentToken.identifier == "true") {
		currentToken.code = tok_number;
		currentToken.number = 1.f;
	}

	if (currentToken.identifier == "false") {
		currentToken.code = tok_number;
		currentToken.number = 0.f;
	}

	return &currentToken;
}

Data::Token *Data::lexComment() {
	while (lastChar != tok_eof && lastChar != '\n' && lastChar != '\r')
		lastChar = lexChar();

	if (lastChar != tok_eof)
		return lexToken();

	currentToken = Token(tok_eof);
	return &currentToken;
}
