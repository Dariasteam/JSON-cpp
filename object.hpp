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


/* Represents a node in the tree structure
 *
 * # Do not directly use this class or its derivatives, use <json::JsonTree> instead
 * ## Description
 *
 * Json format represents a tree hierarchy, so in order to manage it we must replicate
 * that structure in memory. This pure abstract class represent a node of that memory tree, and its derivatives
 * adds spport for each representable type in json. The following schema shows the complete inheritance  hierarchy
 *
 * ![](./pics/abstract_object_diagram.md.png)
 *
 * <json::ObjectContainer> and <json::ObjectFinal> are pure abstract as well, so the trees are
 * truly composed by third level class objects
 * 
 * ## Example
 * ```json
 * {
 *   "components" : {
 *     "first" : 12,
 *     "second" : true,
 *     "third" : "this_is_a_string"
 *   },
 *   "factors" : [0.566, true]
 * }
 * ```
 * ![](pics/tree_diagram_example.md.png)
 *
 * */
class AbstractObject {
protected:
  AbstractObject () {}  

  /* Appends whitespaces to string
   * @txt string to append the spaces
   * @indentLvl number of indentations
   *
   * Simply appends _INDENT_ content 'indentLvl' times to 'txt'.
   * (two spaces is an indentation as defined in the macro)
   *
   * #### Macro
   * ```c++
   * #define INDENT "  "
   * ```
   * */
  void txtIndent (string& txt, int indentLvl);
public:  
  virtual ~AbstractObject () = 0;

  /* Accessor method
   * @path route to the element
   *
   * Reimplementations of this method try to reach the node at 'path'.
   * (<ObjectContainer> derived classes have a working reimplementation
   * while <ObjectFinal> have not; a final can not be father of other node)
   *
   * @return nullptr if node does not exist, the pointer to the node otherwise
   * */
  virtual AbstractObject* get (string path) = 0;

  /* Add method
   * @path route to the node to be father of 'obj'
   * @obj node to be added
   *
   * Reimplementations of this method try to add 'obj' as a son of the
   * node at 'path'. (<ObjectContainer> derived classes have a working
   * method while <ObjectFinal> have not; a final can not be father of other node)
   *
   * @return the operation is successfuly finished or not
   * */
  virtual bool add (string path, AbstractObject* obj) = 0;

  /* Generates pretty json text
   * @txt string to append this node info
   * @indentLvl number of indentations to generate a consistent string
   *
   * Reimplementations of this method recursively generate the text version in
   * json format of the content both node and sons, appending it to 'txt' parameter.
   * The string respects the indentation and is human readable
   * */
  virtual void toTxt (string &txt, int indentLvl) = 0;

  /* Generates ugly json text
   * @txt string to append this node info
   *
   * Reimplementations of this method recursively generate the text version in
   * json format of the content both node and sons, appending it to 'txt' parameter.
   * The string is compressed and uglified
   * */
  virtual void toTxtUgly (string &txt) = 0;

  /* Get node tye name
   *
   * Reimplementations of this method return each type name
   * Used for console debugging of <json::JsonTree> management errors
   *
   * @return class name
   * */
  virtual const char* getName () = 0;

  /* Get a copy of the node
   *
   * @return a new node with same content than this
   * */
  virtual AbstractObject* getCopy () = 0;
};

/* Base class for Vectors & Hashes
 * */
class ObjectContainer : public AbstractObject {
protected:
  ObjectContainer () : AbstractObject () {}
  ~ObjectContainer () = 0;
public:   
  virtual bool insert (string key, AbstractObject* obj) = 0;
  virtual AbstractObject* getCopy () = 0;
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

  AbstractObject* getCopy ();
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

  AbstractObject* getCopy ();
};

class ObjectFinal : public AbstractObject {
protected:
  virtual ~ObjectFinal () = 0;
public:
  virtual void replaceValue (string value) = 0;
  AbstractObject* get (string path);
  bool add (string path,  AbstractObject* obj);

  virtual AbstractObject* getCopy () = 0;
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

  AbstractObject* getCopy ();
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

  AbstractObject* getCopy ();
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

  AbstractObject* getCopy ();
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

  AbstractObject* getCopy ();
};

}
#endif
