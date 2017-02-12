#include "./manager.hpp"

using namespace json;

std::regex JsonTree::lastTokenRgx = std::regex ("^(.*)(?:\\.)(.+)$");

JsonTree::JsonTree () :
  top (new ObjectMap())
{}

JsonTree::JsonTree (const JsonTree &tree, const std::string path) :
  top(new ObjectMap())
  {
    AbstractObject* aux = top;
    tree.copyFrom(aux, path);
    top = (ObjectMap*)aux;
  }

JsonTree::~JsonTree () {
  delete top;
}

bool JsonTree::copyFrom(AbstractObject*& obj, const std::string path) const {
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
    obj = from->getCopy();
    return true;
  }
}

bool JsonTree::set_union(JsonTree &tree) {
  return add(tree);
}

void JsonTree::getterError (const std::string path, AbstractObject* obj, const char* expectedType) {
  std::cerr << "ERROR : ";
  if (obj == nullptr) {
    std::cerr << "Attempting to load the element '" << path
         << "' which does not exist." << std::endl;
  } else {
    std::cerr << "Attempting to load the element '" << path
         << "' as a " << expectedType << " when is of type "
         << obj->getName() << "." << std::endl;
  }
}

AbstractObject* JsonTree::fabricate (const bool value) {
  return new ObjectFinalBool(value);
}

AbstractObject* JsonTree::fabricate (const std::string value) {
  return new ObjectFinalString(std::string(value));
}

AbstractObject* JsonTree::fabricate (const char value) {
  std::string a(&value);
  return new ObjectFinalString(a);
}

AbstractObject* JsonTree::fabricate (const char *value) {
  return new ObjectFinalString(value);
}

AbstractObject* JsonTree::fabricate (const double value) {
  return new ObjectFinalNumber(value);
}

AbstractObject* JsonTree::fabricate (const long long value) {
  return new ObjectFinalNumber(value);
}

AbstractObject* JsonTree::fabricate (const int value) {
  return new ObjectFinalNumber(value);
}

std::vector <std::string> JsonTree::getKeys (const std::string path) {
  AbstractObject* obj = top->get(path);
  if (isMap(obj)) {
    return ((ObjectMap*)obj)->getKeys();
  } else {
    return std::vector<std::string> (0);
  }
}

int JsonTree::getSizeAt (const std::string path) {
  AbstractObject* obj = top->get(path);  
  if (isVector(obj)) {
    return ((ObjectVector*)obj)->size();
  } else {
    getterError(path, obj, ObjectVector::name);
    return -1;
  }
}

std::string JsonTree::toText (const bool uglify, const std::string from) {
  std::string txt;
  AbstractObject* obj = top->get(from);
  if (obj == nullptr)
    obj = top;

  if (uglify)
    obj->toTxtUgly(txt);
  else
    obj->toTxt(txt, 0);
  return txt;
}

bool JsonTree::isNumber (const std::string path) {
  return isNumber (top->get(path));
}

bool JsonTree::isNumber (AbstractObject* const obj) {
  return dynamic_cast<ObjectFinalNumber*> (obj);
}

bool JsonTree::isBool (const std::string path) {
  return isBool (top->get(path));
}

bool JsonTree::isBool (AbstractObject* const obj) {
  return dynamic_cast<ObjectFinalBool*> (obj);
}

bool JsonTree::isString (const std::string path) {
  return isString (top->get(path));
}

bool JsonTree::isString (AbstractObject* const obj) {
  return dynamic_cast<ObjectFinalString*> (obj);
}

bool JsonTree::isMap (const std::string path) {
   return isMap (top->get(path));
}

bool JsonTree::isMap (AbstractObject* const obj) {
   return dynamic_cast<ObjectMap*> (obj);
}

bool JsonTree::isVector (const std::string path) {
  return isVector (top->get(path));
}

bool JsonTree::isVector (AbstractObject* const obj) {
  return dynamic_cast<ObjectVector*> (obj);
}

bool JsonTree::exist (const std::string path) {
  return top->get(path) != nullptr;
}

// FINALS

bool JsonTree::get (bool &to, const std::string path) {
  AbstractObject* obj = top->get(path);
  if (isBool(obj)) {
    to = ((ObjectFinalBool*)obj)->getContent();
    return true;
  }
  getterError(path, obj, ObjectFinalBool::name);
  return false;
}

