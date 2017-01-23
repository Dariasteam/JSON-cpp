#<cldoc:json::Parser>
Manage I/O of .json files

## Description
Provides functionalities for generating a <JsonTree> with the information retrieved from  a `.json` file  and vice versa.
If errors are detected in the file manipulation process, the methods involved returns a flag from <JSON_PARSE_OUTPUT>

If errors or extraneous situations are detected in the json syntax, they are reflected as a <JsonLog> objects.
Those situations are clasified in two groups:

  - Warnings: empty container (hash or array)
  - Errors:   everything else

As described in <JSON_PARSER_FLAG>

## Usage example:
```c++
include "parser.hpp"

int main (void) {

  json::Parser parser;
  json::JsonTree tree;

  std::cout << "Parsing" << std::endl;

  // Parsing
  std::cout << parser.parseFile("no_file.json", tree)                       << std::endl; // returns <JSON_PARSE_OUTPUT::CANT_OPEN_FILE>
  std::cout << parser.parseFile("file_empty.json", tree)                    << std::endl; // returns <JSON_PARSE_OUTPUT::EMPTY_FILE>
  std::cout << parser.parseFile("file_with_errors.json", tree)              << std::endl; // returns <JSON_PARSE_OUTPUT::ERRORS>
  std::cout << parser.parseFile("file_with_warnings.json", tree)            << std::endl; // returns <JSON_PARSE_OUTPUT::WARNINGS> + 1
  std::cout << parser.parseFile("file_with_warnings_and_errors.json", tree) << std::endl; // returns <JSON_PARSE_OUTPUT::WARNINGS> + <JSON_PARSE_OUTPUT::ERRORS>
  std::cout << parser.parseFile("file_ok.json", tree)                       << std::endl; // returns <JSON_PARSE_OUTPUT::OK>

  std::cout << parser.parseFile("some_file.json", tree, true)               << std::endl; // shows information about parsing while doing it

  // Error handling
  parser.getErrors();                                                                     // returns vector <JsonLog> of errors
  parser.getWarnings();                                                                   // returns vector <JsonLog> of warnings

  std::cout << "Saving" << std::endl;

  // Saving
  std::cout << parser.saveFile("/path/with/no/permissions/new_file", tree) << std::endl;  // returns <JSON_PARSE_OUTPUT::CANT_OPEN_FILE>
  std::cout << parser.saveFile("new_file.json", tree) << std::endl;                       // creates human readable file, returns <JSON_PARSE_OUTPUT::OK>
  std::cout << parser.saveFile("new_file_ugly.json", tree, true) << std::endl;            // creates non human readable file, returns <JSON_PARSE_OUTPUT::OK>

}
```
### Output
```
Parsing
2  // 0x00010
16 // 0x10000
8  // 0x01000
4  // 0x00100
12 // 0x01100
1  // 0x00001
Saving
2  // 0x00010
1  // 0x00001
```
### Generated files
#### new_file
```json
{
  "element" : 12,
  "boolean" : true
}
```
#### new_file_ugly
```json
{"element":12,"boolean":true}
```

#<cldoc:Examples>

#<cldoc:Examples::File Manipulation>

#<cldoc:Examples::Serialization>

#<cldoc:Examples::Serialization::Getting started>
The easiest case. Vector-like aproximation
How to serialize one parameter of one non-abstract class using a file called "file.json" to store / retrieve the information.

By default the parameters are serialized in a subpathed vector due to json format restrictions that
does not allow to use vectors as root of the hierarchy.

