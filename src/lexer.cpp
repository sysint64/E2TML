#include "../include/e2ml.h"

using namespace e2ml;

char Data::lexChar() {
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

	// Comment or '/'
	if (lastChar == '/')
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
				} while (lastChar != '\"' && lastChar != EOF);

				if (lastChar == EOF)
					throw LexerError("lex string, end of file");
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
