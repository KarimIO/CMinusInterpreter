#ifndef BASECOMMON_H
#define BASECOMMON_H
#include <string>
#include <vector>

enum Token {
	Invalid,
	EndOfFile,

	Let,
	Number,
	String,

	PlusOp,
	MinusOp,
	MultiplyOp,
	DivideOp,

	LteOp,
	LtOp,
	GtOp,
	GteOp,
	NotEq,
	Equality,

	IntSpec,
	FloatSpec,

	IfWord,
	ElseWord,
	WhileWord,
	ReturnWord,
	ReadWord,
	WriteWord,

	Identifier,

	Comma,
	Semicolon,

	Assignment,

	StartCurlyBracket,
	EndCurlyBracket,

	StartSquareBracket,
	EndSquareBracket,

	StartParenthesis,
	EndParenthesis,
	Comment
};

struct TokenValuePair {
	Token token_;
	void *value_;
	unsigned int line_;
	unsigned int offset_;

	TokenValuePair() : token_(Token::Invalid), value_(nullptr), line_(0), offset_(0) {};
	TokenValuePair(Token t, unsigned int line, unsigned int offset) : token_(t), value_(nullptr), line_(line), offset_(offset) {};
	TokenValuePair(Token t, void *v, unsigned int line, unsigned int offset) : token_(t), value_(v), line_(line), offset_(offset) {};
};

typedef std::vector<TokenValuePair> TokenList;

#endif