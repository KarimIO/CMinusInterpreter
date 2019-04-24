#pragma once

#include "BaseCommon.h"

class Scanner {
public:
	Scanner();
	Scanner(std::string code);
	void setCode(std::string code);
	void run();
	void readIdentifier();
	void skipWhitespace();
	TokenList getTokens();
	void printTokens();
private:
	const char *start_;
	const char *end_;
	const char *current_;
	TokenList tokens_;

	unsigned int curline_;
	unsigned int curoffset_;
	unsigned int saveline_;
	unsigned int saveoffset_;

	Token readToken();
	void readString(char delimeter);
	Token readNumber();
	void printTVP(TokenValuePair &token);
	bool testKeyword(std::string str);
	void readLineComment();
	void readChunkComment();

	double number_;			// For all numbers. Later, only for doubles.
	std::string string_;	// For identifiers and stringss
};