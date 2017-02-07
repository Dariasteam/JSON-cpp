#ifndef MANAGER_H
#define MANAGER_H

#include <vector>
#include <regex>
#include <string>
#include <iostream>

#include "object.hpp"

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
 *  - is*. Checks if the element at the given path is of certain type.
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
 * - set. Adds information from a variable to the tree **both if previously exist or not the given path**
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
 * There is also a method to get the information as a string in json format. <toText>
 *
 * */
class JsonTree {
private:  
  static std::regex lastTokenRgx;

  ObjectMap* top;

  bool isType (AbstractObject* obj, int type);
  void getterError (const std::string path, AbstractObject* obj, const char* expectedType);

  bool replace (AbstractObject* newObj, const std::string path);
  bool set (AbstractObject* newObj,     const std::string path);

  static bool isNumber (AbstractObject* const obj);
  static bool isFloat (AbstractObject* const obj);
  static bool isInt (AbstractObject* const obj);
  static bool isBool (AbstractObject* const obj);
  static bool isString (AbstractObject* const obj);
  static bool isMap (AbstractObject* const obj);
  static bool isVector (AbstractObject* const obj);  

  static AbstractObject* fabricate (const long long value);
  static AbstractObject* fabricate (const int value);
  static AbstractObject* fabricate (const double value);
  static AbstractObject* fabricate (const std::string value);
  static AbstractObject* fabricate (const char* value);
  static AbstractObject* fabricate (const char value);
  static AbstractObject* fabricate (const bool value);

  template <class t>
  static ObjectVector* createVec (const std::vector<t>& array) {
    ObjectVector* aux = new ObjectVector ();
    for (t element : array) {
      aux->insert("", fabricate(element));
    }
    return aux;
  }

  static AbstractObject* insertObject (const std::string path, AbstractObject* obj);

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
  JsonTree (const JsonTree& tree, const std::string path = "");

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
  std::vector <std::string> getKeys (const std::string path);
  /* Gets the size of a vector in the nierarchy
   * @path Path of the vector
   *
   * @return size of the vector, if the element indicated by path is not a vector, returns -1
   * */
  int getSizeAt (const std::string path);

  /* Copy raw elements
   * @obj top of the subtree in which the elements of this will be replicated
   * @path Path to the elements to be copied
   *
   * Returns if the path exist and its content is succesfully copied in obj.
   * obj's previous content is deleted if the copy is valid, otherwise is not modified
   *
   * @return the operation successfuly finish
   * */
  bool copyFrom (AbstractObject*& obj, const std::string path) const;

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
  bool get (double& to,       const std::string path);
  bool get (float& to,        const std::string path);
  bool get (int& to,          const std::string path);
  bool get (long& to,         const std::string path);
  bool get (long long& to,    const std::string path);
  bool get (bool& to,         const std::string path);
  bool get (std::string& to,  const std::string path);
  bool get (char& to,         const std::string path);

  bool get (std::vector<double>& array,      const std::string path);
  bool get (std::vector<int>& array,         const std::string path);
  bool get (std::vector<bool>& array,        const std::string path);
  bool get (std::vector<std::string>& array, const std::string path);

