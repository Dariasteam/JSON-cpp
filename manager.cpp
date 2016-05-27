#include "./manager.hpp"

JsonTree::JsonTree (AbstractObject* root) :
  tokenRgx ("^(\\w+)(:?\\.)?"),
  numberRgx ("^(\\d+)(:?(?:\\w|\\s)*)"),
  vectorAccessRgx ("^(?:\\[(\\d)+(:?\\]))(:?\\.)?"),
  mapAccessRgx ("^(?:\\['(\\w)+(:?'\\]))(:?\\.)?")
  {
  top = (ObjectMap*)root;
}

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

double JsonTree::getNumber (string path) {
  AbstractObject* obj = top->get(path);
  if (obj != nullptr && obj->getType() == NUMBER) {
    return ((ObjectFinalNumber*)obj)->getContent();
  } else {
    getterError(path, obj, NUMBER);
    return -1;
  }
}

string JsonTree::getString (string path) {
  AbstractObject* obj = top->get(path);
  if (obj != nullptr && obj->getType() == STRING) {
    return ((ObjectFinalString*)obj)->getContent();
  } else {
    getterError(path, obj, STRING);
    return "";
  }
}

bool JsonTree::getBool (string path) {
  AbstractObject* obj = top->get(path);
  if (obj != nullptr && obj->getType() == BOOL) {
    return ((ObjectFinalBool*)obj)->getContent();
  } else {
    getterError(path, obj, BOOL);
    return false;
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

bool JsonTree::copyVector (string path, vector<double>& array) {
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
  } else {
    return false;
  }
}

bool JsonTree::copyVector (string path, vector<string>& array) {
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
  } else {
    return false;
  }
}

bool JsonTree::copyVector (string path, vector<bool>& array) {
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
  } else {
    return false;
  }
}

AbstractObject* JsonTree::insertObject(string path, AbstractObject *obj) {
  return nullptr;
}

bool JsonTree::addElement(string path, double value) {
  cout << "la energía comienza a fluir" << endl;
  top->add(path, "a");
  //insertObject (path, top);
  return false;
};
