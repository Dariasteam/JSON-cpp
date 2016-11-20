#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include <string>
#include <type_traits>
#include "manager.hpp"

#define SERIAL_START void serializer (JsonTree* _json_tree_, bool _json_op_, string _json_path_) {serialize (_json_op_, _json_path_, _json_tree_,
#define SERIAL_END ); }

namespace json {

class Serializable {
public:

  inline void serializeIn (JsonTree& tree, string p) {
    serializer(&tree, false, p);
  }

  inline void serializeOut (JsonTree& tree, string p) {
    serializer(&tree, true, p);
  }

protected:
  virtual void serializer (JsonTree* tree, bool b, string path) = 0;

  template <class... Args>
  static void serialize (bool _json_op_, string _json_path_, JsonTree* _json_tree_, Args... args) {
    if (_json_op_){
      _json_tree_->erase(_json_path_);
      _json_tree_->addVector(_json_path_);
      retribution (_json_tree_, 0, _json_path_, args...);
    } else {
      int i = 0;
      int size = _json_tree_->getSizeAt(_json_path_);
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

  // Write
  template <typename t>
  void static retribution (JsonTree* tree, int index, string path, t* element) {
    tree->add(*element, path);
  }


  template <class t, class... Args>
  void static retribution (JsonTree* tree, int index, string path, t element, Args... args) {
    tree->add(*element, path);
    retribution (tree, index+1, path, args...);
  }

  // Vector

  template <class t>
  void static retribution (JsonTree* tree, int index, string path, vector <t>* vect) {

    tree->addVector(path);
    path = path + "." + to_string(index);

    for (int j = 0; j < vect->size(); j++)
      retribution (tree, j, path, &(*vect)[j]);

  }

  template <class t, class... Args>
  void static retribution (JsonTree* tree, int index, string path, vector<t>* vect, Args... args) {

    tree->addVector(path);
    string newPath = path + "." + to_string(index);

    for (int j = 0; j < vect->size(); j++)
      retribution (tree, j, newPath, &(*vect)[j]);

    retribution (tree, index+1, path, args...);
  }


  // Read
  template <typename t, class func>
  void static initialize (JsonTree* tree, func functor, t* element) {
    tree->get(*element, functor());
  }


  template <class t, class func, class... Args>
  void static initialize (JsonTree* tree, func functor, t* element, Args... args) {
    tree->get(*element, functor());
    initialize (tree, functor, args...);
  }

  // Vector
  template <class t, class func>
  void static initialize (JsonTree* tree, func functor, vector <t>* vect) {
    string newPath = functor();
    int size = tree->getSizeAt(newPath);
    vect->resize (size);

    int i = 0;
    for (int j = 0; j < size; j++) {
      initialize (tree, [&] () {
        return newPath + "." + to_string(i);
      }, &(*vect)[j]);
      i++;
    }
  }

  template <class t, class func, class... Args>
  void static initialize (JsonTree* tree, func functor, vector <t>* vect, Args... args) {
    string newPath = functor();

    int size = tree->getSizeAt(newPath);
    vect->resize (size);

    int i = 0;
    for (int j = 0; j < vect->size(); j++) {
      initialize (tree, [&] () {
        return newPath + "." + to_string(i);
      }, &(*vect)[j]);
      i++;
    }
    initialize (tree, functor, args...);
  }

  //Pointers of SERIALIZABLE classes

  template <class t, class func>
  typename std::enable_if<std::is_base_of<Serializable, t>::value, void>::type
  static initialize (JsonTree* tree, func functor, t** element) {
    *element = new t ();
    (*element)->serializeIn (*tree, functor());
  }

  template <class t, class func, class... Args>
  typename std::enable_if<std::is_base_of<Serializable, t>::value, void>::type
  static initialize (JsonTree* tree, func functor, t** element, Args... args) {
    *element = new t ();
    (*element)->serializeIn (*tree, functor());
    initialize (tree, functor, args...);
  }

  //Pointers of NON SERIALIZABLE classes
  template <class t, class func>
  typename std::enable_if<std::is_fundamental<t>::value, void>::type
  static initialize (JsonTree* tree, func functor, t** element) {
    *element = new t ();
    tree->get(**element, functor());
  }

  template <class t, class func, class... Args>
  typename std::enable_if<std::is_fundamental<t>::value, void>::type
  static initialize (JsonTree* tree, func functor, t** element, Args... args) {
    *element = new t ();
    tree->get(**element, functor());
    initialize (tree, functor, args...);
  }


};

}
#endif // SERIALIZABLE_H

