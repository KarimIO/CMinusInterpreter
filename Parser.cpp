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
	case Int:
		return "INT";
	case Float:
		return "FLOAT";
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

void Parser::semanticError(string errorMsg, unsigned int line)
{
	OUTP << "Semantic error on line " << line << ": " << errorMsg << endl;
#ifndef PROJ_DEBUG
	myfile.close();
#else
	system("pause");
#endif
	exit(0);
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
		stmt(true);
}
void Parser::varDeclaration()
{
	DataType t = typeSpecifier();
	if (tokenlist_[i].token_ == Identifier) {
		std::string name = (const char *)tokenlist_[i].value_;
		symboltable_.Add(name, t, tokenlist_[i].line_);
		++i; // match(Identifier);
	}
	match(Semicolon);
}

DataType Parser::typeSpecifier()
{
	if (tokenlist_[i].token_ == IntSpec) {
		match(IntSpec);
		return DT_Int;
	}
	else if (tokenlist_[i].token_ == FloatSpec) {
		match(FloatSpec);
		return DT_Float;
	}
	else
		error("\'int\' or \'float\'");
}

void Parser::stmtList(bool exec)
{
	if (tokenlist_[i].token_ == Let || tokenlist_[i].token_ == StartParenthesis || tokenlist_[i].token_ == Identifier || tokenlist_[i].token_ == Float || tokenlist_[i].token_ == Int || tokenlist_[i].token_ == Semicolon || tokenlist_[i].token_ == StartCurlyBracket || tokenlist_[i].token_ == IfWord || tokenlist_[i].token_ == WhileWord || tokenlist_[i].token_ == ReturnWord)
	{
		stmt(exec);
		while (tokenlist_[i].token_ == Let || tokenlist_[i].token_ == StartParenthesis || tokenlist_[i].token_ == Identifier || tokenlist_[i].token_ == Float || tokenlist_[i].token_ == Int || tokenlist_[i].token_ == Semicolon || tokenlist_[i].token_ == StartCurlyBracket || tokenlist_[i].token_ == IfWord || tokenlist_[i].token_ == WhileWord || tokenlist_[i].token_ == ReturnWord)
		{
			stmt(exec);
		}

	}
}
void Parser::stmt(bool exec)
{
	if (tokenlist_[i].token_ == Let || tokenlist_[i].token_ == StartParenthesis || tokenlist_[i].token_ == Identifier || tokenlist_[i].token_ == Float || tokenlist_[i].token_ == Int || tokenlist_[i].token_ == Semicolon)
	{
		expressionStmt(exec);
	}
	else if (tokenlist_[i].token_ == IfWord)
	{
		selectionStmt(exec);
	}
	else if (tokenlist_[i].token_ == ReadWord)
	{
		readStmt(exec);
	}
	else if (tokenlist_[i].token_ == WriteWord)
	{
		writeStmt(exec);
	}
	else if (tokenlist_[i].token_ == WhileWord)
	{
		iterationStmt(exec);
	}
	else
		error("expression, selection, or iteration statement");
}

void Parser::readVar(bool exec) {
	unsigned int line = tokenlist_[i].line_;

	if (exec) {
		if (tokenlist_[i].token_ == Identifier) {
			std::string name = (const char *)tokenlist_[i].value_;
			DataType type;
			if (symboltable_.GetType(name, type)) {
				std::string val;
				std:cin >> val;

				if (type == DT_Float)
					symboltable_.SetFloat(name, stod(val));
				else
					symboltable_.SetInt(name, stol(val));
			}
			else {
				semanticError("Undefined variable", line);
			}
			++i;
		}
		else {
			error(Identifier);
		}
	}
	else {
		match(Identifier);
	}
}

void Parser::readStmt(bool exec) {
	match(ReadWord);
	readVar(exec);
	while (tokenlist_[i].token_ == Comma) {
		match(Comma);
		readVar(exec);
	}
	match(Semicolon);
}

void Parser::writeVar(bool exec) {
	unsigned int line = tokenlist_[i].line_;

	if (tokenlist_[i].token_ == Let || tokenlist_[i].token_ == StartParenthesis || tokenlist_[i].token_ == Identifier || tokenlist_[i].token_ == Float || tokenlist_[i].token_ == Int) {
		if (exec) {
			DataType t;
			DataValue v;
			expression(t, v);

			if (t == DT_Float)	OUTP << v.f;
			else				OUTP << v.i;
		}
		else
			expression();
	}
	else if (tokenlist_[i].token_ == String) {
		if (exec) {
			std::string value = (const char *)tokenlist_[i].value_;
			std::cout << value;
			++i;
		}
		else
			match(String);
	}
	else {
		error("Expected Identifier or String");
	}
}

