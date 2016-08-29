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

  static regex lastElementRgx;

  bool isType (AbstractObject* obj, int type);
  void getterError (string path, AbstractObject* obj, int type);

  bool replace (AbstractObject* newObj, string path, string key);
  bool set (AbstractObject* newObj, string path, string key);

public:
  JsonTree (AbstractObject* root);
  vector <string> getKeys (string path);
  int getSizeAt (string path);

  bool copy (vector<string>& array, string path);
  bool copy (vector<bool>& array, string path);
  bool copy (vector<double>& array, string path);

  bool copy (string& to, string path);
  bool copy (bool& to, string path);
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

  // returns true if doesn't exist and its sucessfully created
  bool add (string path, double value);
  bool add (string path, string value);
  bool add (string path, bool value);

  bool addVector (string path);
  bool addMap (string map);

  // returns true if exist and its sucessfully updated
  bool replace (string from, string path);
  bool replace (bool from, string path);
  bool replace (double from, string path);

  // returns true if its sucessfully created or replaced if previously exist
  bool set (string value, string path);
  bool set (bool value, string path);
  bool set (double value, string path);



  string toText ();
};


#endif
