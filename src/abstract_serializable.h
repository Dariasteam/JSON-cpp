#ifndef BLOP_H
#define BLOP_H

#include <map>
#include <vector>
#include <string>
#include <cxxabi.h>
#include <functional>
#include <memory>

#define CLASS_TYPE    "$classType"
#define CLASS_CONTENT "$classContent"

namespace json {

  struct __serialization_ender__ {};

class __abstract_serializable__ {
public:
  static std::map<std::string, std::function<__abstract_serializable__*()> > dictionary;
  __abstract_serializable__();
  virtual ~__abstract_serializable__() {}

  //- As seen in http://stackoverflow.com/questions/12877521/human-readable-type-info-name
  // Used to get the class name
  static std::string demangle(const char* mangled) {
    int status;
    std::unique_ptr<char[], void (*)(void*)> result(
      abi::__cxa_demangle(mangled, 0, 0, &status), free);
    return result.get() ? std::string(result.get()) : "error occurred";
  }
};

}

#endif // BLOP_H
