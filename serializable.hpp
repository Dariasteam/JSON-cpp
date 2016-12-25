#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include <string>
#include <type_traits>
#include <cxxabi.h>

#include "manager.hpp"
#include "parser.hpp"

#define CLASS_TYPE "classType"
#define CLASS_CONTENT "classContent"

#define SERIAL_START_INHERITED(y, x) INHERITS_FROM (y, x)                                                                                                 \
                                      virtual bool serializer (json::JsonTree& _json_tree_, bool _json_op_, string _json_path_, InheritanceIndex& _json_from_) { \
                                        return serialize (_json_op_, _json_path_, _json_tree_, _json_from_,

#define SERIAL_START virtual bool serializer (json::JsonTree& _json_tree_, bool _json_op_, string _json_path_, InheritanceIndex& _json_from_) { \
                       return serialize (_json_op_, _json_path_, _json_tree_, _json_from_,


#define SERIAL_END ); }
#define INHERITS(x) bool x::trigger = x::init ();

#define DISAMBIGUATOR_START Serializable* dissambiguator (string s) {
#define ELEMENT(x) if (s == #x) { return new x; }
#define DISAMBIGUATOR_END return nullptr; }

#define INHERITS_FROM(y, x) static bool init () {                                      \
                              json::Serializable::addSon( #y , [] { return new y; });  \
                            }                                                          \
                                                                                       \
                            static bool trigger;                                       \
                                                                                       \
                            virtual bool callFatherSerializer (json::JsonTree& tree, string path, bool op, InheritanceIndex& from) { \
                              if (x::isTopClass())                                                                             \
                                x::serializer (tree, op, path, from);                                                          \
                              else if (!isTopClass())                                                                          \
                                x::callFatherSerializer (tree, path, op, from);                                                \
                              return y::serializer(tree, op, path, from);                                                      \
                            }                                                                                                  \
                                                                                                                               \
                            bool isTopClass() { return false; }

struct InheritanceIndex {
  int index;
  string path;
};

namespace json {

class Serializable {
protected:
  static map<string, function<Serializable*()> > dictionary;
  static void addSon (string name, function<Serializable*()> lambda) { dictionary[name] = lambda; }
public:

  inline void serializeIn (JsonTree& tree, string p = "") {
    InheritanceIndex from = {0, p};
    if (!isTopClass())
      callFatherSerializer (tree, p, false, from);
    else
      serializer(tree, false, p, from);
  }

  inline void serializeOut (JsonTree& tree, string p = "") {
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

  inline void serializeIn (const string file, string p = "") {
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

  inline void serializeOut (const string file, string p = "") {
    JsonTree tree;
    InheritanceIndex from = {0, p};
    callFatherSerializer (tree, p, true, from);
    serializer(tree, true, p, from);
    Parser::saveFile(file, tree);
  }

protected:

  virtual bool serializer (JsonTree& tree, bool b, string path, InheritanceIndex& from) = 0;

  // If we are in the top class, serialize from the first element
  virtual bool callFatherSerializer (JsonTree& tree, string path, bool op, InheritanceIndex& from) { if (!isTopClass()) return serializer(tree, op, path, from); }

  // check if is the top class
  virtual bool isTopClass () {return true;}

  // Pointers generator (solve problems with pure abstract classes)
  template <class t>
  typename std::enable_if<!is_abstract<t>::value, t*>::type
  const initializePointer (t*& element) {
    return new t();
  }

  template <class t>
  typename std::enable_if<is_abstract<t>::value, t*>::type
  const initializePointer (t*& element) {
    return nullptr;
  }

  // Pointers retributors (solve problems with pure abstract classes)
  template <class t>
  typename std::enable_if<!is_abstract<t>::value, bool>::type
  const retributionPointer (JsonTree& tree, t*& element) {
    return element->serializeOut (tree, "p");
  }

  template <class t>
  typename std::enable_if<is_abstract<t>::value, bool>::type
  const retributionPointer (JsonTree& tree, t*& element) {
    return false;
  }

  //- As seen in http://stackoverflow.com/questions/12877521/human-readable-type-info-name
  // Used for get the class name
  static string demangle(const char* mangled) {
    int status;
    unique_ptr<char[], void (*)(void*)> result(
      abi::__cxa_demangle(mangled, 0, 0, &status), free);
    return result.get() ? string(result.get()) : "error occurred";
  }

protected:
 //- without HASH, with depth
 template <class... Args>
  const bool serialize (bool _json_op_, const string _json_path_, JsonTree& _json_tree_, InheritanceIndex& in, Args&... args) {
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
         in.path = _json_path_ + "." + to_string(aux);
         return in.path;
       },
     args...);
   }
 }

  //- With HASH
  template <class str, class... Args>
  typename std::enable_if<std::is_same<const string, str>::value || std::is_same<const char*, str>::value, bool>::type
   const serialize (const bool _json_op_, const string _json_path_, JsonTree& _json_tree_, InheritanceIndex& in, const str path, Args&... args) {
    if (_json_op_){
      retribution (_json_tree_, _json_path_, path, args...);
    } else {
      initialize (_json_tree_, [&] (string key)
        {
          return _json_path_ + "." + key;
        },
      path, args...);
    }
  }

  //- Write

  //- NON SERIALIZABLE classes
  template <typename t>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value, void>::type
   const retribution (JsonTree& tree, int& index, string path, t& element) {
    tree.add(element, path);
    index++;
  }

  template <class t, class... Args>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value, void>::type
   const retribution (JsonTree& tree, int& index, string path, t& element, Args&... args) {
    retribution (tree, index, path, element);
    retribution (tree, index, path, args...);
  }

  //- NON SERIALIZABLE classes WITH HASH KEY
  template <class t, class str>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
   const retribution (JsonTree& tree, string path, const str key, t& element) {
    tree.add (element, path + "." + key);
  }

  template <class t, class str, class... Args>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
   const retribution (JsonTree& tree, string path, const str key, t& element, Args&... args) {
    tree.add(element, path + "." + key);
    retribution (tree, path, args...);
  }

  //- SERIALIZABLE classes
  template <typename t>
  typename std::enable_if<std::is_base_of<Serializable, t>::value, void>::type
   const retribution (JsonTree& tree, int& index, const string path, t& element) {
    JsonTree auxTree;
    element.serializeOut (auxTree, "p");
    tree.add(auxTree, "p", path);
    index++;
  }

  template <class t, class... Args>
  typename std::enable_if<std::is_base_of<Serializable, t>::value, void>::type
   const retribution (JsonTree& tree, int& index, const string path, t& element, Args&... args) {
    retribution (tree, index, path, element); // Base case

    retribution (tree,index, path, args...);
  }

  //- SERIALIZABLE classes WITH HASH KEY
  template <class t, class str>
  typename std::enable_if<std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
   const retribution (JsonTree& tree, string path, const str key, t& element) {
    JsonTree auxTree;
    element.serializeOut (auxTree, "p");
    tree.add(auxTree, "p", path + "." + key);
  }

  template <class t, class str, class... Args>
  typename std::enable_if<std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
   const retribution (JsonTree& tree, string path, const str key, t& element, Args&... args) {
    retribution (tree, path, key, element); // Base case

    retribution (tree, path, args...);
  }

  //- Vector
  template <class t>
  void  const retribution (JsonTree& tree, int& index, const string path, vector <t>& vect) {
    tree.addVector(path);

    string newPath = path + "." + to_string(index);
    for (int j = 0; j < vect.size(); j++) {
      int index = j;
      retribution (tree, index, newPath, vect[j]);
    }
    ++index;
  }

  template <class t, class... Args>
  void  const retribution (JsonTree& tree, int& index, const string path, vector<t>& vect, Args&... args) {
    retribution (tree, index, path, vect);

    retribution (tree,index, path, args...);
  }

  //- Vector WITH HASH KEY
  template <class t, class str>
  void  const retribution (JsonTree& tree, string path, const str key, vector <t>& vect) {

    string newPath = path + "." + key;
    tree.addVector(newPath);

    for (int j = 0; j < vect.size(); j++) {
      int index = j;
      retribution (tree, index, newPath, vect[j]);
    }
  }

  template <class t, class str, class... Args>
  void  const retribution (JsonTree& tree, string path, const str key, vector<t>& vect, Args&... args) {
    retribution (tree, path, key, vect);

    retribution (tree, path, args...);
  }

  //- Pointers of SERIALIZABLE classes
  template <typename t>
  typename std::enable_if<std::is_base_of<Serializable, t>::value, void>::type
   const retribution (JsonTree& tree, int& index, string path, t*& element) {
    JsonTree auxTree;

    if (element == nullptr)
      return;

    tree.addMap(path);
    element->serializeOut (auxTree, "p");

    string className = demangle(typeid(*element).name());
    tree.add(className, path + "." + to_string(index) + "." + CLASS_TYPE);
    tree.add(auxTree, "p", path + "." + to_string(index) + "." + CLASS_CONTENT);

    index++;
  }

  template <class t, class... Args>
  typename std::enable_if<std::is_base_of<Serializable, t>::value, void>::type
   const retribution (JsonTree& tree, int& index, string path, t*& element, Args&... args) {
    retribution (tree, index, path, element);

    retribution (tree,index, path, args...);
  }

  //- Pointers of SERIALIZABLE classes WITH HASH KEY
  template <typename t, class str>
  typename std::enable_if<std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
   const retribution (JsonTree& tree, string path, const str key, t*& element) {
    JsonTree auxTree;

    if (element == nullptr)
      return;

    element->serializeOut (auxTree, "p");

    string className = demangle(typeid(*element).name());

    tree.add(className, path + "." + key + "." + CLASS_TYPE);
    tree.add(auxTree, "p", path + "." + key + "." + CLASS_CONTENT);
  }

  template <class t, class str, class... Args>
  typename std::enable_if<std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
   const retribution (JsonTree& tree, string path, const str key, t*& element, Args&... args) {
    retribution (tree, path, key, element);
    retribution (tree, path, args...);
  }

  //- Pointers of NON SERIALIZABLE classes
  template <typename t>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value, void>::type
   const retribution (JsonTree& tree, int& index, string path, t*& element) {
    tree.add(*element, path);
    index++;
  }

  template <class t, class... Args>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value, void>::type
   const retribution (JsonTree& tree, int& index, string path, t*& element, Args&... args) {
    retribution (tree, index, path, element);
    retribution (tree,index, path, args...);
  }

  //- Pointers of NON SERIALIZABLE classes WITH HASH KEY
  template <typename t, class str>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
   const retribution (JsonTree& tree, string path, const str key, t*& element) {
    tree.add(*element, path + "." + key);
  }

  template <class t, class str, class... Args>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
   const retribution (JsonTree& tree, string path, const str key, t*& element, Args&... args) {
    tree.add(*element, path + "." + key);
    retribution (tree, path, args...);
  }

  //- Read ----------------------------------------------------------------------------

  //- NON SERIALIZABLE classes
  template <typename t, class func>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value, void>::type
   const initialize (JsonTree& tree, func functor, t& element) {
    tree.get(element, functor());
  }

  template <class t, class func, class... Args>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value, void>::type
   const initialize (JsonTree& tree, func functor, t& element, Args&... args) {
    tree.get(element, functor());
    initialize (tree, functor, args...);
  }

  //- NON SERIALIZABLE classes WITH HASH KEY
  template <class t, class str, class func>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
   const initialize (JsonTree& tree, func functor, const str key, t& element) {
    tree.get(element, functor(key));
  }

  template <class t, class func, class str, class... Args>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
   const initialize (JsonTree& tree, func functor, const str key, t& element, Args&... args) {
    tree.get(element, functor(key));
    initialize (tree, functor, args...);
  }

  //- SERIALIZABLE classes
  template <typename t, class func>
  typename std::enable_if<std::is_base_of<Serializable, t>::value, void>::type
   const initialize (JsonTree& tree, func functor, t& element) {
    element.serializeIn (tree, functor());
  }


  template <class t, class func, class... Args>
  typename std::enable_if<std::is_base_of<Serializable, t>::value, void>::type
   const initialize (JsonTree& tree, func functor, t& element, Args&... args) {
    element.serializeIn (tree, functor());

    initialize (tree, functor, args...);
  }

  //- SERIALIZABLE classes WITH HASH KEY
  template <typename t, class str, class func>
  typename std::enable_if<std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
   const initialize (JsonTree& tree, func functor, const str key, t& element) {
    element.serializeIn (tree, functor(key));
  }

  template <class t, class func, class str, class... Args>
  typename std::enable_if<std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
   const initialize (JsonTree& tree, func functor, const str key, t& element, Args&... args) {
    element.serializeIn (tree, functor(key));

    initialize (tree, functor, args...);
  }

  //- Vector
  template <class t, class func>
  void  const initialize (JsonTree& tree, func functor, vector <t>& vect) {
    string newPath = functor();
    int size = tree.getSizeAt(newPath);
    if (size > 0)
      vect.resize (size);

    int i = 0;
    for (int j = 0; j < size; j++) {
      initialize (tree, [&] () {
        return newPath + "." + to_string(i);
      }, vect[j]);
      i++;
    }
  }

  template <class t, class func, class... Args>
  void  const initialize (JsonTree& tree, func functor, vector <t>& vect, Args&... args) {
    initialize (tree, functor, vect);

    initialize (tree, functor, args...);
  }

  //- Vector WITH HASH KEY (STRING)
  template <class t, class str, class func>
  typename std::enable_if<std::is_same<string, str>::value || std::is_same<const char*, str>::value, void>::type
   const initialize (JsonTree& tree, func functor, const str path, vector <t>& vect) {
    string newPath = functor(path);
    int size = tree.getSizeAt(newPath);
    if (size > 0)
      vect.resize (size);
    int i = 0;
    for (int j = 0; j < vect.size(); j++) {
      initialize (tree, [&] () {
        return newPath + "." + to_string(i);
      }, vect[j]);
      i++;
    }
  }

  template <class t, class func, class str, class... Args>
  typename std::enable_if<std::is_same<string, str>::value || std::is_same<const char*, str>::value, void>::type
   const initialize (JsonTree& tree, func functor, const str path, vector <t>& vect, Args&... args) {
    initialize (tree, functor, path, vect);

    initialize (tree, functor, args...);
  }

  //- Pointers of SERIALIZABLE classes
  template <class t, class func>
  typename std::enable_if<std::is_base_of<Serializable, t>::value, void>::type
  const initialize (JsonTree& tree, func functor, t*& element) {

    string path = functor();
    string type;
    tree.get(type, path + "." + CLASS_TYPE);

    auto obj = dictionary[type];

    if (obj != nullptr) {
      element = static_cast<t*>(obj());
      element->serializeIn (tree, path + "." + CLASS_CONTENT);
    } else {
      element = initializePointer (element) ;
    }
  }

  template <class t, class func, class... Args>
  typename std::enable_if<std::is_base_of<Serializable, t>::value, void>::type
   const initialize (JsonTree& tree, func functor, t*& element, Args&... args) {
    initialize (tree, functor, element);

    initialize (tree, functor, args...);
  }

  //- Pointers of SERIALIZABLE classes WITH HASH KEY
  template <class t, class str, class func>
  typename std::enable_if<std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
   const initialize (JsonTree& tree, func functor, const str key, t*& element) {

    string path = functor(key);
    string type;
    tree.get(type, path + "." + CLASS_TYPE);

    auto obj = dictionary[type];

    if (obj != nullptr) {
      element = static_cast<t*>(obj());
      element->serializeIn (tree, path + "." + CLASS_CONTENT);
    } else {
      element = initializePointer (element);
    }
  }

  template <class t, class func, class str, class... Args>
  typename std::enable_if<std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
   const initialize (JsonTree& tree, func functor,  const str key, t*& element, Args&... args) {
    initialize (tree, functor, key, element);

    initialize (tree, functor, args...);
  }

  //- Pointers of NON SERIALIZABLE classes
  template <class t, class func>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value, void>::type
   const initialize (JsonTree& tree, func functor, t*& element) {
    element = new t ();
    tree.get(*element, functor());
  }

  template <class t, class func, class... Args>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value, void>::type
   const initialize (JsonTree& tree, func functor, t*& element, Args&... args) {
    element = new t ();
    tree.get(*element, functor());
    initialize (tree, functor, args...);
  }

  //- Pointers of NON SERIALIZABLE classes WITH HASH KEY
  template <class t, class str, class func>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
   const initialize (JsonTree& tree, func functor, const str key, t*& element) {
    element = new t ();
    tree.get(*element, functor(key));
  }

  template <class t, class func, class str, class... Args>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
   const initialize (JsonTree& tree, func functor,  const str key, t*& element, Args&... args) {
    element = new t ();
    tree.get(*element, functor(key));
    initialize (tree, functor, args...);
  }

};

}
#endif // SERIALIZABLE_H
