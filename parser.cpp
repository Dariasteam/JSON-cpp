#include "./parser.hpp"

pair <AbstractObject*, int> Parser::parseFile (string fileName) {
	Parser& aux = getInstance();
	if (aux.openFile(fileName)) {
		stringstream buffer;
		buffer << aux.getFile ().rdbuf();
		string a = buffer.str();
		return pair <AbstractObject*, int> (aux.parse (a).element, aux.errors);
	}else{
		return pair<AbstractObject*, int> (nullptr, CANT_OPEN_FILE);
	}
}

Parser::Parser () :
	keyDef ("^(?:\\s*)(?:(?:\")(\\w+)(?:\")(?:\\s*:))"),

	betweenBraces ("(?:(?:\")(\\w+)(?:\")(?:\\s*:\\s*))?(?:\\{)((?:\\w|\\s|\"|'|:|,|\\.|\\(|\\)|\\{|\\}|\\[|\\])*)(?:\\})"),
	startBrace ("^(?:\\s*)(?:(?:\")(\\w+)(?:\")(?:\\s*:\\s*))(\\{)"),
	startBracket ("^(?:\\s*)(?:(?:\")(\\w+)(?:\")(?:\\s*:\\s*))(\\[)"),
	finalQuote ("^(?:\\s*)(?:(?:\")(\\w+)(?:\")(?:\\s*:\\s*))(?:(?:\")(\\w+)(?:\"))(,)?"),
	finalBoolean ("^(?:\\s*)(?:(?:\")(\\w+)(?:\")(?:\\s*:\\s*))(true|false)(,)?"),
	finalNumber ("^(?:\\s*)(?:(?:\")(\\w+)(?:\")(?:\\s*:\\s*))(\\d+)(,)?"),

	nextBrace("^(?:\\s*)(\\})(,)?"),
	nextBracket("^(?:\\s*)(?:\\])(,)?")
	{}

Parser& Parser::getInstance () {
	static Parser instance;
	return instance;
}

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

ObjectNameFlags Parser::parseFinalQuote (string& content, smatch& matcher) {
	cout << "Encuentro una string" << endl;
	content = content.substr(matcher[0].length(), content.size());
	return {new ObjectFinalString (matcher[2]), matcher[1], hasComma(matcher[3])};
}

ObjectNameFlags Parser::parseFinalBoolean (string& content, smatch& matcher) {
	cout << "Encuentro un booleano" << endl;
	content = content.substr(matcher[0].length(), content.size());
	double value;
	if (matcher[2] == "true")
		value = 1;
	else
		value = 0;
	return {new ObjectFinalNumber (value), matcher[1], hasComma(matcher[3])};
}

ObjectNameFlags Parser::parseFinalNumber (string& content, smatch& matcher) {
	cout << "Encuentro un número" << endl;
	content = content.substr(matcher[0].length(), content.size());
	double value = stod(matcher[2]);
	return {new ObjectFinalNumber (value), matcher[1], hasComma(matcher[3])};
}

ObjectNameFlags Parser::parseBrace (string& content, smatch& matcher) {
	content = content.substr(matcher[0].length(), content.size());
	cout << "Encuentro un hash " << content << endl;
	ObjectMap* hash = new ObjectMap ();
	ObjectNameFlags aux;
	int flag;
	do {
		cout << "Exploro contenido del hash" << endl;
		aux = parse (content);
		hash->insert (aux.key, aux.element);
		cout << "Last element flag: " << aux.flags << endl;

	} while (aux.flags == REGULAR_ELEMENT);
	if (hash->size() > 1 && aux.flags != LAST_ELEMENT)
		cout << "Se esperan más elementos pero no" << endl;

	if (regex_search (content, matcher, nextBrace)) {
		content = content.substr(matcher[0].length(), content.size());
		flag = hasComma(matcher[2]);
		cout << "Fin del hash con flag " << flag << endl;
	} else {
		flag = NO_CLOSED;
		cout << "ERROR NO CLOSED " << endl;
	}
	return {hash, matcher[1], flag};
}

ObjectNameFlags Parser::parseBracket (string& content, smatch& matcher) {
	content = content.substr(matcher[0].length(), content.size());
	cout << "Encuentro un hash " << content << endl;
	ObjectVector* array = new ObjectVector ();
	ObjectNameFlags aux;
	int flag;
	do {
		cout << "Exploro contenido del hash" << endl;
		aux = parse (content);
		array->insert (aux.element);
		cout << "Last element flag: " << aux.flags << endl;

	} while (aux.flags == REGULAR_ELEMENT);
	if (array->size() > 1 && aux.flags != LAST_ELEMENT)
		cout << "Se esperan más elementos pero no" << endl;

	if (regex_search (content, matcher, nextBracket)) {
		content = content.substr(matcher[0].length(), content.size());
		flag = hasComma(matcher[2]);
		cout << "Fin del hash con flag " << flag << endl;
	} else {
		flag = NO_CLOSED;
		cout << "ERROR NO CLOSED " << endl;
	}
	return {array, matcher[1], flag};
}

ObjectNameFlags Parser::parse (string& content) {
	smatch matcher;
	if (regex_search (content, matcher, keyDef))
		cout << "a" << endl;
	else if (regex_search(content, matcher, finalQuote))
		return parseFinalQuote (content, matcher);
	else if (regex_search(content, matcher, finalBoolean))
		return parseFinalBoolean (content, matcher);
	else if (regex_search(content, matcher, finalNumber))
		return parseFinalNumber (content, matcher);
	else if (regex_search (content, matcher, startBrace))
		return parseBrace (content, matcher);
	else if (regex_search (content, matcher, startBracket))
		return parseBracket (content, matcher);
	return {nullptr, "", EMPTY};
}
