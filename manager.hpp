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

  static ObjectVector* createVec (const vector<double>& array);
  static ObjectVector* createVec (const vector<int>& array);
  static ObjectVector* createVec (const vector<bool>& array);
  static ObjectVector* createVec (const vector<string>& array);

  static AbstractObject* insertObject (const string path, AbstractObject* obj);

public:
  JsonTree (AbstractObject* root);
  JsonTree ();
  JsonTree (const JsonTree& tree, const string path = "");
  ~JsonTree ();
  inline void setTop (AbstractObject* root) { delete top; top = (ObjectMap*)root; }

  vector <string> getKeys (const string path);
  int getSizeAt (const string path);

  // returns if the path exist and its content is succesfully copied in obj.
  // Obj previous content is deleted if the copy is valid, otherwise is not modified
  bool copyFrom (AbstractObject*& obj, const string path) const;

  // returns if this object is successfuly transformed to the union between tree and itself
  bool set_union (JsonTree& tree);

  //- returns true if the path exist, has the appropiate type and the value is succesfully copied
  bool get (double& to,       const string path);
  bool get (float& to,        const string path);
  bool get (int& to,          const string path);
  bool get (long& to,         const string path);
  bool get (long long& to,    const string path);
  bool get (bool& to,         const string path);
  bool get (string& to,       const string path);
  bool get (char& to,         const string path);

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
  bool add (const double value,      const string path);
  bool add (const float value,       const string path);
  bool add (const int value,         const string path);
  bool add (const long value,        const string path);
  bool add (const long long value,   const string path);
  bool add (const bool value,        const string path);
  bool add (const string value,      const string path);
  bool add (const char* value,       const string path);

  bool add (const vector<double>& array, const string path);
  bool add (const vector<int>& array,    const string path);
  bool add (const vector<bool>& array,   const string path);
  bool add (const vector<string>& array, const string path);

  bool addVector (const string path);
  bool addMap (const string path);

  // returns true if successfully copied the elements indicated in 'from' in the path.
  // The path mustn't exist previously
  bool add (const JsonTree& tree, const string from = "", const string path = "");

  //- returns true if exist and its sucessfully updated
  bool replace (const double from,      const string path);
  bool replace (const float from,       const string path);
  bool replace (const int from,         const string path);
  bool replace (const long from,        const string path);
  bool replace (const long long from,   const string path);
  bool replace (const bool from,        const string path);
  bool replace (const string from,      const string path);
  bool replace (const char* from,       const string path);

  bool replace (const vector<double>& array, const string path);
  bool replace (const vector<int>& array,    const string path);
  bool replace (const vector<bool>& array,   const string path);
  bool replace (const vector<string>& array, const string path);

  //- returns true if its sucessfully created or replaced if previously exist
  bool set (const double value,      const string path);
  bool set (const float value,       const string path);
  bool set (const int value,         const string path);
  bool set (const long value,        const string path);
  bool set (const long long value,   const string path);
  bool set (const bool value,        const string path);
  bool set (const string value,      const string path);
  bool set (const char* value,       const string path);

  bool set (const vector<double>& array, const string path);
  bool set (const vector<int>& array,    const string path);
  bool set (const vector<bool>& array,   const string path);
  bool set (const vector<string>& array, const string path);

  //- return true if exist and its successfully removed, false if doesn't exist
  bool erase (const string path);

  //- return true if successfully removed or doesn't exist
  bool remove (const string path);
  string toText (const bool uglify = false);

};

}

#endif
