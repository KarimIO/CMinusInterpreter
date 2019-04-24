#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <map>
#include <string>
#include "BaseCommon.h"

struct SymbolTableValue {
	DataType type;
	union {
		double f;
		long int i;
	} value;
	unsigned int line;
	bool used = false;
};

typedef std::map<std::string, SymbolTableValue> SymbolTableEntry;

class SymbolTable {
public:
public:
	void Add(std::string name, DataType type, unsigned int line);
	bool CheckExists(std::string name);
	bool SetFloat(std::string name, double f);
	bool SetInt(std::string name, long int i);
	bool GetFloat(std::string name, double &f);
	bool GetInt(std::string name, long int &i);
	bool GetType(std::string name, DataType &type);
	bool GetLine(std::string name, unsigned int &line);
	bool WasUsed(std::string name, bool &used);
	bool GetEntry(std::string name, SymbolTableValue &out);
	bool Remove(std::string name);

	void Print(std::ostream &o);
private:
	SymbolTableEntry entries_;
};

#endif