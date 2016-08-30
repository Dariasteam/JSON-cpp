#include "./manager.hpp"

regex JsonTree::lastTokenRgx = regex ("^(.*)(?:\\.)(.+)$");

JsonTree::JsonTree () {
  top = new ObjectMap ();
}

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

ObjectVector* JsonTree::createVec (vector<double> &array) {
  ObjectVector* object = new ObjectVector ();
  for (double number : array) {
    object->insert("", new ObjectFinalNumber (number));
  }
  return object;
}

ObjectVector* JsonTree::createVec (vector<int> &array) {
  ObjectVector* object = new ObjectVector ();
  for (int number : array) {
    object->insert("", new ObjectFinalNumber (float(number)));
  }
  return object;
}

ObjectVector* JsonTree::createVec (vector<bool> &array) {
  ObjectVector* object = new ObjectVector ();
  for (bool boolean : array) {
    object->insert("", new ObjectFinalBool (boolean));
  }
  return object;
}

ObjectVector* JsonTree::createVec (vector<string> &array) {
  ObjectVector* object = new ObjectVector ();
  for (string word : array) {
    object->insert("", new ObjectFinalString (word));
  }
  return object;
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

string JsonTree::toText () {
  string txt;
  top->toTxt(txt, 0);
  return txt;
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

bool JsonTree::copy (vector<int>& array, string path) {
  AbstractObject* obj = top->get(path);
  if (isType (obj, VECTOR)) {
    ObjectVector* vect = (ObjectVector*)obj;
    int size = vect->size();
    array.resize (size);
    for (int i = 0; i < size; i++) {
      if (((ObjectFinalNumber*)vect->operator[](i))->getType() == NUMBER)
        array[i] = int(((ObjectFinalNumber*)vect->operator[](i))->getContent());
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

// ADD

bool JsonTree::add (double value, string path) {
  AbstractObject* object = new ObjectFinalNumber (value);
  return top->add (path, object);
};

bool JsonTree::add (float value, string path) {
  return add (double(value), path);
};

bool JsonTree::add (int value, string path) {
  return add (double(value), path);
};

bool JsonTree::add (bool value, string path) {
  AbstractObject* object = new ObjectFinalBool (value);
  return top->add (path, object);
};

bool JsonTree::add (string value, string path) {
  AbstractObject* object = new ObjectFinalString (value);
  return top->add (path, object);
};

bool JsonTree::add (const char* value, string path) {
  return add (string(value), path);
};

bool JsonTree::add (vector<double> &array, string path) {
  return top->add (path, createVec(array));
}

bool JsonTree::add (vector<int> &array, string path) {
  return top->add (path, createVec(array));
}

bool JsonTree::add (vector<bool> &array, string path) {
  return top->add (path, createVec(array));
}

bool JsonTree::add (vector<string> &array, string path) {
  return top->add (path, createVec(array));
}

bool JsonTree::addMap (string path) {
  AbstractObject* object = new ObjectMap;
  return top->add (path, object);
}

bool JsonTree::addVector (string path) {
  AbstractObject* object = new ObjectVector;
  return top->add (path, object);
}

// REPLACE

bool JsonTree::replace (AbstractObject *newObj, string path) {
  smatch matcher;
  if (regex_search (path, matcher, lastTokenRgx)) {
      AbstractObject* father = top->get (matcher[1]);
      if (father->getType() == MAP)
        return ((ObjectMap*)father)->replace (matcher[2], newObj);
      return false;
  } else if (!path.empty()) {
    return ((ObjectMap*)top)->replace (path, newObj);
  }
  return false;
}

bool JsonTree::replace (double from, string path) {
  AbstractObject* object = new ObjectFinalNumber (from);
  return replace (object, path);
}

bool JsonTree::replace (float from, string path) {
  return replace (double(from), path);
}

bool JsonTree::replace (int from, string path) {
  return replace (double(from), path);
}

bool JsonTree::replace (bool from, string path) {
  AbstractObject* object = new ObjectFinalBool (from);
  return replace (object, path);
}

bool JsonTree::replace (string from, string path) {
  AbstractObject* object = new ObjectFinalString (from);
  return replace (object, path);
}

bool JsonTree::replace (const char* from, string path) {
  return replace (string(from), path);
}

bool JsonTree::replace (vector<double>& array, string path) {
  return replace (createVec (array), path);
}

bool JsonTree::replace (vector<int>& array, string path) {
  return replace (createVec (array), path);
}

bool JsonTree::replace (vector<bool>& array, string path) {
  return replace (createVec (array), path);
}

bool JsonTree::replace (vector<string>& array, string path) {
  return replace (createVec (array), path);
}

// SET

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

bool JsonTree::set (float from, string path) {
  return set (double(from), path);
}

bool JsonTree::set (int from, string path) {
  return set (double(from), path);
}

bool JsonTree::set (bool from, string path) {
  AbstractObject* object = new ObjectFinalBool (from);
  return set (object, path);
}

bool JsonTree::set (string value, string path) {
  AbstractObject* object = new ObjectFinalString (value);
  return set (object, path);
}

bool JsonTree::set(const char *value, string path) {
  return set (string(value), path);
}

// ERASE

bool JsonTree::erase (string path) {
  smatch matcher;
  if (regex_search (path, matcher, lastTokenRgx)) {
      AbstractObject* father = top->get (matcher[1]);
      if (father->getType() == MAP)
        return ((ObjectMap*)father)->erase (matcher[2]);
      return false;
  } else if (!path.empty())
    return ((ObjectMap*)top)->erase (path);
  return false;
}

// REMOVE

bool JsonTree::remove (string path) {
  smatch matcher;
  if (regex_search (path, matcher, lastTokenRgx)) {
      AbstractObject* father = top->get (matcher[1]);
      if (father->getType() == MAP)
        return ((ObjectMap*)father)->remove (matcher[2]);
      return false;
  } else if (!path.empty())
    return ((ObjectMap*)top)->erase (path);
  return false;
}
