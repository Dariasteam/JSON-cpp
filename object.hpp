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
protected:
  AbstractObject (unsigned short t) : type (t) {}
  virtual ~AbstractObject () = 0;
public:
  inline unsigned short getType () { return type; }
};

class ObjectContainer : public AbstractObject {
protected:
  ObjectContainer (int type) : AbstractObject (type) {}
  ~ObjectContainer () = 0;
public:
  virtual const AbstractObject* operator[](int index) = 0;
  virtual const AbstractObject* operator[](string key) = 0;
};

class ObjectVector : public ObjectContainer{
private:
  vector <AbstractObject*> array;
public:
  ObjectVector () : ObjectContainer (VECTOR) {}
  AbstractObject* getContentAt (int index);
  inline void insert (AbstractObject* element) {array.push_back (element); }
  inline int size () { return array.size(); }
  AbstractObject* operator[](int index);
  AbstractObject* operator[](string key) { return nullptr; }
  ObjectVector& get () { return *this; }
};

class ObjectMap : public ObjectContainer {
private:
  vector <string> keys;
  map <string, AbstractObject*> hash;
public:
  ObjectMap () : ObjectContainer (MAP) {}
  void insert (string key, AbstractObject* obj);
  inline int size () { return hash.size(); }
  inline vector <string>& getKeys () { return keys; }
  AbstractObject* operator[](string key) { return hash[key]; }
  AbstractObject* operator[](int index) { return nullptr; }
  ObjectMap& get () { return *this; }
};

class ObjectFinal : public AbstractObject {
protected:
  ObjectFinal (int type) : AbstractObject (type) {}
  virtual ~ObjectFinal () = 0;
};

class ObjectFinalNumber : public ObjectFinal {
private:
  double number;
public:
  ObjectFinalNumber (double n) : ObjectFinal (NUMBER), number (n) {}
  inline double getContent () { return number; }
  ObjectFinalNumber& get () { return *this; }
};

class ObjectFinalString : public ObjectFinal {
private:
  string text;
public:
  ObjectFinalString (string s) : ObjectFinal (STRING), text (s) {}
  inline string getContent () { return text; }
  ObjectFinalString& get () { return *this; }
};


#endif
