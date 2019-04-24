#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <string>
#include "Scanner.h"
#include "SymbolTable.h"

using namespace std;

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
	void semanticError(string errorMsg);
	//void semanticError(string errorMsg);
	void parse();

	void program(); // grammer rule no. 1
	void declarationList();
	void declaration();
	void varDeclaration();
	ST_Type typeSpecifier();
	void readVar();
	void readStmt();
	void writeVar();
	void writeStmt();
	/*void funDeclaration();
	void params();
	void paramList();
	void param();
	void compoundStmt();
	void localDeclaration();*/
	void stmtList();
	void stmt();
	//void expression(string & exp_typ, int & val);
	void expressionStmt();
	void selectionStmt();
	void iterationStmt();
	void expression(string & exp_typ, int & val);
	void var(string & exp_typ, int & val);
	void simpleExpression(string & exp_typ, int & val);
	void expression();
	void var();
	void simpleExpression();
	void relop();
	void additiveExpression(string & exp_typ, int & val);
	void additiveExpression();
	void addOp();
	void term(string & exp_typ, int & val);
	void term();
	void mulOp();
	void factor(string & exp_typ, int & val);
	void factor();
	/*void call();
	void args();
	void argList();*/

private:
	SymbolTable symboltable_;
public:
	string output_filepath;
	void run_program(string);
};

#endif /* PARSER_H */