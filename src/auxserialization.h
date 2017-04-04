#ifndef AUXSERIALIZATION_H
#define AUXSERIALIZATION_H

#include <string>
#include <vector>

namespace json {

#define AUX_SERIAL_START public: template <class T> \
                           bool stuff (std::string path, T& tree) { return tree.get (path,
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
