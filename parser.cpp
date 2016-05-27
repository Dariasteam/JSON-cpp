#include "./parser.hpp"


regex Parser::startBrace = regex ("^(?:\\s*)(\\{)");
regex Parser::startBracket = regex ("^(?:\\s*)(\\[)");
regex Parser::keyDef = regex ("^(?:\\s*)(?:(?:\")(\\w+)(?:\")(?:\\s*:))");
regex Parser::finalQuote = regex ("^(?:\\s*)(?:\")((?:\\w|\\s|\\d)+)(?:\")(?:\\s*)(,)?");
regex Parser::finalNumber = regex ("^(?:\\s*)((?:\\+|\\-)?\\d+(?:\\.\\d+)?(?:e(?:\\+|\\-)?\\d+)?)(?:\\s*)(,)?");
regex Parser::finalBoolean = regex ("^(?:\\s*)(true|false)(?:\\s*)(,)?");
regex Parser::nextBrace = regex ("^(?:\\s*)(\\})(?:\\s*)(,)?");
regex Parser::nextBracket = regex ("^(?:\\s*)(\\])(?:\\s*)(,)?");



bool Parser::parseFile (string fileName) {
	if (openFile(fileName)) {
		stringstream buffer;
		buffer << getFile ().rdbuf();
		string fileContent = buffer.str();
		ObjectNameFlags result = parse (fileContent, "");
		tree = JsonTree (result.element);
		if (fileContent.size() > 0)
			evaluateFlag(NO_CLOSED, ".");
		return !hasErrors();
	} else {
		evaluateFlag(CANT_OPEN_FILE, "");
		return false;
	}
}

Parser::Parser () :
	errors (0),
	tree (nullptr)
	{}

bool Parser::openFile (string fileName) {
	ifstream& file = getFile();
	file.open(fileName);
	if (file.is_open())
		return true;
	else
		return false;
}

bool Parser::hasComma (string buffer) {
	return (buffer.size() > 0);
}

ObjectNameFlags Parser::parseFinal (string& content, smatch& matcher, ObjectFinal* obj) {
	content = content.substr(matcher[0].length(), content.size());
	obj->setValue(matcher[1]);
	return {obj, "", hasComma(matcher[2])};
}

ObjectNameFlags Parser::parseContainer
(string& content, smatch& matcher, regex& rgx, ObjectContainer* obj, string path)
{
	content = content.substr(matcher[0].length(), content.size());
	ObjectNameFlags aux;
	int flag;
	do {
		aux = parse (content, path);
		obj->insert (aux.key, aux.element);
	} while (aux.flags == REGULAR_ELEMENT && !regex_search (content, matcher, rgx));
	if (obj->size() > 1 && aux.flags != LAST_ELEMENT) {
		flag = EXPECTED_MORE;
		evaluateFlag(flag, path.append(".").append(aux.key));
	} else if (regex_search (content, matcher, rgx)) {
		content = content.substr(matcher[0].length(), content.size());
		flag = hasComma(matcher[2]);
	} else {
		flag = NO_CLOSED;
		evaluateFlag(flag, path.append(".").append(aux.key));
	}
	return {obj, "", flag};
}

void Parser::evaluateFlag (int flag, string path) {
	cerr << "Error parsing JSON: " << reverseFlags[flag] << " in path: " << path << endl;
	errors.push_back ({path, flag});
}

ObjectNameFlags Parser::parseKeyDef (string& content, smatch& matcher, string path) {
	string key = matcher[1];
	content = content.substr(matcher[0].length(), content.size());
	path.append(".").append(key);
	ObjectNameFlags aux = parse (content, path);
	return {aux.element, key, aux.flags};
}

ObjectNameFlags Parser::parse (string& content, string path) {
	smatch matcher;
	ObjectNameFlags Obj;
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