#### json file, "file.json"
```json
{
  "some_path" : [
    10
  ]
}
```
#### code
```c++
#include <iostream>
#include "./serializable.hpp"

class A : public json::Serializable {         // derivate from 'Serializable'
private:
  int parameter;
  int nonSerializableParameter;

  SERIAL_START
    parameter                                 // we must declare the variables to be serialized
  SERIAL_END                                  // we close without declaring 'noSerializableParameter', so its not serialized
public:
  void printContent () { std::cout << parameter << std::endl; }
  void changeContent () { parameter = 14; }
};

int main (void) {
  A obj;
  obj.serializeIn("file.json", "some_path");   // reads "file.json" file and updates the value of 'parameter'
  obj.printContent();                          // prints 10
  obj.changeContent();                         // changes the value of 'parameter' to 14
  obj.serializeOut("file.json", "other_path"); // rewrites "file.json" with the information of the object 'obj', at path 'some_path'
}
```
#### ouput
```
10
```
#### modified json file, "file.json"
```json
{
  "other_path" : [
    14
  ]
}
```
Note that information **is not** appended to the file, the path "some_path" is lost and the new file isn't like this:
```json
{
  "some_path" : [
    10
  ]
  "other_path" : [
    14
  ]
}
```
#<cldoc:Examples::Serialization::Using multiple parameters>
Using multiple parameters. Vector-like aproximation
How to serialize multiple heterogeneous parameters of one non-abstract class using a file called "file.json" to store / retrieve the information.

As we are using the vector-like aproximation, we must know exactly the order of the parameters both in json file and code.

#### json file, "file.json"
```json
{
  "some_path" : [
    10,
    true,
    0.5661,
    "And though your dead and gone, believe me, your memory will carry on"
  ]
}
```
#### code
```c++
#include <iostream>
#include <string>
#include "serializable.hpp"

class A : public json::Serializable {         // derivate from 'Serializable'
private:
  int         number;
  bool        boolean;
  double      precision;
  std::string blckprde;

  SERIAL_START                                // the parameters must be in the same order than in file
    number,                                   // note the comma after every element except last. Just like in json
    boolean,
    precision,
    blckprde
  SERIAL_END
public:
  void printContent () {
    std::cout << number    << std::endl
              << boolean   << std::endl
              << precision  << std::endl
              << blckprde  << std::endl;
  }
  void changeContent () {
    number   = 78;
    boolean  = false;
    precision = 10.1;
    blckprde = "We'll carry on";
  }
};

int main (void) {
  A obj;
  obj.serializeIn("file.json", "some_path");   // reads "file.json" file
  obj.printContent();
  obj.changeContent();
  obj.serializeOut("file.json", "other_path"); // rewrites "file.json" with the information of the object 'obj', at path 'some_path'
}
```
#### ouput
```
10
true
0.5661
And though your dead and gone, believe me, your memory will carry on
```

#### modified json file, "file.json"
```json
{
  "other_path" : [
    78,
    false,
    10.09999999999999964,
    "We'll carry on"
  ]
}
```
#<cldoc:Examples::Serialization::Hash aproximation>
Unordered name dependent serialization

Same as previous example but using hash-like serialization
Note that in this case we can put the elements directly on the root

#### json file, "file.json"
```json
{
  "integer"    : 10,
  "likeThis"   : true,
  "accurancy"  : 0.5661,
  "songLyrics" : "And though your dead and gone, believe me, your memory will carry on"
}
```
#### code
```c++
#include <iostream>
#include <string>
#include "serializable.hpp"

class A : public json::Serializable {               // derivate from 'Serializable'
private:
  int         number;
  bool        boolean;
  double      precision;
  std::string blckprde;

  SERIAL_START                                      // asociate each parameter with its name (which must be the same as in 'file.json')
    "likeThis",   boolean,                          // note the comma in between the name and its parameter.
    "accurancy",  precision,                        // parameters are order independent
    "songLyrics", blckprde,                         // the macro is the same
    "integer",    number                            // last element has no comma
  SERIAL_END
public:
  void printContent () {
    std::cout << number    << std::endl
              << boolean   << std::endl
              << precision << std::endl
              << blckprde  << std::endl;
  }
  void changeContent () {
    number    = 78;
    boolean   = false;
    precision = 10.1;
    blckprde  = "We'll carry on";
  }
};

int main (void) {
  A obj;
  obj.serializeIn("file.json");  // as the elements are at root, we don't need to specify the path
  obj.printContent();
  obj.changeContent();
  obj.serializeOut("file.json"); // rewrites "file.json" with the information of the object 'obj', at path 'some_path'
}
```

#### ouput
```
10
true
0.5661
And though your dead and gone, believe me, your memory will carry on
```

#### modified json file, "file.json"
```json
{
  "likeThis" : false,
  "accurancy" : 10.09999999999999964,
  "songLyrics" : "We'll carry on",
  "integer" : 78
}
```
Note the order of the elements has changed and now is the same than used in **SERIALI_START** macro. Obviously that is not a problem
if we wanted to run the program again.

