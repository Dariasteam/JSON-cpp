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

  /* Search for a certain node
   * @path route to the element
   *
   * Overrides of this method try to reach the node at 'path'.
   * (<ObjectContainer> derived classes have a full working reimplementation
   * while <ObjectFinal> have not; a final node cannot be father of other node)
   *
   * The basic policy is to select the first token in 'path' (all elements before the first '.')
   * and check if any son node's key/ index matches with token's content
   *
   * - Matches: splits 'path' deleting the first token and
   * recursively calls this method for matching son (sending the splitted 'path') and returns its result
   *
   * - Not maches: returns nullptr
   *
   * If 'path' is empty, _this_ (whatever be the AbsctractObject derived object pointed by _this_) is returned
   *
   * @return nullptr if node does not exist, node's pointer otherwise
   * */
  virtual AbstractObject* get (string path) = 0;

  /* Adds a node to tree
   * @path route to the node which will be father of 'obj'
   * @obj node to be added
   *
   * Overrides of this method try to add 'obj' as a son of the
   * node at 'path'. (<ObjectContainer> derived classes have a working
   * method while <ObjectFinal> have not; a final node cannot be father of other node)
   *
   * The basic policy is to select the first token in 'path' (all elements before the first '.')
   * and splits 'path' deleting that token. Then check if already exist a son with the same key / index associated.
   *
   * - Already exist: checks if node is final or not
   *   - is <json::ObjectFinal>: 'obj' is deleted, returns false
   *
   *   - is <json::ObjectContainer>: recursively calls this method for matching son (sending the splitted 'path') and returns its result
   *
   *
   * - Not exist:
   *   - path is empty: inserts 'obj' as node of this object with token's content as key / index, returns true
   *
   *   - path is not empty: creates a new <json::ObjectMap>, inserts it as son of this (at token's key / index).
   *     Recursively calls this method for that new object (sending the splitted 'path'), and returns its result
   *
   * @return the operation is successfuly finished or not
   * */
  virtual bool add (string path, AbstractObject* obj) = 0;

  /* Generates and appends json text
   * @txt string to append this node's info
   * @indentLvl number of indentations to generate a consistent string
   *
   * Overrides of this method recursively generate the text version in
   * json format of the content both node and sons, appending it to 'txt' parameter.
   * The string respects the indentation and is human readable (See <JsonTree::toText>)
   * */
  virtual void toTxt (string &txt, int indentLvl) = 0;

  /* Generates ugly json text
   * @txt string to append this node's info
   *
   * Overrides of this method recursively generate the text version in
   * json format of the content both node and sons, appending it to 'txt' parameter.
   * The string is compressed and uglified (See <JsonTree::toText>)
   * */
  virtual void toTxtUgly (string &txt) = 0;

  /* Get node tye name
   *
   * Overrides of this method return each type name
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
  /* Inserts a son node in this object
   * @key Key of the element ("" in vector case)
   * @obj Node to add
   *
   * Overrides of this method are used when
   * creating the tree or adding information to it
   *
   * @return the operation is successfuly finished or not
   * */
  virtual bool insert (string key, AbstractObject* obj) = 0;
};

/* Representation of []
 *
 * Internally contains a
 * ```c++
 * std::vector <AbstractObject*>
 * ```
 * which stores the sons nodes
 *
 * Vectors can **only** push back elements, that means no deletetions or substitutions.
 * In order to archive that, you must replace all object with a new one wich contains
 * the desired data
 * */
class ObjectVector : public ObjectContainer{
private:
  vector <AbstractObject*> array;
  static regex tokenRgx;
public:
  ObjectVector () {}
  ~ObjectVector ();

  /* Copy constructor
   * @obj node to copy
   *
   * Initialize object with a copy of obj's content
   * (Recursively calling <json::AbstractObject::getCopy> for every son)
   * */
  ObjectVector (const ObjectVector& obj);

  /* This class's name, for debugging purposes */
  const static char* const name;

  /* Get method for <name>
   *
   * @return <name>
   * */
  inline const char* getName () { return name; }