bool JsonTree::get (std::string &to, const std::string path) {
  AbstractObject* obj = top->get(path);
  if (isString(obj)) {
    to = ((ObjectFinalString*)obj)->getContent();
    return true;
  }
  getterError(path, obj, ObjectFinalString::name);
  return false;
}

bool JsonTree::get (double &to, const std::string path) {
  AbstractObject* obj = top->get(path);
  if (isNumber(obj)) {
    to = ((ObjectFinalNumber*)obj)->getContent();
    return true;
  }
  getterError(path, obj, ObjectFinalNumber::name);
  return false;
}

bool JsonTree::get (long long &to, const std::string path) {
  AbstractObject* obj = top->get(path);
  if (isNumber(obj)) {
    to = ((ObjectFinalNumber*)obj)->getContent();
    return true;
  }
  getterError(path, obj, ObjectFinalNumber::name);
  return false;
}

bool JsonTree::get (long &to, const std::string path) {
  long long aux;
  if (get (aux, path)) {
    to = aux;
    return true;
  } else {
    return false;
  }
}

bool JsonTree::get (int &to, const std::string path) {
  long long aux;
  if (get (aux, path)) {
    to = aux;
    return true;
  } else {
    return false;
  }
}

bool JsonTree::get (float &to, const std::string path) {
  double aux;
  if (get (aux, path)) {
    to = aux;
    return true;
  } else {
    return false;
  }
}

bool JsonTree::get (char &to, const std::string path) {
  AbstractObject* obj = top->get(path);
  if (isString(obj)) {
    to = ((ObjectFinalString*)obj)->getContent()[0];
    return true;
  }
  getterError(path, obj, ObjectFinalString::name);
  return false;
}

// VECTORS

bool JsonTree::get (std::vector<double>& array, const std::string path) {
  AbstractObject* obj = top->get(path);
  if (isVector (obj)) {
    ObjectVector* vect = (ObjectVector*)obj;
    int size = vect->size();
    array.resize (size);
    for (int i = 0; i < size; i++) {
      if (isNumber((ObjectFinalNumber*)vect->operator[](i)))
        array[i] = int(((ObjectFinalNumber*)vect->operator[](i))->getContent());
      else {
        getterError(path, obj, ObjectVector::name);
        return false;
      }
    }
    return true;
  }
  getterError(path, obj, ObjectVector::name);
  return false;
}

bool JsonTree::get (std::vector<int>& array, const std::string path) {
  AbstractObject* obj = top->get(path);
  if (isVector (obj)) {
    ObjectVector* vect = (ObjectVector*)obj;
    int size = vect->size();
    array.resize (size);
    for (int i = 0; i < size; i++) {
      if (isNumber((ObjectFinalNumber*)vect->operator[](i)))
        array[i] = int(((ObjectFinalNumber*)vect->operator[](i))->getContent());
      else {
        getterError(path, obj, ObjectVector::name);
        return false;
      }
    }
    return true;
  }
  getterError(path, obj, ObjectVector::name);
  return false;
}

bool JsonTree::get (std::vector<std::string>& array, const std::string path) {
  AbstractObject* obj = top->get(path);
  if (isVector (obj)) {
    ObjectVector* vect = (ObjectVector*)obj;
    int size = vect->size();
    array.resize (size);
    for (int i = 0; i < size; i++) {
      if (isString((ObjectFinalString*)vect->operator[](i)))
        array[i] = ((ObjectFinalString*)vect->operator[](i))->getContent();
      else {
        getterError(path, obj, ObjectVector::name);
        return false;
      }
    }
    return true;
  }
  getterError(path, obj, ObjectVector::name);
  return false;
}

bool JsonTree::get (std::vector<bool>& array, const std::string path) {
  AbstractObject* obj = top->get(path);
  if (isVector (obj)) {
    ObjectVector* vect = (ObjectVector*)obj;
    int size = vect->size();
    array.resize (size);
    for (int i = 0; i < size; i++) {
      if (isBool((ObjectFinalBool*)vect->operator[](i)))
        array[i] = ((ObjectFinalBool*)vect->operator[](i))->getContent();
      else {
        getterError(path, obj, ObjectVector::name);
        return false;
      }
    }
    return true;
  }
  getterError(path, obj, ObjectVector::name);
  return false;
}

AbstractObject* JsonTree::insertObject(const std::string path, AbstractObject *obj) {
  return nullptr;
}

// ADD

bool JsonTree::add (double value, const std::string path) {
  AbstractObject* object = new ObjectFinalNumber (value);
  return top->add (path, object);
};

