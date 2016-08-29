#include "./manager.hpp"

regex JsonTree::lastTokenRgx = regex ("^(.+)(?:\\.)(.+)$");

JsonTree::JsonTree (AbstractObject* root)
  { top = (ObjectMap*)root; }

void JsonTree::getterError (string path, AbstractObject* obj, int type) {
  cerr << "ERROR : ";
  if (obj == nullptr) {
    cerr << "Attempting to load the element " << path
         << " which does not exist."
         << " The default value will be returned " << endl;
  } else {
    cerr << "Attempting to load the element " << path
         << " as a "<< objectsTypesReverse[type] << " when is of type "
         << objectsTypesReverse[obj->getType()]
         << ". The default value will be returned " << endl;
  }
}

vector <string> JsonTree::getKeys (string path) {
  AbstractObject* obj = top->get(path);
  if (obj != nullptr && obj->getType() == MAP) {
    return ((ObjectMap*)obj)->getKeys();
  } else {
    return vector<string> (0);
  }
}

int JsonTree::getSizeAt (string path) {
  AbstractObject* obj = top->get(path);
  if (obj != nullptr && obj->getType() == VECTOR) {
    return ((ObjectVector*)obj)->size();
  } else {
    return 0;
  }
}

bool JsonTree::isNumber (string path) {
  return isType (top->get(path), NUMBER);
}

bool JsonTree::isBool (string path) {
  return isType (top->get(path), BOOL);
}

bool JsonTree::isString (string path) {
  return isType (top->get(path), STRING);
}

bool JsonTree::isMap (string path) {
  return isType (top->get(path), MAP);
}

bool JsonTree::isVector (string path) {
  return isType (top->get(path), VECTOR);
}

bool JsonTree::isType (AbstractObject* obj, int type) {
  return (obj != nullptr && obj->getType() == type);
}

bool JsonTree::exist (string path) {
  return top->get(path) != nullptr;
}

// FINALS

bool JsonTree::copy (bool &to, string path) {
  AbstractObject* obj = top->get(path);
  if (isType(obj, BOOL)) {
    to = ((ObjectFinalNumber*)obj)->getContent();
    return true;
  }
  getterError(path, obj, BOOL);
  return false;
}

bool JsonTree::copy (string &to, string path) {
  AbstractObject* obj = top->get(path);
  if (isType(obj, STRING)) {
    to = ((ObjectFinalNumber*)obj)->getContent();
    return true;
  }
  getterError(path, obj, STRING);
  return false;
}

bool JsonTree::copy (double &to, string path) {
  AbstractObject* obj = top->get(path);
  if (isType(obj, NUMBER)) {
    to = ((ObjectFinalNumber*)obj)->getContent();
    return true;
  }
  getterError(path, obj, NUMBER);
  return false;
}

bool JsonTree::copy (float &to, string path) {
  AbstractObject* obj = top->get(path);
  if (isType(obj, NUMBER)) {
    to = ((ObjectFinalNumber*)obj)->getContent();
    return true;
  }
  getterError(path, obj, NUMBER);
  return false;
}

bool JsonTree::copy (int &to, string path) {
  AbstractObject* obj = top->get(path);
  if (isType(obj, NUMBER)) {
    to = ((ObjectFinalNumber*)obj)->getContent();
    return true;
  }
  getterError(path, obj, NUMBER);
  return false;
}

// VECTORS

bool JsonTree::copy (vector<double>& array, string path) {
  AbstractObject* obj = top->get(path);
  if (isType (obj, VECTOR)) {
    ObjectVector* vect = (ObjectVector*)obj;
    int size = vect->size();
    array.resize (size);
    for (int i = 0; i < size; i++) {
      if (((ObjectFinalNumber*)vect->operator[](i))->getType() == NUMBER)
        array[i] = ((ObjectFinalNumber*)vect->operator[](i))->getContent();
      else
        return false;
    }
    return true;
  }
  return false;
}

bool JsonTree::copy (vector<string>& array, string path) {
  AbstractObject* obj = top->get(path);
  if (isType (obj, VECTOR)) {
    ObjectVector* vect = (ObjectVector*)obj;
    int size = vect->size();
    array.resize (size);
    for (int i = 0; i < size; i++) {
      if (((ObjectFinalNumber*)vect->operator[](i))->getType() == STRING)
        array[i] = ((ObjectFinalString*)vect->operator[](i))->getContent();
      else
        return false;
    }
    return true;
  }
  return false;
}

bool JsonTree::copy (vector<bool>& array, string path) {
  AbstractObject* obj = top->get(path);
  if (isType (obj, VECTOR)) {
    ObjectVector* vect = (ObjectVector*)obj;
    int size = vect->size();
    array.resize (size);
    for (int i = 0; i < size; i++) {
      if (((ObjectFinalNumber*)vect->operator[](i))->getType() == BOOL)
        array[i] = ((ObjectFinalNumber*)vect->operator[](i))->getContent();
      else
        return false;
    }
    return true;
  }
  return false;
}

AbstractObject* JsonTree::insertObject(string path, AbstractObject *obj) {
  return nullptr;
}

bool JsonTree::add (string path, double value) {
  AbstractObject* object = new ObjectFinalNumber (value);
  return top->add (path, object);
};

bool JsonTree::add (string path, bool value) {
  AbstractObject* object = new ObjectFinalBool (value);
  return top->add (path, object);
};

bool JsonTree::add (string path, string value) {
  AbstractObject* object = new ObjectFinalString (value);
  return top->add (path, object);
};

bool JsonTree::addMap (string path) {
  AbstractObject* object = new ObjectMap;
  return top->add (path, object);
}

bool JsonTree::addVector (string path) {
  AbstractObject* object = new ObjectVector;
  return top->add (path, object);
}

string JsonTree::toText () {
  string txt;
  top->toTxt(txt, 0);
  return txt;
}

bool JsonTree::replace(AbstractObject *newObj, string path) {
  smatch matcher;
  if (regex_search (path, matcher, lastTokenRgx)) {
      AbstractObject* father = top->get (matcher[1]);
      if (father->getType() == MAP)
        return ((ObjectMap*)father)->replace (matcher[2], newObj);
      return false;
  }
  return false;
}

bool JsonTree::replace (string from, string path) {
  AbstractObject* object = new ObjectFinalString (from);
  return replace (object, path);
}

bool JsonTree::replace (bool from, string path) {
  AbstractObject* object = new ObjectFinalBool (from);
  return replace (object, path);
}

bool JsonTree::replace (double from, string path) {
  AbstractObject* object = new ObjectFinalNumber (from);
  return replace (object, path);
}

bool JsonTree::set(AbstractObject *newObj, string path) {
  if (exist (path))
    return replace (newObj, path);
  else
    return top->add (path, newObj);
}

bool JsonTree::set (double from, string path) {
  AbstractObject* object = new ObjectFinalNumber (from);
  return set (object, path);
}

bool JsonTree::set (string from, string path) {
  AbstractObject* object = new ObjectFinalString (from);
  return set (object, path);
}

bool JsonTree::set (bool from, string path) {
  AbstractObject* object = new ObjectFinalBool (from);
  return set (object, path);
}
