#ifndef AUXSERIALIZATION_H
#define AUXSERIALIZATION_H

#include <string>
#include <vector>
#include <map>
#include <cxxabi.h>

#include "object.hpp"
#include "blop.h"
#include "manager.hpp"

namespace json {

#define AUX_SERIAL_START_INHERITED(y, x) AUX_INHERITS_FROM (y, x)                          \
                         bool serialize (json::AbstractObject* obj, json::JsonTree& tree, int& index) { \
                           if (!y::isTopClass())                                           \
                             if (x::serialize (obj, tree, index));                         \
                               return y::serializeInherits (obj, tree, index);             \
                           return false;                                                   \
                         }                                                                 \
                         inline virtual bool isTopClass () { return false; }               \
                         bool serializeInherits (json::AbstractObject* obj, json::JsonTree& tree, int& index) { return tree.get (obj, index,


#define AUX_SERIAL_START bool serialize (json::AbstractObject* obj, json::JsonTree& tree, int& index) { return tree.get (obj, index,


#define AUX_SERIAL_END ,__e__); }

#define AUX_INHERITS_FROM(y, x) static bool __init__ () {                                      \
                                  json::AuxSerialization::addSon( #y , [] { return new y; });  \
                                }                                                              \
                                                                                               \
                                static bool trigger;                                           \

#define AUX_INHERITS(x) bool x::trigger = x::__init__ ();

//struct ender {};

class AuxSerialization : public BLOP {
public:

protected:
  ender __e__;
  static void addSon (std::string name, std::function<AuxSerialization*()> lambda) { dictionary[name] = lambda; }

  // checks if is the top class
  inline virtual bool isTopClass () { return true; }

  virtual bool serialize (json::AbstractObject* obj, json::JsonTree& tree, int& index) = 0;

  //- As seen in http://stackoverflow.com/questions/12877521/human-readable-type-info-name
  // Used to get the class name
  static std::string demangle(const char* mangled) {
    int status;
    std::unique_ptr<char[], void (*)(void*)> result(
      abi::__cxa_demangle(mangled, 0, 0, &status), free);
    return result.get() ? std::string(result.get()) : "error occurred";
  }
public:  
  AuxSerialization();
};

}

#endif // AUXSERIALIZATION_H
