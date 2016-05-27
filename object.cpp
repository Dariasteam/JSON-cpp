#include "./object.hpp"

AbstractObject::~AbstractObject() {}

ObjectContainer::~ObjectContainer() {}

ObjectFinal::~ObjectFinal() {}

void ObjectFinalBool::setValue (string value) {
  if (value == "true")
    boolean = true;
}

void ObjectFinalNumber::setValue (string value) {
  number = stod (value);
}

void ObjectFinalString::setValue (string value) {
  text = value;
}

AbstractObject* ObjectVector::operator[] (unsigned index) {
  if (index >= array.size()) // changed to unsigned
    return nullptr;
  else
    return array [index];
}

#include <iostream>

AbstractObject* ObjectMap::operator[](string key) {
  if (hash.count (key))
    return hash[key];
  else
    return nullptr;
}

void ObjectMap::insert (string key, AbstractObject* obj) {
  keys.push_back (key);
  hash.insert(pair <string, AbstractObject*> (key, obj));
}

AbstractObject* ObjectVector::get (string path) {
  smatch matcher;
  if (path.size() == 0)
    return this;
  if (regex_search (path, matcher, tokenRgx)) {
    path = path.substr(matcher[0].length(), path.size());
    AbstractObject* son = operator[](stoi (matcher[1]));
    if (son != nullptr)
      return son->get (path);
  }
  return nullptr;
}

AbstractObject* ObjectMap::get (string path) {
  smatch matcher;
  if (path.size() == 0)
    return this;
  if (regex_search (path, matcher, tokenRgx)) {
    path = path.substr(matcher[0].length(), path.size());
    AbstractObject* son = operator[](matcher[1]);
    if (son != nullptr)
      return son->get (path);
  }
  return nullptr;
}

AbstractObject* ObjectFinal::get (string path) {
  if (path.size () == 0)
    return this;
  else
    return nullptr;
}
