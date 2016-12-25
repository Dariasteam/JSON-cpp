#ifndef ABSTRACTOBJECT_H
#define ABSTRACTOBJECT_H

#include <vector>
#include <string>
#include <map>
#include <regex>
#include <iomanip>
#include <iostream>

using namespace std;

#define END_LINE "\n"
#define INDENT "  "
#define QUOTE "\""
#define POINTS ":"
#define SPACE " "
#define COMMA ","

namespace json {

enum ObjetsTypes {
  VECTOR,
  MAP,
  // used for comparisons, if (getType() > FINAL) -> is a final object
  FINAL,
  NUMBER_FLOAT,
  NUMBER_INT,
  STRING,
  BOOL
};

class AbstractObject {
private:
  unsigned short type;
protected:
  AbstractObject (unsigned short t) : type (t) {}
public:
  virtual ~AbstractObject () = 0;
  inline unsigned short getType () const { return type; }
  virtual AbstractObject* get (string path) = 0;
  virtual bool add (string path, AbstractObject* obj) = 0;
  virtual void toTxt (string &txt, int indentLvl) = 0;
  virtual void toTxtUgly (string &txt) = 0;
  void txtIndent (string& txt, int indentLvl);

  static AbstractObject* copy (const AbstractObject* obj);
};

class ObjectContainer : public AbstractObject {
protected:
  ObjectContainer (int type) : AbstractObject (type) {}
  ~ObjectContainer () = 0;
public:
  virtual const AbstractObject* operator[](unsigned index) { return nullptr; }
  virtual const AbstractObject* operator[](string key) { return nullptr; }
  virtual bool insert (string key, AbstractObject* obj) = 0;
};

class ObjectVector : public ObjectContainer{
private:
  vector <AbstractObject*> array;
  static regex tokenRgx;
public:
  ~ObjectVector ();
  ObjectVector (const ObjectVector& obj);
  ObjectVector () : ObjectContainer (VECTOR) {}
  AbstractObject* getContentAt (int indeox);

  bool insert (string key, AbstractObject* obj);

  inline int size () const { return array.size(); }
  inline const vector<AbstractObject*>& getContent () const { return array; }

  AbstractObject* operator[](unsigned index);
  AbstractObject* get (string path);
  bool add (string path, AbstractObject* obj);
  void toTxt (string &txt, int indentLvl);
  void toTxtUgly (string &txt);
};

class ObjectMap : public ObjectContainer {
private:
  vector <string> keys;
  map <string, AbstractObject*> hash;
  static regex tokenRgx;
public:
  ~ObjectMap ();
  ObjectMap (const ObjectMap& obj);
  ObjectMap () : ObjectContainer (MAP) {}

  inline int size () const { return hash.size(); }
  inline const vector <string>& getKeys () const { return keys; }
  inline const map<string, AbstractObject*>& getContent () const { return hash; }

  AbstractObject* operator[](string key);
  AbstractObject* get (string path);

  bool insert (string key, AbstractObject* obj);
  bool add (string path, AbstractObject* obj);
  void toTxt (string &txt, int indentLvl);
  void toTxtUgly (string &txt);
  bool replace (string key, AbstractObject* obj);
  bool set (string key, AbstractObject* obj);
  bool remove (string key);
  bool erase (string key);
};

class ObjectFinal : public AbstractObject {
protected:
  ObjectFinal (int type) : AbstractObject (type) {}
  virtual ~ObjectFinal () = 0;
public:
  virtual void replaceValue (string value) = 0;
  AbstractObject* get (string path);
  bool add (string path,  AbstractObject* obj);
};

class ObjectFinalNumberFloat : public ObjectFinal {
private:
  double number;
public:
  ~ObjectFinalNumberFloat ();
  ObjectFinalNumberFloat (const ObjectFinalNumberFloat& obj);
  ObjectFinalNumberFloat () : ObjectFinal (NUMBER_FLOAT) {}
  ObjectFinalNumberFloat (double n) : ObjectFinal (NUMBER_FLOAT), number (n) {}
  inline double getContent () const { return number; }
  void replaceValue (string value);
  void toTxt (string &txt, int indentLvl);
  void toTxtUgly (string &txt);
};

class ObjectFinalNumberInt : public ObjectFinal {
private:
  long long number;
public:
  ~ObjectFinalNumberInt ();
  ObjectFinalNumberInt (const ObjectFinalNumberInt& obj);
  ObjectFinalNumberInt () : ObjectFinal (NUMBER_INT) {}
  ObjectFinalNumberInt (long int n) : ObjectFinal (NUMBER_INT), number (n) {}
  inline long int getContent () const { return number; }
  void replaceValue (string value);
  void toTxt (string &txt, int indentLvl);
  void toTxtUgly (string &txt);
};

class ObjectFinalString : public ObjectFinal {
private:
  string text;
public:
  ~ObjectFinalString ();
  ObjectFinalString (const ObjectFinalString& obj);
  ObjectFinalString () : ObjectFinal (STRING) {}
  ObjectFinalString (string s) : ObjectFinal (STRING), text (s) {}
  inline string getContent () const { return text; }
  void replaceValue (string value);
  void toTxt (string &txt, int indentLvl);
  void toTxtUgly (string &txt);
};

class ObjectFinalBool : public ObjectFinal {
private:
  bool boolean;
public:
  ~ObjectFinalBool ();
  ObjectFinalBool (const ObjectFinalBool& obj);
  ObjectFinalBool() : ObjectFinal (BOOL), boolean (false) {}
  ObjectFinalBool(string s) : ObjectFinal (BOOL) { replaceValue(s); }
  ObjectFinalBool(bool s) : ObjectFinal (BOOL), boolean (s) {}
  inline bool getContent () const { return boolean; }
  void replaceValue (string value);
  void toTxt (string &txt, int indentLvl);
  void toTxtUgly (string &txt);
};


}
#endif
