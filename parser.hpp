#ifndef PARSER_H
#define PARSER_H

#include <regex>
#include <fstream>
#include <string>
#include <iostream>

#include "./object.hpp"

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
	//Atributes
	regex startBrace;
	regex startBracket;

	regex keyDef;
	regex finalQuote;
	regex finalNumber;
	regex finalBoolean;

	regex nextBrace;
	regex nextBracket;



	int errors;

	// Methods
	Parser ();
	bool openFile (string fileName);
	bool hasComma (string buffer);
	static Parser& getInstance ();
	ObjectNameFlags parse (string& content);

	ObjectNameFlags parseKeyDef (string& content, smatch& matcher);

	ObjectNameFlags parseFinalQuote (string& content, smatch& matcher);
	ObjectNameFlags parseFinalBoolean (string& content, smatch& matcher);
	ObjectNameFlags parseFinalNumber (string& content, smatch& matcher);

	ObjectNameFlags parseBrace (string& content, smatch& matcher);
	ObjectNameFlags parseBracket (string& content, smatch& matcher);

	// Getters / Setters
	inline ifstream& getFile () { return file; }
public:
	static pair<AbstractObject*, int> parseFile (string fileName);

};

#endif
