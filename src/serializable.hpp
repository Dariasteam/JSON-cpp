#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include <string>
#include <type_traits>
#include <cxxabi.h>

#include "manager.hpp"
#include "parser.hpp"

#define CLASS_TYPE "$classType"
#define CLASS_CONTENT "$classContent"

#define SERIAL_START_INHERITED(y, x) INHERITS_FROM (y, x)                                                                                                 \
                                      virtual bool serializer (json::JsonTree& _json_tree_, bool _json_op_, std::string _json_path_, InheritanceIndex& _json_from_) { \
                                        return serialize (_json_op_, _json_path_, _json_tree_, _json_from_,

#define SERIAL_START virtual bool serializer (json::JsonTree& _json_tree_, bool _json_op_, std::string _json_path_, InheritanceIndex& _json_from_) { \
                       return serialize (_json_op_, _json_path_, _json_tree_, _json_from_,


#define SERIAL_END ,__dummy__); }
#define INHERITS(x) bool x::trigger = x::init ();

#define DISAMBIGUATOR_START Serializable* dissambiguator (std::string s) {
#define ELEMENT(x) if (s == #x) { return new x; }
#define DISAMBIGUATOR_END return nullptr; }

#define INHERITS_FROM(y, x) static bool init () {                                      \
                              json::Serializable::addSon( #y , [] { return new y; });  \
                            }                                                          \
                                                                                       \
                            static bool trigger;                                       \
                                                                                       \
                            virtual bool callFatherSerializer (json::JsonTree& tree, std::string path, bool op, InheritanceIndex& from) { \
                              if (x::isTopClass())                                                                             \
                                x::serializer (tree, op, path, from);                                                          \
                              else if (!isTopClass())                                                                          \
                                x::callFatherSerializer (tree, path, op, from);                                                \
                              return y::serializer(tree, op, path, from);                                                      \
                            }                                                                                                  \
                                                                                                                               \
                            bool isTopClass() { return false; }

namespace json {



/* Base class for serializable custom classes
 * ## Description
 *
 *
 * ## Macros
 *
 * - SERIAL_START
 *
 * - SERIAL_END
 *
 * - SERIAL_INHERITED_START (x, y)
 *
 * - INHERITED (x)
 *
 * */
class Serializable {
protected:

  // Used to manage the parameters order in serialization inheritance
  struct InheritanceIndex {
    // what is next element in the vector
    int index;
    // path in the hierarchy
    std::string path;
  };

  struct __end_dummy__ {};

  __end_dummy__ __dummy__;

  static std::map<std::string, std::function<Serializable*()> > dictionary;
  static void addSon (std::string name, std::function<Serializable*()> lambda) { dictionary[name] = lambda; }

public:

  inline void serializeIn (JsonTree& tree, std::string p = "") {
    InheritanceIndex from = {0, p};
    if (!isTopClass())
      callFatherSerializer (tree, p, false, from);
    else
      serializer(tree, false, p, from);
  }

  inline void serializeOut (JsonTree& tree, std::string p = "") {
    if (!p.empty())
      tree.erase(p);
    else
      tree.erase(".");
    InheritanceIndex from = {0, p};
    if (!isTopClass())
      callFatherSerializer (tree, p, true, from);
    else
      serializer(tree, true, p, from);
  }

  inline void serializeIn (const std::string file, std::string p = "") {
    JsonTree tree;
    Parser parser;
    if (parser.parseFile(file, tree) & JSON_PARSE_OUTPUT::OK) {
      InheritanceIndex from = {0, p};
      if (!isTopClass())
        callFatherSerializer (tree, p, false, from);
      else
        serializer(tree, false, p, from);
    } else {
      return;
    }
  }

  inline void serializeOut (const std::string file, std::string p = "") {
    JsonTree tree;
    InheritanceIndex from = {0, p};
    callFatherSerializer (tree, p, true, from);
    //serializer(tree, true, p, from);
    Parser::saveFile(file, tree);
  }

  inline std::string toText (const bool uglify = false) {
    JsonTree tree;
    InheritanceIndex from = {0, ""};
    callFatherSerializer (tree, "path", true, from);
    return tree.toText (uglify, "path");
  }

protected:

  virtual bool serializer (JsonTree& tree, bool b, std::string path, InheritanceIndex& from) = 0;

  // If we are in the top class, serialize from the first element
  virtual bool callFatherSerializer (JsonTree& tree, std::string path, bool op, InheritanceIndex& from) {
    //if (!isTopClass())
      return serializer(tree, op, path, from);
  }

  // checks if is the top class
  virtual bool isTopClass () {return true;}

  // Pointers generator (solve problems with pure abstract classes)
  template <class t>
  typename std::enable_if<!std::is_abstract<t>::value, t*>::type
  const initializePointer (t*& element) {
    return new t();
  }

  template <class t>
  typename std::enable_if<std::is_abstract<t>::value, t*>::type
  const initializePointer (t*& element) {
    return nullptr;
  }

  // Pointers retributors (solve problems with pure abstract classes)
  template <class t>
  typename std::enable_if<!std::is_abstract<t>::value, bool>::type
  const retributionPointer (JsonTree& tree, t*& element) {
    return element->serializeOut (tree, "p");
  }

  template <class t>
  typename std::enable_if<std::is_abstract<t>::value, bool>::type
  const retributionPointer (JsonTree& tree, t*& element) {
    return false;
  }

  //- As seen in http://stackoverflow.com/questions/12877521/human-readable-type-info-name
  // Used for get the class name
  static std::string demangle(const char* mangled) {
    int status;
    std::unique_ptr<char[], void (*)(void*)> result(
      abi::__cxa_demangle(mangled, 0, 0, &status), free);
    return result.get() ? std::string(result.get()) : "error occurred";
  }

protected:
 //- without HASH, with depth
 template <class... Args>
  const bool serialize (bool _json_op_, const std::string _json_path_, JsonTree& _json_tree_, InheritanceIndex& in, Args&... args) {
   if (_json_op_){
     if (in.index == 0)
      _json_tree_.addVector(_json_path_);
     retribution (_json_tree_, in.index, _json_path_, args...);
   } else {
     int size = _json_tree_.getSizeAt(_json_path_);
     initialize (_json_tree_, [&] ()
       {
         int aux = in.index;
         if (in.index < size - 1)
           in.index++;
         in.path = _json_path_ + "." + std::to_string(aux);
         return in.path;
       },
     args...);
    }
  }

  //- With HASH
  template <class str, class... Args>
  typename std::enable_if<std::is_same<const std::string, str>::value || std::is_same<const char*, str>::value, bool>::type
   const serialize (const bool _json_op_, const std::string _json_path_, JsonTree& _json_tree_, InheritanceIndex& in, const str path, Args&... args) {
    if (_json_op_){
      retribution (_json_tree_, _json_path_, path, args...);
    } else {
      initialize (_json_tree_, [&] (std::string key)
        {
          return _json_path_ + "." + key;
        },
      path, args...);
    }
  }

  //- Write

  // DUMMY
  void retribution (JsonTree& tree, std::string path, __end_dummy__& dummy) {}
  void retribution (JsonTree& tree, int& index, std::string path, __end_dummy__& dummy) {}
  void retribution (JsonTree& tree, int& index, __end_dummy__& dummy) {}

  //- NON SERIALIZABLE classes
  template <class t, class... Args>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value, void>::type
   const retribution (JsonTree& tree, int& index, std::string path, t& element, Args&... args) {
    tree.add(element, path);
    index++;
    retribution (tree, index, path, args...);
  }

  //- NON SERIALIZABLE classes WITH HASH KEY
  template <class t, class str, class... Args>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value && (std::is_same<std::string, str>::value || std::is_same<const char*, str>::value), void>::type
   const retribution (JsonTree& tree, std::string path, const str key, t& element, Args&... args) {
    tree.add(element, path + "." + key);
    retribution (tree, path, args...);
  }

  //- SERIALIZABLE classes
  template <class t, class... Args>
  typename std::enable_if<std::is_base_of<Serializable, t>::value, void>::type
   const retribution (JsonTree& tree, int& index, const std::string path, t& element, Args&... args) {
    JsonTree auxTree;
    element.serializeOut (auxTree, "p");
    tree.add(auxTree, "p", path);
    index++;
    retribution (tree,index, path, args...);
  }

  //- SERIALIZABLE classes WITH HASH KEY
  template <class t, class str, class... Args>
  typename std::enable_if<std::is_base_of<Serializable, t>::value && (std::is_same<std::string, str>::value || std::is_same<const char*, str>::value), void>::type
   const retribution (JsonTree& tree, std::string path, const str key, t& element, Args&... args) {
    JsonTree auxTree;
    element.serializeOut (auxTree, "p");
    tree.add(auxTree, "p", path + "." + key);
    retribution (tree, path, args...);
  }

  //- Vector
  template <class t, class... Args>
  void  const retribution (JsonTree& tree, int& index, const std::string path, std::vector<t>& vect, Args&... args) {
    tree.addVector(path);

    std::string newPath = path + "." + std::to_string(index);
    for (int j = 0; j < vect.size(); j++) {
      int index = j;
      retribution (tree, index, newPath, vect[j], __dummy__);
    }
    ++index;
    retribution (tree,index, path, args...);
  }

  //- Vector WITH HASH KEY
  template <class t, class str, class... Args>
  void  const retribution (JsonTree& tree, std::string path, const str key, std::vector<t>& vect, Args&... args) {
    std::string newPath = path + "." + key;
    tree.addVector(newPath);

    for (int j = 0; j < vect.size(); j++) {
      int index = j;
      retribution (tree, index, newPath, vect[j], __dummy__);
    }    
    retribution (tree, path, args...);
  }

  //- Pointers of SERIALIZABLE classes
  template <class t, class... Args>
  typename std::enable_if<std::is_base_of<Serializable, t>::value, void>::type
   const retribution (JsonTree& tree, int& index, std::string path, t*& element, Args&... args) {
    JsonTree auxTree;

    if (element == nullptr)
      return;

    tree.addMap(path);
    element->serializeOut (auxTree, "p");

    std::string className = demangle(typeid(*element).name());
    tree.add(className, path + "." + std::to_string(index) + "." + CLASS_TYPE);
    tree.add(auxTree, "p", path + "." + std::to_string(index) + "." + CLASS_CONTENT);

    index++;

    retribution (tree,index, path, args...);
  }

  //- Pointers of SERIALIZABLE classes WITH HASH KEY
  template <class t, class str, class... Args>
  typename std::enable_if<std::is_base_of<Serializable, t>::value && (std::is_same<std::string, str>::value || std::is_same<const char*, str>::value), void>::type
   const retribution (JsonTree& tree, std::string path, const str key, t*& element, Args&... args) {
    JsonTree auxTree;

    if (element == nullptr)
      return;

    element->serializeOut (auxTree, "p");

    std::string className = demangle(typeid(*element).name());

    tree.add(className, path + "." + key + "." + CLASS_TYPE);
    tree.add(auxTree, "p", path + "." + key + "." + CLASS_CONTENT);
    retribution (tree, path, args...);
  }

  //- Pointers of NON SERIALIZABLE classes  
  template <class t, class... Args>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value, void>::type
   const retribution (JsonTree& tree, int& index, std::string path, t*& element, Args&... args) {
    tree.add(*element, path);
    index++;
    retribution (tree,index, path, args...);
  }

  //- Pointers of NON SERIALIZABLE classes WITH HASH KEY
  template <class t, class str, class... Args>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value && (std::is_same<std::string, str>::value || std::is_same<const char*, str>::value), void>::type
   const retribution (JsonTree& tree, std::string path, const str key, t*& element, Args&... args) {
    tree.add(*element, path + "." + key);
    retribution (tree, path, args...);
  }

  //- Read ----------------------------------------------------------------------------

  // DUMMY
  template <class func>
  void const initialize (JsonTree& tree, func functor, __end_dummy__& dummy) {}

  //- NON SERIALIZABLE classes
  template <class t, class func, class... Args>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value, void>::type
   const initialize (JsonTree& tree, func functor, t& element, Args&... args) {
    tree.get(element, functor());
    initialize (tree, functor, args...);
  }

  //- NON SERIALIZABLE classes WITH HASH KEY
  template <class t, class func, class str, class... Args>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value && (std::is_same<std::string, str>::value || std::is_same<const char*, str>::value), void>::type
   const initialize (JsonTree& tree, func functor, const str key, t& element, Args&... args) {
    tree.get(element, functor(key));
    initialize (tree, functor, args...);
  }

  //- SERIALIZABLE classes
  template <class t, class func, class... Args>
  typename std::enable_if<std::is_base_of<Serializable, t>::value, void>::type
   const initialize (JsonTree& tree, func functor, t& element, Args&... args) {
    element.serializeIn (tree, functor());
    initialize (tree, functor, args...);
  }

  //- SERIALIZABLE classes WITH HASH KEY
  template <class t, class func, class str, class... Args>
  typename std::enable_if<std::is_base_of<Serializable, t>::value && (std::is_same<std::string, str>::value || std::is_same<const char*, str>::value), void>::type
   const initialize (JsonTree& tree, func functor, const str key, t& element, Args&... args) {
    element.serializeIn (tree, functor(key));
    initialize (tree, functor, args...);
  }

  //- Vector
  template <class t, class func, class... Args>
  void  const initialize (JsonTree& tree, func functor, std::vector <t>& vect, Args&... args) {
    std::string newPath = functor();
    int size = tree.getSizeAt(newPath);
    if (size > 0)
      vect.resize (size);

    int i = 0;
    for (int j = 0; j < size; j++) {
      initialize (tree, [&] () {
        return newPath + "." + std::to_string(i);
      }, vect[j], __dummy__);
      i++;
    }    
    initialize (tree, functor, args...);
  }

  //- Vector WITH HASH KEY
  template <class t, class func, class str, class... Args>
  typename std::enable_if<std::is_same<std::string, str>::value || std::is_same<const char*, str>::value, void>::type
   const initialize (JsonTree& tree, func functor, const str path, std::vector <t>& vect, Args&... args) {
    std::string newPath = functor(path);
    int size = tree.getSizeAt(newPath);
    if (size > 0)
      vect.resize (size);
    int i = 0;
    for (int j = 0; j < vect.size(); j++) {
      initialize (tree, [&] () {
        return newPath + "." + std::to_string(i);
      }, vect[j], __dummy__);
      i++;
    }    
    initialize (tree, functor, args...);
  }

  //- Pointers of SERIALIZABLE classes
  template <class t, class func, class... Args>
  typename std::enable_if<std::is_base_of<Serializable, t>::value, void>::type
   const initialize (JsonTree& tree, func functor, t*& element, Args&... args) {
    std::string path = functor();
    std::string type;
    tree.get(type, path + "." + CLASS_TYPE);

    auto obj = dictionary[type];

    if (obj != nullptr) {
      element = static_cast<t*>(obj());
      element->serializeIn (tree, path + "." + CLASS_CONTENT);
    } else {
      element = initializePointer (element) ;
    }

    initialize (tree, functor, args...);
  }

  //- Pointers of SERIALIZABLE classes WITH HASH KEY
  template <class t, class func, class str, class... Args>
  typename std::enable_if<std::is_base_of<Serializable, t>::value && (std::is_same<std::string, str>::value || std::is_same<const char*, str>::value), void>::type
   const initialize (JsonTree& tree, func functor,  const str key, t*& element, Args&... args) {    
    std::string path = functor(key);
    std::string type;
    tree.get(type, path + "." + CLASS_TYPE);

    auto obj = dictionary[type];

    if (obj != nullptr) {
      element = static_cast<t*>(obj());
      element->serializeIn (tree, path + "." + CLASS_CONTENT);
    } else {
      element = initializePointer (element);
    }

    initialize (tree, functor, args...);
  }

  //- Pointers of NON SERIALIZABLE classes
  template <class t, class func, class... Args>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value, void>::type
   const initialize (JsonTree& tree, func functor, t*& element, Args&... args) {
    element = new t ();
    tree.get(*element, functor());
    initialize (tree, functor, args...);
  }

  //- Pointers of NON SERIALIZABLE classes WITH HASH KEY
  template <class t, class func, class str, class... Args>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value && (std::is_same<std::string, str>::value || std::is_same<const char*, str>::value), void>::type
   const initialize (JsonTree& tree, func functor,  const str key, t*& element, Args&... args) {
    element = new t ();
    tree.get(*element, functor(key));
    initialize (tree, functor, args...);
  }

};

}
#endif // SERIALIZABLE_H
