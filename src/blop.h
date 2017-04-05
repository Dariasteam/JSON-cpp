#ifndef BLOP_H
#define BLOP_H

#include <map>
#include <vector>
#include <string>
#include <functional>

#define CLASS_TYPE    "$classType"
#define CLASS_CONTENT "$classContent"

namespace json {

  struct ender {};

class BLOP
{
public:
  static std::map<std::string, std::function<BLOP*()> > dictionary;
  BLOP();
};

}

#endif // BLOP_H