#<cldoc:Examples::Serialization::Using complex parameters, Vector-like>
Serializaing n dimensional vectors

You can use n dimensional vectors of vectors, the library can handle it for you!

#### json file, "file.json"
```json
{
  "content" : [
    [
      [1, 3, 4],
      [6, 4, 1],
      [4, 0, 0]
    ]
  ]
}
```

Note this is a 2 dimensional vector **inside a vector of serializable parameters** (content)

#### code
```c++
#include <iostream>
#include <vector>
#include "serializable.hpp"

class A : public json::Serializable {               // derivate from 'Serializable'
private:
  std::vector <std::vector <int> > matrix;          // you can use the depth you want

  SERIAL_START
    matrix
  SERIAL_END
public:
  void printContent () {
    for (std::vector <int> rows : matrix) {
      for (int element : rows) {
        std::cout << element << " ";
      }
      std::cout << std::endl;
    }
  }
  void changeContent () {
    for (int i = 0; i < matrix.size(); i++) {
      for (int j = 0; j < matrix[i].size(); j++) {
        matrix[i][j] = i * matrix.size() + j;
      }
    }
  }
};

int main (void) {
  A obj;
  obj.serializeIn("file.json", "content");
  obj.printContent();
  obj.changeContent();
  obj.serializeOut("file.json", "content");
}
```
#### ouput
```
1 3 4
6 4 1
4 0 0
```
#### modified json file, "file.json"
```json
{
  "content" : [
    [
      [
        0, 1, 2
      ],
      [
        3, 4, 5
      ],
      [
        6, 7, 8
      ]
    ]
  ]
}
```

#<cldoc:Examples::Serialization::Using complex parameters, Hash-like>
Serializaing n dimensional vectors

You can use n dimensional vectors of vectors, the library can handle it for you!

#### json file, "file.json"
```json
{
  "two-dimensional-matrix" : [
    [1, 3, 4],
    [6, 4, 1],
    [4, 0, 0]
  ]
}

```
#### code
```c++
#include <iostream>
#include <vector>
#include "serializable.hpp"

class A : public json::Serializable {               // derivate from 'Serializable'
private:
  std::vector <std::vector <int> > matrix;          // you can use the depth you want

  SERIAL_START
    "two-dimensional-matrix", matrix
  SERIAL_END
public:
  void printContent () {
    for (std::vector <int> rows : matrix) {
      for (int element : rows) {
        std::cout << element << " ";
      }
      std::cout << std::endl;
    }
  }
  void changeContent () {
    for (int i = 0; i < matrix.size(); i++) {
      for (int j = 0; j < matrix[i].size(); j++) {
        matrix[i][j] = i * matrix.size() + j;
      }
    }
  }
};

int main (void) {
  A obj;
  obj.serializeIn("file.json");
  obj.printContent();
  obj.changeContent();
  obj.serializeOut("file.json");
}
```
#### ouput
```
1 3 4
6 4 1
4 0 0
```
#### modified json file, "file.json"
```json
{
  "two-dimensional-matrix" : [
    [
      0, 1, 2
    ],
    [
      3, 4, 5
    ],
    [
      6, 7, 8
    ]
  ]
}
```

#<cldoc:Examples::Serialization::Using serializable classes as parameters>
Also mixing hash-like and vector-like serialization

The following example shows you how to use other serializable classes as parameters
mixing vector-like and hash-like modes.

class 'Hangar' has a vector that stores many instances of class 'Rocket'.
The first class uses the hash method while second uses the vector one.

Hangar also has an instance of a 'Personal' which is hash-like serialized

