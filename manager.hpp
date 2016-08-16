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
  string objectsTypesReverse [6] = {"Vector",
                                    "Map",
                                    "Final",
                                    "Number",
                                    "String",
                                    "Bool"};
  ObjectMap* top;
  static regex tokenRgx;
  static regex numberRgx;
  static regex vectorAccessRgx;
  static regex mapAccessRgx;

  bool isType (AbstractObject* obj, int type);
  void getterError (string path, AbstractObject* obj, int type);

public:
  JsonTree (AbstractObject* root);
  vector <string> getKeys (string path);
  int getSizeAt (string path);

  bool copy (vector<string>& array, string path);
  bool copy (vector<bool>& array, string path);
  bool copy (vector<double>& array, string path);

  bool copy (bool& to, string path);
  bool copy (string& to, string path);
  bool copy (double& to, string path);
  bool copy (float& to, string path);
  bool copy (int& to, string path);

  bool isNumber (string path);
  bool isBool (string path);
  bool isString (string path);
  bool isMap (string path);
  bool isVector (string path);
  bool exist (string path);

  bool hasFailed () {}
  AbstractObject* insertObject (string path, AbstractObject* obj);
  bool addElement (string path, double value);
};

#endif
