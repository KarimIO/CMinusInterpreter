#include <iostream>
#include <string>
#include <fstream>

#include "Parser.h"

int main(int argc, const char *arv[]) {
	Parser parser;
	parser.output_filepath = "Output.txt";
	parser.run_program("Input.txt");

	system("pause");

	return 0;
}