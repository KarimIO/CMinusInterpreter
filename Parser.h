#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <string>
#include "Scanner.h"
#include "SymbolTable.h"

using namespace std;

typedef union { double f; long int i; } DataValue;

class Parser
{
private:
	unsigned int num_errors_;
	TokenList tokenlist_;
	ofstream myfile;
	int i = 0;
	int line = 0;
	//Scanner scanner();
	void initialize(string);
	void match(Token);
	void error(Token);
	void error(string);
	void semanticError(string errorMsg, unsigned int line);
	//void semanticError(string errorMsg);
	void parse();

	void program(); // grammer rule no. 1
	void declarationList();
	void declaration();
	void varDeclaration();
	DataType typeSpecifier();
	void readVar(bool exec);
	void readStmt(bool exec);
	void writeVar(bool exec);
	void writeStmt(bool exec);
	void compoundStmt(bool exec);
	/*void funDeclaration();
	void params();
	void paramList();
	void param();
	void compoundStmt();
	void localDeclaration();*/
	void stmtList(bool exec);
	void stmt(bool exec);
	void expressionStmt(bool exec);
	void selectionStmt(bool exec);
	void iterationStmt(bool exec);

	bool evaluateExpression(DataType exp_typ, DataValue val);

	void expression(DataType &exp_typ, DataValue & val);
	void expression();

	void var(DataType &exp_typ, DataValue & val);
	void var();

	void simpleExpression(DataType &exp_typ, DataValue & val);
	void simpleExpression();

	void additiveExpression(DataType &exp_typ, DataValue & val);
	void additiveExpression();

	void term(DataType &exp_typ, DataValue & val);
	void term();

	void factor(DataType &exp_typ, DataValue & val);
	void factor();

	void mulOp();
	void addOp();
	void relop();

private:
	SymbolTable symboltable_;
public:
	string output_filepath;
	void run_program(string);
};

#endif /* PARSER_H */