#include "./parser.hpp"

bool Parser::parseFile (string fileName) {
	if (openFile(fileName)) {
		stringstream buffer;
		buffer << getFile ().rdbuf();
		string fileContent = buffer.str();
		ObjectNameFlags result = parse (fileContent);
		tree = JsonTree (result.element);
		if (result.flags != LAST_ELEMENT)
			errors++;
		return !hasErrors();
	} else {
		errors++;
		return false;
	}
}

Parser::Parser () :
	keyDef ("^(?:\\s*)(?:(?:\")(\\w+)(?:\")(?:\\s*:))"),
	startBrace ("^(?:\\s*)(\\{)"),
	startBracket ("^(?:\\s*)(\\[)"),
	finalQuote ("^(?:\\s*)(?:\")((?:\\w|\\s|\\d)+)(?:\")(?:\\s*)(,)?"),
	finalBoolean ("^(?:\\s*)(true|false)(?:\\s*)(,)?"),
	finalNumber ("^(?:\\s*)((?:\\+|\\-)?\\d+(?:\\.\\d+)?)(?:\\s*)(,)?"),
	nextBrace("^(?:\\s*)(\\})(?:\\s*)(,)?"),
	nextBracket("^(?:\\s*)(\\])(?:\\s*)(,)?"),
	errors (0),
	tree (nullptr)
	{}

bool Parser::openFile (string fileName) {
	ifstream& file = getFile();
	file.open(fileName);
	if (file.is_open()) {
		return true;
	}else{
		return false;
	}
}

bool Parser::hasComma (string buffer) {
	return (buffer.size() > 0);
}

ObjectNameFlags Parser::parseFinal (string& content, smatch& matcher, ObjectFinal* obj) {
	// cout << "Contenido" << content << endl;
	content = content.substr(matcher[0].length(), content.size());
	obj->setValue(matcher[1]);
	return {obj, "", hasComma(matcher[2])};
}

ObjectNameFlags Parser::parseContainer (string& content, smatch& matcher, regex& rgx, ObjectContainer* obj) {
	content = content.substr(matcher[0].length(), content.size());
	ObjectNameFlags aux;
	int flag;
	do {
		aux = parse (content);
		obj->insert (aux.key, aux.element);
	} while (aux.flags == REGULAR_ELEMENT);
	if (obj->size() > 1 && aux.flags != LAST_ELEMENT) {
		errors++;
		return {obj, "", EXPECTED_MORE};
	}
	if (regex_search (content, matcher, rgx)) {
		content = content.substr(matcher[0].length(), content.size());
		flag = hasComma(matcher[2]);
	} else {
		flag = NO_CLOSED;
		errors++;
	}
	return {obj, "", flag};
}

void Parser::evaluateFlag (int flag) {

}

ObjectNameFlags Parser::parseKeyDef (string& content, smatch& matcher) {
	string key = matcher[1];
	content = content.substr(matcher[0].length(), content.size());
	ObjectNameFlags aux = parse (content);
	return {aux.element, key, aux.flags};
}

ObjectNameFlags Parser::parse (string& content) {
	smatch matcher;
	ObjectNameFlags Obj;
	if (regex_search (content, matcher, keyDef))
		return parseKeyDef (content, matcher);
	else if (regex_search(content, matcher, finalQuote))
		return parseFinal (content, matcher, new ObjectFinalString());
	else if (regex_search(content, matcher, finalBoolean))
		return parseFinal (content, matcher, new ObjectFinalBool());
	else if (regex_search(content, matcher, finalNumber))
		return parseFinal (content, matcher, new ObjectFinalNumber());
	else if (regex_search (content, matcher, startBrace))
		return parseContainer (content, matcher, nextBrace, new ObjectMap ());
	else if (regex_search (content, matcher, startBracket))
		return parseContainer (content, matcher, nextBracket, new ObjectVector ());


	return {nullptr, "", EMPTY};
}
