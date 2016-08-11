#include "./object.hpp"

AbstractObject::~AbstractObject() {}

ObjectContainer::~ObjectContainer() {}

ObjectFinal::~ObjectFinal() {}

regex ObjectVector::tokenRgx = regex ("^(?:\\[(\\d)+(:?\\]))(:?\\.)?");
regex ObjectMap::tokenRgx = regex ("^(?:\\[')?(\\w+)(:?'\\])?(:?\\.)?");

void ObjectFinalBool::setValue (string value) {
  if (value == "true")
    boolean = true;
}

#include <iostream>

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

AbstractObject* ObjectMap::operator[](string key) {
  if (hash.count (key))
    return hash[key];
  else
    return nullptr;
}

bool ObjectVector::insert (string key, AbstractObject* obj) {
  if (key == "") {
    array.push_back (obj);
    return true;
  } else {
    return false;
  }
}

bool ObjectMap::insert (string key, AbstractObject* obj) {
  if (key != "" && !hash.count (key)) {
    keys.push_back (key);
    hash.insert(pair <string, AbstractObject*> (key, obj));
    return true;
  } else {
    return false;
  }
}

AbstractObject* ObjectVector::get (string path) {
  smatch matcher;
  if (path.size() == 0)
    return this;
  if (regex_search (path, matcher, tokenRgx)) {
    AbstractObject* son = operator[](stoi (matcher[1]));
    path = path.substr(matcher[0].length(), path.size());
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
    AbstractObject* son = operator[](matcher[1]);
    path = path.substr(matcher[0].length(), path.size());
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

bool ObjectVector::add (string path, string value) {
  smatch matcher;
  if (regex_search (path, matcher, tokenRgx)) {
    AbstractObject* son = operator[](stoi (matcher[1]));
    if (son != nullptr) {
      return son->add (path.substr(matcher[0].length(), path.size()), value);
    } else {
      AbstractObject* nuevo = addObjectDecisor (path, value);
      insert (matcher[1], nuevo);
      if (nuevo->getType() > FINAL) // its final object
        return true;
      else
        return nuevo->add (path.substr(matcher[0].length(), path.size()), value);
    }
  }
  return false;
}

bool ObjectMap::add (string path, string value) {
  smatch matcher;
  if (regex_search (path, matcher, tokenRgx)) {
    AbstractObject* son = operator[](matcher[1]);
    if (son != nullptr) {
      return son->add (path.substr(matcher[0].length(), path.size()), value);
    } else {
      AbstractObject* nuevo = addObjectDecisor (path, value);
      insert (matcher[1], nuevo);
      if (nuevo->getType() > FINAL) // its final object
        return true;
      else
        return nuevo->add (path.substr(matcher[0].length(), path.size()), value);
    }
  }
  return false;
}

bool ObjectFinal::add (string path, string value) {
  return false;
}

AbstractObject* ObjectContainer::addObjectDecisor (string& path, string value) {
  cout << "Genero" << endl;
  return new ObjectFinalNumber (23);
}
