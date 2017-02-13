#include "./object.hpp"

using namespace json;

const char* const ObjectVector::name = "VECTOR";
const char* const ObjectMap::name = "MAP";
const char* const ObjectFinalBool::name = "BOOL";
const char* const ObjectFinalString::name = "STRING";
const char* const ObjectFinalNumber::name = "NUMBER";

void AbstractObject::txtIndent(std::string &txt, int indentLvl) {
  for (int i = 0; i < indentLvl; i++)
    txt.append(INDENT);
}

AbstractObject* ObjectMap::getCopy () {
  return new ObjectMap (*this);
}

AbstractObject* ObjectVector::getCopy () {
  return new ObjectVector (*this);
}

AbstractObject* ObjectFinalBool::getCopy () {
  return new ObjectFinalBool (*this);
}

AbstractObject* ObjectFinalNumber::getCopy () {
  return new ObjectFinalNumber (*this);
}

AbstractObject* ObjectFinalString::getCopy () {
  return new ObjectFinalString (*this);
}

ObjectFinalBool::ObjectFinalBool (const ObjectFinalBool &obj) :  
  boolean(obj.getContent())
{}

ObjectFinalNumber::ObjectFinalNumber (const ObjectFinalNumber &obj) :
  number(obj.getContent())
{}

ObjectFinalString::ObjectFinalString (const ObjectFinalString &obj) :  
  text(obj.getContent())
{}

ObjectMap::ObjectMap (const ObjectMap &obj) :  
  keys(obj.getKeys())
  {
    for (std::string key : keys) {
      hash[key] = obj.getContent().at(key)->getCopy ();
    }
  }

ObjectVector::ObjectVector (const ObjectVector &obj) :
  array(obj.size())
  {
    for (int i = 0; i < obj.size(); i++){
      array[i] = obj.getContent()[i]->getCopy ();
    }
  }

void ObjectFinalBool::replaceValue (std::string value) {
  if (value == "true")
    boolean = true;
  else
    boolean = false;
}

void ObjectFinalNumber::replaceValue (std::string value) {
  number = stod (value);
}

void ObjectFinalString::replaceValue (std::string value) {
  text = value;
}

AbstractObject* ObjectVector::operator[] (unsigned index) {
  if (index >= array.size()) // changed to unsigned
    return nullptr;
  else
    return array [index];
}

AbstractObject* ObjectMap::operator[](std::string key) {  
  auto it = hash.find(key);
  if (it != hash.end())
    return it->second;
  else
    return nullptr;
}

bool ObjectVector::insert (std::string key, AbstractObject* obj) {
  if (key.empty()) {
    array.push_back (obj);
    return true;
  } else {
    delete obj;
    return false;
  }
}

bool ObjectMap::insert (std::string key, AbstractObject* obj) {
  if (!key.empty() && !hash.count (key)) {
    keys.push_back (key);
    hash.insert(std::pair <std::string, AbstractObject*> (key, obj));
    return true;
  } else {
    delete obj;
    return false;
  }
}

bool ObjectMap::replace (std::string key, AbstractObject* obj) {
  if (!hash.count (key))
    return false;
  keys.erase(std::remove(keys.begin(), keys.end(), key), keys.end());
  hash.erase (key);
  return insert (key, obj);
}

bool ObjectMap::set (std::string key, AbstractObject* obj) {
  if (!hash.count (key))
    return insert (key, obj);
  else
    return replace (key, obj);
}

AbstractObject* ObjectVector::get (std::string path) {  
  if (path.size() == 0)
    return this;
  std::string key = pathSplitter(path);
  if (!key.empty()) {
    int index;
    try {
      index = stoi (key);
    }
    catch (std::invalid_argument e) {
      return nullptr;
    }
    AbstractObject* son = operator[](index);
    if (son != nullptr) {
      return son->get (path);
    }
  }
  return nullptr;
}

AbstractObject* ObjectMap::get (std::string path) {  
  if (path.empty() || (path.size() == 1 &&  path[0] == '.'))
    return this;
  std::string key = pathSplitter(path);
  if (!key.empty()) {
    AbstractObject* son = operator[](key);
    if (son != nullptr)
      return son->get (path);
  }
  return nullptr;
}

AbstractObject* ObjectFinal::get (std::string path) {
  if (path.size () == 0)
    return this;
  else
    return nullptr;
}

bool ObjectMap::remove (std::string key) {
  AbstractObject* aux = operator[](key);
  if (aux == nullptr)
    return true;
  keys.erase(std::remove(keys.begin(), keys.end(), key), keys.end());
  hash.erase (key);
  delete aux;
  return true;
}

bool ObjectMap::erase (std::string key) {
  if (!hash.count (key))
    return false;
  else
    return remove (key);
}

std::string ObjectVector::pathSplitter(std::string &path) {
  int i = 0;
  std::string key;
  if (path[0] == '.') {
    path.erase(0, 1);
  }
  if (!path.empty() && path[0] == '[') {
    i++;
    while (i < path.size() && path[i] != ']'){
      key.push_back(path[i]);
      i++;
    }
  } else {
    while (i < path.size() && !(path[i] == '.')){
      key.push_back(path[i]);
      i++;
    }
  }
  path.erase(0, i + 1);
  return key;
}

