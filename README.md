# JSON-cpp [![Build Status](https://travis-ci.org/Dariasteam/JSON-cpp.svg?branch=master)](https://travis-ci.org/Dariasteam/JSON-cpp)

#### [Documentation](http://dariasteam.github.io/JSON-cpp/)

Simple _serialization_ and json file manipulation in C++11 on the fly!

This library allows you to do some fancy stuff like these:
## Serialization
```c++
#include "..../serializable.hpp"

class A : public json::Serializable {
  int a;
  string b;
  bool c;
  std::vector <int> d;

  SERIAL_START    // magic macro
    "a", a,       // key / value
    "b", b,
    "c", c,
    "d", d
  SERIAL_END
};

int main (void) {
  A obj;
  // some values initializations
  A.serializeOut ("file.json");
}
```
#### file.json
```json
{
  "a" : 12,
  "b" : "this is awesome!",
  "c" : true,
  "d" : [
    56, 78, 90, 10
  ]
}
```
## Raw json manipulation
```c++
#include "..../manager.hpp"
#include "..../parser.hpp"
int main (void) {
  json::Parser parser;
  json::JsonTree tree;

  parser.parseFile("file.json", tree);

  int a;
  tree.get(a, "a");
  std::cout << a << std::endl;
  tree.get(a, "b");
}     
```
#### ouput
```
12
ERROR : Attempting to load the element 'b' as a NUMBER_INT when is of type STRING.
```
## Usage
Just copy the /src content into your project and include the convenient headers
```c++
#include "..../parser.h"         // for reading json files
#include "..../manager.h"        // for managing json trees one parsed / created
#include "..../serializable.h"   // for creating custom serializable classes
```

## Third party libraries
  - Documentation generation : [cldoc](https://jessevdk.github.io/cldoc/)              
  - Diagram generation : [mermaid](https://github.com/knsv/mermaid/blob/master/README.md)  
  - Unit testing : [Catch](https://github.com/philsquared/Catch)


Special thanks to [Eleazar Díaz](https://github.com/EleDiaz)
