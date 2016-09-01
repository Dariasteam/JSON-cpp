#include "./parser.hpp"

regex Parser::startBrace = regex ("^(?:\\s*)(\\{)");
regex Parser::startBracket = regex ("^(?:\\s*)(\\[)");
regex Parser::keyDef = regex ("^(?:\\s*)(?:\")(.+?)(?:\")(?:\\s*)\\:");
regex Parser::finalQuote = regex ("^(?:\\s*)(?:\")(.*?)(?:\")(?:\\s*)(,)?");
regex Parser::finalNumber = regex ("^(?:\\s*)((?:\\+|\\-)?\\d+(?:\\.\\d+)?(?:e(?:\\+|\\-)?\\d+)?)(?:\\s*)(,)?");
regex Parser::finalBoolean = regex ("^(?:\\s*)(true|false)(?:\\s*)(,)?");
regex Parser::nextBrace = regex ("^(?:\\s*)(\\})(?:\\s*)(,)?");
regex Parser::nextBracket = regex ("^(?:\\s*)(\\])(?:\\s*)(,)?");


int Parser::parseFile (string fileName) {
	int returnValue = 0;
	if (openFile(fileName)) {
		stringstream buffer;
		buffer << getFile().rdbuf();
		string fileContent = buffer.str();
		ObjectNameFlag result = parse (fileContent, "");
		JsonTree* tree = new JsonTree (result.element);
		if (fileContent.size())
			evaluateFlag(NO_CLOSED, ".", "");
		if (hasErrors())
			returnValue = returnValue | ERRORS;
		if (hasWarnings())
			returnValue = returnValue | WARNINGS;
		return returnValue;
	} else {
		return CANT_OPEN_FILE;
	}
	return OK;
}

Parser::Parser () :
	errors (0),
	warnings (0),
	tree (nullptr)
	{}

bool Parser::openFile (string fileName) {
	file.open(fileName);
	if (file.is_open())
		return true;
	else
		return false;
}

bool Parser::hasComma (string buffer) {
	return (buffer.size() > 0);
}

ObjectNameFlag Parser::parseFinal (string& content, smatch& matcher, ObjectFinal* obj) {
	obj->replaceValue(matcher[1]);
	content = content.substr(matcher[0].length(), content.size());
	return {obj, "", hasComma(matcher[2])};
}

ObjectNameFlag Parser::parseContainer (string& content, smatch& matcher,
														regex& endSymbol, ObjectContainer* obj, string path)
{
	content = content.substr(matcher[0].length(), content.size());
	ObjectNameFlag aux;
	int flag = REGULAR_ELEMENT;
	do {
		aux = parse (content, path);
		if (aux.flag == EMPTY) {
			evaluateFlag(EMPTY, path, aux.key);
			break;
		}
		if (!obj->insert (aux.key, aux.element)) {
			evaluateFlag(INVALID_KEY, path, aux.key);
			break;
		}
	} while (!regex_search (content, matcher, endSymbol) && aux.flag == REGULAR_ELEMENT);
	if (aux.flag == REGULAR_ELEMENT) {
		flag = EXPECTED_MORE;
		evaluateFlag(flag, path, aux.key);
	} else if (matcher[0].length() > 0) { 													// has matched
		content = content.substr(matcher[0].length(), content.size());
		flag = hasComma(matcher[2]);
	} else {
		flag = NO_CLOSED;
		evaluateFlag(flag, path, aux.key);
	}
	return {obj, "", flag};
}

void Parser::evaluateFlag (int flag, string path, string finalElement) {
	path.append(".").append(finalElement);
	if (flag < CONTROL_WARNING) {
		cerr << "Error";
		errors.push_back ({path, flag});
	} else {
		cerr << "Warning";
		warnings.push_back ({path, flag});
	}
	cerr << " parsing JSON: " << reverseflag[flag] << " in path: " << path << endl;
}

ObjectNameFlag Parser::parseKeyDef (string& content, smatch& matcher, string path) {
	string key = matcher[1];
	content = content.substr(matcher[0].length(), content.size());
	path.append(".").append(key);
	ObjectNameFlag aux = parse (content, path);
	return {aux.element, key, aux.flag};
}

ObjectNameFlag Parser::parse (string& content, string path) {
	smatch matcher;
	ObjectNameFlag Obj;
	if (regex_search (content, matcher, keyDef))
		return parseKeyDef (content, matcher, path);
	else if (regex_search(content, matcher, finalQuote))
		return parseFinal (content, matcher, new ObjectFinalString());
	else if (regex_search(content, matcher, finalBoolean))
		return parseFinal (content, matcher, new ObjectFinalBool());
	else if (regex_search(content, matcher, finalNumber))
		return parseFinal (content, matcher, new ObjectFinalNumber());
	else if (regex_search (content, matcher, startBrace))
		return parseContainer (content, matcher, nextBrace, new ObjectMap (), path);
	else if (regex_search (content, matcher, startBracket))
		return parseContainer (content, matcher, nextBracket, new ObjectVector (), path);
	return {nullptr, "", EMPTY};
}

int Parser::saveFile (string fileName, JsonTree& info) {
	ofstream file;
	file.open (fileName);
	file << info.toText();
	file.close ();
	return 0;
}
