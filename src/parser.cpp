#include "./parser.hpp"

using namespace json;

int Parser::parseFile (const std::string fileName, JsonTree& tree, bool verbs) {
  errors.resize (0);
  warnings.resize (0);
  //verbose = verbs;
  int returnValue = OK;
  if (openFile(fileName)) {
    std::stringstream buffer;
    buffer << getFile().rdbuf();
    content = buffer.str();
    std::string empty ("");
    ObjectNameFlag result = parseExpectingElement (empty);
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
  warnings (0)
  {}

bool Parser::openFile (std::string fileName) {
  file.open(fileName, std::ifstream::in);
  if (file.is_open())
    return true;
  else
    return false;
}

bool Parser::hasComma () {
  content.erase(remove_if(content.begin(), content.end(), isspace), content.end());
  if(content[0] == ',') {
    content.erase(0, 1);
    return true;
  } else {
    return false;
  }
}

Parser::ObjectNameFlag Parser::parseVector (std::string path)
{
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
    content.erase(remove_if(content.begin(), content.end(), isspace), content.end());
  } while (content[0] != ']' && aux.flag == REGULAR_ELEMENT);
  if (aux.flag == REGULAR_ELEMENT) {
    flag = EXPECTED_MORE;
    evaluateFlag(flag, path, aux.key);
  } else if (content[0] == ']') { 													// has matched
    content.erase(0, 1);
    flag = hasComma();
  } else {
    flag = NO_CLOSED;
    evaluateFlag(flag, path, aux.key);
  }
  return {obj, "", flag};
}

Parser::ObjectNameFlag Parser::parseMap (std::string& path)
{
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

    content.erase(remove_if(content.begin(), content.end(), isspace), content.end());
  } while (content[0] != '}' && aux.flag == REGULAR_ELEMENT);
  if (aux.flag == REGULAR_ELEMENT) {
    flag = EXPECTED_MORE;
    evaluateFlag(flag, path, aux.key);
  } else if (content[0] == '}') {
    content.erase(0, 1);
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
  content.erase(remove_if(content.begin(), content.end(), isspace), content.end());
  if (content[0] == '\"') {
    content.erase(0, 1);
    int i = 0;
    std::string key;
    while (content[i] != '\"') {
      key.push_back(content[i]);
      i++;
    }
    while (content[i] != ':') {
      i++;
    }
    content.erase(0, i + 1);
    std::string newPath (path + "." + key);
    ObjectNameFlag aux = parseExpectingElement (newPath);
    return { aux.element, key, aux.flag };
  } else {
    return { nullptr, "", EMPTY };
  }
}

Parser::ObjectNameFlag Parser::parseQuote() {
  content.erase(0, 1);
  int i = 0;
  std::string buff;
  while (content[i] != '\"') {
    buff.push_back(content[i]);
    i++;
  }
  content.erase(0, i + 1);
  return {new ObjectFinalString (buff), "", hasComma()};
}

Parser::ObjectNameFlag Parser::parseBool() {
  if (content.substr(0, 4) == "true") {
    content.erase(0, 4);
    return {new ObjectFinalBool (true), "", hasComma()};
  } else if (content.substr(0, 5) == "false") {
    content.erase(0, 5);
    return {new ObjectFinalBool (false), "", hasComma()};
  } else {
    return {nullptr, "", EXPECTED_KEY};
  }
}

Parser::ObjectNameFlag Parser::parseNumber() {
  int i = 0;
  std::string buff;
  bool end = false;  
  while (!end && i < content.size()) {
    switch (content[i]) {
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
        buff.push_back(content[i]);
        i++;
        break;
    }    
  }
  if (i > 0) {
    double number;
    try {
      number = stod (buff);
    }
    catch (std::invalid_argument e) {
      return {nullptr, "", INVALID_KEY};
    }
    content.erase(0, i);
    return {new ObjectFinalNumberFloat (number), "", hasComma()};
  } else {
    return {nullptr, "", EMPTY};
  }
}

Parser::ObjectNameFlag Parser::parseExpectingElement (std::string& path) {
  content.erase(remove_if(content.begin(), content.end(), isspace), content.end());
  switch (content[0]) {
    case '{':
      content.erase(0, 1);
      content.erase(remove_if(content.begin(), content.end(), isspace), content.end());
      return parseMap (path);
    case '[':
      content.erase(0, 1);
      content.erase(remove_if(content.begin(), content.end(), isspace), content.end());
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
