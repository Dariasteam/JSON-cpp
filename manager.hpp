#ifndef MANAGER_H
#define MANAGER_H

#include <vector>
#include <regex>
#include <string>
#include <iostream>

#include "object.hpp"


using namespace std;

namespace json {

/* Contains a json hirarchy and allows its manipulation
 * ## Description
 * This class acts like a wrapper for a <json::AbstractObject> tree. While that structures contains all the hierarchied information, for security and
 * convenience, its content only can be used through this object. For that, it contains a pointer to the root node of an <json::AbstractObject>'s tree (which is always <json::ObjectMap>)
 * and the methods for accessing and manipulatng its data.
 * The initialization of the object can be done handly or using a <json::Parser> to read the info from a file.
 *
 * The interface is designed to never expose raw nodes out of the class, only <setTop> and <copyFrom> works directly with nodes and must be used
 * carefuly (For most situations there is a non-raw secure method for wathever you are trying to do).
 *
 * The acessing / manipulation methods are clasified like this
 *
 * - is*. Checks if the element at the path given is of certain type.
 *  - isNumber
 *  - isBool
 *  - isVector
 *  - isMap
 *  - isString
 *  - isInt
 *  - insFloat
 *
 * - get. Copies information of the tree to a variable given a path
 * - add. Adds information from a variable to the tree if **the given path doesn't previously exist**
 * - replace. Adds information from a variable to the tree if **the given path previously exist**
 * - set. Adds information from a variable to the tree **both if previously exist or not the path given**
 * - erase. Deletes information from the tree if the given path previously exist
 * - remove. Deletes information from the tree both if the given path previously exist or not
 *
 * All these methods share this syntax for simplicity:
 *
 * ```
 * bool METHOD_NAME (<optional variable>, <path>)
 * ```
 * Where optional variable exist o not depending of the method and can be of several types (int, float, vector<int>, etc.) so the methods are heavily overloaded for supporting each type.
 *
 * There is also a method for get the information as a string in json format. <toText>
 *
 * */
class JsonTree {
private:
  static string const objectsTypesReverse [7];
  static regex lastTokenRgx;

  ObjectMap* top;

  bool isType (AbstractObject* obj, int type);
  void getterError (const string path, AbstractObject* obj, int type);

  bool replace (AbstractObject* newObj, const string path);
  bool set (AbstractObject* newObj,     const string path);

  bool isNumber (AbstractObject* obj);
  bool isFloat (AbstractObject* obj);
  bool isInt (AbstractObject* obj);
  bool isBool (AbstractObject* obj);
  bool isString (AbstractObject* obj);
  bool isMap (AbstractObject* obj);
  bool isVector (AbstractObject* obj);  

  static ObjectVector* createVec (const vector<double>& array);
  static ObjectVector* createVec (const vector<int>& array);
  static ObjectVector* createVec (const vector<bool>& array);
  static ObjectVector* createVec (const vector<string>& array);

  static AbstractObject* insertObject (const string path, AbstractObject* obj);

public:
  /* Default constructor
   * Internally generates a new empty ObjectMap for the root of this tree
   * */
  JsonTree ();

  /* Copy constructor
   * @tree contains the information to be copied
   * @path Subpath of the tree that will be copied, by default ="" (root)
   *
   * Copies the element at `path` in the object `tree` and its possible derived nodes to this object.
   * The path's element copy is the root of this object
   * */
  JsonTree (const JsonTree& tree, const string path = "");

  /* Default destroyer
   *
   * Recursively deletes the information of the tree
   * */
  ~JsonTree ();

  /* Changes the tree
   * @root Root of a json hierarchy to replace the root (top) of this object
   * */
  inline void setTop (AbstractObject* root) { delete top; top = (ObjectMap*)root; }

  /* Gets the keys of a Map in the hierarchy
   * @path Path of the map
   *
   * @return keys of the map
   * */
  vector <string> getKeys (const string path);
  /* Gets the size of a vector in the nierarchy
   * @path Path of the vector
   *
   * @return size of the vector, if the element indicated by path is not a vector, returns -1
   * */
  int getSizeAt (const string path);

  /* Copy raw elements
   * @obj top of the subtree in which the elements of this will be replicated
   * @path Path to the elements to be copied
   *
   * Returns if the path exist and its content is succesfully copied in obj.
   * obj's previous content is deleted if the copy is valid, otherwise is not modified
   *
   * @return the operation successfuly finish
   * */
  bool copyFrom (AbstractObject*& obj, const string path) const;

  // returns if this object is successfuly transformed to the union between tree and itself
  bool set_union (JsonTree& tree);

