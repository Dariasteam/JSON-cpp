#ifndef PARSER_H
#define PARSER_H

#include <regex>
#include <fstream>
#include <string>
#include <iostream>

#include "./object.hpp"
#include "manager.hpp"

using namespace std;

// do not move these elements, their numeric value is used for comparisons
enum JSON_PARSER_FLAG {
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

enum JSON_PARSE_OUTPUT {
	CANT_OPEN_FILE,
	OK,							  // value = 1, allows "if (parser.parseFile ("a.json"))"
	// FILE_BAD_FORMAT,
	WARNINGS,
	ERRORS,
	BOTH_ERR_WARN
};

struct ObjectNameFlag {
	AbstractObject* element;
	string key;
	int flag;
};

struct JsonLog {
	string path;
	int flag;
};

class Parser {
	const string reverseflag [7] = {"LAST_ELEMENT",
	 													"REGULAR_ELEMENT",
														"NO_CLOSED",
														"EXPECTED_MORE",
														"INVALID_KEY",
														"-",
														"EMPTY"
														};
private:
	vector <JsonLog> errors;
	vector <JsonLog> warnings;
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
	void evaluateFlag (int flag, string path, string finalElement);

	ObjectNameFlag parse (string& content, string path);
	ObjectNameFlag parseKeyDef (string& content, smatch& matcher, string path);
	ObjectNameFlag parseFinal (string& content, smatch& matcher, ObjectFinal* obj);
	ObjectNameFlag parseContainer (string& content, smatch& matcher, regex& endSymbol, ObjectContainer* obj, string path);

	inline bool hasErrors () { return errors.size() > 0; }
	inline bool hasWarnings () { return warnings.size() > 0; }
	inline ifstream& getFile () { return file; }
public:
	Parser ();
	JsonTree getTree () { return tree; }
	int parseFile (string fileName);
	static int saveFile (string fileName, JsonTree& info);
	inline const vector<JsonLog>& getErrors () { return errors; }
	inline const vector<JsonLog>& getWarnings () { return warnings; }
};

#endif
