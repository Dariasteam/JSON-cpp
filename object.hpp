#ifndef ABSTRACTOBJECT_H
#define ABSTRACTOBJECT_H

#include <vector>
#include <string>
#include <map>

using namespace std;

enum ObjetsTypes {
  VECTOR,
  MAP,
  NUMBER,
  STRING
};

class AbstractObject {
private:
  unsigned short type;
public:
  AbstractObject (unsigned short t) : type (t) {}
  virtual ~AbstractObject () = 0;
  inline unsigned short getType () { return type; }
};

class ObjectVector : public AbstractObject{
private:
  vector <AbstractObject*> array;
public:
  ObjectVector () : AbstractObject (VECTOR) {}
  inline int size () { return array.size(); }
  inline void insert (AbstractObject* element) {array.push_back (element); }
  inline AbstractObject* getContentAt (int index);
};

class ObjectMap : public AbstractObject {
private:
  vector <string> keys;
  map <string, AbstractObject*> hash;
public:
  ObjectMap () : AbstractObject (MAP) {}
  void insert (string name, AbstractObject* obj);
  inline int size () { return hash.size(); }
  inline vector <string>& getKeys () { return keys; }
  inline AbstractObject* getContentAt (string key) { return hash[key]; }
};

class ObjectFinalNumber : public AbstractObject {
private:
  double number;
public:
  ObjectFinalNumber (double n) : AbstractObject (NUMBER), number (n) {}
  inline double getContent () { return number; }
};

class ObjectFinalString : public AbstractObject {
private:
  string text;
public:
  ObjectFinalString (string s) : AbstractObject (STRING), text (s) {}
  inline string getContent () { return text; }
};


#endif