  /* Retrieves information
   * @to Variable to be assigned with the information retrieved
   * @path Path to the information to be retrieved
   *
   * ## Description
   * Tries to get the information indicated by path and to copy it into 'to' variable.
   * The method follows these steps:
   *
   * 1. Check the `path` exists
   * 2. Check the element at `path` is of the same type that `to` variable
   * 3. Assing the value at `path` to `to`
   *
   * There are 3 possible situations
   *
   * - The path exist and it's content has the same type of 'to':
   *   The element in path is copied into 'to' and the method returns true
   * - The path exist and it's content has not the same type as 'to':
   *   The operation is aborted, the method returns false and 'to' is not modified. cerr shows this message:
   *
   *   `ERROR : Attempting to load the element $path as a $to_type when is of type $path_type.`
   *
   * - The path does not exist:
   *   The operation is aborted, the method returns false and 'to' is not modified. cerr shows this message:
   *
   *   `ERROR : Attempting to load the element $path which does not exist.`
   *
   * Note that this method has many overloads for supporting many 'to' variable types. However, you must know the type of the element
   * which want to retrieve from 'path' in order to call this method with a 'to' variable of the appropiate type. To check the type of 'path' use
   * one of these methods:
   *
   * -  <isNumber>
   * -  <isFloat>
   * -  <isInt>
   * -  <isBool>
   * -  <isString>
   * -  <isVector>
   * -  <isMap>
   *
   * ## Usage Example
   * #### Initial hirarchy
   * ```json
   * {
   *  "A" : 40
   * }
   * ```
   * #### Code
   * ```c++
   * json::JsonTree tree;
   * //some code
   *
   * std::string aux_str = "default value";
   * int aux_int = 0;
   *
   * tree.get (aux_str, "A"); // returns false, the element "A" exists but is not a string but a number. aux_string is not modified
   * tree.get (aux_int, "O"); // returns false, the element "O" does not exist. aux_int is not modified
   * tree.get (aux_int, "A"); // returns true, now aux_int contains the value 40
   *
   * std::cout << "Value: " << aux_str << " " << aux_int << std::endl;
   *```
   * #### Output
   *```
   * ERROR : Attempting to load the element A as a String when is of type Number_Int.
   * ERROR : Attempting to load the element O which does not exist.
   * Values: default value 40
   *```
   *
   * @return true if the path exist, has the appropiate type and the value is succesfully copied
   * */
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

  /* Checks type Numeric
   * @path Path of the element to be checked
   *
   * Checks if the element specifacated by path is numeric (double or long long int equally)
   *
   * @return the element specifacated by path is numeric
   * */
  //- returns if the element on the path exist and is of the type
  bool isNumber (const string path);
  /* Checks type Float
   * @path Path of the element to be checked
   *
   * Checks if the element specifacated by path is a floating point number (inner it will be a double for better precision)
   *
   * @return the element specifacated by path is a floating point number
   * */
  bool isFloat  (const string path);
  /* Checks type Int
   * @path Path of the element to be checked
   *
   * Checks if the element specifacated by path is an integer (inner it will be a long long int for better precision)
   *
   * @return the element specifacated by path is an integer
   * */
  bool isInt    (const string path);
  /* Checks type Bool
   * @path Path of the element to be checked
   *
   * @return the element specifacated by path is a bool
   * */
  bool isBool   (const string path);
  /* Checks type String
   * @path Path of the element to be checked
   *
   * @return the element specifacated by path is a string
   * */
  bool isString (const string path);
  /* Checks type Map
   * @path Path of the element to be checked
   *
   * @return the element specifacated by path is a map (hash)
   * */
  bool isMap    (const string path);
  /* Checks type Vector
   * @path Path of the element to be checked
   *
   * @return the element specifacated by path is a vector
   * */
  bool isVector (const string path);
  /* Checks exists
   * @path Path of the element to be checked
   *
   * @return the element exists
   * */
  bool exist    (const string path);

  /* Adds information
   * @value Variable with the information to be added
   * @path Path in which one the info will be added
   *
   * ## Description
   *
   * Tries to add information to the hierarchy. If the route indicated by path
   * already exist, the operation is cancelled and the method returns false. Otherwise,
   * the route is created as a hierarchy of hashes, where the last element has a **copy** of the
   * information of 'value'.
   *
   * Note that this method has many overloads for many `value` variable types.
   *
   * ## Usage Example
   *
   *
   * #### Initial hirarchy
   * ```json
   * {
   *  "A" : 40
   * }
   * ```
   *
   * #### Code
   * ```c++
   * json::JsonTree tree;
   * //some code
   * tree.add(56, "A");                   // returns false, the path "A" already exist
   * tree.add(56, "some.long.path");      // returns true, adds the element at the correct path (which is created)
   * tree.add(70, "some.long.otherPath"); // returns true, adds the element at the correct path (which now mostly exists because of the previous function call)
   * ```
   *
   * ##### Modified hierarchy
   * ```json
   * {
   *  "A" : 40,
   *  "some" : {
   *   "long" : {
   *    "path" : 56,
   *    "otherPath" : 70
   *   }
   *  }
   * }
   * ```
   *
   * @return true if `path` doesn't exist and its sucessfully created
   * */
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

  /* Adds a vector
   * @path Where the vector should be added
   *
   * Tries to create a vector in the route specifacted by `path`.
   * If already exist something there, the method returns false and
   * no vector is created
   *
   * @return true if the vector is created at `path`
   * */
  bool addVector (const string path);

