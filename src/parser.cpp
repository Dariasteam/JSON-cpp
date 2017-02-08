#include "./parser.hpp"

using namespace json;

int Parser::parseFile (const std::string fileName, JsonTree& tree, bool verbs) {
  errors.resize (0);
  warnings.resize (0);
  //verbose = verbs;
  int returnValue = OK;
  parseIndex = 0;
  if (openFile(fileName)) {
    std::stringstream buffer;
    buffer << getFile().rdbuf();
    content = buffer.str();
    std::string empty ("");
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    ObjectNameFlag result = parseExpectingElement (empty);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    file.close();
    tree.setTop (result.element);
    if (result.flag == EMPTY)
      return EMPTY_FILE;
    if (hasWarnings())
      returnValue += WARNINGS;
    if (hasErrors()) {
      returnValue += ERRORS;
      return returnValue & (INT_MAX - 1); // removes the OK flag
    }
  } else {
    return CANT_OPEN_FILE;
  }
  return returnValue;
}

int Parser::parseString (std::string cnt, JsonTree& tree, bool verbs ) {
  verbose = verbs;
  content = cnt;
  errors.resize (0);
  warnings.resize (0);
  std::string empty ("");
  int returnValue = OK;
  ObjectNameFlag result = parseExpectingElement (empty);
  tree.setTop (result.element);
  if (result.flag == EMPTY)
     return EMPTY_FILE;
  if (hasWarnings())
    returnValue += WARNINGS;
  if (hasErrors()) {
     returnValue += ERRORS;
    return returnValue & (INT_MAX - 1); // removes the OK flag
  }
  return returnValue;
}

Parser::Parser () :
  errors (0),
  warnings (0),
  parseIndex (0)
  {}

bool Parser::openFile (std::string fileName) {
  file.open(fileName, std::ifstream::in);
  if (file.is_open())
    return true;
  else
    return false;
}

bool Parser::hasComma () {
  removeFirstBlanks();
  if(content[parseIndex] == ',') {
    parseIndex++;
    return true;
  } else {
    return false;
  }
}

Parser::ObjectNameFlag Parser::parseVector (std::string path) {
  ObjectVector* obj = new ObjectVector ();
  ObjectNameFlag aux;
  int flag = REGULAR_ELEMENT;
  int i = 0;
  do {    
    std::string newPath (path + "." + std::to_string(i));
    aux = parseExpectingElement (newPath);
    if (aux.flag == EMPTY) {
      evaluateFlag(EMPTY, path, aux.key);
      break;
    }
    if (!obj->insert (aux.key, aux.element)) {
      evaluateFlag(INVALID_KEY, path, aux.key);
      break;
    }
    i++;
    removeFirstBlanks();
  } while (content[parseIndex] != ']' && aux.flag == REGULAR_ELEMENT);
  if (aux.flag == REGULAR_ELEMENT) {
    flag = EXPECTED_MORE;
    evaluateFlag(flag, path, aux.key);
  } else if (content[parseIndex] == ']') { 													// has matched
    parseIndex++;
    flag = hasComma();
  } else {
    flag = NO_CLOSED;
    evaluateFlag(flag, path, aux.key);
  }
  return {obj, "", flag};
}

Parser::ObjectNameFlag Parser::parseMap (std::string& path) {
  ObjectMap* obj = new ObjectMap ();
  int flag = REGULAR_ELEMENT;
  ObjectNameFlag aux;

  do {
    aux = parseExpectingKeyDef (path);

    if (aux.flag == EMPTY) {
      evaluateFlag(EMPTY, path, aux.key);
      break;
    }


    if (!obj->insert (aux.key, aux.element)) {
      evaluateFlag(INVALID_KEY, path, aux.key);
      break;
    }

    removeFirstBlanks();
  } while (content[parseIndex] != '}' && aux.flag == REGULAR_ELEMENT);
  if (aux.flag == REGULAR_ELEMENT) {
    flag = EXPECTED_MORE;
    evaluateFlag(flag, path, aux.key);
  } else if (content[parseIndex] == '}') {
    parseIndex++;
    flag = hasComma();
  } else {
    flag = NO_CLOSED;
    evaluateFlag(flag, path, aux.key);
  }
  return {obj, "", flag};
}