bool JsonTree::add (long long value, const std::string path) {
  AbstractObject* object = new ObjectFinalNumber (value);
  return top->add (path, object);
}

bool JsonTree::add (long value, const std::string path) {
  long long aux = value;
  return add (aux, path);
}

bool JsonTree::add (int value, const std::string path) {
  long long aux = value;
  return add (aux, path);
};

bool JsonTree::add (bool value, const std::string path) {
  AbstractObject* object = new ObjectFinalBool (value);
  return top->add (path, object);
};

bool JsonTree::add (std::string value, const std::string path) {
  AbstractObject* object = new ObjectFinalString (value);
  return top->add (path, object);
};

bool JsonTree::add (float value, const std::string path) {
  return add (double(value), path);
};

bool JsonTree::add (const char* value, const std::string path) {
  return add (std::string(value), path);
};

bool JsonTree::add (const char& value, const std::string path) {
  AbstractObject* object = new ObjectFinalString (value);
  return top->add (path, object);
};

bool JsonTree::addMap (const std::string path) {
  return top->add (path, new ObjectMap ());
}

bool JsonTree::addVector (const std::string path) {
  return top->add (path, new ObjectVector ());
}

bool JsonTree::add(const JsonTree &tree, const std::string from, const std::string path) {
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

bool JsonTree::replace (AbstractObject *newObj, const std::string path) {
  std::smatch matcher;
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

bool JsonTree::replace (double from, const std::string path) {
  AbstractObject* object = new ObjectFinalNumber (from);
  return replace (object, path);
}

bool JsonTree::replace (float from, const std::string path) {
  return replace (double(from), path);
}

bool JsonTree::replace (long long from, const std::string path) {
  AbstractObject* object = new ObjectFinalNumber (from);
  return replace (object, path);
}

bool JsonTree::replace (long from, const std::string path) {
  long long aux = from;
  return replace (aux, path);
}

bool JsonTree::replace (int from, const std::string path) {
  long long aux = from;
  return replace (aux, path);
}

bool JsonTree::replace (bool from, const std::string path) {
  AbstractObject* object = new ObjectFinalBool (from);
  return replace (object, path);
}

bool JsonTree::replace (std::string from, const std::string path) {
  AbstractObject* object = new ObjectFinalString (from);
  return replace (object, path);
}

bool JsonTree::replace (const char* from, const std::string path) {
  return replace (std::string(from), path);
}

bool JsonTree::replace (char from, const std::string path) {
  AbstractObject* object = new ObjectFinalString (from);
  return replace (object, path);
}

// SET

bool JsonTree::set(AbstractObject *newObj, const std::string path) {
  if (exist (path))
    return replace (newObj, path);
  else
    return top->add (path, newObj);
}

bool JsonTree::set (double from, const std::string path) {
  AbstractObject* object = new ObjectFinalNumber (from);
  return set (object, path);
}

bool JsonTree::set (float from, const std::string path) {
  return set (double(from), path);
}

bool JsonTree::set (long long from, const std::string path) {
  AbstractObject* object = new ObjectFinalNumber (from);
  return set (object, path);
}

bool JsonTree::set (long from, const std::string path) {
  long long aux = from;
  return set (aux, path);
}

bool JsonTree::set (int from, const std::string path) {
  long long aux = from;
  return set (aux, path);
}

bool JsonTree::set (bool from, const std::string path) {
  AbstractObject* object = new ObjectFinalBool (from);
  return set (object, path);
}

bool JsonTree::set (std::string value, const std::string path) {
  AbstractObject* object = new ObjectFinalString (value);
  return set (object, path);
}

bool JsonTree::set(const char *value, const std::string path) {
  return set (std::string(value), path);
}

bool JsonTree::set(char value, const std::string path) {
  AbstractObject* object = new ObjectFinalString (value);
  return set (object, path);
}

// ERASE

// Can't send empty string for security
bool JsonTree::erase (const std::string path) {
  std::smatch matcher;
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

bool JsonTree::remove (const std::string path) {
  std::smatch matcher;
  if (regex_search (path, matcher, lastTokenRgx)) {
      AbstractObject* father = top->get (matcher[1]);
      if (father != nullptr && isMap(father))
        return ((ObjectMap*)father)->remove (matcher[2]);
      return true;
  } else if (!path.empty())
    return ((ObjectMap*)top)->erase (path);
  return false;
}
