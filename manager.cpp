#include "manager.hpp"

JsonTree::JsonTree (AbstractObject* root) :
  tokenRgx ("(\\w*)(:?\\.)?")
  {
  top = (ObjectMap*)root;
}

AbstractObject* JsonTree::getObject (string path, AbstractObject* obj) {
  if (path.size() != 0 && obj != nullptr) {
    smatch matcher;
    if (regex_search (path, matcher, tokenRgx)) {
      path = path.substr(matcher[0].length(), path.size());
      return getObject (path, searchSon (matcher[1], obj));
    } else {
      return nullptr;
    }
  } else {
    return obj;
  }
}

AbstractObject* JsonTree::searchSon (string key, AbstractObject* obj) {
  if (obj->getType() == MAP) {
    return ((ObjectMap*)obj)->operator[](key);
  } else if (obj->getType() == VECTOR) {
    return ((ObjectMap*)obj)->operator[](stoi(key));
  } else {
    return nullptr;
  }
}

double JsonTree::getNumber (string path) {
  AbstractObject* obj = getObject (path, top);
  if (obj != nullptr && obj->getType() == NUMBER) {
    return ((ObjectFinalNumber*)obj)->getContent();
  } else {
    return -1;
  }
}

string JsonTree::getString (string path) {
  AbstractObject* obj = getObject (path, top);
  if (obj != nullptr && obj->getType() == STRING) {
    return ((ObjectFinalString*)obj)->getContent();
  } else {
    return "";
  }
}

bool JsonTree::getBool (string path) {
  AbstractObject* obj = getObject (path, top);
  if (obj != nullptr && obj->getType() == NUMBER) {
    return ((ObjectFinalNumber*)obj)->getContent();
  } else {
    return false;
  }
}

vector <string> JsonTree::getKeys (string path) {
  AbstractObject* obj = getObject (path, top);
  if (obj != nullptr && obj->getType() == MAP) {
    return ((ObjectMap*)obj)->getKeys();
  } else {
    return vector<string> (0);
  }
}

int JsonTree::getSizeAt (string path) {
  AbstractObject* obj = getObject (path, top);
  if (obj != nullptr && obj->getType() == VECTOR) {
    return ((ObjectVector*)obj)->size();
  } else {
    return 0;
  }
}

bool JsonTree::isNumber (string path) {
  return isType (path, NUMBER);
}

bool JsonTree::isBool (string path) {
return isType (path, BOOL);
}

bool JsonTree::isString (string path) {
  return isType (path, STRING);
}

bool JsonTree::isMap (string path) {
  return isType (path, MAP);
}

bool JsonTree::isVector (string path) {
  return isType (path, VECTOR);
}

bool JsonTree::isType (string path, int type) {
  AbstractObject* obj = getObject (path, top);
  return (obj != nullptr && obj->getType() == type);
}

bool JsonTree::exist (string path) {
  return getObject (path, top) != nullptr;
}
