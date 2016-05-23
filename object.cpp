#include "./object.hpp"

AbstractObject::~AbstractObject() {}

AbstractObject* ObjectVector::getContentAt (int index) {
  if (index < 0 || index >= array.size())
    return nullptr;
  else
    return array [index];
  }

void ObjectMap::insert (string name, AbstractObject* obj) {
  hash.insert(pair <string, AbstractObject*> (name, obj));
}