  /* Inserts a son node in this object
   * @key Key of the element (must be "")
   * @obj Node to add
   *
   * If 'path' is "" (vectors don't use keys but indexes),
   * it pushs back 'obj' in inner vector, returning true.
   * Otherwise 'obj' is deleted and method returns false
   *
   * @return 'obj' is succesfully inserted
   * */
  bool insert (string key, AbstractObject* obj);  

  /* Get size
   *
   * @return size of inner vector (Quantity of sons nodes)
   * */
  inline int size () const { return array.size(); }

  /* Get inner vector
   *
   * Used by copy constructor
   *
   * @return inner vector
   * */
  inline const vector<AbstractObject*>& getContent () const { return array; }

  /* Access a son
   * @index position of the element in inner vector
   *
   * @return node at 'index' if exist, nullptr otherwise
   * */
  AbstractObject* operator[](unsigned index);

  /* Search for a certain node
   * @path route to the element
   *
   * @return object found or nullptr if doesn't exist
   * */
  AbstractObject* get (string path);

  /* Adds a node to tree
   * @path route to the node which will be father of 'obj'
   * @obj node to be added
   *
   * @return the operation is successfuly finished or not
   * */
  bool add (string path, AbstractObject* obj);

  /* Generates json text
   * @txt string to append this node's info
   * */
  void toTxt (string &txt, int indentLvl);

  /* Generates ugly json text
   * @txt string to append this node's info
   * */
  void toTxtUgly (string &txt);

  /* Clone object
   *
   * Creates a copy of this object and its content
   * (Recursively doing the same operation for every son)
   *
   * @return a copy of this object
   * */
  AbstractObject* getCopy ();
};

/* Representation of {}
 *
 * Internally contains a
 * ```c++
 * std::map<std::string, AbstractObject*>
 * ```
 * which stores the pairs key / sons nodes and a
 * ```c++
 * std::vector <std::string>
 * ```
 * for <getKeys> method
 * */
class ObjectMap : public ObjectContainer {
private:
  vector <string> keys;
  map <string, AbstractObject*> hash;
  static regex tokenRgx;
public:
  ~ObjectMap ();
  ObjectMap () {}

  /* Copy constructor
   * @obj node to copy
   *
   * Initialize object with a copy of obj's content
   * (Recursively calling <json::AbstractObject::getCopy> for every son)
   * */
  ObjectMap (const ObjectMap& obj);

  /* This class's name, for debugging purposes */
  const static char* const name;

  /* Get method for <name>
   *
   * @return <name>
   * */
  inline const char* getName () { return name; }    

  /* Get size
   *
   * @return size of inner vector (Quantity of sons nodes)
   * */
  inline int size () const { return hash.size(); }

  /* Get all keys
   *
   * @return inner keys vector
   * */
  inline const vector <string>& getKeys () const { return keys; }

  /* Get inner map
   *
   * Used by copy constructor
   *
   * @return inner map
   * */
  inline const map<string, AbstractObject*>& getContent () const { return hash; }

  /* Access a son
   * @key key of the element in inner map
   *
   * @return node at 'key' if exist, nullptr otherwise
   * */
  AbstractObject* operator[](string key);

  /* Search for a certain node
   * @path route to the element
   *
   * @return object found or nullptr if doesn't exist
   * */
  AbstractObject* get (string path);

  /* Inserts a son node in this object
   * @key Key of the element
   * @obj Node to add
   *
   * If 'path' is not "" and does not previously exist,
   * it adds the pair key / obj in inner map, returning true.
   * Otherwise 'obj' is deleted and method returns false
   *
   * @return 'obj' is succesfully inserted
   * */
  bool insert (string key, AbstractObject* obj);

  /* Adds a node to tree
   * @path route to the node which will be father of 'obj'
   * @obj node to be added
   *
   * @return the operation is successfuly finished or not
   * */
  bool add (string path, AbstractObject* obj);

  /* Generates json text
   * @txt string to append this node's info
   * */
  void toTxt (string &txt, int indentLvl);

  /* Generates ugly json text
   * @txt string to append this node's info
   * */
  void toTxtUgly (string &txt);

  /* Replace one node by another
   * @key key (**not** path) of the node to be replaced
   * @obj new node
   *
   * If node at 'key' exist, changes it by 'obj', deletes the
   * original one and returns true. Otherwise returns false
   *
   * For more information about replace's meaning see <json::JsonTree::replace>
   *
   * @return the operation is successfuly finished or not
   * */
  bool replace (string key, AbstractObject* obj);

