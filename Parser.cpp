#include "Parser.h"
#include <iostream>
#include <fstream>

// Set PROJ_DEBUG in your compiler settings to output to cout instead of myfile
#ifdef PROJ_DEBUG
#define OUTP std::cout
#else
#define OUTP myfile
#endif

string getTokenName(Token tok) {
	switch (tok) {
	case Invalid:
		return "Invalid Token";
	case EndOfFile:
		return "End of File";

	case Let:
		return "let";
	case Number:
		return "NUM";
	case String:
		return "STRING";

	case PlusOp:
		return "+";
	case MinusOp:
		return "-";
	case MultiplyOp:
		return "*";
	case DivideOp:
		return "/";

	case LteOp:
		return "<=";
	case LtOp:
		return "<";
	case GtOp:
		return ">";
	case GteOp:
		return ">=";
	case NotEq:
		return "!=";
	case Equality:
		return "==";

	case IntSpec:
		return "int";
	case FloatSpec:
		return "float";

	case IfWord:
		return "if";
	case ElseWord:
		return "else";
	case WhileWord:
		return "while";
	case ReturnWord:
		return "return";

	case Identifier:
		return "ID";

	case Comma:
		return ",";
	case Semicolon:
		return ";";

	case Assignment:
		return "=";

	case StartCurlyBracket:
		return "{";
	case EndCurlyBracket:
		return "}";

	case StartSquareBracket:
		return "[";
	case EndSquareBracket:
		return "]";

	case StartParenthesis:
		return "(";
	case EndParenthesis:
		return ")";
	case Comment:
		return "Comment";
	}
}

void Parser::run_program(string file_path) {
	initialize(file_path);
}

void Parser::initialize(string file_path)
{
	// Input Code
	std::string input_str;
	std::ifstream inputf(file_path);

	if (inputf.fail()) {
		throw std::runtime_error("Failed to load file Input.txt");
	}

	inputf.seekg(0, std::ios::end);
	input_str.reserve(inputf.tellg());
	inputf.seekg(0, std::ios::beg);

	input_str.assign((std::istreambuf_iterator<char>(inputf)),
		std::istreambuf_iterator<char>());

	// Run scanner
	Scanner scanner(input_str);
	tokenlist_ = scanner.getTokens();

	parse();
}

void Parser::match(Token expectedToken)
{
	if (tokenlist_[i].token_ == expectedToken)
	{
		i++;
	}
	else
		error(expectedToken);
}

void Parser::error(Token expectedToken)
{
	string t = getTokenName(expectedToken);
	error(t);
}

void Parser::error(string expectedToken)
{
	++num_errors_;
	OUTP << "Parsing Failed at Line: " << tokenlist_[i].line_ << ": unexpected \'" << getTokenName(tokenlist_[i].token_) << "\', expecting " << expectedToken << "\n";
#ifndef PROJ_DEBUG
	myfile.close();
#else
	system("pause");
#endif
	exit(0);
}

void Parser::semanticError(string errorMsg)
{
	OUTP << errorMsg << endl;
}

void Parser::parse()
{
#ifndef PROJ_DEBUG
	myfile.open(output_filepath);
	if (myfile.fail()) {
		printf("Unable to open file: File doesn't exist.\n");
		return;
	}

	myfile.clear();
#endif

	num_errors_ = 0;
	program();
	if (num_errors_ == 0) {
		OUTP << "\n\n===========================\n\n";
		symboltable_.Print(OUTP);
		OUTP << "Parsing Successful" << std::endl;
	}

#ifndef PROJ_DEBUG
	myfile.close();
#endif
}

void Parser::program() // program = declaration_list
{
	// TODO: Call declaration_list() which is grammer 
	declarationList();
}

void Parser::declarationList()
{
	declaration();
	while (tokenlist_[i].token_ != EndOfFile)
	{
		declaration();
	}
}

void Parser::declaration()
{
	if (tokenlist_[i].token_ == IntSpec || tokenlist_[i].token_ == FloatSpec)
		varDeclaration();
	else
		stmt();
}
void Parser::varDeclaration()
{
	ST_Type t = typeSpecifier();
	if (tokenlist_[i].token_ == Identifier) {
		std::string name = (const char *)tokenlist_[i].value_;
		symboltable_.Add(name, t, tokenlist_[i].line_);
		++i; // match(Identifier);
	}
	match(Semicolon);
}

ST_Type Parser::typeSpecifier()
{
	if (tokenlist_[i].token_ == IntSpec) {
		match(IntSpec);
		return ST_Int;
	}
	else if (tokenlist_[i].token_ == FloatSpec) {
		match(FloatSpec);
		return ST_Float;
	}
	else
		error("\'int\' or \'float\'");
}

