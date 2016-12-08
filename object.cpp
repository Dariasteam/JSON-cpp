#include "./object.hpp"

#include <iostream>

using namespace json;

regex ObjectVector::tokenRgx = regex ("^(?:\\.)?(?:\\[)?(\\d+)(:?\\])?(:?\\.)?");
regex ObjectMap::tokenRgx = regex    ("^(?:\\.)?(?:\\[')?(\\w+)(:?'\\])?(:?\\.)?");

void AbstractObject::txtIndent(string &txt, int indentLvl) {
  for (int i = 0; i < indentLvl; i++)
    txt.append(INDENT);
}

AbstractObject* AbstractObject::copy (const AbstractObject* obj) {
  switch (obj->getType()) {
    case BOOL:
      return new ObjectFinalBool (*((ObjectFinalBool*)obj));
      break;
    case NUMBER_FLOAT:
      return new ObjectFinalNumberFloat (*((ObjectFinalNumberFloat*)obj));
      break;
    case NUMBER_INT:
      return new ObjectFinalNumberInt (*((ObjectFinalNumberInt*)obj));
      break;
    case STRING:
      return new ObjectFinalString (*((ObjectFinalString*)obj));
      break;
    case MAP:
      return new ObjectMap (*((ObjectMap*)obj));
      break;
    case VECTOR:
      return new ObjectVector (*((ObjectVector*)obj));
      break;
    }
}

ObjectFinalBool::ObjectFinalBool (const ObjectFinalBool &obj) :
  ObjectFinal (BOOL),
  boolean(obj.getContent())
{}

ObjectFinalNumberFloat::ObjectFinalNumberFloat (const ObjectFinalNumberFloat &obj) :
  ObjectFinal (NUMBER_FLOAT),
  number(obj.getContent())
{}

ObjectFinalNumberInt::ObjectFinalNumberInt (const ObjectFinalNumberInt &obj) :
  ObjectFinal (NUMBER_INT),
  number(obj.getContent())
{}

ObjectFinalString::ObjectFinalString (const ObjectFinalString &obj) :
  ObjectFinal (STRING),
  text(obj.getContent())
{}

ObjectMap::ObjectMap (const ObjectMap &obj) :
  ObjectContainer (MAP),
  keys(obj.getKeys())
  {
    for (string key : keys) {
      hash[key] = copy (obj.getContent().at(key));
    }
  }

ObjectVector::ObjectVector (const ObjectVector &obj) :
  ObjectContainer (VECTOR),
  array(obj.size())
  {
    for (int i = 0; i < obj.size(); i++){
      array[i] = copy (obj.getContent()[i]);
    }
  }

void ObjectFinalBool::replaceValue (string value) {
  if (value == "true")
    boolean = true;
  else
    boolean = false;
}

void ObjectFinalNumberFloat::replaceValue (string value) {
  number = stod (value);
}

void ObjectFinalNumberInt::replaceValue (string value) {
  number = atol (value.c_str());
}

void ObjectFinalString::replaceValue (string value) {
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
    delete obj;
    return false;
  }
}

bool ObjectMap::insert (string key, AbstractObject* obj) {
  if (key != "" && !hash.count (key)) {
    keys.push_back (key);
    hash.insert(pair <string, AbstractObject*> (key, obj));
    return true;
  } else {
    delete obj;
    return false;
  }
}

bool ObjectMap::replace (string key, AbstractObject* obj) {
  if (!hash.count (key))
    return false;
  keys.erase(std::remove(keys.begin(), keys.end(), key), keys.end());
  hash.erase (key);
  return insert (key, obj);
}

bool ObjectMap::set (string key, AbstractObject* obj) {
  if (!hash.count (key))
    return insert (key, obj);
  else
    return replace (key, obj);
}

AbstractObject* ObjectVector::get (string path) {
  smatch matcher;
  if (path.size() == 0)
    return this;
  if (regex_search (path, matcher, tokenRgx)) {
    AbstractObject* son = operator[](stoi (matcher[1]));
    path = path.substr(matcher[0].length(), path.size());
    if (son != nullptr) {
      return son->get (path);
    }
  }
  return nullptr;
}

