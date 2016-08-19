#ifndef ABSTRACTOBJECT_H
#define ABSTRACTOBJECT_H

#include <vector>
#include <string>
#include <map>
#include <regex>

using namespace std;

#define END_LINE "\n"
#define INDENT "  "
#define QUOTE "\""
#define POINTS " : "
#define COMMA ","

enum ObjetsTypes {
  VECTOR,
  MAP,
  FINAL,    // used for comparisons, if (getType () > FINAL) -> is a final object
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
  virtual AbstractObject* get (string path) = 0;
  virtual bool add (string path, AbstractObject* obj) = 0;
  virtual void toTxt (string &txt, int indentLvl) = 0;
  void txtIndent (string& txt, int indentLvl);
};

class ObjectContainer : public AbstractObject {
protected:
  ObjectContainer (int type) : AbstractObject (type) {}
  ~ObjectContainer () = 0;
public:
  virtual const AbstractObject* operator[](unsigned index) { return nullptr; };
  virtual const AbstractObject* operator[](string key) { return nullptr; };
  virtual bool insert (string key, AbstractObject* obj) = 0;
  virtual int size () = 0;
};

class ObjectVector : public ObjectContainer{
private:
  vector <AbstractObject*> array;
  static regex tokenRgx;
public:
  ObjectVector () : ObjectContainer (VECTOR) {}
  AbstractObject* getContentAt (int indeox);
  bool insert (string key, AbstractObject* obj);
  inline int size () { return array.size(); }
  AbstractObject* operator[](unsigned index);
  AbstractObject* get (string path);
  bool add (string path, AbstractObject* obj);
  void toTxt (string &txt, int indentLvl);
};

class ObjectMap : public ObjectContainer {
private:
  vector <string> keys;
  map <string, AbstractObject*> hash;
  static regex tokenRgx;
public:
  ObjectMap () : ObjectContainer (MAP) {}
  bool insert (string key, AbstractObject* obj);
  inline int size () { return hash.size(); }
  inline const vector <string>& getKeys () { return keys; }
  AbstractObject* operator[](string key);
  AbstractObject* get (string path);
  bool add (string path, AbstractObject* obj);
  void toTxt (string &txt, int indentLvl);  
};

class ObjectFinal : public AbstractObject {
protected:
  ObjectFinal (int type) : AbstractObject (type) {}
  virtual ~ObjectFinal () = 0;
public:
  virtual void setValue (string value) = 0;
  AbstractObject* get (string path);
  bool add (string path,  AbstractObject* obj);
};

class ObjectFinalNumber : public ObjectFinal {
private:
  double number;
public:
  ObjectFinalNumber () : ObjectFinal (NUMBER) {}
  ObjectFinalNumber (double n) : ObjectFinal (NUMBER), number (n) {}
  inline double getContent () { return number; }
  void setValue (string value);
  void toTxt (string &txt, int indentLvl);
};

class ObjectFinalString : public ObjectFinal {
private:
  string text;
public:
  ObjectFinalString () : ObjectFinal (STRING) {}
  ObjectFinalString (string s) : ObjectFinal (STRING), text (s) {}
  inline string getContent () { return text; }
  void setValue (string value);
  void toTxt (string &txt, int indentLvl);
};

class ObjectFinalBool : public ObjectFinal {
private:
  bool boolean;
public:
  ObjectFinalBool() : ObjectFinal (BOOL), boolean (false) {}
  ObjectFinalBool(string s) : ObjectFinal (BOOL) { setValue(s); }
  ObjectFinalBool(bool s) : ObjectFinal (BOOL), boolean (s) {}
  inline bool getContent () { return boolean; }
  void setValue (string value);
  void toTxt (string &txt, int indentLvl);
};

#endif