#### json file, "file.json"
```
{
  "temperature" : 15.6,
  "rockets" : [
    [ "Saturn V" ],
    [ "Soyuz"    ],
    [ "Proton"   ],
    [ "R7"       ],
    [ "Falcon 9" ]
  ],
  "personal" : {
    "engineers"     : 12,
    "security"      : 4,
    "enough-people" : false
  }
}
```
The 'rockets' vector has many vectors in it because the 'Rocket' class is serialized vector-like, the code show this clearly
#### code
```
#include <iostream>
#include <vector>
#include <string>
#include "serializable.hpp"

class Rocket : public json::Serializable {               // vector serialization
private:
  string name;

  SERIAL_START
    name
  SERIAL_END
public:
  void printContent () { std::cout << name << std::endl; }
  void changeContent () { name = "myNewName"; }
};


class Personal : public json::Serializable {              // hash serialization
private:
  int engi;
  int sec;
  bool enough;

  SERIAL_START
    "engineers", engi,
    "security", sec,
    "enough-people", enough
  SERIAL_END
public:
  void printContent () {
    std::cout << engi   << std::endl
              << sec    << std::endl
              << enough << std::endl;
  }
  void changeContent () {
    engi   = 100;
    sec    = 200;
    enough = true;
  }
};

class Hangar : public json::Serializable {                // hash serialization
private:
  vector <Rocket> elements;
  Personal people;
  float temp;

  SERIAL_START
    "personal", people,
    "rockets", elements,
    "temperature", temp
  SERIAL_END

public:
  void printContent () {
    std::cout << "Temperature: " << temp
              << ", content:"    << std::endl;
    people.printContent();
    for (Rocket r : elements) {
      r.printContent();
    }
  }
  void changeContent () {
    temp = 0;
    for (Rocket& r : elements) {
      r.changeContent();
    }
    people.changeContent();
  }
};

int main (void) {
  Hangar obj;
  obj.serializeIn("file.json");
  obj.printContent();
  obj.changeContent();
  obj.serializeOut("file.json");
}
```
#### ouput
```
Temperature: 15.6, content:
12
4
0
Saturn V
Soyuz
Proton
R7
Falcon 9

```
#### modified json file, "file.json"
```json
{
  "personal" : {
    "engineers" : 100,
    "security" : 200,
    "enough-people" : true
  },
  "rockets" : [
    [
      "myNewName"
    ],
    [
      "myNewName"
    ],
    [
      "myNewName"
    ],
    [
      "myNewName"
    ],
    [
      "myNewName"
    ]
  ],
  "temperature" : 0
}
```
#<cldoc:Examples::Serialization::Inheriting serializable ability>
No multiple inheritance supported yet

There is a new macro for this case (see <Examples::Serialization::Using pointers as parameters> to know why):

  `SERIAL_START_INHERITED (this_derived_class, base_class)`

To reminding it, think the order is the same than the c++ inheritance declaration
```c++
class derived_class : public base_class {
  SERIAL_START_INHERITED (derived_class, base_class)
  ...
  SERIAL_END
};

```
Serialization occurs starting from the base class to derived

You **cannot** mix vector-like and hash-like modes. The used in base class must be used for derived ones.

#### json file, "file.json"
```
{
  "content" : [
    12, "Run"
  ]
}
```
#### code
```
#include <iostream>
#include <vector>
#include <string>
#include "serializable.hpp"

class A : public json::Serializable {
protected:                              // because we want this to be accessible by B class
  int number;

  SERIAL_START
    number
  SERIAL_END
public:
  virtual void printContent () { std::cout << number << std::endl; }
  virtual void changeContent () { number = 90; }
};

class B : public A {
private:
  std::string word;

  SERIAL_START_INHERITED (B, A)
    word
  SERIAL_END
public:
  void printContent () {
    std::cout << number << " " << word << std::endl;
  }
  void changeContent () {
    number = 90;
    word = "to the hills";
  }
};

int main (void) {
  B obj;
  obj.serializeIn("file.json", "content");
  obj.printContent();
  obj.changeContent();
  obj.serializeOut("file.json", "content");
}
```
#### output
```
10 run
```
#### modified json file, "file.json"
```json
{
  "content" : [
    90, "to the hills"
  ]
}
```

#<cldoc:Examples::Serialization::Using pointers as parameters>
The pointers will be initialized for you

You can use pointers to any type used in the previous examples including other serializable classes.

Note that pointers **can not be used for arrays** of elements **yet** due the methods can not know the lenght of that array,
so only the first element is serialized.


#### json file, "file.json"
```
{
  "vector-pointer" : [
      1, 2, 3, 4, 5, 6, 7, 8
  ],
  "vector-string" : "If it doen't show, give it time",
  "normal-string" : "to read between the lines"
}
```





