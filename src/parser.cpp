#include "./parser.hpp"

using namespace json;

int Parser::parseFile (const std::string fileName, JsonTree& tree, bool verbs) {
  errors.clear();
  warnings.clear();
  verbose = verbs;
  std::ifstream file;
  file.open(fileName, std::ifstream::in);
  if (file.is_open()) {

    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    content = std::string(size, ' ');
    file.seekg(0);
    file.read(&content[0], size);

    file.close();
    return parse (tree);
  } else {
    return CANT_OPEN_FILE;
  }  
}

int Parser::parseString (std::string cnt, JsonTree& tree, bool verbs ) {
  errors.clear();
  warnings.clear();
  verbose = verbs;
  content = cnt;
  return parse (tree);
}

int Parser::parse (JsonTree& tree) {  
  parseIndex = 0;
  int returnValue = OK;
  ObjectNameFlag result = parseExpectingElement ("");
  tree.setTop (result.element);
  if (result.flag == EMPTY)
     return EMPTY_FILE;
  if (hasWarnings())
    returnValue += WARNINGS;
  if (hasErrors())
    returnValue = (returnValue & (INT_MAX - 1)) + ERRORS;
  return returnValue;
}

Parser::Parser () :
  errors (0),
  warnings (0),
  parseIndex (0)
  {}

bool Parser::hasComma () {
  ignoreFirstBlanks();
  return (content[parseIndex] == ',' && ++parseIndex);
}

Parser::ObjectNameFlag Parser::parseContainer (std::string path, ObjectContainer* obj, char symbol,
                                               std::function<ObjectNameFlag(std::string a, int b)> continueParsing) {
  parseIndex++;  
  ObjectNameFlag aux;
  int flag = REGULAR_ELEMENT;
  unsigned i = 0;
  do {
    aux = continueParsing(path, i);
    if (aux.flag == EMPTY) {
      evaluateFlag(EMPTY, path, aux.key);
      break;
    }
    if (!obj->insert (aux.key, aux.element)) {
      evaluateFlag(INVALID_KEY, path, aux.key);
      break;
    }
    i++;
    ignoreFirstBlanks();
  } while (content[parseIndex] != symbol && aux.flag == REGULAR_ELEMENT);
  if (aux.flag == REGULAR_ELEMENT) {
    flag = EXPECTED_MORE;
    evaluateFlag(flag, path, aux.key);
  } else if (content[parseIndex] == symbol) { 													// has matched
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

Parser::ObjectNameFlag Parser::parseExpectingKeyDef (std::string path) {
  ignoreFirstBlanks();
  unsigned size = content.size();
  if (content[parseIndex] == '\"') {
    parseIndex++;
    int from = parseIndex;
    while (content[++parseIndex] != '\"' && parseIndex < size);
    std::string key (content.substr(from, parseIndex - from));
    while (content[++parseIndex] != ':' && parseIndex < size);
    parseIndex++;    
    ObjectNameFlag aux = parseExpectingElement (path + "." + key);
    return { aux.element, key, aux.flag };
  } else {
    return { nullptr, "", EMPTY};
  }
}

Parser::ObjectNameFlag Parser::parseQuote() {
  parseIndex++;
  std::string buff;
  unsigned from = parseIndex;
  while (content[++parseIndex] != '\"');
  buff = content.substr(from, parseIndex - from);
  parseIndex++;
  return {new ObjectFinalString (buff), "", hasComma()};
}

Parser::ObjectNameFlag Parser::parseBool(bool boolean) {
  if (boolean && content.substr(parseIndex, 4) == "true") {
    parseIndex += 4;
    return { new ObjectFinalBool (true), "", hasComma() };
  } else if (content.substr(parseIndex, 5) == "false") {
    parseIndex += 5;
    return { new ObjectFinalBool (false), "", hasComma() };
  } else {
    return { nullptr, "", EXPECTED_KEY };
  }
}

Parser::ObjectNameFlag Parser::parseNumber() {  
  std::string buff;
  bool end = false;
  unsigned size = content.size();
  char element = content[parseIndex];
  while (!(element == ' ' || element == ',' || element == ']' || element == '}') && parseIndex < size) {
    buff.push_back(element);
    element = content[++parseIndex];
  }
  if (!buff.empty()) {
    double number;
    try {
      number = stod (buff);
      return {new ObjectFinalNumber (number), "", hasComma()};
    }
    catch (std::invalid_argument e) {
      return {nullptr, "", INVALID_KEY};
    }
  } else {
    return {nullptr, "", EMPTY};
  }
}

// Updates parseIndex position ignoring all blanks
void Parser::ignoreFirstBlanks() {
  unsigned size = content.size();
  char element = content[parseIndex];
  while((element == ' ' || element == '\t'|| element == '\n' ) && parseIndex < size) {
    element = content[++parseIndex];
  }
}

Parser::ObjectNameFlag Parser::parseExpectingElement (std::string path) {
  ignoreFirstBlanks();
  switch (content[parseIndex]) {
    case '{':
      return parseContainer(path, new ObjectMap(), '}', [&](std::string a, int b) {
          return parseExpectingKeyDef(a);
        });
    case '[':      
      return parseContainer(path, new ObjectVector(), ']', [&](std::string a, int b) {
          return parseExpectingElement(a + "." + std::to_string(b));
        });
    case '\"':
      return parseQuote();
    case 't':
      return parseBool(true);
    case 'f':
      return parseBool(false);
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
