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
public:
  AbstractObject* searchSon (string key, AbstractObject* obj);
  AbstractObject* getObjectAt (string path, AbstractObject* obj);
  JsonTree (AbstractObject* root);
  vector <string> getKeysAt (string path);
  int getSizeAt (string path);
  pair<double, bool> getNumberAt (string path);
  pair<bool, bool> getBoolAt (string path);
  pair<string, bool> getStringAt (string path);
};

#endif
