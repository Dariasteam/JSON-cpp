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

class AbstractObject {
protected:
  AbstractObject () {}  
public:
  virtual ~AbstractObject () = 0;  
  virtual AbstractObject* get (string path) = 0;
  virtual bool add (string path, AbstractObject* obj) = 0;
  virtual void toTxt (string &txt, int indentLvl) = 0;
  virtual void toTxtUgly (string &txt) = 0;
  void txtIndent (string& txt, int indentLvl);

  virtual const char* getName () = 0;

  static AbstractObject* copy (AbstractObject* obj);
};

class ObjectContainer : public AbstractObject {
protected:
  ObjectContainer () : AbstractObject () {}
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
  const static char* const name;
  inline const char* getName () { return name; }

  ObjectVector () {}
  ~ObjectVector ();
  ObjectVector (const ObjectVector& obj);
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
  const static char* const name;
  inline const char* getName () { return name; }
  ~ObjectMap ();
  ObjectMap () {}
  ObjectMap (const ObjectMap& obj);

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
  const static char* const name;
   inline const char* getName () { return name; }

  ~ObjectFinalNumberFloat ();
  ObjectFinalNumberFloat (const ObjectFinalNumberFloat& obj);
  ObjectFinalNumberFloat () : ObjectFinal () {}
  ObjectFinalNumberFloat (double n) : number (n) {}
  inline double getContent () const { return number; }  
  void replaceValue (string value);
  void toTxt (string &txt, int indentLvl);
  void toTxtUgly (string &txt);
};

class ObjectFinalNumberInt : public ObjectFinal {
private:
  long long number;
public:
  const static char* const name;
  inline const char* getName () { return name; }

  ~ObjectFinalNumberInt ();
  ObjectFinalNumberInt () {}
  ObjectFinalNumberInt (const ObjectFinalNumberInt& obj);  
  ObjectFinalNumberInt (long int n) : number (n) {}
  inline long int getContent () const { return number; }  
  void replaceValue (string value);
  void toTxt (string &txt, int indentLvl);
  void toTxtUgly (string &txt);
};

class ObjectFinalString : public ObjectFinal {
private:
  string text;
public:
  const static char* const name;
  inline const char* getName () { return name; }

  ~ObjectFinalString ();
  ObjectFinalString () {}
  ObjectFinalString (const ObjectFinalString& obj);
  ObjectFinalString (string s) : text (s) {}
  inline string getContent () const { return text; }  
  void replaceValue (string value);
  void toTxt (string &txt, int indentLvl);
  void toTxtUgly (string &txt);
};

class ObjectFinalBool : public ObjectFinal {
private:
  bool boolean;
public:
  const static char* const name;
  inline const char* getName () { return name; }

  ~ObjectFinalBool ();  
  ObjectFinalBool (const ObjectFinalBool& obj);
  ObjectFinalBool() : boolean (false) {}
  ObjectFinalBool(string s) { replaceValue(s); }
  ObjectFinalBool(bool s) : boolean (s) {}
  inline bool getContent () const { return boolean; }  
  void replaceValue (string value);
  void toTxt (string &txt, int indentLvl);
  void toTxtUgly (string &txt);
};

}
#endif
