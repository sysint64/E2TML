#include "../include/e2ml.h"

char e2ml::Data::lexChar() {
	return (char) ifStreams.back().get();
}

Token *e2ml::Data::lexNextToken() {
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

Token *e2ml::Data::lexPrevToken() {
	--stackCursor;

	if (skipTab)
		while (tokenStack[stackCursor-1].code == tok_tab)
			--stackCursor;

	currentToken = tokenStack[stackCursor-1];
	return &tokenStack[stackCursor-1];
}

Token *e2ml::Data::lexToken() {
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

