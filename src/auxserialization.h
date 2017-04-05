#ifndef AUXSERIALIZATION_H
#define AUXSERIALIZATION_H

#include <string>
#include <vector>
#include <map>

#include "object.hpp"

namespace json {

#define AUX_SERIAL_START_INHERITED(y, x) AUX_INHERITS_FROM (y, x)                          \
                         template <class T>                                                \
                         bool serialize (json::AbstractObject* obj, T& tree, int& index) { \
                           if (!y::isTopClass())                                           \
                             if (x::serialize (obj, tree, index));                         \
                               return y::serializeInherits (obj, tree, index);             \
                           return false;                                                   \
                         }                                                                 \
                         inline virtual bool isTopClass () { return false; }               \
                         template <class T>                                                \
                         bool serializeInherits (json::AbstractObject* obj, T& tree, int& index) { return tree.get (obj, index,


#define AUX_SERIAL_START public: template <class T> \
                           bool serialize (json::AbstractObject* obj, T& tree, int& index) { return tree.get (obj, index,


#define AUX_SERIAL_END ,__e__); }

#define AUX_INHERITS_FROM(y, x) static bool __init__ () {                                      \
                                  json::AuxSerialization::addSon( #y , [] { return new y; });  \
                                }                                                              \
                                                                                               \
                                static bool trigger;                                           \

#define CLASS_TYPE    "$classType"
#define CLASS_CONTENT "$classContent"

struct ender {};

class AuxSerialization {
protected:
  ender __e__;
  static std::map<std::string, std::function<AuxSerialization*()> > dictionary;
  static void addSon (std::string name, std::function<AuxSerialization*()> lambda) { dictionary[name] = lambda; }

  // checks if is the top class
  inline virtual bool isTopClass () { return true; }
public:  
  AuxSerialization();
};

}

#endif // AUXSERIALIZATION_H
