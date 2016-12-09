#include "./manager.hpp"

using namespace json;

regex JsonTree::lastTokenRgx = regex ("^(.*)(?:\\.)(.+)$");

JsonTree::JsonTree () :
  top (new ObjectMap())
{}

JsonTree::JsonTree (AbstractObject* root) {
  top = (ObjectMap*)root;
}

JsonTree::JsonTree (const JsonTree &tree, const string path) :
  top(new ObjectMap())
  {
    tree.copyFrom(top, path);
  }

JsonTree::~JsonTree () {
  delete top;
}

bool JsonTree::copyFrom(AbstractObject* obj, const string path) const {
  AbstractObject* from;
  if (path.empty() || path == ".")
    from = top;
  else
    from = top->get(path);

  if (from == nullptr) {
    return false;
  } else {
    if (obj != nullptr)
      delete obj;
    obj = AbstractObject::copy (from);
    return true;
  }
}

void JsonTree::getterError (const string path, AbstractObject* obj, int type) {
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
    object->insert("", new ObjectFinalNumberFloat (number));
  }
  return object;
}

ObjectVector* JsonTree::createVec (vector<int> &array) {
  ObjectVector* object = new ObjectVector ();
  for (int number : array) {
    object->insert("", new ObjectFinalNumberInt (float(number)));
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

vector <string> JsonTree::getKeys (const string path) {
  AbstractObject* obj = top->get(path);
  if (obj != nullptr && obj->getType() == MAP) {
    return ((ObjectMap*)obj)->getKeys();
  } else {
    return vector<string> (0);
  }
}

int JsonTree::getSizeAt (const string path) {
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

bool JsonTree::isNumber (const string path) {
  return (isFloat(path) || isInt(path));
}

bool JsonTree::isFloat (const string path) {
  return isType (top->get(path), NUMBER_FLOAT);
}

bool JsonTree::isInt (const string path) {
  return isType (top->get(path), NUMBER_INT);
}

bool JsonTree::isBool (const string path) {
  return isType (top->get(path), BOOL);
}

bool JsonTree::isString (const string path) {
  return isType (top->get(path), STRING);
}

bool JsonTree::isMap (const string path) {
  return isType (top->get(path), MAP);
}

bool JsonTree::isVector (const string path) {
  return isType (top->get(path), VECTOR);
}

bool JsonTree::isType (AbstractObject* obj, int type) {
  return (obj != nullptr && obj->getType() == type);
}

bool JsonTree::exist (const string path) {
  return top->get(path) != nullptr;
}

int JsonTree::getType(const string path) {
  AbstractObject* obj = top->get(path);
  if (obj != nullptr)
    return obj->getType();
  else
    return -1;
}

// FINALS

bool JsonTree::get (bool &to, const string path) {
  AbstractObject* obj = top->get(path);
  if (isType(obj, BOOL)) {
    to = ((ObjectFinalBool*)obj)->getContent();
    return true;
  }
  getterError(path, obj, BOOL);
  return false;
}

bool JsonTree::get (string &to, const string path) {
  AbstractObject* obj = top->get(path);
  if (isType(obj, STRING)) {
    to = ((ObjectFinalString*)obj)->getContent();
    return true;
  }
  getterError(path, obj, STRING);
  return false;
}

bool JsonTree::get (double &to, const string path) {
  AbstractObject* obj = top->get(path);
  if (isType(obj, NUMBER_INT)) {
    to = ((ObjectFinalNumberInt*)obj)->getContent();
    return true;
  }
  if (isType(obj, NUMBER_FLOAT)) {
    to = ((ObjectFinalNumberFloat*)obj)->getContent();
    return true;
  }
  getterError(path, obj, NUMBER_FLOAT);
  return false;
}

bool JsonTree::get (float &to, const string path) {
  AbstractObject* obj = top->get(path);
  if (isType(obj, NUMBER_INT)) {
    to = ((ObjectFinalNumberInt*)obj)->getContent();
    return true;
  }
  if (isType(obj, NUMBER_FLOAT)) {
    to = ((ObjectFinalNumberFloat*)obj)->getContent();
    return true;
  }
  getterError(path, obj, NUMBER_FLOAT);
  return false;
}

bool JsonTree::get (int &to, const string path) {
  AbstractObject* obj = top->get(path);
  if (isType(obj, NUMBER_INT)) {
    to = ((ObjectFinalNumberInt*)obj)->getContent();
    return true;
  }
  if (isType(obj, NUMBER_FLOAT)) {
    to = ((ObjectFinalNumberFloat*)obj)->getContent();
    return true;
  }
  getterError(path, obj, NUMBER_INT);
  return false;
}

// VECTORS

bool JsonTree::get (vector<double>& array, const string path) {
  AbstractObject* obj = top->get(path);
  if (isType (obj, VECTOR)) {
    ObjectVector* vect = (ObjectVector*)obj;
    int size = vect->size();
    array.resize (size);
    for (int i = 0; i < size; i++) {
      if (((ObjectFinalNumberFloat*)vect->operator[](i))->getType() == NUMBER_FLOAT)
        array[i] = int(((ObjectFinalNumberFloat*)vect->operator[](i))->getContent());
      else if (((ObjectFinalNumberInt*)vect->operator[](i))->getType() == NUMBER_INT)
        array[i] = int(((ObjectFinalNumberInt*)vect->operator[](i))->getContent());
      else {
        getterError(path, obj, VECTOR);
        return false;
      }
    }
    return true;
  }
  getterError(path, obj, VECTOR);
  return false;
}

bool JsonTree::get (vector<int>& array, const string path) {
  AbstractObject* obj = top->get(path);
  if (isType (obj, VECTOR)) {
    ObjectVector* vect = (ObjectVector*)obj;
    int size = vect->size();
    array.resize (size);
    for (int i = 0; i < size; i++) {
      if (((ObjectFinalNumberFloat*)vect->operator[](i))->getType() == NUMBER_FLOAT)
        array[i] = int(((ObjectFinalNumberFloat*)vect->operator[](i))->getContent());
      else if (((ObjectFinalNumberInt*)vect->operator[](i))->getType() == NUMBER_INT)
        array[i] = int(((ObjectFinalNumberInt*)vect->operator[](i))->getContent());
      else {
        getterError(path, obj, VECTOR);
        return false;
      }
    }
    return true;
  }
  getterError(path, obj, VECTOR);
  return false;
}

bool JsonTree::get (vector<string>& array, const string path) {
  AbstractObject* obj = top->get(path);
  if (isType (obj, VECTOR)) {
    ObjectVector* vect = (ObjectVector*)obj;
    int size = vect->size();
    array.resize (size);
    for (int i = 0; i < size; i++) {
      if (((ObjectFinalString*)vect->operator[](i))->getType() == STRING)
        array[i] = ((ObjectFinalString*)vect->operator[](i))->getContent();
      else {
        getterError(path, obj, VECTOR);
        return false;
      }
    }
    return true;
  }
  getterError(path, obj, VECTOR);
  return false;
}

bool JsonTree::get (vector<bool>& array, const string path) {
  AbstractObject* obj = top->get(path);
  if (isType (obj, VECTOR)) {
    ObjectVector* vect = (ObjectVector*)obj;
    int size = vect->size();
    array.resize (size);
    for (int i = 0; i < size; i++) {
      if (((ObjectFinalBool*)vect->operator[](i))->getType() == BOOL)
        array[i] = ((ObjectFinalBool*)vect->operator[](i))->getContent();
      else {
        getterError(path, obj, VECTOR);
        return false;
      }
    }
    return true;
  }
  getterError(path, obj, VECTOR);
  return false;
}

AbstractObject* JsonTree::insertObject(const string path, AbstractObject *obj) {
  return nullptr;
}

// ADD

bool JsonTree::add (double value, const string path) {
  AbstractObject* object = new ObjectFinalNumberFloat (value);
  return top->add (path, object);
};

bool JsonTree::add (int value, const string path) {
  AbstractObject* object = new ObjectFinalNumberInt (value);
  return top->add (path, object);
};

bool JsonTree::add (bool value, const string path) {
  AbstractObject* object = new ObjectFinalBool (value);
  return top->add (path, object);
};

bool JsonTree::add (string value, const string path) {
  AbstractObject* object = new ObjectFinalString (value);
  return top->add (path, object);
};

bool JsonTree::add (float value, const string path) {
  return add (double(value), path);
};

bool JsonTree::add (const char* value, const string path) {
  return add (string(value), path);
};

bool JsonTree::add (vector<double> &array, const string path) {
  return top->add (path, createVec(array));
}

bool JsonTree::add (vector<int> &array, const string path) {
  return top->add (path, createVec(array));
}

bool JsonTree::add (vector<bool> &array, const string path) {
  return top->add (path, createVec(array));
}

bool JsonTree::add (vector<string> &array, const string path) {
  return top->add (path, createVec(array));
}

bool JsonTree::addMap (const string path) {
  return top->add (path, new ObjectMap ());
}

bool JsonTree::addVector (const string path) {
  return top->add (path, new ObjectVector ());
}

bool JsonTree::add(JsonTree &tree, string from, const string path) {
  AbstractObject* obj = top->get(path);
  if (obj == nullptr) {
    if(top->add(path, obj))
      return tree.copyFrom(obj, from);
    else
      return false;
  } else if (obj->getType() == VECTOR) {
    return tree.copyFrom(obj, from);
  }
  return false;
}

// REPLACE

bool JsonTree::replace (AbstractObject *newObj, const string path) {
  smatch matcher;
  if (regex_search (path, matcher, lastTokenRgx)) {
      AbstractObject* father = top->get (matcher[1]);
      if (father != nullptr && father->getType() == MAP) {
        return ((ObjectMap*)father)->replace (matcher[2], newObj);
      } else {
        delete newObj;
        return false;
      }
  } else if (!path.empty()) {
    return ((ObjectMap*)top)->replace (path, newObj);
  }
  delete newObj;
  return false;
}

bool JsonTree::replace (double from, const string path) {
  AbstractObject* object = new ObjectFinalNumberFloat (from);
  return replace (object, path);
}

bool JsonTree::replace (float from, const string path) {
  return replace (double(from), path);
}

bool JsonTree::replace (int from, const string path) {
  AbstractObject* object = new ObjectFinalNumberInt (from);
  return replace (object, path);
}

bool JsonTree::replace (bool from, const string path) {
  AbstractObject* object = new ObjectFinalBool (from);
  return replace (object, path);
}

bool JsonTree::replace (string from, const string path) {
  AbstractObject* object = new ObjectFinalString (from);
  return replace (object, path);
}

bool JsonTree::replace (const char* from, const string path) {
  return replace (string(from), path);
}

bool JsonTree::replace (vector<double>& array, const string path) {
  return replace (createVec (array), path);
}

bool JsonTree::replace (vector<int>& array, const string path) {
  return replace (createVec (array), path);
}

bool JsonTree::replace (vector<bool>& array, const string path) {
  return replace (createVec (array), path);
}

bool JsonTree::replace (vector<string>& array, const string path) {
  return replace (createVec (array), path);
}

// SET

bool JsonTree::set(AbstractObject *newObj, const string path) {
  if (exist (path))
    return replace (newObj, path);
  else
    return top->add (path, newObj);
}

bool JsonTree::set (double from, const string path) {
  AbstractObject* object = new ObjectFinalNumberFloat (from);
  return set (object, path);
}

bool JsonTree::set (float from, const string path) {
  return set (double(from), path);
}

bool JsonTree::set (int from, const string path) {
  AbstractObject* object = new ObjectFinalNumberInt (from);
  return set (object, path);
}

bool JsonTree::set (bool from, const string path) {
  AbstractObject* object = new ObjectFinalBool (from);
  return set (object, path);
}

bool JsonTree::set (string value, const string path) {
  AbstractObject* object = new ObjectFinalString (value);
  return set (object, path);
}

bool JsonTree::set(const char *value, const string path) {
  return set (string(value), path);
}

bool JsonTree::set (vector<double>& array, const string path) {
  AbstractObject* object = createVec (array);
  return set (object, path);
}

bool JsonTree::set (vector<int>& array, const string path) {
  AbstractObject* object = createVec (array);
  return set (object, path);
}

bool JsonTree::set (vector<bool>& array, const string path) {
  AbstractObject* object = createVec (array);
  return set (object, path);
}

bool JsonTree::set (vector<string>& array, const string path) {
  AbstractObject* object = createVec (array);
  return set (object, path);
}

// ERASE

// Can't send empty string for security
bool JsonTree::erase (const string path) {
  smatch matcher;
  if (path == ".") {
    delete top;
    top = new ObjectMap();
  } else if (regex_search (path, matcher, lastTokenRgx)) {
      AbstractObject* father = top->get (matcher[1]);
      if (father != nullptr && father->getType() == MAP)
        return ((ObjectMap*)father)->erase (matcher[2]);
      return false;
  } else if (!path.empty()) {
    return ((ObjectMap*)top)->erase (path);
  }
  return false;
}

// REMOVE

bool JsonTree::remove (const string path) {
  smatch matcher;
  if (regex_search (path, matcher, lastTokenRgx)) {
      AbstractObject* father = top->get (matcher[1]);
      if (father != nullptr && father->getType() == MAP)
        return ((ObjectMap*)father)->remove (matcher[2]);
      return true;
  } else if (!path.empty())
    return ((ObjectMap*)top)->erase (path);
  return false;
}
