#ifndef AUXSERIALIZATION_H
#define AUXSERIALIZATION_H

#include <string>
#include <vector>

#include "object.hpp"

namespace json {

#define AUX_SERIAL_START public: template <class T> \
                           bool __trigger__ (json::AbstractObject* obj, T& tree) { return tree.get (obj,
#define AUX_SERIAL_END ,__e__); }

struct ender {};

class AuxSerialization {
protected:
  ender __e__;

public:  
  AuxSerialization();
};

}

#endif // AUXSERIALIZATION_H
