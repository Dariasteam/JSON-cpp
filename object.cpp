#include "./object.hpp"

AbstractObject::~AbstractObject() {}

ObjectContainer::~ObjectContainer() {}

ObjectFinal::~ObjectFinal() {}

AbstractObject* ObjectVector::operator[] (int index) {
  if (index < 0 || index >= array.size())
    return nullptr;
  else
    return array [index];
  }

void ObjectMap::insert (string key, AbstractObject* obj) {
  keys.push_back (key);
  hash.insert(pair <string, AbstractObject*> (key, obj));
}
