#ifndef PARSER_H
#define PARSER_H

#include <regex>
#include <fstream>
#include <string>
#include <iostream>
#include <algorithm>
#include <cctype> // isDigit for String c++11

#include "./object.hpp"
#include "manager.hpp"

#define OK 0
#define CANT_OPEN_FILE -1
#define FILE_BAD_FORMAT -2

using namespace std;

enum Flags {
	LAST_ELEMENT,
	REGULAR_ELEMENT,
	NO_CLOSED,
	EMPTY
};

struct ObjectNameFlags {
	AbstractObject* element;
	string key;
	int flags;
};

class Parser {
private:
	ifstream file;

	JsonTree tree;

	regex startBrace;
	regex startBracket;

	regex keyDef;
	regex finalQuote;
	regex finalNumber;
	regex finalBoolean;

	regex nextBrace;
	regex nextBracket;

	int errors;
	bool openFile (string fileName);
	bool hasComma (string buffer);

	ObjectNameFlags parse (string& content);

	ObjectNameFlags parseFinal (string& content, smatch& matcher, ObjectFinal* obj);
	ObjectNameFlags parseKeyDef (string& content, smatch& matcher);
	ObjectNameFlags parseContainer (string& content, smatch& matcher, regex& rgx, ObjectContainer* obj);


	inline bool hasErrors () { return errors > 0; }
	inline ifstream& getFile () { return file; }
	bool isDigits(const string); // isDigit for String c++11
public:
	Parser ();
	JsonTree getTree () { return tree; }
	bool parseFile (string fileName);
};

#endif
