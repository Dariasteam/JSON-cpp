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

#define AUX_SERIAL_START_INHERITED(y, x) AUX_INHERITS_FROM (y, x)                           \
                         json::AbstractObject* serialize (json::AbstractObject* obj, json::JsonTree& tree, int& index, bool op) { \
                           if (!y::isTopClass())                                            \
                             if (x::serialize (path, tree, index, op) != nullptr);          \
                               return y::serializeInherits (path, tree, index, op);         \
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

#define AUX_INHERITS_FROM(y, x) static bool __init__ () {                                      \
                                  json::AuxSerialization::addSon( #y , [] { return new y; });  \
                                }                                                              \
                                __abstract_serializable__                                      \
                                static bool trigger;                                           \

#define AUX_INHERITS(x) bool x::trigger = x::__init__ ();

class AuxSerialization : public __abstract_serializable__ {
public:
  virtual json::AbstractObject* serialize (json::AbstractObject* obj, json::JsonTree& tree, int& index, bool op) = 0;
  virtual json::AbstractObject* polymorphicSerialize (json::JsonTree& tree, bool op) = 0;

  bool serializeIn (std::string file, std::string path);
  bool serializeIn (JsonTree& tree, std::string path);

  bool serializeOut (std::string file, std::string path);
  bool serializeOut (JsonTree& tree, std::string path);
protected:
  __serialization_ender__ __e__;
  static void addSon (std::string name, std::function<AuxSerialization*()> lambda) { dictionary[name] = lambda; } 
  inline virtual bool isTopClass () { return true; } 

public:  
  AuxSerialization();
};

}

#endif // AUXSERIALIZATION_H
