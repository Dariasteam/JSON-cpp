#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include <string>
#include <type_traits>

#include "manager.hpp"
#include "parser.hpp"

#define SERIAL_START void serializer (JsonTree& _json_tree_, bool _json_op_, string _json_path_) { serialize (_json_op_, _json_path_, _json_tree_,
#define SERIAL_END ); }

namespace json {

class Serializable {
public:

  inline void serializeIn (JsonTree& tree, string p = "") {
    serializer(tree, false, p);
  }

  inline void serializeOut (JsonTree& tree, string p = "") {
    serializer(tree, true, p);
  }

  inline void serializeIn (string file, string p = "") {
    JsonTree tree;
    Parser parser;
    if (parser.parseFile(file, tree) & JSON_PARSE_OUTPUT::OK)
      serializer(tree, false, p);
    else
      return;
  }

  inline void serializeOut (string file, string p = "") {
    JsonTree tree;
    serializer(tree, true, p);
    Parser::saveFile(file, tree);
  }

protected:
  virtual void serializer (JsonTree& tree, bool b, string path) = 0;

  //- without HASH
  template <class... Args>
  static const void serialize (bool _json_op_, string _json_path_, JsonTree& _json_tree_, Args&... args) {
    if (_json_op_){
      if (!_json_path_.empty()) {
        _json_tree_.erase(_json_path_);
      } else {
        _json_tree_.erase(".");
      }
      _json_tree_.addVector(_json_path_);
      retribution (_json_tree_, 0, _json_path_, args...);
    } else {
      int i = 0;
      int size = _json_tree_.getSizeAt(_json_path_);
      initialize (_json_tree_, [&] ()
        {
          int aux = i;
          if (i < size - 1)
            i++;
          return _json_path_ + "." + to_string(aux);
        },
      args...);
    }
  }

