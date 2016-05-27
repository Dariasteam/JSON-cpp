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

void ObjectMap::insert (string key, AbstractObject* obj) {
  keys.push_back (key);
  hash.insert(pair <string, AbstractObject*> (key, obj));
}
