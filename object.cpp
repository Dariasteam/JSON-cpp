#include "./object.hpp"

AbstractObject::~AbstractObject() {}

ObjectContainer::~ObjectContainer() {}

ObjectFinal::~ObjectFinal() {}

#include <iostream>

regex ObjectVector::tokenRgx = regex ("^(?:\\[)?(\\d+)(:?\\])?(:?\\.)?");
regex ObjectMap::tokenRgx = regex    ("^(?:\\[')?(\\w+)(:?'\\])?(:?\\.)?");

void AbstractObject::txtIndent(string &txt, int indentLvl) {
  for (int i = 0; i < indentLvl; i++)
    txt.append(INDENT);
}

void ObjectFinalBool::setValue (string value) {
  if (value == "true")
    boolean = true;
  else
    boolean = false;
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

bool ObjectVector::add (string path,  AbstractObject* obj) {
  smatch matcher;
  if (regex_search (path, matcher, tokenRgx)) {
    AbstractObject* son = operator[](stoi (matcher[1]));
    if (son != nullptr) {
      return son->add (path.substr(matcher[0].length(), path.size()), obj);
    } else {
      cout << "it worked " << insert ("", obj) << endl;
      if (obj->getType() > FINAL) // its final object
        return true;
      else
        return obj->add (path.substr(matcher[0].length(), path.size()), obj);
    }
  }
  return false;
}

bool ObjectMap::add (string path,  AbstractObject* obj) {
  smatch matcher;
  if (regex_search (path, matcher, tokenRgx)) {
    AbstractObject* son = operator[](matcher[1]);
    if (son != nullptr) {
      return son->add (path.substr(matcher[0].length(), path.size()), obj);
    } else {
      insert (matcher[1], obj);
      if (obj->getType() > FINAL) // its final object
        return true;
      else
        return obj->add (path.substr(matcher[0].length(), path.size()), obj);
    }
  }
  return false;
}

bool ObjectFinal::add (string path,  AbstractObject* obj) {
  return false;
}

void ObjectVector::toTxt (string& txt, int indentLvl) {
  txt.append("[\n");
  indentLvl++;
  for (int i = 0; i < size(); i++) {
    txtIndent (txt, indentLvl);
    operator[](i)->toTxt(txt, indentLvl);
    if (i < size() -1)
      txt.append(COMMA).append(END_LINE);
  }
  indentLvl--;
  txt.append(END_LINE);
  txtIndent (txt, indentLvl);
  txt.append("]");
}

void ObjectMap::toTxt (string& txt, int indentLvl) {
  txt.append("{\n");
  indentLvl++;
  for (int i = 0; i < getKeys().size(); i++) {
    txtIndent (txt, indentLvl);
    txt.append(QUOTE).append(getKeys()[i]).append(QUOTE).append(POINTS);
    operator[](getKeys()[i])->toTxt(txt, indentLvl);
    if (i < getKeys().size() -1)
      txt.append(COMMA).append(END_LINE);
  }
  indentLvl--;
  txt.append(END_LINE);
  txtIndent (txt, indentLvl);
  txt.append("}");
}

void ObjectFinalBool::toTxt (string& txt, int indentLvl) {
  if (getContent())
    txt.append("true");
  else
    txt.append("false");
}

void ObjectFinalNumber::toTxt (string& txt, int indentLvl) {
  txt.append(to_string(getContent()));
}

void ObjectFinalString::toTxt (string& txt, int indentLvl) {
  txt.append(QUOTE).append(getContent()).append(QUOTE);
}
