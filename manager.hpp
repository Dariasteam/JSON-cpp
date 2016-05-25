#ifndef MANAGER_H
#define MANAGER_H

#include <vector>
#include <regex>
#include <string>
#include <iostream>

#include "object.hpp"

using namespace std;

class JsonTree {
private:
  string objectsTypesReverse [5] = {"Vector",
                                    "Map",
                                    "Number",
                                    "String",
                                    "Bool"};
  ObjectMap* top;
  regex tokenRgx;
  regex numberRgx;

  bool isType (AbstractObject* obj, int type);
  AbstractObject* searchSon (string key, AbstractObject* obj);
  void getterError (string path, AbstractObject* obj, int type);
public:
  JsonTree (AbstractObject* root);
  AbstractObject* getObject (string path, AbstractObject* obj);
  vector <string> getKeys (string path);
  int getSizeAt (string path);

  double getNumber (string path);
  bool getBool (string path);
  string getString (string path);

  bool copyVector (string path, vector<double>& array);
  bool copyVector (string path, vector<bool>& array);
  bool copyVector (string path, vector<string>& array);

  bool isNumber (string path);
  bool isBool (string path);
  bool isString (string path);
  bool isMap (string path);
  bool isVector (string path);
  bool exist (string path);
};

#endif