AbstractObject* ObjectMap::get (string path) {
  smatch matcher;
  if (path.empty() || path == ".")
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

bool ObjectMap::remove (string key) {
  AbstractObject* aux = operator[](key);
  if (aux == nullptr)
    return true;
  keys.erase(std::remove(keys.begin(), keys.end(), key), keys.end());
  hash.erase (key);
  delete aux;
  return true;
}

bool ObjectMap::erase (string key) {
  if (!hash.count (key))
    return false;
  else
    return remove (key);
}

bool ObjectMap::add (string path, AbstractObject* obj) {
  smatch matcher;
  if (regex_search (path, matcher, tokenRgx)) {
    AbstractObject* son = operator[](matcher[1]);
    string newPath = path.substr(matcher[0].length(), path.size());
    if (son != nullptr) {
      if (!newPath.empty() || son->getType() == VECTOR) {
        return son->add (newPath, obj);
      } else {
        delete obj;
        return false;
      }
    } else {
      if (!newPath.empty()) {
        son = new ObjectMap();
        if (insert (matcher[1], son))
          return son->add (newPath, obj);
        else
          return false;
      } else {
        return insert (path, obj);
      }
    }
  }
  delete obj;
  return false;
}

bool ObjectVector::add (string path, AbstractObject* obj) {
  smatch matcher;
  AbstractObject* son;
  if (regex_search (path, matcher, tokenRgx)) {
    son = operator[](stoi (matcher[1]));
    string newPath = path.substr(matcher[0].length(), path.size());
    if (son != nullptr) {
      return son->add (newPath, obj);
    } else {
      delete obj;
      return false;
    }
  } else if (path.empty()) {
    return insert ("", obj);
  } else {
    son = new ObjectMap();
    if(insert ("", son))
      return son->add (path, obj);
    else
      return false;
  }
  delete obj;
  return false;
}

bool ObjectFinal::add (string path, AbstractObject* obj) {
  delete obj;
  return false;
}

void ObjectVector::toTxt (string& txt, int indentLvl) {
  txt.append("[");
  indentLvl++;
  AbstractObject* obj = this;
  for (int i = 0; i < size(); i++) {
    if (obj->getType() != NUMBER_INT) {
      txt.append(END_LINE);
      txtIndent (txt, indentLvl);
    } else {
      txt.append(" ");
    }
    obj = operator[](i);
    obj->toTxt(txt, indentLvl);
    txt.append(COMMA);
  }
  if (size() > 0)
    txt.pop_back();
  indentLvl--;
  txt.append(END_LINE);
  txtIndent (txt, indentLvl);
  txt.append("]");
}

void ObjectMap::toTxt (string& txt, int indentLvl) {
  txt.append("{");
  indentLvl++;
  for (int i = 0; i < getKeys().size(); i++) {
    txt.append(END_LINE);
    txtIndent (txt, indentLvl);
    txt.append(QUOTE).append(getKeys()[i]).append(QUOTE).append(POINTS);
    operator[](getKeys()[i])->toTxt(txt, indentLvl);
    txt.append(COMMA);
  }
  if (getKeys().size() > 0)
    txt.pop_back();
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

void ObjectFinalNumberFloat::toTxt (string& txt, int indentLvl) {
  txt.append(to_string(getContent()));
}

void ObjectFinalNumberInt::toTxt (string& txt, int indentLvl) {
  txt.append(to_string(getContent()));
}

void ObjectFinalString::toTxt (string& txt, int indentLvl) {
  txt.append(QUOTE).append(getContent()).append(QUOTE);
}


ObjectVector::~ObjectVector () {
  array.clear();
}

ObjectMap::~ObjectMap () {
  hash.clear();
}

ObjectFinalBool::~ObjectFinalBool () {

}

ObjectFinalNumberFloat::~ObjectFinalNumberFloat () {

}

ObjectFinalNumberInt::~ObjectFinalNumberInt () {

}

ObjectFinalString::~ObjectFinalString () {

}

AbstractObject::~AbstractObject() {}

ObjectContainer::~ObjectContainer() {}

ObjectFinal::~ObjectFinal() {}


