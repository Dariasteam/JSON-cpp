#ifndef MANAGER_H
#define MANAGER_H

#include <vector>
#include <regex>
#include <string>
#include <iostream>

#include "object.hpp"

using namespace std;

namespace json {

class JsonTree {
private:
  string objectsTypesReverse [7] = { "Vector",
                                           "Map",
                                           "Final",
                                           "Number_Float",
                                           "Number_Int",
                                           "String",
                                           "Bool"};
  static regex lastTokenRgx;

  ObjectMap* top;


  bool isType (AbstractObject* obj, int type);
  void getterError (string path, AbstractObject* obj, int type);

  bool replace (AbstractObject* newObj, string path);
  bool set (AbstractObject* newObj, string path);

  static ObjectVector* createVec (vector<double>& array);
  static ObjectVector* createVec (vector<int>& array);
  static ObjectVector* createVec (vector<bool>& array);
  static ObjectVector* createVec (vector<string>& array);

  static AbstractObject* insertObject (string path, AbstractObject* obj);

public:
  JsonTree (AbstractObject* root);
  JsonTree ();

  vector <string> getKeys (string path);
  int getSizeAt (string path);

  // returns true if the path exist, has the appropiate type and the value is copied succesfully
  bool get (double& to, string path);
  bool get (float& to, string path);
  bool get (int& to, string path);
  bool get (bool& to, string path);
  bool get (string& to, string path);

  bool get (vector<double>& array, string path);
  bool get (vector<int>& array, string path);
  bool get (vector<bool>& array, string path);
  bool get (vector<string>& array, string path);

  bool isNumber (string path);
  bool isFloat (string path);
  bool isInt (string path);
  bool isBool (string path);
  bool isString (string path);
  bool isMap (string path);
  bool isVector (string path);
  bool exist (string path);
  int getType (string path);

  // returns true if doesn't exist and its sucessfully created
  bool add (double value, string path);
  bool add (float value, string path);
  bool add (int value, string path);
  bool add (bool value, string path);
  bool add (string value, string path);
  bool add (const char* value, string path);

  bool add (vector<double>& array, string path);
  bool add (vector<int>& array, string path);
  bool add (vector<bool>& array, string path);
  bool add (vector<string>& array, string path);

  bool addVector (string path);
  bool addMap (string map);

  // returns true if exist and its sucessfully updated
  bool replace (double from, string path);
  bool replace (float from, string path);
  bool replace (int from, string path);
  bool replace (bool from, string path);
  bool replace (string from, string path);
  bool replace (const char* from, string path);

  bool replace (vector<double>& array, string path);
  bool replace (vector<int>& array, string path);
  bool replace (vector<bool>& array, string path);
  bool replace (vector<string>& array, string path);

  // returns true if its sucessfully created or replaced if previously exist
  bool set (double value, string path);
  bool set (float value, string path);
  bool set (int value, string path);
  bool set (bool value, string path);
  bool set (string value, string path);
  bool set (const char* value, string path);

  bool set (vector<double>& array, string path);
  bool set (vector<int>& array, string path);
  bool set (vector<bool>& array, string path);
  bool set (vector<string>& array, string path);

  // return true if exist and its successfully removed, false if doesn't exist
  bool erase (string path);

  // return true if successfully removed or doesn't exist
  bool remove (string path);
  string toText ();

  bool getSubTree (JsonTree* tree, string path);
};

}

#endif