std::string ObjectMap::pathSplitter(std::string &path) {
  int i = 0;
  std::string key;
  if (path[0] == '.') {
    path.erase(0, 1);
  }
  while (i < path.size() && !(path[i] == '.')){
    key.push_back(path[i]);
    i++;
  }
  path.erase(0, i + 1);
  return key;
}

bool ObjectMap::add (std::string path, AbstractObject* obj) {  
  std::string key = pathSplitter(path);
  std::string oldPath = path;
  if (!key.empty()) {
    AbstractObject* son = operator[](key);
    std::string newPath = path;
    path = oldPath;
    if (son != nullptr) {
      if (!newPath.empty() || dynamic_cast<ObjectVector*>(son)) {
        return son->add (newPath, obj);
      } else {
        delete obj;
        return false;
      }
    } else {
      if (!newPath.empty()) {
        son = new ObjectMap();
        if (insert (key, son))
          return son->add (newPath, obj);
        else
          return false;
      } else {
        return insert (key, obj);
      }
    }
  }
  delete obj;
  return false;
}

bool ObjectVector::add (std::string path, AbstractObject* obj) {
  AbstractObject* son;
  std::string key = pathSplitter(path);
  std::string oldPath = path;
  if (!key.empty()) {
    int index;
    try {
      index = stoi (key);
    }
    catch (std::invalid_argument e) {
      return false;
    }
    son = operator[](index);
    std::string newPath = path;
    path = oldPath;
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

bool ObjectFinal::add (std::string path, AbstractObject* obj) {
  delete obj;
  return false;
}

bool ObjectVector::toTxt (std::string& txt, int indentLvl) {
  txt.append("[");
  indentLvl++;
  AbstractObject* obj = this;
  bool newLine = false;
  txt.append(END_LINE);
  txtIndent (txt, indentLvl);
  for (int i = 0; i < size(); i++) {            
    obj = operator[](i);
    newLine = obj->toTxt(txt, indentLvl);
    txt.append(COMMA);
    if (newLine) {
      txt.append(END_LINE);
      txtIndent (txt, indentLvl);
    } else {
      txt.append(SPACE);
    }
  }
  if (size() > 0)
    txt.pop_back();
  indentLvl--;
  txt.append(END_LINE);
  txtIndent (txt, indentLvl);
  txt.append("]");
  return true;
}

bool ObjectMap::toTxt (std::string& txt, int indentLvl) {
  txt.append("{");
  indentLvl++;  
  for (std::string key : getKeys()) {
    txt.append(END_LINE);
    txtIndent (txt, indentLvl);
    txt.append(QUOTE).append(key).append(QUOTE).append(SPACE).append(POINTS).append(SPACE);
    operator[](key)->toTxt(txt, indentLvl);
    txt.append(COMMA);    
  }
  if (getKeys().size() > 0)
    txt.pop_back();
  indentLvl--;
  txt.append(END_LINE);
  txtIndent (txt, indentLvl);
  txt.append("}");
  return true;
}

bool ObjectFinalBool::toTxt (std::string& txt, int indentLvl) {
  if (getContent())
    txt.append("true");
  else
    txt.append("false");
  return false;
}

typedef std::numeric_limits< double > dbl;
bool ObjectFinalNumber::toTxt (std::string& txt, int indentLvl) {
  double intpart;
  std::ostringstream strs;
  strs << std::setprecision(dbl::max_digits10) << getContent() << std::flush;
  txt.append(strs.str());
  if(std::modf(getContent(), &intpart) == 0.0)
    return false;
  else
    return true;
}

bool ObjectFinalString::toTxt (std::string& txt, int indentLvl) {
  txt.append(QUOTE).append(getContent()).append(QUOTE);
  if (getContent().size() > 25)
    return true;
  else
    return false;
}

//* Ugly

void ObjectVector::toTxtUgly (std::string& txt) {
  txt.append("[");
  AbstractObject* obj;
  for (int i = 0; i < size(); i++) {
    obj = operator[](i);
    obj->toTxtUgly(txt);
    txt.append(COMMA);
  }
  if (size() > 0)
    txt.pop_back();
  txt.append("]");
}

void ObjectMap::toTxtUgly (std::string& txt) {
  txt.append("{");
  for (int i = 0; i < getKeys().size(); i++) {
    txt.append(QUOTE).append(getKeys()[i]).append(QUOTE).append(POINTS);
    operator[](getKeys()[i])->toTxtUgly(txt);
    txt.append(COMMA);
  }
  if (getKeys().size() > 0)
    txt.pop_back();
  txt.append("}");
}

void ObjectFinalBool::toTxtUgly (std::string& txt) {
  toTxt(txt, 0);
}

void ObjectFinalNumber::toTxtUgly (std::string& txt) {
  toTxt(txt, 0);
}

void ObjectFinalString::toTxtUgly (std::string& txt) {
  toTxt(txt, 0);
}


ObjectVector::~ObjectVector () {
  for (auto element : array)
    delete element;
  array.clear();
}

ObjectMap::~ObjectMap () {
  for (auto element : hash)
    delete element.second;
  hash.clear();
}

ObjectFinalBool::~ObjectFinalBool () {}

ObjectFinalNumber::~ObjectFinalNumber () {}

ObjectFinalString::~ObjectFinalString () {}

AbstractObject::~AbstractObject() {}

ObjectContainer::~ObjectContainer() {}

ObjectFinal::~ObjectFinal() {}