void Parser::evaluateFlag (int flag, std::string path, std::string finalElement) {
  path.append("." + finalElement);
  if (flag < CONTROL_WARNING) {
    errors.push_back ({path, flag});
    if (verbose)
      std::cerr << "Error";
  } else {
    warnings.push_back ({path, flag});
    if (verbose)
      std::cerr << "Warning";
  }
  if (verbose)
    std::cerr << " parsing JSON: " << reverseflag[flag] << " in path: " << path << std::endl;
}

Parser::ObjectNameFlag Parser::parseExpectingKeyDef (std::string& path) {
  removeFirstBlanks();
  if (content[parseIndex] == '\"') {
    parseIndex++;
    std::string key;
    while (content[parseIndex] != '\"') {
      key.push_back(content[parseIndex]);
      parseIndex++;
    }
    while (content[parseIndex] != ':') {
      parseIndex++;
    }
    parseIndex++;
    std::string newPath (path + "." + key);
    ObjectNameFlag aux = parseExpectingElement (newPath);
    return { aux.element, key, aux.flag };
  } else {
    return { nullptr, "", EMPTY };
  }
}

Parser::ObjectNameFlag Parser::parseQuote() {
  parseIndex++;
  std::string buff;
  while (content[parseIndex] != '\"') {
    buff.push_back(content[parseIndex]);
    parseIndex++;
  }
  parseIndex++;
  return {new ObjectFinalString (buff), "", hasComma()};
}

Parser::ObjectNameFlag Parser::parseBool() {
  if (content.substr(parseIndex, 4) == "true") {
    parseIndex += 4;
    return {new ObjectFinalBool (true), "", hasComma()};
  } else if (content.substr(parseIndex, 5) == "false") {
    parseIndex += 5;
    return {new ObjectFinalBool (false), "", hasComma()};
  } else {
    return {nullptr, "", EXPECTED_KEY};
  }
}

Parser::ObjectNameFlag Parser::parseNumber() {  
  std::string buff;
  bool end = false;  
  while (!end && parseIndex < content.size()) {
    switch (content[parseIndex]) {
      case ' ':
        end = true;
        break;
      case '}':
        end = true;
        break;
      case ']':
        end = true;
        break;
      case ',':
        end = true;
        break;
      default:
        buff.push_back(content[parseIndex]);
        parseIndex++;
        break;
    }    
  }
  if (!buff.empty()) {
    double number;
    try {
      number = stod (buff);
    }
    catch (std::invalid_argument e) {
      return {nullptr, "", INVALID_KEY};
    }    
    return {new ObjectFinalNumberFloat (number), "", hasComma()};
  } else {
    return {nullptr, "", EMPTY};
  }
}

void Parser::removeFirstBlanks() {
  int max = content.size();
  while(1 && parseIndex < max) {
    if (!(content[parseIndex] == ' ' || content[parseIndex] == '\t'|| content[parseIndex] == '\n' ))
      break;
    parseIndex++;
  }
}

Parser::ObjectNameFlag Parser::parseExpectingElement (std::string& path) {
  removeFirstBlanks();
  switch (content[parseIndex]) {
    case '{':
      parseIndex++;
      removeFirstBlanks();
      return parseMap (path);
    case '[':
      parseIndex++;
      removeFirstBlanks();
      return parseVector (path);
    case '\"':
      return parseQuote();
    case 't':
      return parseBool();
    case 'f':
      return parseBool();
  }
  return parseNumber();
}

int Parser::saveFile (std::string fileName, JsonTree& tree, bool uglify) {
  std::ofstream file;
  file.open (fileName);
  if (!file.is_open())
    return CANT_OPEN_FILE;
  file << tree.toText(uglify);
  file.close ();
  return OK;
}
