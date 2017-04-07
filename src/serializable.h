#ifndef AUXSERIALIZATION_H
#define AUXSERIALIZATION_H

#include <string>
#include <vector>
#include <map>

#include "object.hpp"
#include "abstract_serializable.h"
#include "manager.hpp"
#include "parser.hpp"

namespace json {

#define AUX_SERIAL_START_INHERITED(y, x)                                                \
                         static bool __init__ () {                                      \
                           json::Serializable::addSon( #y , [] { return new y; });  \
                         }                                                              \
                         static bool trigger;                                           \
                         json::AbstractObject* serialize (json::AbstractObject* obj, json::JsonTree& tree, int& index, bool op) { \
                           if (!y::isTopClass())                                            \
                             if (x::serialize (obj, tree, index, op) != nullptr);          \
                               return y::serializeInherits (obj, tree, index, op);         \
                           return nullptr;                                                  \
                         }                                                                  \
                         inline virtual bool isTopClass () { return false; }                \
                         json::AbstractObject* serializeInherits (json::AbstractObject* obj, json::JsonTree& tree, int& index, bool op) { return tree.start (obj, index, op,


#define AUX_SERIAL_START json::AbstractObject* serialize (json::AbstractObject* obj, json::JsonTree& tree, int& index, bool op) { return tree.start (obj, index, op,


#define AUX_SERIAL_END ,__e__); }                                                                                     \
  json::AbstractObject* polymorphicSerialize (json::JsonTree& tree, bool op) {                                        \
    json::ObjectMap* aux = new json::ObjectMap();                                                                     \
    aux->insert(CLASS_TYPE, new json::ObjectFinalString(demangle(typeid(*this).name())));                             \
    int index = 0;                                                                                                    \
    json::AbstractObject* content = serialize(nullptr, tree, index, op);                                              \
    if (content != nullptr && aux->insert(CLASS_CONTENT, content))                                                    \
       return aux;                                                                                                    \
    else                                                                                                              \
      return nullptr;                                                                                                 \
  }

#define AUX_INHERITS(x) bool x::trigger = x::__init__ ();

class Serializable : public __abstract_serializable__ {
public:
  virtual json::AbstractObject* serialize (json::AbstractObject* obj, json::JsonTree& tree, int& index, bool op) = 0;
  virtual json::AbstractObject* polymorphicSerialize (json::JsonTree& tree, bool op) = 0;

  bool serializeIn (std::string file, std::string path);
  bool serializeIn (JsonTree& tree, std::string path);

  bool serializeOut (std::string file, std::string path);
  bool serializeOut (JsonTree& tree, std::string path);
protected:
  __serialization_ender__ __e__;
  static void addSon (std::string name, std::function<Serializable*()> lambda) { dictionary[name] = lambda; }
  inline virtual bool isTopClass () { return true; } 

public:  
  Serializable();
};

}

#endif // AUXSERIALIZATION_H