  /* Checks type Numeric
   * @path Path of the element to be checked
   *
   * Checks if the element specifacated by path is numeric (double or long long int equally)
   *
   * @return the element specifacated by path is numeric
   * */
  //- returns if the element on the path exist and is of the type
  bool isNumber (const std::string path);
  /* Checks type Float
   * @path Path of the element to be checked
   *
   * Checks if the element specifacated by path is a floating point number (inner it will be a double for better precision)
   *
   * @return the element specifacated by path is a floating point number
   * */
  bool isFloat  (const std::string path);
  /* Checks type Int
   * @path Path of the element to be checked
   *
   * Checks if the element specifacated by path is an integer (inner it will be a long long int for better precision)
   *
   * @return the element specifacated by path is an integer
   * */
  bool isInt    (const std::string path);
  /* Checks type Bool
   * @path Path of the element to be checked
   *
   * @return the element specifacated by path is a bool
   * */
  bool isBool   (const std::string path);
  /* Checks type String
   * @path Path of the element to be checked
   *
   * @return the element specifacated by path is a string
   * */
  bool isString (const std::string path);
  /* Checks type Map
   * @path Path of the element to be checked
   *
   * @return the element specifacated by path is a map (hash)
   * */
  bool isMap    (const std::string path);
  /* Checks type Vector
   * @path Path of the element to be checked
   *
   * @return the element specifacated by path is a vector
   * */
  bool isVector (const std::string path);
  /* Checks exists
   * @path Path of the element to be checked
   *
   * @return the element exists
   * */
  bool exist    (const std::string path);

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
  bool add (const double value,      const std::string path);
  bool add (const float value,       const std::string path);
  bool add (const int value,         const std::string path);
  bool add (const long value,        const std::string path);
  bool add (const long long value,   const std::string path);
  bool add (const bool value,        const std::string path);
  bool add (const std::string value,      const std::string path);
  bool add (const char& value,       const std::string path);
  bool add (const char* value,       const std::string path);

  // Supports any vector
  template <class t>
  bool add (const std::vector<t>& array, const std::string path) {
    return top->add (path, createVec (array));
  }

  /* Adds a vector
   * @path Where the vector should be added
   *
   * Tries to create a vector in the route specifacted by `path`.
   * If already exist something there, the method returns false and
   * no vector is created
   *
   * @return true if the vector is created at `path`
   * */
  bool addVector (const std::string path);

  /* Adds a map
   * @path Where the map should be added
   *
   * Tries to create a map in the route specificated by `path`.
   * If already exist something there, the method returns false and
   * no map is created
   *
   * @return true if the map is created at `path`
   * */
  bool addMap (const std::string path);

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
  bool add (const JsonTree& tree, const std::string from = "", const std::string path = "");

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
  bool replace (const double from,      const std::string path);
  bool replace (const float from,       const std::string path);
  bool replace (const int from,         const std::string path);
  bool replace (const long from,        const std::string path);
  bool replace (const long long from,   const std::string path);
  bool replace (const bool from,        const std::string path);
  bool replace (const std::string from,      const std::string path);
  bool replace (const char* from,       const std::string path);
  bool replace (const char  from,       const std::string path);

  template <class t>
  bool replace (const std::vector<t>& array, const std::string path) {
    return replace (createVec (array), path);
  }

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
  bool set (const double value,      const std::string path);
  bool set (const float value,       const std::string path);
  bool set (const int value,         const std::string path);
  bool set (const long value,        const std::string path);
  bool set (const long long value,   const std::string path);
  bool set (const bool value,        const std::string path);
  bool set (const std::string value,      const std::string path);
  bool set (const char* value,       const std::string path);
  bool set (const char  value,       const std::string path);

  template <class t>
  bool set (const std::vector<t>& array, const std::string path) {
    return set (createVec (array), path);
  }

  /* Delete existing elements
   * @path Path to the elements to be deleted
   *
   * Checks if `path` exists in this object, and if so it recursively delete its content
   * If not exists returns false
   *
   * @return true if `path` exist and is successfully removed, false if doesn't exist
   * */
  bool erase (const std::string path);

  /* Delete possible elements
   * @path Path to the elements to be deleted
   *
   * Checks if `path` exists in this object, and if so it recursively delete its content.
   * If not exists also returns true
   *
   * @return true if `path` is successfully removed or doesn't previously exist
   * */
  bool remove (const std::string path);

  /* Formats content for output as json
   * @uglify compacts or not the json information, by default =false (not compact, human readable)
   * @from path to the elements involved, by default = "" (all the tree)
   *
   *
   * Creates a string which content is the hierarchy of this object formatted as json
   * If the path indicated by 'from', all tree is formated
   *
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
  std::string toText (const bool uglify = false, const std::string from = "");

};

}

#endif