  /* Replace one node by another
   * @key key (**not** path) were 'obj' will be setted
   * @obj new node
   *
   * Both exist or not a node at 'key', changes it by / insert 'obj'
   * (deletes the original one if there was) and returns true. Otherwise returns false
   *
   * For more information about set's meaning see <json::JsonTree::set>
   *
   * @return the operation is successfuly finished or not
   * */
  bool set (string key, AbstractObject* obj);

  /* Removes a node
   * @key key (**not** path) of the node to be removed
   *
   * Deletes the node at 'key' if exist or does nothing otherwise.
   *
   * For more information about remove's meaning see <json::JsonTree::remove>
   * @return the operation is successfuly finished or not
   * */
  bool remove (string key);

  /* Erases a node
   * @key key (**not** path) of the node to be erased
   *
   * Erase the node at 'key' if exist and returns true.
   * If there wasn't returns false
   *
   * For more information about erases's meaning see <json::JsonTree::erase>
   *
   * @return the operation is successfuly finished or not
   * */
  bool erase (string key);

  /* Clone object
   *
   * Creates a copy of this object and its content
   * (Recursively doing the same operation for every son)
   *
   * @return a copy of this object
   * */
  AbstractObject* getCopy ();
};

/* Base class for final nodes
 * */
class ObjectFinal : public AbstractObject {
protected:
  virtual ~ObjectFinal () = 0;
public:
  /* Changes this object's content
   * @value value expresed in json
   *
   * Overrides of this method transforms string data
   * (string 'true', string '0.2', string 'text') to its
   * proper value for each type (bool = true, double = 0.2, string = text)
   *
   * The method is used by <json::Parser> when creating the tree
   * */
  virtual void replaceValue (string value) = 0;

  /* Search for a certain node
   * @path route to the element
   *
   * As a final node node cannot have sons, this is only useful
   * when 'path' is "", (which means this is the required node)
   *
   * @return this node when 'path' is empty, nullptr otherwise
   * */
  AbstractObject* get (string path);

  /* Adds a node to tree
   * @path route to the node which will be father of 'obj'
   * @obj node to be added
   *
   * As a final node node cannot have sons, this method
   * always deletes 'obj' and returns false
   *
   * @return false **always**
   * */
  bool add (string path,  AbstractObject* obj);
};

/* Represents a float
 *
 * Internally uses a double for better precision
 * */
class ObjectFinalNumberFloat : public ObjectFinal {
private:
  double number;
public:
  ~ObjectFinalNumberFloat ();
  ObjectFinalNumberFloat () : ObjectFinal () {}
  ObjectFinalNumberFloat (double n) : number (n) {}
  /* Copy constructor
   * @obj node to copy
   *
   * calls <getCopy>
   * */
  ObjectFinalNumberFloat (const ObjectFinalNumberFloat& obj);

  /* This class's name, for debugging purposes */
  const static char* const name;

  /* Get method for <name>
   *
   * @return <name>
   * */
  inline const char* getName () { return name; }

  /* Get inner data
   *
   * @return inner data
   * */
  inline double getContent () const { return number; }  

  /* Replaces data
   * @value new data
   *
   * Updates the content with the data of 'value'
   * */
  void replaceValue (string value);

  /* Generates and appends json text
   * @txt string to append this node's info
   * @indentLvl number of indentations to generate a consistent string
   *
   * Appends json content to 'txt' respecting the indentation
   * */
  void toTxt (string &txt, int indentLvl);

  /* Generates and appends json text
   * @txt string to append this node's info
   * @indentLvl number of indentations to generate a consistent string
   *
   * Appends uglified json content to 'txt'
   * */
  void toTxtUgly (string &txt);

  /* Clone object
   *
   * Creates a copy of this object and its content
   *
   * @return a copy of this object
   * */
  AbstractObject* getCopy ();
};

/* Represents an integer
 *
 * Internally uses a long long for better precision
 * */
