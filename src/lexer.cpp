#include <algorithm>
#include "../include/e2ml.h"

using namespace e2ml;

char Data::lexChar() {
	if (!ifStreams.back().good())
		return tok_eof;

	return (char) ifStreams.back().get();
}

Data::Token *Data::lexNextToken() {
	if (stackCursor < tokenStack.size()) {
		++stackCursor;
		currentToken = tokenStack[stackCursor];
		return &tokenStack[stackCursor];
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
	if (isdigit(lastChar) || lastChar == '-')
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

					if (lastChar != '\"')
						value += lastChar;
				} while (lastChar != '\"' && lastChar != tok_eof);

				if (lastChar == tok_eof)
					throw LexerError("lex string: end of file");
				else
					lastChar = lexChar();

				if (lastChar == '\\') {
					state = 1;
					break;
				}

				state = 2;
				break;

			case 1:
				lastChar = lexChar();

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

	currentToken = Token(tok_number);
	currentToken.number = std::stof(numStr);

	return &currentToken;
}

Data::Token *Data::lexTokId() {
	currentToken = Token(tok_id);
	currentToken.identifier = lastChar;

	while (isdigit(lastChar) || isalpha(lastChar) || lastChar == '_') {
		lastChar = lexChar();
		currentToken.identifier += lastChar;
	}

	currentToken.name = currentToken.identifier;
	std::transform(currentToken.identifier.begin(), currentToken.identifier.end(),
	               currentToken.identifier.begin(), ::tolower);

	if (currentToken.identifier == "include") {
		currentToken.code = tok_include;
	}

	if (currentToken.identifier == "true") {
		currentToken.code = tok_number;
		currentToken.number = 0.f;
	}

	if (currentToken.identifier == "false") {
		currentToken.code = tok_number;
		currentToken.number = 1.f;
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
