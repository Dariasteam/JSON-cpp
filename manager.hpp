#ifndef MANAGER_H
#define MANAGER_H

#include <vector>
#include <regex>

#include "object.hpp"

using namespace std;

class JsonTree {
private:
  ObjectMap* top;
  regex tokenRgx;

  bool isType (string path, int type);
  AbstractObject* searchSon (string key, AbstractObject* obj);
public:
  JsonTree (AbstractObject* root);
  AbstractObject* getObject (string path, AbstractObject* obj);
  vector <string> getKeys (string path);
  int getSizeAt (string path);

  double getNumber (string path);
  bool getBool (string path);
  string getString (string path);

  bool isNumber (string path);
  bool isBool (string path);
  bool isString (string path);
  bool isMap (string path);
  bool isVector (string path);
  bool exist (string path);
};

#endif