class ObjectFinalNumberInt : public ObjectFinal {
private:
  long long number;
public:
  ~ObjectFinalNumberInt ();
  ObjectFinalNumberInt () {}
  ObjectFinalNumberInt (long int n) : number (n) {}
  /* Copy constructor
   * @obj node to copy
   *
   * calls <getCopy>
   * */
  ObjectFinalNumberInt (const ObjectFinalNumberInt& obj);

  /* This class's name, for debugging purposes */
  const static char* const name;

  /* Get method for <name>
   *
   * @return <name>
   * */
  inline const char* getName () { return name; }

  /* Get inner data
   *
   * @return inner data
   * */
  inline long int getContent () const { return number; }

  /* Replaces data
   * @value new data
   *
   * Updates the content with the data of 'value'
   * */
  void replaceValue (string value);

  /* Generates and appends json text
   * @txt string to append this node's info
   * @indentLvl number of indentations to generate a consistent string
   *
   * Appends json content to 'txt' respecting the indentation
   * */
  void toTxt (string &txt, int indentLvl);

  /* Generates and appends json text
   * @txt string to append this node's info
   * @indentLvl number of indentations to generate a consistent string
   *
   * Appends uglified json content to 'txt'
   * */
  void toTxtUgly (string &txt);

  /* Clone object
   *
   * Creates a copy of this object and its content
   *
   * @return a copy of this object
   * */
  AbstractObject* getCopy ();
};

/* Represents an integer
 *
 * Internally uses std::string
 * */
class ObjectFinalString : public ObjectFinal {
private:
  string text;
public:
  ~ObjectFinalString ();
  ObjectFinalString () {}
  ObjectFinalString (string s) : text (s) {}
  /* Copy constructor
   * @obj node to copy
   *
   * calls <getCopy>
   * */
  ObjectFinalString (const ObjectFinalString& obj);

  /* This class's name, for debugging purposes */
  const static char* const name;

  /* Get method for <name>
   *
   * @return <name>
   * */
  inline const char* getName () { return name; }

  /* Get inner data
   *
   * @return inner data
   * */
  inline string getContent () const { return text; }  

  /* Replaces data
   * @value new data
   *
   * Updates the content with the data of 'value'
   * */
  void replaceValue (string value);

  /* Generates and appends json text
   * @txt string to append this node's info
   * @indentLvl number of indentations to generate a consistent string
   *
   * Appends json content to 'txt' respecting the indentation
   * */
  void toTxt (string &txt, int indentLvl);

  /* Generates and appends json text
   * @txt string to append this node's info
   * @indentLvl number of indentations to generate a consistent string
   *
   * Appends uglified json content to 'txt'
   * */
  void toTxtUgly (string &txt);

  /* Clone object
   *
   * Creates a copy of this object and its content
   *
   * @return a copy of this object
   * */
  AbstractObject* getCopy ();
};

/* Represents an integer
 *
 * Internally uses a bool
 * */
class ObjectFinalBool : public ObjectFinal {
private:
  bool boolean;
public:
  ~ObjectFinalBool ();
  ObjectFinalBool() : boolean (false) {}
  ObjectFinalBool(string s) { replaceValue(s); }
  ObjectFinalBool(bool s) : boolean (s) {}
  /* Copy constructor
   * @obj node to copy
   *
   * calls <getCopy>
   * */
  ObjectFinalBool (const ObjectFinalBool& obj);

  /* This class's name, for debugging purposes */
  const static char* const name;

  /* Get method for <name>
   *
   * @return <name>
   * */
  inline const char* getName () { return name; }

  /* Get inner data
   *
   * @return inner data
   * */
  inline bool getContent () const { return boolean; }  

  /* Replaces data
   * @value new data
   *
   * Updates the content with the data of 'value'
   * */
  void replaceValue (string value);

  /* Generates and appends json text
   * @txt string to append this node's info
   * @indentLvl number of indentations to generate a consistent string
   *
   * Appends json content to 'txt' respecting the indentation
   * */
  void toTxt (string &txt, int indentLvl);

  /* Generates and appends json text
   * @txt string to append this node's info
   * @indentLvl number of indentations to generate a consistent string
   *
   * Appends uglified json content to 'txt'
   * */
  void toTxtUgly (string &txt);

  /* Clone object
   *
   * Creates a copy of this object and its content
   *
   * @return a copy of this object
   * */
  AbstractObject* getCopy ();
};

}
#endif
