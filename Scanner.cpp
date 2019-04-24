#include "Scanner.h"
#include <iostream>

Scanner::Scanner() : start_(nullptr), end_(nullptr), current_(nullptr), number_(0.0), string_("") {}

Scanner::Scanner(std::string code) : start_(code.data()), end_(start_ + code.size()), current_(start_), number_(0.0), string_("") {
	run();
}

void Scanner::setCode(std::string code) {
	start_ = code.data();
	current_ = start_;
	end_ = start_ + code.size();
}

void Scanner::run() {
	curline_ = 1;
	curoffset_ = 1;

	if (current_ == nullptr)
		throw std::runtime_error("Invalid pointer");

	while (true) {
		Token const token = readToken();

		if (token == Token::Comment) {
			continue;
		}

		tokens_.emplace_back(token, saveline_, saveoffset_);
		TokenValuePair &p = tokens_.back();

		if (token == Token::Invalid) {
			break;
		}
		else if (token == Token::EndOfFile) {
			break;
		}
		else if (token == Token::Float) {
			p.value_ = new double(number_);
		}
		else if (token == Token::Int) {
			p.value_ = new long int(number_);
		}
		else if (token == Token::Identifier || token == Token::String) {
			int s = string_.size() + 1;
			char *str = new char[s];
			memcpy(str, string_.data(), s);
			p.value_ = str;
		}
	}
}

Token Scanner::readToken() {
	skipWhitespace();

	saveline_ = curline_;
	saveoffset_ = curoffset_;

	if (current_ == end_)
		return Token::EndOfFile;

	const char c = *current_;

	switch (c) {
	case '<':
		++current_;
		++curoffset_;
		if (current_ != end_ && *current_ == '=') {
			++current_;
			++curoffset_;
			return Token::LteOp;
		}
		else {
			return Token::LtOp;
		}
	case '>':
		++current_;
		++curoffset_;
		if (current_ != end_ && *current_ == '=') {
			++current_;
			++curoffset_;
			return Token::GteOp;
		}
		else {
			return Token::GtOp;
		}
	case '+':
		++current_;
		++curoffset_;
		return Token::PlusOp;
	case '-':
		++current_;
		++curoffset_;
		return Token::MinusOp;
	case '*':
		++current_;
		++curoffset_;
		return Token::MultiplyOp;
	case ',':
		++current_;
		++curoffset_;
		return Token::Comma;
	case ';':
		++current_;
		++curoffset_;
		return Token::Semicolon;
	case '(':
		++current_;
		++curoffset_;
		return Token::StartParenthesis;
	case ')':
		++current_;
		++curoffset_;
		return Token::EndParenthesis;
	case '{':
		++current_;
		++curoffset_;
		return Token::StartCurlyBracket;
	case '}':
		++current_;
		++curoffset_;
		return Token::EndCurlyBracket;
	case '[':
		++current_;
		++curoffset_;
		return Token::StartSquareBracket;
	case ']':
		++current_;
		++curoffset_;
		return Token::EndSquareBracket;
	case '=':
		++current_;
		++curoffset_;
		if (current_ != end_ && *current_ == '=') {
			++current_;
			++curoffset_;
			return Token::Equality;
		}
		else {
			return Token::Assignment;
		}
	case '\'':
		readString('\'');
		return Token::String;
	case '"':
		readString('"');
		return Token::String;
	}

	auto diff = end_ - current_;
	if (c == '/') {
		if (diff >= 2) {
			const char c2 = *(current_ + 1);

			if (c2 == '/') {
				readLineComment();
				return Token::Comment;
			}
			else if (c2 == '*') {
				readChunkComment();
				return Token::Comment;
			}
		}

		++current_;
		++curoffset_;
		return Token::DivideOp;
	}

	if (c == '.' || (c >= '0' && c <= '9')) {
		return readNumber();
	}

	if (isalpha(c)) {
		// Keywords:
		if (testKeyword("if")) return Token::IfWord;
		if (testKeyword("int")) return Token::IntSpec;
		if (testKeyword("let")) return Token::Let;
		if (testKeyword("read")) return Token::ReadWord;
		if (testKeyword("write")) return Token::WriteWord;
		if (testKeyword("else")) return Token::ElseWord;
		if (testKeyword("return")) return Token::ReturnWord;
		if (testKeyword("float")) return Token::FloatSpec;
		if (testKeyword("while")) return Token::WhileWord;

		// If not a keyword read identifier
		readIdentifier();
		return Token::Identifier;
	}

	return Token::Invalid;
}

bool Scanner::testKeyword(std::string str) {
	// Return false if it's too long.
	auto diff = end_ - current_;
	if (diff <= str.size()) return false;

	for (int i = 0; i < str.size(); ++i) {
		if (current_[i] != str[i]) {
			return false;
		}
	}

	// If the character after is alpha then it's an identifier.
	if (diff > str.size() + 1 && isalpha(current_[str.size()]))
		return false;

	current_ += str.size();
	curoffset_ += str.size();

	return true;
}

