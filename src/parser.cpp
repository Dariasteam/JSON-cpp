#include "./parser.hpp"

using namespace json;

std::regex Parser::startBrace = std::regex 	 ("^(?:\\s*)(\\{)");
std::regex Parser::startBracket = std::regex     ("^(?:\\s*)(\\[)");
std::regex Parser::keyDef = std::regex           ("^(?:\\s*)(?:\")([^,]+?)(?:\")(?:\\s*):");
std::regex Parser::finalQuote = std::regex       ("^(?:\\s*)(?:\")(.*?)(?:\")(?:\\s*)(,)?");
std::regex Parser::finalNumberInt = std::regex   ("^(?:\\s*)((?:\\+|\\-)?\\d+)(?:\\s*)(,)?");
std::regex Parser::finalNumberFloat = std::regex ("^(?:\\s*)((?:\\+|\\-)?(?:\\d+)?(?:(?:(?:\\.\\d+)?e(?:\\+|\\-)?\\d+)|(?:\\.\\d+)))(?:\\s*)(,)?");
std::regex Parser::finalBoolean = std::regex     ("^(?:\\s*)(true|false)(?:\\s*)(,)?");
std::regex Parser::nextBrace = std::regex 	 ("^(?:\\s*)(\\})(?:\\s*)(,)?");
std::regex Parser::nextBracket = std::regex      ("^(?:\\s*)(\\])(?:\\s*)(,)?");

int Parser::parseFile (std::string fileName, JsonTree& tree, bool verbs ) {
  verbose = verbs;
  errors.resize (0);
  warnings.resize (0);
  int returnValue = OK;
  if (openFile(fileName)) {
    std::stringstream buffer;
    buffer << getFile().rdbuf();
    std::string fileContent = buffer.str();
    ObjectNameFlag result = parseExpectingElement (fileContent, "");
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

int Parser::parseString (std::string content, JsonTree& tree, bool verbs ) {
  verbose = verbs;
  errors.resize (0);
  warnings.resize (0);
  int returnValue = OK;
    ObjectNameFlag result = parseExpectingElement (content, "");
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

bool Parser::hasComma (std::string buffer) {
  return (buffer.size() > 0);
}

Parser::ObjectNameFlag Parser::parseFinal (std::string& content, std::smatch& matcher, ObjectFinal* obj) {
  obj->replaceValue(matcher[1]);
  content = content.substr(matcher[0].length(), content.size());
  return {obj, "", hasComma(matcher[2])};
}

Parser::ObjectNameFlag Parser::parseContainer (std::string& content, std::smatch& matcher,
                            std::regex& endSymbol, std::function<ObjectNameFlag(std::string&, std::string)> parseFunction, ObjectContainer* obj, std::string path)
{
  content = content.substr(matcher[0].length(), content.size());
  ObjectNameFlag aux;
  int flag = REGULAR_ELEMENT;
  do {
    aux = parseFunction (content, path);
    if (aux.flag == EMPTY) {
      evaluateFlag(EMPTY, path, aux.key);
      break;
    }
    if (!obj->insert (aux.key, aux.element)) {
      evaluateFlag(INVALID_KEY, path, aux.key);
      break;
    }
  } while (!std::regex_search (content, matcher, endSymbol) && aux.flag == REGULAR_ELEMENT);
  std::regex_search (content, matcher, endSymbol);
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

void Parser::evaluateFlag (int flag, std::string path, std::string finalElement) {
  path.append(".").append(finalElement);
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

Parser::ObjectNameFlag Parser::parseExpectingKeyDef (std::string &content, std::string path) {
  std::smatch matcher;
  ObjectNameFlag Obj;
  if (std::regex_search (content, matcher, keyDef)) {
      std::string key = matcher[1];
      content = content.substr(matcher[0].length(), content.size());
      path.append(".").append(key);
      ObjectNameFlag aux = parseExpectingElement (content, path);
      return {aux.element, key, aux.flag};
  } else {
    return {nullptr, "", EMPTY};
  }
}

Parser::ObjectNameFlag Parser::parseExpectingElement (std::string& content, std::string path) {
  std::smatch matcher;
  ObjectNameFlag Obj; 
  if (std::regex_search(content, matcher, finalQuote))
    return parseFinal (content, matcher, new ObjectFinalString());
  else if (std::regex_search(content, matcher, finalBoolean))
    return parseFinal (content, matcher, new ObjectFinalBool());
  else if (std::regex_search(content, matcher, finalNumberFloat))
    return parseFinal (content, matcher, new ObjectFinalNumberFloat());
  else if (std::regex_search(content, matcher, finalNumberInt))
    return parseFinal (content, matcher, new ObjectFinalNumberInt());
  else if (std::regex_search (content, matcher, startBrace))
    return parseContainer (content, matcher, nextBrace,
           [&](std::string& a, std::string b) { return parseExpectingKeyDef(a, b); },
    new ObjectMap (), path);
  else if (std::regex_search (content, matcher, startBracket))
    return parseContainer (content, matcher, nextBracket,
           [&](std::string& a, std::string b) { return parseExpectingElement(a, b); },
    new ObjectVector (), path);
  return {nullptr, "", EMPTY};
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