void Parser::writeStmt(bool exec) {
	match(WriteWord);
	writeVar(exec);
	while (tokenlist_[i].token_ == Comma) {
		match(Comma);
		writeVar(exec);
	}
	match(Semicolon);
}

void Parser::expressionStmt(bool exec)
{
	if (tokenlist_[i].token_ == Let || tokenlist_[i].token_ == StartParenthesis || tokenlist_[i].token_ == Identifier || tokenlist_[i].token_ == Float || tokenlist_[i].token_ == Int)
	{
		if (exec) {
			DataType t;
			DataValue v;
			expression(t, v);
		}
		else
			expression();

		match(Semicolon);
	}
	else {
		match(Semicolon);
	}
}
void Parser::selectionStmt(bool exec)
{
	unsigned int line = tokenlist_[i].line_;

	match(IfWord);
	match(StartParenthesis);
	DataType t;
	DataValue v;

	if (exec)	expression(t, v);
	else		expression();

	match(EndParenthesis);

	bool success = true;
	if (exec) success = evaluateExpression(t, v);

	stmt(exec && success);
	if (tokenlist_[i].token_ == ElseWord)
	{
		match(ElseWord);
		stmt(exec && !success);
	}
}

void Parser::iterationStmt(bool exec)
{
	match(WhileWord);
	unsigned int repeat_part = i;

	bool success = true;

	while (true) {
		match(StartParenthesis);
		if (exec) {
			DataType t;
			DataValue v;
			expression(t, v);
			success = evaluateExpression(t, v);
		}
		else {
			expression();
			success = true;
		}
		
		match(EndParenthesis);

		if (success) {
			stmt(exec);
			i = repeat_part;
		}
		else {
			stmt(success && exec);
			break;
		}
	}

}

bool Parser::evaluateExpression(DataType exp_typ, DataValue val) {
	if (exp_typ != DT_Int) {
		semanticError("Expressions in If conditions must be of type int", line);
	}

	return (val.i > 0);
}

