# JSON-cpp [![Build Status](https://travis-ci.org/Dariasteam/JSON-cpp.svg?branch=master)](https://travis-ci.org/Dariasteam/JSON-cpp)

### [Documentation](http://dariasteam.github.io/JSON-cpp/)

Simple _serialization_ and json file manipulation for C++11 on the fly!

This library allows you to do some fancy stuff like these:
## Serialization
```c++
#include "..../serializable.hpp"

class A : public json::Serializable {
  int a;
  string b;
  bool c;
  std::vector <int> d;

  SERIAL_START    // magic macro for reflection
    "a", a,       // key / value
    "b", b,
    "c", c,
    "d", d
  SERIAL_END
};

int main (void) {
  A obj;
  // some values initializations
  A.serializeOut ("file.json"); // obviously there is the serializeIn equivalent
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
  tree.get(a, "b");             // data won't change due to invalid types
  std::cout << a << std::endl;
}     
```
#### ouput
```
12
ERROR : Attempting to load the element 'b' as a NUMBER_INT when is of type STRING.
12
```
## Ok, but what can I serialize?
- Both value & pointer
  - `int`, `long int`, `long long`
  - `float`, `double`  
  - `bool`
  - `string`, `char`  
  - custom serializable classes

- Only value
  - `std::vector <any_of_the_above>`
  - `std::vector <any_of_the_above*>`  

- Magical features
  - Inheritance of custom serializable classes (any depth)
  - Polymorphism of custom serializable classes
  - n-dimensional matrixes: `std::vector < std::vector <std::vector <.....> > >`

Note: pointers arrays currently not work, use std::vector <t> instead
## Usage
Just copy the [/src](https://github.com/Dariasteam/JSON-cpp/tree/master/src) content into your project and include the convenient headers
```c++
#include "..../parser.h"         // for reading json files
#include "..../manager.h"        // for managing json data
#include "..../serializable.h"   // for creating custom serializable classes
```

## Third party libraries
  - Documentation generator : [cldoc](https://jessevdk.github.io/cldoc/)              
  - Diagrams generator : [mermaid](https://github.com/knsv/mermaid/blob/master/README.md)  
  - Unit testing : [Catch](https://github.com/philsquared/Catch)


Special thanks to [Eleazar Díaz](https://github.com/EleDiaz)
