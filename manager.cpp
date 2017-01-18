#include "./manager.hpp"

using namespace json;

string const JsonTree::objectsTypesReverse[7] = { "Vector",
                                                  "Map",
                                                  "Final",
                                                  "Number_Float",
                                                  "Number_Int",
                                                  "String",
                                                  "Bool" };

regex JsonTree::lastTokenRgx = regex ("^(.*)(?:\\.)(.+)$");

JsonTree::JsonTree () :
  top (new ObjectMap())
{}

JsonTree::JsonTree (const JsonTree &tree, const string path) :
  top(new ObjectMap())
  {
    AbstractObject* aux = top;
    tree.copyFrom(aux, path);
    top = (ObjectMap*)aux;
  }

JsonTree::~JsonTree () {
  delete top;
}

bool JsonTree::copyFrom(AbstractObject*& obj, const string path) const {
  AbstractObject* from = nullptr;
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

bool JsonTree::set_union(JsonTree &tree) {
  return add(tree);
}

void JsonTree::getterError (const string path, AbstractObject* obj, int type) {
  cerr << "ERROR : ";
  if (obj == nullptr) {
    cerr << "Attempting to load the element " << path
         << " which does not exist." << endl;
  } else {
    cerr << "Attempting to load the element " << path
         << " as a "<< objectsTypesReverse[type] << " when is of type "
         << objectsTypesReverse[obj->getType()] << "." << endl;
  }
}

ObjectVector* JsonTree::createVec (const vector<double> &array) {
  ObjectVector* object = new ObjectVector ();
  for (double number : array) {
    object->insert("", new ObjectFinalNumberFloat (number));
  }
  return object;
}

ObjectVector* JsonTree::createVec (const vector<int> &array) {
  ObjectVector* object = new ObjectVector ();
  for (int number : array) {
    object->insert("", new ObjectFinalNumberInt (float(number)));
  }
  return object;
}

ObjectVector* JsonTree::createVec (const vector<bool> &array) {
  ObjectVector* object = new ObjectVector ();
  for (bool boolean : array) {
    object->insert("", new ObjectFinalBool (boolean));
  }
  return object;
}

ObjectVector* JsonTree::createVec (const vector<string> &array) {
  ObjectVector* object = new ObjectVector ();
  for (string word : array) {
    object->insert("", new ObjectFinalString (word));
  }
  return object;
}

vector <string> JsonTree::getKeys (const string path) {
  AbstractObject* obj = top->get(path);
  if (obj != nullptr && isMap(obj)) {
    return ((ObjectMap*)obj)->getKeys();
  } else {
    return vector<string> (0);
  }
}

int JsonTree::getSizeAt (const string path) {
  AbstractObject* obj = top->get(path);
  if (obj != nullptr && isVector(obj)) {
    return ((ObjectVector*)obj)->size();
  } else {
    getterError(path, obj, VECTOR);
    return -1;
  }
}

string JsonTree::toText (const bool uglify) {
  string txt;
  if (uglify)
    top->toTxtUgly(txt);
  else
    top->toTxt(txt, 0);
  return txt;
}

bool JsonTree::isNumber (const string path) {
  return (isFloat(path) || isInt(path));
}

bool JsonTree::isNumber (AbstractObject* const obj) {
  return (isFloat(obj) || isInt(obj));
}

bool JsonTree::isFloat (const string path) {
  return isFloat (top->get(path));
}

bool JsonTree::isFloat (AbstractObject* const obj) {
  return dynamic_cast<ObjectFinalNumberFloat*> (obj);
}

bool JsonTree::isInt (const string path) {
  return isInt (top->get(path));
}

bool JsonTree::isInt (AbstractObject* const obj) {
  return dynamic_cast<ObjectFinalNumberInt*> (obj);
}

bool JsonTree::isBool (const string path) {
  return isBool (top->get(path));
}

bool JsonTree::isBool (AbstractObject* const obj) {
  return dynamic_cast<ObjectFinalBool*> (obj);
}

bool JsonTree::isString (const string path) {
  return isString (top->get(path));
}

bool JsonTree::isString (AbstractObject* const obj) {
  return dynamic_cast<ObjectFinalString*> (obj);
}

bool JsonTree::isMap (const string path) {
   return isMap (top->get(path));
}

bool JsonTree::isMap (AbstractObject* const obj) {
   return dynamic_cast<ObjectMap*> (obj);
}

bool JsonTree::isVector (const string path) {
  return isVector (top->get(path));
}

bool JsonTree::isVector (AbstractObject* obj) {
  return dynamic_cast<ObjectVector*> (obj);
}

bool JsonTree::exist (const string path) {
  return top->get(path) != nullptr;
}

// FINALS

bool JsonTree::get (bool &to, const string path) {
  AbstractObject* obj = top->get(path);
  if (isBool(obj)) {
    to = ((ObjectFinalBool*)obj)->getContent();
    return true;
  }
  getterError(path, obj, BOOL);
  return false;
}

bool JsonTree::get (string &to, const string path) {
  AbstractObject* obj = top->get(path);
  if (isString(obj)) {
    to = ((ObjectFinalString*)obj)->getContent();
    return true;
  }
  getterError(path, obj, STRING);
  return false;
}

bool JsonTree::get (double &to, const string path) {
  AbstractObject* obj = top->get(path);
  if (isInt(obj)) {
    to = ((ObjectFinalNumberInt*)obj)->getContent();
    return true;
  }
  if (isFloat(obj)) {
    to = ((ObjectFinalNumberFloat*)obj)->getContent();
    return true;
  }
  getterError(path, obj, NUMBER_FLOAT);
  return false;
}

bool JsonTree::get (long long &to, const string path) {
  AbstractObject* obj = top->get(path);
  if (isInt(obj)) {
    to = ((ObjectFinalNumberInt*)obj)->getContent();
    return true;
  }
  if (isFloat(obj)) {
    to = ((ObjectFinalNumberFloat*)obj)->getContent();
    return true;
  }
  getterError(path, obj, NUMBER_INT);
  return false;
}

bool JsonTree::get (long &to, const string path) {
  long long aux;
  get (aux, path);
  to = aux;
}

bool JsonTree::get (int &to, const string path) {
  long long aux;
  get (aux, path);
  to = aux;
}

bool JsonTree::get (float &to, const string path) {
  double aux;
  get (aux, path);
  to = aux;
}

bool JsonTree::get (char &to, const string path) {
  AbstractObject* obj = top->get(path);
  if (isString(obj)) {
    to = ((ObjectFinalString*)obj)->getContent()[0];
    return true;
  }
  getterError(path, obj, STRING);
  return false;
}

// VECTORS

bool JsonTree::get (vector<double>& array, const string path) {
  AbstractObject* obj = top->get(path);
  if (isVector (obj)) {
    ObjectVector* vect = (ObjectVector*)obj;
    int size = vect->size();
    array.resize (size);
    for (int i = 0; i < size; i++) {
      if (isFloat((ObjectFinalNumberFloat*)vect->operator[](i)))
        array[i] = int(((ObjectFinalNumberFloat*)vect->operator[](i))->getContent());
      else if (isInt((ObjectFinalNumberInt*)vect->operator[](i)))
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
  if (isVector (obj)) {
    ObjectVector* vect = (ObjectVector*)obj;
    int size = vect->size();
    array.resize (size);
    for (int i = 0; i < size; i++) {
      if (isFloat((ObjectFinalNumberFloat*)vect->operator[](i)))
        array[i] = int(((ObjectFinalNumberFloat*)vect->operator[](i))->getContent());
      else if (isInt((ObjectFinalNumberInt*)vect->operator[](i)))
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
  if (isVector (obj)) {
    ObjectVector* vect = (ObjectVector*)obj;
    int size = vect->size();
    array.resize (size);
    for (int i = 0; i < size; i++) {
      if (isString((ObjectFinalString*)vect->operator[](i)))
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
  if (isVector (obj)) {
    ObjectVector* vect = (ObjectVector*)obj;
    int size = vect->size();
    array.resize (size);
    for (int i = 0; i < size; i++) {
      if (isBool((ObjectFinalBool*)vect->operator[](i)))
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

bool JsonTree::add (long long value, const string path) {
  AbstractObject* object = new ObjectFinalNumberInt (value);
  return top->add (path, object);
}

bool JsonTree::add (long value, const string path) {
  long long aux = value;
  return add (aux, path);
}

bool JsonTree::add (int value, const string path) {
  long long aux = value;
  return add (aux, path);
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

bool JsonTree::add (const vector<double> &array, const string path) {
  return top->add (path, createVec(array));
}

bool JsonTree::add (const vector<int> &array, const string path) {
  return top->add (path, createVec(array));
}

bool JsonTree::add (const vector<bool> &array, const string path) {
  return top->add (path, createVec(array));
}

bool JsonTree::add (const vector<string> &array, const string path) {
  return top->add (path, createVec(array));
}

bool JsonTree::addMap (const string path) {
  return top->add (path, new ObjectMap ());
}

bool JsonTree::addVector (const string path) {
  return top->add (path, new ObjectVector ());
}

bool JsonTree::add(const JsonTree &tree, const string from, const string path) {
  AbstractObject* obj = top->get(path);
  if (obj == nullptr) {
    if(tree.copyFrom(obj, from)) {
      return top->add(path, obj);
    } else {
       if (obj != nullptr)
        delete obj;
       return false;
    }
  } else if (isVector(obj)) {
    AbstractObject* aux = nullptr;
    if(tree.copyFrom(aux, from)) {
      return ((ObjectVector*)obj)->add("", aux);
    } else {
      if (obj != nullptr)
        delete obj;
      return false;
    }
  }
  return false;
}

// REPLACE

bool JsonTree::replace (AbstractObject *newObj, const string path) {
  smatch matcher;
  if (regex_search (path, matcher, lastTokenRgx)) {
      AbstractObject* father = top->get (matcher[1]);
      if (father != nullptr && isMap(father)) {
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

bool JsonTree::replace (long long from, const string path) {
  AbstractObject* object = new ObjectFinalNumberInt (from);
  return replace (object, path);
}

bool JsonTree::replace (long from, const string path) {
  long long aux = from;
  return replace (aux, path);
}

bool JsonTree::replace (int from, const string path) {
  long long aux = from;
  return replace (aux, path);
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

bool JsonTree::replace (const vector<double>& array, const string path) {
  return replace (createVec (array), path);
}

bool JsonTree::replace (const vector<int>& array, const string path) {
  return replace (createVec (array), path);
}

bool JsonTree::replace (const vector<bool>& array, const string path) {
  return replace (createVec (array), path);
}

bool JsonTree::replace (const vector<string>& array, const string path) {
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

bool JsonTree::set (long long from, const string path) {
  AbstractObject* object = new ObjectFinalNumberInt (from);
  return set (object, path);
}

bool JsonTree::set (long from, const string path) {
  long long aux = from;
  return set (aux, path);
}

bool JsonTree::set (int from, const string path) {
  long long aux = from;
  return set (aux, path);
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

bool JsonTree::set (const vector<double>& array, const string path) {
  AbstractObject* object = createVec (array);
  return set (object, path);
}

bool JsonTree::set (const vector<int>& array, const string path) {
  AbstractObject* object = createVec (array);
  return set (object, path);
}

bool JsonTree::set (const vector<bool>& array, const string path) {
  AbstractObject* object = createVec (array);
  return set (object, path);
}

bool JsonTree::set (const vector<string>& array, const string path) {
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
      if (father != nullptr && isMap(father))
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
      if (father != nullptr && isMap(father))
        return ((ObjectMap*)father)->remove (matcher[2]);
      return true;
  } else if (!path.empty())
    return ((ObjectMap*)top)->erase (path);
  return false;
}
