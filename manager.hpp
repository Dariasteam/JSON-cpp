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

  static regex lastTokenRgx;

  bool isType (AbstractObject* obj, int type);
  void getterError (string path, AbstractObject* obj, int type);

  bool replace (AbstractObject* newObj, string path);
  bool set (AbstractObject* newObj, string path);

public:
  JsonTree (AbstractObject* root);
  JsonTree ();
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

  // returns true if doesn't exist and its sucessfully created
  bool add (double value, string path);
  bool add (float value, string path);
  bool add (int value, string path);
  bool add (bool value, string path);
  bool add (string value, string path);
  bool add (const char* value, string path);

  bool addVector (string path);
  bool addMap (string map);

  // returns true if exist and its sucessfully updated
  bool replace (double from, string path);
  bool replace (float from, string path);
  bool replace (int from, string path);
  bool replace (bool from, string path);
  bool replace (string from, string path);
  bool replace (const char* from, string path);

  // returns true if its sucessfully created or replaced if previously exist
  bool set (double value, string path);
  bool set (float value, string path);
  bool set (int value, string path);
  bool set (bool value, string path);
  bool set (string value, string path);
  bool set (const char* value, string path);

  // return true if exist and its successfully removed, false if doesn't exist
  bool erase (string path);

  // return true if successfully removed or doesn't exist
  bool remove (string path);

  AbstractObject* insertObject (string path, AbstractObject* obj);
  string toText ();
};


#endif