  //- With HASH
  template <class str, class... Args>
  typename std::enable_if<std::is_same<string, str>::value || std::is_same<const char*, str>::value, void>::type
  static const serialize (bool _json_op_, string _json_path_, JsonTree& _json_tree_, const str path, Args&... args) {
    if (_json_op_){
      if (!_json_path_.empty()) {
        _json_tree_.erase(_json_path_);
      } else {
        _json_tree_.erase(".");
      }
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
  static const retribution (JsonTree& tree, int index, string path, t& element) {
    tree.add(element, path);
  }

  template <class t, class... Args>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value, void>::type
  static const retribution (JsonTree& tree, int index, string path, t& element, Args&... args) {
    tree.add(element, path);
    retribution (tree, index+1, path, args...);
  }

  //- NON SERIALIZABLE classes WITH HASH KEY
  template <class t, class str>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
  static const retribution (JsonTree& tree, string path, const str key, t& element) {
    tree.add (element, path + "." + key);
  }

  template <class t, class str, class... Args>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
  static const retribution (JsonTree& tree, string path, const str key, t& element, Args&... args) {
    tree.add(element, path + "." + key);
    retribution (tree, path, args...);
  }

  //- SERIALIZABLE classes
  template <typename t>
  typename std::enable_if<std::is_base_of<Serializable, t>::value, void>::type
  static const retribution (JsonTree& tree, int index, const string path, t& element) {
    JsonTree auxTree;
    element.serializeOut (auxTree, "p");
    tree.add(auxTree, "p", path);
  }

  template <class t, class... Args>
  typename std::enable_if<std::is_base_of<Serializable, t>::value, void>::type
  static const retribution (JsonTree& tree, int index, const string path, t& element, Args&... args) {
    retribution (tree, index, path, element); // Base case

    retribution (tree, index+1, path, args...);
  }

  //- SERIALIZABLE classes WITH HASH KEY
  template <class t, class str>
  typename std::enable_if<std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
  static const retribution (JsonTree& tree, string path, const str key, t& element) {
    JsonTree auxTree;
    element.serializeOut (auxTree, "p");
    tree.add(auxTree, "p", path + "." + key);
  }

  template <class t, class str, class... Args>
  typename std::enable_if<std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
  static const retribution (JsonTree& tree, string path, const str key, t& element, Args&... args) {
    retribution (tree, path, key, element); // Base case

    retribution (tree, path, args...);
  }

  //- Vector
  template <class t>
  void static const retribution (JsonTree& tree, int index, const string path, vector <t>& vect) {
    tree.addVector(path);

    string newPath = path + "." + to_string(index);
    for (int j = 0; j < vect.size(); j++)
      retribution (tree, j, newPath, vect[j]);
  }

  template <class t, class... Args>
  void static const retribution (JsonTree& tree, int index, const string path, vector<t>& vect, Args&... args) {
    retribution (tree, index, path, vect);

    retribution (tree, index+1, path, args...);
  }

  //- Vector WITH HASH KEY
  template <class t, class str>
  void static const retribution (JsonTree& tree, string path, const str key, vector <t>& vect) {

    string newPath = path + "." + key;
    tree.addVector(newPath);

    for (int j = 0; j < vect.size(); j++)
      retribution (tree, j, newPath, vect[j]);
  }

  template <class t, class str, class... Args>
  void static const retribution (JsonTree& tree, string path, const str key, vector<t>& vect, Args&... args) {
    retribution (tree, path, key, vect);

    retribution (tree, path, args...);
  }

  //- Pointers of SERIALIZABLE classes
  template <typename t>
  typename std::enable_if<std::is_base_of<Serializable, t>::value, void>::type
  static const retribution (JsonTree& tree, int index, string path, t*& element) {
    JsonTree auxTree;
    element->serializeOut (auxTree, "p");
    tree.add(auxTree, "p", path);
  }

  template <class t, class... Args>
  typename std::enable_if<std::is_base_of<Serializable, t>::value, void>::type
  static const retribution (JsonTree& tree, int index, string path, t*& element, Args&... args) {
    retribution (tree, index, path, element);

    retribution (tree, index+1, path, args...);
  }

  //- Pointers of SERIALIZABLE classes WITH HASH KEY
  template <typename t, class str>
  typename std::enable_if<std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
  static const retribution (JsonTree& tree, string path, const str key, t*& element) {
    JsonTree auxTree;
    element->serializeOut (auxTree, "p");
    tree.add(auxTree, "p", path + key);
  }

  template <class t, class str, class... Args>
  typename std::enable_if<std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
  static const retribution (JsonTree& tree, string path, const str key, t*& element, Args&... args) {
    retribution (tree, path, key, element);
    retribution (tree, path, args...);
  }

  //- Pointers of NON SERIALIZABLE classes
  template <typename t>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value, void>::type
  static const retribution (JsonTree& tree, int index, string path, t*& element) {
    tree.add(*element, path);
  }

  template <class t, class... Args>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value, void>::type
  static const retribution (JsonTree& tree, int index, string path, t*& element, Args&... args) {
    tree.add(*element, path);
    retribution (tree, index+1, path, args...);
  }

  //- Pointers of NON SERIALIZABLE classes WITH HASH KEY
  template <typename t, class str>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
  static const retribution (JsonTree& tree, string path, const str key, t*& element) {
    tree.add(*element, path + "." + key);
  }

  template <class t, class str, class... Args>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
  static const retribution (JsonTree& tree, string path, const str key, t*& element, Args&... args) {
    tree.add(*element, path + "." + key);
    retribution (tree, path, args...);
  }

  //- Read ----------------------------------------------------------------------------

  //- NON SERIALIZABLE classes
  template <typename t, class func>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value, void>::type
  static const initialize (JsonTree& tree, func functor, t& element) {
    tree.get(element, functor());
  }

  template <class t, class func, class... Args>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value, void>::type
  static const initialize (JsonTree& tree, func functor, t& element, Args&... args) {
    tree.get(element, functor());
    initialize (tree, functor, args...);
  }

  //- NON SERIALIZABLE classes WITH HASH KEY
  template <class t, class str, class func>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
  static const initialize (JsonTree& tree, func functor, const str key, t& element) {
    tree.get(element, functor(key));
  }

  template <class t, class func, class str, class... Args>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
  static const initialize (JsonTree& tree, func functor, const str key, t& element, Args&... args) {
    tree.get(element, functor(key));
    initialize (tree, functor, args...);
  }

  //- SERIALIZABLE classes
  template <typename t, class func>
  typename std::enable_if<std::is_base_of<Serializable, t>::value, void>::type
  static const initialize (JsonTree& tree, func functor, t& element) {
    element.serializeIn (tree, functor());
  }


  template <class t, class func, class... Args>
  typename std::enable_if<std::is_base_of<Serializable, t>::value, void>::type
  static const initialize (JsonTree& tree, func functor, t& element, Args&... args) {
    element.serializeIn (tree, functor());

    initialize (tree, functor, args...);
  }

  //- SERIALIZABLE classes WITH HASH KEY
  template <typename t, class str, class func>
  typename std::enable_if<std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
  static const initialize (JsonTree& tree, func functor, const str key, t& element) {
    element.serializeIn (tree, functor(key));
  }

  template <class t, class func, class str, class... Args>
  typename std::enable_if<std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
  static const initialize (JsonTree& tree, func functor, const str key, t& element, Args&... args) {
    element.serializeIn (tree, functor(key));

    initialize (tree, functor, args...);
  }

  //- Vector
  template <class t, class func>
  void static const initialize (JsonTree& tree, func functor, vector <t>& vect) {
    string newPath = functor();
    int size = tree.getSizeAt(newPath);
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
  void static const initialize (JsonTree& tree, func functor, vector <t>& vect, Args&... args) {
    initialize (tree, functor, vect);

    initialize (tree, functor, args...);
  }

  //- Vector WITH HASH KEY (STRING)
  template <class t, class str, class func>
  typename std::enable_if<std::is_same<string, str>::value || std::is_same<const char*, str>::value, void>::type
  static const initialize (JsonTree& tree, func functor, const str path, vector <t>& vect) {
    string newPath = functor(path);
    vect.resize (tree.getSizeAt(newPath));
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
  static const initialize (JsonTree& tree, func functor, const str path, vector <t>& vect, Args&... args) {
    initialize (tree, functor, path, vect);

    initialize (tree, functor, args...);
  }

  //- Pointers of SERIALIZABLE classes
  template <class t, class func>
  typename std::enable_if<std::is_base_of<Serializable, t>::value, void>::type
  static const initialize (JsonTree& tree, func functor, t*& element) {
    element = new t ();
    element->serializeIn (tree, functor());
  }

  template <class t, class func, class... Args>
  typename std::enable_if<std::is_base_of<Serializable, t>::value, void>::type
  static const initialize (JsonTree& tree, func functor, t*& element, Args&... args) {
    initialize (tree, functor, element);

    initialize (tree, functor, args...);
  }

  //- Pointers of SERIALIZABLE classes WITH HASH KEY
  template <class t, class str, class func>
  typename std::enable_if<std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
  static const initialize (JsonTree& tree, func functor, const str key, t*& element) {
    element = new t ();

    element->serializeIn (tree, functor(key));
  }

  template <class t, class func, class str, class... Args>
  typename std::enable_if<std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
  static const initialize (JsonTree& tree, func functor,  const str key, t*& element, Args&... args) {
    initialize (tree, functor, key, element);

    initialize (tree, functor, args...);
  }

  //- Pointers of NON SERIALIZABLE classes
  template <class t, class func>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value, void>::type
  static const initialize (JsonTree& tree, func functor, t*& element) {
    element = new t ();
    tree.get(*element, functor());
  }

  template <class t, class func, class... Args>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value, void>::type
  static const initialize (JsonTree& tree, func functor, t*& element, Args&... args) {
    element = new t ();
    tree.get(*element, functor());
    initialize (tree, functor, args...);
  }

  //- Pointers of NON SERIALIZABLE classes WITH HASH KEY
  template <class t, class str, class func>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
  static const initialize (JsonTree& tree, func functor, const str key, t*& element) {
    element = new t ();
    tree.get(*element, functor(key));
  }

  template <class t, class func, class str, class... Args>
  typename std::enable_if<!std::is_base_of<Serializable, t>::value && (std::is_same<string, str>::value || std::is_same<const char*, str>::value), void>::type
  static const initialize (JsonTree& tree, func functor,  const str key, t*& element, Args&... args) {
    element = new t ();
    tree.get(*element, functor(key));
    initialize (tree, functor, args...);
  }

};

}
#endif // SERIALIZABLE_H
