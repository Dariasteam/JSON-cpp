#include "./parser.hpp"

bool Parser::parseFile (string fileName) {
	if (openFile(fileName)) {
		stringstream buffer;
		buffer << getFile ().rdbuf();
		string a = buffer.str();
		ObjectNameFlags result = parse (a);
		tree = JsonTree (result.element);
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
	finalQuote ("^(?:\\s*)(?:(?:\")((\\w|\\s)+)(?:\"))(,)?"),
	finalBoolean ("^(?:\\s*)(true|false)(?:\\s*)(,)?"),
	finalNumber ("^(?:\\s*)(\\d+)(?:\\s*)(,)?"),
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

ObjectNameFlags Parser::parseFinalQuote (string& content, smatch& matcher) {
	cout << "Encuentro una string" << endl;
	content = content.substr(matcher[0].length(), content.size());
	return {new ObjectFinalString (matcher[1]), "", hasComma(matcher[2])};
}

ObjectNameFlags Parser::parseFinalBoolean (string& content, smatch& matcher) {
	cout << "Encuentro un booleano" << endl;
	content = content.substr(matcher[0].length(), content.size());
	double value;
	if (matcher[1] == "true")
		value = 1;
	else
		value = 0;
	return {new ObjectFinalNumber (value), "", hasComma(matcher[2])};
}

ObjectNameFlags Parser::parseFinalNumber (string& content, smatch& matcher) {
	cout << "Encuentro un número" << matcher[1] << endl;
	content = content.substr(matcher[0].length(), content.size());
	double value = stod(matcher[1]);
	return {new ObjectFinalNumber (value), "", hasComma(matcher[2])};
}

ObjectNameFlags Parser::parseBrace (string& content, smatch& matcher) {
	content = content.substr(matcher[0].length(), content.size());
	cout << "Encuentro un hash " << endl;
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
	cout << "Encuentro un vector " << endl;
	ObjectVector* array = new ObjectVector ();
	ObjectNameFlags aux;
	int flag;
	do {
		cout << "Exploro contenido del vector" << endl;
		aux = parse (content);
		array->insert (aux.element);
		cout << "Last element flag: " << aux.flags << endl;
	} while (aux.flags == REGULAR_ELEMENT);
	if (array->size() > 1 && aux.flags != LAST_ELEMENT)
		cout << "Se esperan más elementos pero no" << endl;

	if (regex_search (content, matcher, nextBracket)) {
		content = content.substr(matcher[0].length(), content.size());
		flag = hasComma(matcher[2]);
		cout << "Fin del vector con flag " << flag << matcher[2]<< endl;
	} else {
		flag = NO_CLOSED;
		cout << "ERROR NO CLOSED " << endl;
	}
	return {array, matcher[1], flag};
}

ObjectNameFlags Parser::parseKeyDef (string& content, smatch& matcher) {
	cout << "Encuentro una clave " << matcher[1] << endl;
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
