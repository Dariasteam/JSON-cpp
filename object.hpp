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
  STRING,
  BOOL
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
  virtual void insert (string key, AbstractObject* obj) = 0;
  virtual int size () = 0;
};

class ObjectVector : public ObjectContainer{
private:
  vector <AbstractObject*> array;
public:
  ObjectVector () : ObjectContainer (VECTOR) {}
  AbstractObject* getContentAt (int index);
  inline void insert (string key, AbstractObject* obj) {array.push_back (obj); }
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
  inline vector <string> getKeys () { return keys; }
  AbstractObject* operator[](string key) { return hash[key]; }
  AbstractObject* operator[](int index) { return nullptr; }
  ObjectMap& get () { return *this; }
};

class ObjectFinal : public AbstractObject {
protected:
  ObjectFinal (int type) : AbstractObject (type) {}
  virtual ~ObjectFinal () = 0;
public:
  virtual void setValue (string value) = 0;
};

class ObjectFinalNumber : public ObjectFinal {
private:
  double number;
public:
  ObjectFinalNumber () : ObjectFinal (NUMBER) {}
  ObjectFinalNumber (double n) : ObjectFinal (NUMBER), number (n) {}
  inline double getContent () { return number; }
  ObjectFinalNumber& get () { return *this; }
  void setValue (string value);
};

class ObjectFinalString : public ObjectFinal {
private:
  string text;
public:
  ObjectFinalString () : ObjectFinal (STRING) {}
  ObjectFinalString (string s) : ObjectFinal (STRING), text (s) {}
  inline string getContent () { return text; }
  ObjectFinalString& get () { return *this; }
  void setValue (string value);
};

class ObjectFinalBool : public ObjectFinal {
private:
  bool boolean;
public:
  ObjectFinalBool() : ObjectFinal (BOOL), boolean (false) {}
  ObjectFinalBool(string s) : ObjectFinal (BOOL) { setValue(s); }
  inline bool getContent () { return boolean; }
  ObjectFinalBool& get () { return *this; }
  void setValue (string value);
};

#endif