  /* Adds a map
   * @path Where the map should be added
   *
   * Tries to create a map in the route specificated by `path`.
   * If already exist something there, the method returns false and
   * no map is created
   *
   * @return true if the map is created at `path`
   * */
  bool addMap (const string path);

  //- returns true if successfully copied the elements indicated in 'from' in the path.
  //- The path mustn't exist previously

  /* Copy info between JsonTree's
   * @tree Contains the informaton to be copied
   * @from route to the elements of `tree` to be copied
   * @path route where the elements will be stored
   *
   * Tries to copy information from `tree` to this object. The `path` must not exist in this object due this is an add method
   * (can not replace or set data) but will be created.
   *
   * @return true if `path` didn't exist, is successfuly created and the elemets of `tree` indicated in from are copied into it
   * */
  bool add (const JsonTree& tree, const string from = "", const string path = "");

  /* Replace information
   * @from Contains the information to be used
   * @path Path to the element to be replaced
   *
   * ## Description
   *
   * Tries to replace one element of the hierarchy. If the route does not exist the operation is aborted and
   * the method returns false. Otherwise, the information at path is replaced by a **copy** of the one inside 'from' and after that is recursively deleted.
   *
   * **Warning**: The replacement can be **from any type to any type** and the previous data will **be completely lost**.
   * You can use this as a morphing method changing for example values from int to vector<int>.
   *
   * Note that this method has many overloads for many `from` variable types.
   *
   * ## Usage Example
   *
   * #### Initial hirarchy
   * ```json
   * {
   *  "A" : 40
   * }
   * ```
   *
   * #### Code
   * ```c++
   * json::JsonTree tree;
   * //some code
   * tree.replace(56, "O");                   // returns false, the path "O" does not exist so can not be replaced
   * tree.replace(56.8, "A");                 // returns true, now A contains a float which value is 56.8, instead of the original integer
   * tree.replace("this is a string", "A");   // returns true, now A contains a string which value is "this is a string" instead of the previous float
   *
   * std::vector <int> array_of_int = {1, 3, 4, 5, 6};
   * tree.replace(array_of_int, "A");         // returns true, now A contains a copy of the vector `array_of_int`
   * ```
   *
   * ##### Modified hierarchy
   * ```json
   * {
   *  "A" : [
   *    1, 3, 4, 5, 6
   *  ]
   * }
   * ```
   *
   * @return true if `path` exists and is sucessfully replaced
   * */
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

  //- returns

  /* Sets information
   * @value Contains the information to be used
   * @path Path to the element to be setted
   *
   * ## Description
   *
   * Tries to set information at `path` (either it previously exist or not)
   * This method guarantees that the information will be stored unless a major
   * problem happen. As same as`replace` methods, it has the danger of erase useful information
   * if not propperly used.
   *
   * Note that this method has many overloads for many `value` variable types.
   *
   * ## Usage Example
   *
   * #### Initial hirarchy
   * ```json
   * {
   *  "A" : 40
   * }
   * ```
   *
   * #### Code
   * ```c++
   * json::JsonTree tree;
   * //some code
   * tree.set(56, "A");                       // returns true, the path A already exist but its value is replaced by a 56
   * tree.replace("this is a string", "O");   // returns true, the path O didn't exist but is created with "this is a string" as content
   * ```
   *
   * ##### Modified hierarchy
   * ```json
   * {
   *  "A" : 56,
   *  "B" : "this is a string"
   * }
   * ```
   *
   * @return true if `path` its sucessfully created or replaced if previously exist
   * */
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

  /* Delete existing elements
   * @path Path to the elements to be deleted
   *
   * Checks if `path` exists in this object, and if so it recursively delete its content
   * If not exists returns false
   *
   * @return true if `path` exist and is successfully removed, false if doesn't exist
   * */
  bool erase (const string path);

  /* Delete possible elements
   * @path Path to the elements to be deleted
   *
   * Checks if `path` exists in this object, and if so it recursively delete its content.
   * If not exists also returns true
   *
   * @return true if `path` is successfully removed or doesn't previously exist
   * */
  bool remove (const string path);

  /* Formats content for output as json
   * @uglify compacts or not the json information, by default =false (not compact, human readable)
   *
   * Creates a string which content is the hierarchy of this object formatted as json
   *
   * #### Uglified
   * ```json
   * {"pah_a":45,"path_b":[1,3,4,5],"path_c":{"subpath_a":false,"subpath_b":"hello"}}
   * ```
   * #### Not uglified
   * ```json
   * {
   *  "pah_a" : 45,
   *  "path_b" : [
   *    1, 3, 4, 5
   *  ],
   *  "path_c" : {
   *    "subpath_a" : false,
   *    "subpath_b" : "hello"
   *  }
   * }
   * ```
   *
   * @return json formatted content
   * */
  string toText (const bool uglify = false);

};

}

#endif
