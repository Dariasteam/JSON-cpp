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
                                     "Bool" };
  static regex lastTokenRgx;

  ObjectMap* top;

  bool isType (AbstractObject* obj, int type);
  void getterError (const string path, AbstractObject* obj, int type);

  bool replace (AbstractObject* newObj, const string path);
  bool set (AbstractObject* newObj,     const string path);

  static ObjectVector* createVec (vector<double>& array);
  static ObjectVector* createVec (vector<int>& array);
  static ObjectVector* createVec (vector<bool>& array);
  static ObjectVector* createVec (vector<string>& array);

  static AbstractObject* insertObject (const string path, AbstractObject* obj);

public:
  JsonTree (AbstractObject* root);
  JsonTree ();
  ~JsonTree ();

  vector <string> getKeys (const string path);
  int getSizeAt (const string path);

  ObjectMap* getCopy ();
  inline void setTop (AbstractObject* root) { delete top; top = (ObjectMap*)root; }

  //- returns true if the path exist, has the appropiate type and the value is copied succesfully
  bool get (double& to, const string path);
  bool get (float& to,  const string path);
  bool get (int& to,    const string path);
  bool get (bool& to,   const string path);
  bool get (string& to, const string path);

  bool get (vector<double>& array, const string path);
  bool get (vector<int>& array,    const string path);
  bool get (vector<bool>& array,   const string path);
  bool get (vector<string>& array, const string path);

  //- returns if the element on the path exist and is of the type
  bool isNumber (const string path);
  bool isFloat  (const string path);
  bool isInt    (const string path);
  bool isBool   (const string path);
  bool isString (const string path);
  bool isMap    (const string path);
  bool isVector (const string path);
  bool exist    (const string path);

  //- returns the type
  int getType (const string path);

  //- returns true if doesn't exist and its sucessfully created
  bool add (double value,      const string path);
  bool add (float value,       const string path);
  bool add (int value,         const string path);
  bool add (bool value,        const string path);
  bool add (string value,      const string path);
  bool add (const char* value, const string path);

  bool add (vector<double>& array, const string path);
  bool add (vector<int>& array,    const string path);
  bool add (vector<bool>& array,   const string path);
  bool add (vector<string>& array, const string path);

  bool addVector (const string path);
  bool addMap (string map);

  // returns true if succesfully copied the tree in the path
  bool add (JsonTree& tree, const string path);

  // returns true if successfully copied the elements indicated in 'from' in the path
  bool add (JsonTree& tree, string from, const string path);

  //- returns true if exist and its sucessfully updated
  bool replace (double from,      const string path);
  bool replace (float from,       const string path);
  bool replace (int from,         const string path);
  bool replace (bool from,        const string path);
  bool replace (string from,      const string path);
  bool replace (const char* from, const string path);

  bool replace (vector<double>& array, const string path);
  bool replace (vector<int>& array,    const string path);
  bool replace (vector<bool>& array,   const string path);
  bool replace (vector<string>& array, const string path);

  //- returns true if its sucessfully created or replaced if previously exist
  bool set (double value,      const string path);
  bool set (float value,       const string path);
  bool set (int value,         const string path);
  bool set (bool value,        const string path);
  bool set (string value,      const string path);
  bool set (const char* value, const string path);

  bool set (vector<double>& array, const string path);
  bool set (vector<int>& array,    const string path);
  bool set (vector<bool>& array,   const string path);
  bool set (vector<string>& array, const string path);

  //- return true if exist and its successfully removed, false if doesn't exist
  bool erase (const string path);

  //- return true if successfully removed or doesn't exist
  bool remove (const string path);
  string toText ();

};

}

#endif
