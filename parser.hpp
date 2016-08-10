#ifndef PARSER_H
#define PARSER_H

#include <regex>
#include <fstream>
#include <string>
#include <iostream>

#include "./object.hpp"
#include "manager.hpp"

#define OK 0
#define CANT_OPEN_FILE -1
#define FILE_BAD_FORMAT -2

using namespace std;

// do not move these elements, their numeric value is used for comparisons
enum Flags {
	LAST_ELEMENT,
	REGULAR_ELEMENT,
	// errors
	NO_CLOSED,				// there is no end brace / bracket after the last element
	EXPECTED_MORE,		// there is an end brace / bracket after non last element
	INVALID_KEY,
	// warnings
	CONTROL_WARNING,
	EMPTY
};

struct ObjectNameFlags {
	AbstractObject* element;
	string key;
	int flags;
};

struct JsonError {
	string path;
	int type;
};

class Parser {
	string reverseFlags [7] = {"LAST_ELEMENT",
	 													"REGULAR_ELEMENT",
														"NO_CLOSED_AFTER",
														"EXPECTED_MORE",
														"INVALID_KEY",
														"-",
														"EMPTY"
														};
private:
	vector <JsonError> errors;
	ifstream file;
	JsonTree tree;

	static regex startBrace;
	static regex startBracket;
	static regex keyDef;
	static regex finalQuote;
	static regex finalNumber;
	static regex finalBoolean;
	static regex nextBrace;
	static regex nextBracket;

	bool openFile (string fileName);
	bool hasComma (string buffer);
	void evaluateFlag (int flag, string path);

	ObjectNameFlags parse (string& content, string path);
	ObjectNameFlags parseKeyDef (string& content, smatch& matcher, string path);
	ObjectNameFlags parseFinal (string& content, smatch& matcher, ObjectFinal* obj);
	ObjectNameFlags parseContainer (string& content, smatch& matcher, regex& rgx, ObjectContainer* obj, string path);

	inline bool hasErrors () { return errors.size() > 0; }
	inline ifstream& getFile () { return file; }
public:
	Parser ();
	JsonTree getTree () { return tree; }
	bool parseFile (string fileName);
	const vector<JsonError>& getErrors () { return errors; }
};

#endif
