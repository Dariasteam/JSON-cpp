#include "manager.hpp"

JsonTree::JsonTree (AbstractObject* root) :
  tokenRgx ("(\\w*)(:?\\.)?")
  {
  top = (ObjectMap*)root;
}

AbstractObject* JsonTree::getObjectAt (string path, AbstractObject* obj) {
  if (path.size() != 0 && obj != nullptr) {
    smatch matcher;
    if (regex_search (path, matcher, tokenRgx)) {
      path = path.substr(matcher[0].length(), path.size());
      return getObjectAt (path, searchSon (matcher[1], obj));
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

pair <double, bool> JsonTree::getNumberAt (string path) {
  AbstractObject* obj = getObjectAt (path, top);
  if (obj != nullptr && obj->getType() == NUMBER) {
    double number = ((ObjectFinalNumber*)obj)->getContent();
    return pair <double, bool> (number, true);
  } else {
    return pair <double, bool> (-1, false);
  }
}

pair <string, bool> JsonTree::getStringAt (string path) {
  AbstractObject* obj = getObjectAt (path, top);
  if (obj != nullptr && obj->getType() == STRING) {
    string text = ((ObjectFinalString*)obj)->getContent();
    return pair <string, bool> (text, true);
  } else {
    return pair <string, bool> ("", false);
  }
}

pair <bool, bool> JsonTree::getBoolAt (string path) {
  AbstractObject* obj = getObjectAt (path, top);
  if (obj != nullptr && obj->getType() == NUMBER) {
    bool content = ((ObjectFinalNumber*)obj)->getContent();
    return pair <bool, bool> (content, true);
  } else {
    return pair <bool, bool> (false, false);
  }
}

vector <string> JsonTree::getKeysAt (string path) {
  AbstractObject* obj = getObjectAt (path, top);
  if (obj != nullptr && obj->getType() == MAP) {
    return ((ObjectMap*)obj)->getKeys();
  } else {
    return vector<string> (0);
  }
}

int JsonTree::getSizeAt (string path) {
  AbstractObject* obj = getObjectAt (path, top);
  if (obj != nullptr && obj->getType() == VECTOR) {
    return ((ObjectVector*)obj)->size();
  } else {
    return 0;
  }
}