void Parser::stmtList()
{
	if (tokenlist_[i].token_ == Let || tokenlist_[i].token_ == StartParenthesis || tokenlist_[i].token_ == Identifier || tokenlist_[i].token_ == Number || tokenlist_[i].token_ == Semicolon || tokenlist_[i].token_ == StartCurlyBracket || tokenlist_[i].token_ == IfWord || tokenlist_[i].token_ == WhileWord || tokenlist_[i].token_ == ReturnWord)
	{
		stmt();
		while (tokenlist_[i].token_ == Let || tokenlist_[i].token_ == StartParenthesis || tokenlist_[i].token_ == Identifier || tokenlist_[i].token_ == Number || tokenlist_[i].token_ == Semicolon || tokenlist_[i].token_ == StartCurlyBracket || tokenlist_[i].token_ == IfWord || tokenlist_[i].token_ == WhileWord || tokenlist_[i].token_ == ReturnWord)
		{
			stmt();
		}

	}
}
void Parser::stmt()
{
	if (tokenlist_[i].token_ == Let || tokenlist_[i].token_ == StartParenthesis || tokenlist_[i].token_ == Identifier || tokenlist_[i].token_ == Number || tokenlist_[i].token_ == Semicolon)
	{
		expressionStmt();
	}
	else if (tokenlist_[i].token_ == IfWord)
	{
		selectionStmt();
	}
	else if (tokenlist_[i].token_ == ReadWord)
	{
		readStmt();
	}
	else if (tokenlist_[i].token_ == WriteWord)
	{
		writeStmt();
	}
	else if (tokenlist_[i].token_ == WhileWord)
	{
		iterationStmt();
	}
	else
		error("expression, selection, or iteration statement");
}

void Parser::readVar() {
	if (tokenlist_[i].token_ == Identifier) {
		std::string name = (const char *)tokenlist_[i].value_;
		ST_Type type;
		if (symboltable_.GetType(name, type)) {
			std::string val;
			std:cin >> val;

			if (type == ST_Float)
				symboltable_.SetFloat(name, stod(val));
			else
				symboltable_.SetInt(name, stol(val));
		}
		else {
			semanticError("Undefined variable on line " + tokenlist_[i].line_);
		}
		++i;
	}
	else {
		error(Identifier);
	}
}

void Parser::readStmt() {
	match(ReadWord);
	readVar();
	while (tokenlist_[i].token_ == Comma) {
		match(Comma);
		readVar();
	}
	match(Semicolon);
}

void Parser::writeVar() {
	if (tokenlist_[i].token_ == Identifier) {
		std::string name = (const char *)tokenlist_[i].value_;
		ST_Type type;
		if (symboltable_.GetType(name, type)) {
			if (type == ST_Float) {
				double val;
				symboltable_.GetFloat(name, val);
				std::cout << val;
			}
			else {
				long int val;
				symboltable_.GetInt(name, val);
				std::cout << val;
			}
		}
		else {
			semanticError("Undefined variable on line " + tokenlist_[i].line_);
		}
		++i;
	}
	else if (tokenlist_[i].token_ == String) {
		std::string value = (const char *)tokenlist_[i].value_;
		std::cout << value;
		++i;
	}
	else {
		error("Expected Identifier or String");
	}
}

void Parser::writeStmt() {
	match(WriteWord);
	writeVar();
	while (tokenlist_[i].token_ == Comma) {
		match(Comma);
		writeVar();
	}
	match(Semicolon);
}