// TODO: Handle Escaping
void Scanner::readString(char delimeter) {
	++current_;
	++curoffset_;
	string_ = "";

	while (current_ != end_) {
		char c = *current_;

		++current_;
		++curoffset_;

		if (c == '\\') {
			if (current_ == end_)
				throw std::runtime_error("Unended string.");

			char c2 = *current_;
			switch (c2) {
			case 'a':
				c = '\a';
				break;
			case 'b':
				c = '\b';
				break;
			case 'e':
				c = '\e';
				break;
			case 'f':
				c = '\f';
				break;
			case 'n':
				c = '\n';
				break;
			case 't':
				c = '\t';
				break;
			case 'v':
				c = '\v';
				break;
			case '\\':
				c = '\\';
				break;
			case '\'':
				c = '\'';
				break;
			case '\"':
				c = '\"';
				break;
			case '\?':
				c = '\?';
				break;
			}

			++current_;
			++curoffset_;
		}
		else {
			// TODO: Check for escape character.
			if (c == delimeter) {
				return;
			}
		}

		string_ += c;
	}

	throw std::runtime_error("Unended string.");
}

Token Scanner::readNumber() {
	bool digit = false;
	bool point = false;
	unsigned divide = 1;
	number_ = 0.0;

	while (current_ != end_) {
		char c = *current_;
		if (c >= '0' && c <= '9') {
			digit = true;

			char newnum = (c - '0');
			number_ = 10 * number_ + newnum;

			if (point) {
				divide *= 10;
			}
		}
		else if (c == '.') {
			point = true;
		}
		else {
			break;
		}

		++current_;
		++curoffset_;
	}

	if (digit) {
		number_ /= divide;
		return point ? Token::Float : Token::Int;
	}

	throw std::runtime_error("Unexpected character in number.");
}

void Scanner::printTVP(TokenValuePair &tvp) {
	Token token = tvp.token_;
	void *value = tvp.value_;

	printf("%u %u:", tvp.line_, tvp.offset_);

	if (token == Token::Invalid)
		printf("Invalid\n");
	else if (token == Token::EndOfFile)
		printf("End of file\n");
	else if (token == Token::Int)
		printf("INT: [%.0f]\n", *((double *)value));
	else if (token == Token::Float)
		printf("FLOAT: [%.2f]\n", *((double *)value));
	else if (token == Token::PlusOp)
		printf("ADD\n");
	else if (token == Token::MinusOp)
		printf("SUB\n");
	else if (token == Token::MultiplyOp)
		printf("MULT\n");
	else if (token == Token::DivideOp)
		printf("DIV\n");
	else if (token == Token::Let)
		printf("LET\n");
	else if (token == Token::ReadWord)
		printf("READ\n");
	else if (token == Token::WriteWord)
		printf("WRITE\n");
	else if (token == Token::IfWord)
		printf("IF\n");
	else if (token == Token::ElseWord)
		printf("ELSE\n");
	else if (token == Token::WhileWord)
		printf("WHILE\n");
	else if (token == Token::ReturnWord)
		printf("RETURN\n");
	else if (token == Token::IntSpec)
		printf("INT\n");
	else if (token == Token::FloatSpec)
		printf("FLOAT\n");
	else if (token == Token::GteOp)
		printf("GTE\n");
	else if (token == Token::GtOp)
		printf("GT\n");
	else if (token == Token::LteOp)
		printf("LTE\n");
	else if (token == Token::LtOp)
		printf("LT\n");
	else if (token == Token::String)
		printf("STR:[%s]\n", value);
	else if (token == Token::Identifier)
		printf("ID:[%s]\n", value);
	else if (token == Token::StartParenthesis)
		printf("(\n");
	else if (token == Token::EndParenthesis)
		printf(")\n");
	else if (token == Token::StartCurlyBracket)
		printf("{\n");
	else if (token == Token::EndCurlyBracket)
		printf("}\n");
	else if (token == Token::StartSquareBracket)
		printf("[\n");
	else if (token == Token::EndSquareBracket)
		printf("]\n");
	else if (token == Token::Equality)
		printf("==\n");
	else if (token == Token::Assignment)
		printf("=\n");
	else if (token == Token::Comma)
		printf(",\n");
	else if (token == Token::Semicolon)
		printf(";\n");
	else
		std::cout << "UNKNOWN\n";
}

void Scanner::readLineComment() {
	current_ += 2;

	while (current_ != end_) {
		char c = *current_;

		++current_;
		++curoffset_;

		if (c == '\n') {
			++curline_;
			curoffset_ = 1;
			return;
		}
	}
}

void Scanner::readChunkComment() {
	current_ += 2;
	curoffset_ += 2;

	bool found_star = false;

	while (current_ != end_) {
		char c = *current_;

		++current_;
		++curoffset_;

		if (c == '*') {
			found_star = true;
		}
		else {
			if (c == '\n') {
				++curline_;
				curoffset_ = 1;
			}
			else if (found_star && c == '/') {
				return;
			}
			found_star = false;
		}
	}
}

void Scanner::readIdentifier() {
	string_ = *current_;
	++current_;
	++curoffset_;

	while (current_ != end_) {
		char c = *current_;

		if (isalpha(c)) {
			string_ += c;
		}
		else {
			break;
		}

		++current_;
		++curoffset_;
	}
}

void Scanner::skipWhitespace() {
	while (current_ != end_) {
		if (*current_ == '\n') {
			++current_;
			++curline_;
			curoffset_ = 1;
		}
		else if (isspace(*current_)) {
			++current_;
			++curoffset_;
		}
		else {
			break;
		}
	}
}

TokenList Scanner::getTokens() {
	return tokens_;
}

void Scanner::printTokens() {
	std::cout << "Scanned Tokens: \n";

	for (auto &t : tokens_) {
		std::cout << "\tTOKEN: ";
		printTVP(t);
	}

	std::cout << "\n\n";
}