void Parser::expression(DataType &exp_typ, DataValue &val)
{
	unsigned int line = tokenlist_[i].line_;

	DataType lt, rt;
	DataValue lv, rv;

	bool hasL = tokenlist_[i].token_ == Let;
	std::string id = "";
	if (hasL)
	{
		match(Let);
		if (tokenlist_[i].token_ == Identifier) {
			id = (const char *)tokenlist_[i].value_;
			var(lt, lv);
		}
		match(Assignment);
	}

	simpleExpression(rt, rv);

	if (hasL) {
		if (lt != rt) {
			semanticError("Left hand type does not match right hand type", line);
		}

		if (lt == DT_Float)
			symboltable_.SetFloat(id, rv.f);
		else
			symboltable_.SetInt(id, rv.i);
	}

	exp_typ = rt;
	val = rv;
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

void Parser::var(DataType &exp_typ, DataValue &val)
{
	unsigned int line = tokenlist_[i].line_;

	if (tokenlist_[i].token_ == Identifier) {
		std::string name = (const char *)tokenlist_[i].value_;
		if (symboltable_.GetType(name, exp_typ)) {
			if (exp_typ == DT_Float) {
				double v;
				symboltable_.GetFloat(name, v);
				val.f = v;
			}
			else {
				long int v;
				symboltable_.GetInt(name, v);
				val.i = v;
			}
		}
		else {
			semanticError("Undefined variable", line);
		}
		++i;
	}
	else
		error(Identifier);
}

void Parser::var()
{
	match(Identifier);
}

void Parser::simpleExpression(DataType &exp_typ, DataValue &val)
{
	unsigned int line = tokenlist_[i].line_;

	DataType left_type, right_type;
	//Declare local variable that will be used as parameters ("val" synthesized attributes) of the term function
	DataValue left_val, right_val;
	string temp; // variable to keep the name of the temporary variable
	// The BNF of expr is converted to EBNF to avoid the left recursion
	additiveExpression(left_type, left_val); //term() will return type and value of term
	// This loop is the check the types of the operands and evaluate the expression
	if (tokenlist_[i].token_ == GtOp || tokenlist_[i].token_ == LtOp || tokenlist_[i].token_ == LteOp || tokenlist_[i].token_ == GteOp || tokenlist_[i].token_ == Equality || tokenlist_[i].token_ == NotEq)
	{
		Token t = tokenlist_[i].token_;

		relop();
		additiveExpression(right_type, right_val);
		// Check left and right operands types
		if (right_type != left_type)
			semanticError("Operands are not the same type", line);
		// Compute the left and right operands and put the results in the variable left_val used to accumulate the results
		if (left_type == DT_Float) {
			switch (t) {
			case GtOp:
				left_val.i = left_val.f > right_val.f;
				break;
			case LtOp:
				left_val.i = left_val.f < right_val.f;
				break;
			case LteOp:
				left_val.i = left_val.f <= right_val.f;
				break;
			case GteOp:
				left_val.i = left_val.f >= right_val.f;
				break;
			case Equality:
				left_val.i = left_val.f == right_val.f;
				break;
			case NotEq:
				left_val.i = left_val.f != right_val.f;
				break;
			}
		}
		else {
			switch (t) {
			case GtOp:
				left_val.i = left_val.i > right_val.i;
				break;
			case LtOp:
				left_val.i = left_val.i < right_val.i;
				break;
			case LteOp:
				left_val.i = left_val.i <= right_val.i;
				break;
			case GteOp:
				left_val.i = left_val.i >= right_val.i;
				break;
			case Equality:
				left_val.i = left_val.i == right_val.i;
				break;
			case NotEq:
				left_val.i = left_val.i != right_val.i;
				break;
			}
		}

		exp_typ = DT_Int;
		val = left_val;
	}
	else {
		exp_typ = left_type;
		val = left_val;
	}
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

void Parser::additiveExpression(DataType &exp_typ, DataValue &val)
{
	unsigned int line = tokenlist_[i].line_;

	//Declare local variable that will be used as parameters ("type" synthesized attributes) of the term function
	DataType left_type, right_type;
	//Declare local variable that will be used as parameters ("val" synthesized attributes) of the term function
	DataValue left_val, right_val;
	string temp; // variable to keep the name of the temporary variable
	// The BNF of expr is converted to EBNF to avoid the left recursion
	term(left_type, left_val); //term() will return type and value of term
	// This loop is the check the types of the operands and evaluate the expression
	while (tokenlist_[i].token_ == PlusOp || tokenlist_[i].token_ == MinusOp) {
		bool isPlus = tokenlist_[i].token_ == PlusOp;
		addOp();
		term(right_type, right_val);
		// Check left and right operands types
		if (right_type != left_type)
			semanticError("Operand are not the same type", line);
		// Compute the left and right operands and put the results in the variable left_val used to accumulate the results
		if (left_type == DT_Float) {
			if (isPlus)
				left_val.f = left_val.f + right_val.f;
			else
				left_val.f = left_val.f - right_val.f;
		}
		else {
			if (isPlus)
				left_val.i = left_val.i + right_val.i;
			else
				left_val.i = left_val.i - right_val.i;
		}
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

void Parser::term(DataType &exp_typ, DataValue &val)
{
	unsigned int line = tokenlist_[i].line_;

	//Declare local variable that will be used as parameters ("type" synthesized attributes) of the term function
	DataType left_type, right_type;
	//Declare local variable that will be used as parameters ("val" synthesized attributes) of the term function
	DataValue left_val, right_val;
	string temp; // variable to keep the name of the temporary variable
	// The BNF of expr is converted to EBNF to avoid the left recursion
	factor(left_type, left_val); //term() will return type and value of term
	// This loop is the check the types of the operands and evaluate the expression
	while (tokenlist_[i].token_ == MultiplyOp || tokenlist_[i].token_ == DivideOp) {
		bool isMul = tokenlist_[i].token_ == MultiplyOp;
		mulOp();
		factor(right_type, right_val);
		// Check left and right operands types
		if (right_type != left_type)
			semanticError("Operand are not the same type", line);
		// Compute the left and right operands and put the results in the variable left_val used to accumulate the results
		if (left_type == DT_Float) {
			if (isMul)
				left_val.f = left_val.f * right_val.f;
			else
				left_val.f = left_val.f / right_val.f;
		}
		else {
			if (isMul)
				left_val.f = left_val.i * right_val.i;
			else
				left_val.f = left_val.i / right_val.i;
		}
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

void Parser::factor(DataType &exp_typ, DataValue &val)
{
	if (tokenlist_[i].token_ == StartParenthesis)
	{
		match(StartParenthesis);
		// factor.val = expression.val
		expression(exp_typ, val);
		match(EndParenthesis);
	}
	else if (tokenlist_[i].token_ == Float)
	{
		exp_typ = DT_Float;
		val.f = *(double *)tokenlist_[i].value_;
		match(Float);
	}
	else if (tokenlist_[i].token_ == Int)
	{
		exp_typ = DT_Int;
		val.i = *(long int *)tokenlist_[i].value_;
		match(Int);
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
	else if (tokenlist_[i].token_ == Float)
	{
		match(Float);
	}
	else if (tokenlist_[i].token_ == Int)
	{
		match(Int);
	}
	else if (tokenlist_[i].token_ == Identifier)
	{
		var();
	}
	else
		error("expression starts with ( or NUM or ID");
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

void Parser::addOp()
{
	if (tokenlist_[i].token_ == PlusOp)
		match(PlusOp);
	else if (tokenlist_[i].token_ == MinusOp)
		match(MinusOp);
	else
		error("\'+\' or \'-\'");
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