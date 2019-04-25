#include "SymbolTable.h"
#include <iostream>

void SymbolTable::Add(std::string name, DataType type, unsigned int line) {
	entries_[name] = SymbolTableValue();
	SymbolTableValue &s = entries_[name];
	s.type = type;
	if (type == DT_Float)
		s.value.f = 0.0;
	else
		s.value.i = 0;
	s.line = line;
}

void SymbolTable::Print(std::ostream &myfile) {
	for (SymbolTableEntry::iterator it = entries_.begin(); it != entries_.end(); ++it) {
		SymbolTableValue &s = it->second;
		OUTPUT(it->first << " (" << s.line);
		if (s.type == DT_Float) {
			OUTPUT(") [float]: " << s.value.f << std::endl);
		}
		else {
			OUTPUT(") [ int ]: " << s.value.i << std::endl);
		}
	}
}

bool SymbolTable::CheckExists(std::string name) {
	return (entries_.find(name) != entries_.end());
}

bool SymbolTable::SetFloat(std::string name, double f) {
	if (CheckExists(name) && entries_[name].type == DT_Float) {
		entries_[name].used = true;
		entries_[name].value.f = f;

		return true;
	}

	return false;
}

bool SymbolTable::SetInt(std::string name, long int i) {
	if (CheckExists(name) && entries_[name].type == DT_Int) {
		entries_[name].used = true;
		entries_[name].value.i = i;

		return true;
	}

	return false;
}

bool SymbolTable::GetFloat(std::string name, double &f) {
	if (CheckExists(name) && entries_[name].type == DT_Float) {
		f = entries_[name].value.f;

		return true;
	}

	return false;
}

bool SymbolTable::GetInt(std::string name, long int &i) {
	if (CheckExists(name) && entries_[name].type == DT_Int) {
		i = entries_[name].value.i;

		return true;
	}

	return false;
}

bool SymbolTable::WasUsed(std::string name, bool &used) {
	if (CheckExists(name)) {
		used = entries_[name].used;

		return true;
	}

	return false;
}

bool SymbolTable::GetType(std::string name, DataType &type) {
	if (CheckExists(name)) {
		type = entries_[name].type;
		return true;
	}

	return false;
}

bool SymbolTable::GetLine(std::string name, unsigned int &line) {
	if (CheckExists(name)) {
		line = entries_[name].line;
		return true;
	}
		
	return false;
}

bool SymbolTable::GetEntry(std::string name, SymbolTableValue &entry) {
	if (CheckExists(name)) {
		entry = entries_[name];
		return true;
	}

	return false;
}

bool SymbolTable::Remove(std::string name) {
	if (CheckExists(name)) {
		entries_.erase(name);
		return true;
	}

	return false;
}