void Parser::expressionStmt()
{
	if (tokenlist_[i].token_ == Let || tokenlist_[i].token_ == StartParenthesis || tokenlist_[i].token_ == Identifier || tokenlist_[i].token_ == Number)
	{
		expression();
		match(Semicolon);
	}
	else {
		match(Semicolon);
	}
}
void Parser::selectionStmt()
{
	match(IfWord);
	match(StartParenthesis);
	expression();
	match(EndParenthesis);
	stmt();
	if (tokenlist_[i].token_ == ElseWord)
	{
		match(ElseWord);
		stmt();
	}
}
void Parser::iterationStmt()
{
	match(WhileWord);
	match(StartParenthesis);
	expression();
	match(EndParenthesis);
	stmt();
}
void Parser::expression(string &exp_typ, int &val)
{
	if (tokenlist_[i].token_ == Let)
	{
		match(Let);
		var(exp_typ, val);
		match(Assignment);
	}
	simpleExpression();
}
void Parser::var(string &exp_typ, int &val)
{
	// consult symbol table here
	// enter(id.name, var.type)
	match(Identifier);
}
void Parser::simpleExpression(string &exp_typ, int &val)
{
	string left_type, right_type;
	//Declare local variable that will be used as parameters ("val" synthesized attributes) of the term function
	int left_val, right_val;
	string temp; // variable to keep the name of the temporary variable
	// The BNF of expr is converted to EBNF to avoid the left recursion
	additiveExpression(left_type, left_val); //term() will return type and value of term
	// This loop is the check the types of the operands and evaluate the expression
	if (tokenlist_[i].token_ == GtOp || tokenlist_[i].token_ == LtOp || tokenlist_[i].token_ == LteOp || tokenlist_[i].token_ == GteOp || tokenlist_[i].token_ == Equality || tokenlist_[i].token_ == NotEq)
	{
		relop();
		additiveExpression(right_type, right_val);
		// Check left and right operands types
		if (right_type != left_type)
			semanticError("Operand are not the same type");
		// Compute the left and right operands and put the results in the variable left_val used to accumulate the results
		left_val = left_val + right_val;
	};
	exp_typ = left_type;
	val = left_val;
}
void Parser::expression()
{
	while (tokenlist_[i].token_ == Let)
	{
		match(Let);
		var();
		match(Assignment);
	}
	simpleExpression();
}
void Parser::var()
{
	match(Identifier);
}
void Parser::simpleExpression()
{
	additiveExpression();
	if (tokenlist_[i].token_ == GtOp || tokenlist_[i].token_ == LtOp || tokenlist_[i].token_ == LteOp || tokenlist_[i].token_ == GteOp || tokenlist_[i].token_ == Equality || tokenlist_[i].token_ == NotEq)
	{
		relop();
		additiveExpression();
	}
}
void Parser::relop()
{
	if (tokenlist_[i].token_ == GtOp)
		match(GtOp);
	else if (tokenlist_[i].token_ == LtOp)
		match(LtOp);
	else if (tokenlist_[i].token_ == LteOp)
		match(LteOp);
	else if (tokenlist_[i].token_ == GteOp)
		match(GteOp);
	else if (tokenlist_[i].token_ == Equality)
		match(Equality);
	else if (tokenlist_[i].token_ == NotEq)
		match(NotEq);
	else
		error("\'<=\' or \'<\' or \'>\' or \'<=\' or \'==\' or \'!=\'");
}
void Parser::additiveExpression(string &exp_typ, int &val)
{
	//Declare local variable that will be used as parameters ("type" synthesized attributes) of the term function
	string left_type, right_type;
	//Declare local variable that will be used as parameters ("val" synthesized attributes) of the term function
	int left_val, right_val;
	string temp; // variable to keep the name of the temporary variable
	// The BNF of expr is converted to EBNF to avoid the left recursion
	term(left_type, left_val); //term() will return type and value of term
	// This loop is the check the types of the operands and evaluate the expression
	while (tokenlist_[i].token_ == PlusOp || tokenlist_[i].token_ == MinusOp) {
		addOp();
		term(right_type, right_val);
		// Check left and right operands types
		if (right_type != left_type)
			semanticError("Operand are not the same type");
		// Compute the left and right operands and put the results in the variable left_val used to accumulate the results
		left_val = left_val + right_val;
	};
	exp_typ = left_type;
	val = left_val;
}
void Parser::additiveExpression()
{
	term();
	while (tokenlist_[i].token_ == PlusOp || tokenlist_[i].token_ == MinusOp)
	{
		addOp();
		term();
	}
}
void Parser::addOp()
{
	if (tokenlist_[i].token_ == PlusOp)
		match(PlusOp);
	else if (tokenlist_[i].token_ == MinusOp)
		match(MinusOp);
	else
		error("\'+\' or \'-\'");
}
void Parser::term(string &exp_typ, int &val)
{
	//Declare local variable that will be used as parameters ("type" synthesized attributes) of the term function
	string left_type, right_type;
	//Declare local variable that will be used as parameters ("val" synthesized attributes) of the term function
	int left_val, right_val;
	string temp; // variable to keep the name of the temporary variable
	// The BNF of expr is converted to EBNF to avoid the left recursion
	factor(left_type, left_val); //term() will return type and value of term
	// This loop is the check the types of the operands and evaluate the expression
	while (tokenlist_[i].token_ == MultiplyOp || tokenlist_[i].token_ == DivideOp) {
		mulOp();
		factor(right_type, right_val);
		// Check left and right operands types
		if (right_type != left_type)
			semanticError("Operand are not the same type");
		// Compute the left and right operands and put the results in the variable left_val used to accumulate the results
		left_val = left_val + right_val;
	};
	exp_typ = left_type;
	val = left_val;
}
void Parser::term()
{
	factor();
	while (tokenlist_[i].token_ == MultiplyOp || tokenlist_[i].token_ == DivideOp)
	{
		mulOp();
		factor();
	}
}
void Parser::mulOp()
{
	if (tokenlist_[i].token_ == MultiplyOp)
		match(MultiplyOp);
	else if (tokenlist_[i].token_ == DivideOp)
		match(DivideOp);
	else
		error("* or /");
}
void Parser::factor(string &exp_typ, int &val)
{
	if (tokenlist_[i].token_ == StartParenthesis)
	{
		match(StartParenthesis);
		// factor.val = expression.val
		expression(exp_typ, val);
		match(EndParenthesis);
	}
	else if (tokenlist_[i].token_ == Number)
	{
		// exp_type and val should be assigned here to sth 
		//factor.val = num.val
		match(Number);
	}
	else if (tokenlist_[i].token_ == Identifier)
	{
		var(exp_typ, val);
	}
	else
		error("expression starts with ( or NUM or ID");
}

void Parser::factor()
{
	if (tokenlist_[i].token_ == StartParenthesis)
	{
		match(StartParenthesis);
		expression();
		match(EndParenthesis);
	}
	else if (tokenlist_[i].token_ == Number)
	{
		match(Number);
	}
	else if (tokenlist_[i].token_ == Identifier)
	{
		var();
	}
	else
		error("expression starts with ( or NUM or ID");
}
