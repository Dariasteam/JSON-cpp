#<cldoc:json::Parser>
Translate between <json::JsonTree> and json files / strings

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

#<cldoc:Tutorials::File Manipulation>

#<cldoc:Tutorials::Serialization>


#<cldoc:Tutorials::Serialization::Quick Reference>
First read the entire tutorial

#### Serializable types

- Both value & pointer
  - int
  - long int
  - long long
  - float
  - double
  - bool
  - string
  - char
  - custom serializable classes


- Only value
  - vector <any_of_the_above>
  - vector <any_of_the_above*>


- Magical features
  - inheritance of custom serializable classes (any depth)
  - polymorphism of custom serializable classes


#<cldoc:Tutorials::Serialization::Getting started>
How the info is stored and basics about process

#### Serialization modes

Taking advantages of the json hash and vector support, serialization is implemented in two different ways,
giving flexibility to the programmer to choose the best in each case.

- _Vector-like_ : The information is put in a vector, so it position dependent

  ```
  [5, false, "We're no strangers to love"]
  ```


- _Hash-like_ : The information has a key for each element so it can be handle by its "name" no matter order

  ```
  {
    "hours" : 5,
    "arrived-yet" : false,
    "rckrll" : "We're no strangers to love"
  }
  ```

Note that in json the root of a file must be always a hash, so if you want to serialize in vector way you must
always handle one path (at least).

```json
{
  "path-to-elements-serialized-as-vectors" : [
    5,
    false,
    "We're no strangers to love"
  ]
}
```
#### Understanding the process

Serialization process is divided in two sections

- a) To translate information between serializable objects and <json::JsonTree>
- b) To translate the information between <json::JsonTree> and a file / string

Depending if you are `Serializing In` (from file / string to c++ objects) or `Serializing Out`
(from c++ object to file / string) the steps order is **b, a** or **a, b**.
Lets explain more precissely each one:

**Serialization** is archived due to the combination of the inheritance from json::serializable and
the explicit declaration of the involved parameters of your custom class in between two special macros.
One of the macros adds to your class the method `serialize` which receives a <json::JsonTree> and depending
of the situation can

  - copy the values of the attributes of the serializable object in it. Also respecting the inner existing hierarchy.
  - initialize the parameters of the object with the data of the JsonTree

So when the method execution has finished, also has done it the Serialization process.

**String management** is made by <json::JsonTree.toText> and <json::JsonTree::parseString> functions.
It is auto included in the file handling process so you don't need to worry about this phase unless you
explicitly need a string. (For example for debugging reasons)

**File handling** is done entirely by a <json::Parser> and can do nothing more (or less) than it.
That means you can do things like putting serializable objects data and other stuff or many not own relationed classes in the same file.

A dedicated section to what can be done at this ambit can be found at <Tutorials::Serialization::File management>

#<cldoc:Tutorials::Serialization::Show me the code!>
Simple serialization, vector-like

This example shows how to serialize a class with one parameter in the vector way using a
file called "file.json" to store / retrieve the information.

#### json file, "file.json"
As we previously said, the root can't be a vector.
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
#include "./serializable.hpp"                 // include this file

class A : public json::Serializable {         // derivate from 'Serializable'
private:
  int parameter;                              // this parameter will be serialized
  int nonSerializableParameter;               // this will not

  SERIAL_START                                // declare the parameters to be serialized in between the macros
    parameter
  SERIAL_END                                  // we close without declaring 'noSerializableParameter', so its not serialized
public:
  void changeContent () { parameter = 14; }
};

int main (void) {
  A obj;
  obj.serializeIn("file.json", "some_path");   // reads "file.json", generate an inner 'JsonTree' and use it to initialize the parameters of 'obj'
  std::cout << obj.toText() << std::endl;      // prints object's content
  obj.changeContent();                         // changes the value of 'parameter' to 14
  obj.serializeOut("file.json", "other_path"); // rewrites "file.json" with the information of the object 'obj', at path 'some_path'
}
```
#### output
```
[
  10
]

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
Check <Tutorials::Serialization::Files management> to know how to archive that.

#<cldoc:Tutorials::Serialization::Using multiple parameters>
Using multiple parameters. Vector-like
How to serialize multiple heterogeneous parameters of one non-abstract class using a file called "file.json" to store / retrieve the information.

As we are using the _vector-like_ aproximation, we must know exactly the order of the parameters both in json file and code.

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
    number,                                   // note the comma after every element except last. Just like json
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
};

int main (void) {
  A obj;
  obj.serializeIn("file.json", "some_path");   // reads "file.json" file
  std::cout << obj.toText() << std::endl;
  obj.changeContent();
  obj.serializeOut("file.json", "other_path"); // rewrites "file.json" with the information of the object 'obj', at path 'some_path'
}
```
#### output
```
[
  10, true,
  0.5661000000000000476,
  "And though your dead and gone, believe me, your memory will carry on"
]
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
#<cldoc:Tutorials::Serialization::Hash aproximation>
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
  std::cout << obj.toText() << std::endl;
  obj.changeContent();
  obj.serializeOut("file.json"); // rewrites "file.json" with the information of the object 'obj', at path 'some_path'
}
```

#### output
Note the order of the elements has changed and now is the same than used in **SERIAL_START** macro. Obviously that is not a problem
if we wanted to run the program again as it would be if we used _vector-like_ mode.
```
{
  "likeThis" : true,
  "accurancy" : 0.5661000000000000476,
  "songLyrics" : "And though your dead and gone, believe me, your memory will carry on",
  "integer" : 10
}
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

#<cldoc:Tutorials::Serialization::File management>
Reusing files for multiple purposes

In the previous example we used <json::Serializable::serializeIn> and <json::Serializable::serializeOut> for file handling.
Those functions are overload (click on them to have more info) and are thinked to use them in two ways

- Fast single object serialization, the one used in previous code:
  ```c++
  A obj; // consider A as a serializable class

  obj.serializeIn ("file_to_open", "path_for_using_as_root")
  obj.serializeOut ("file_to_open", "path_for_using_as_root")
  ```
  This mode opens the file and read / overwrite it with the info of the class 'obj'


- Raw <json::JsonTree> serialization
  ```c++
  A obj; // consider A as a serializable class

  json::Parser parser;
  json::JsonTree tree;
  ...
  // Raw traitment
  ...
  obj.serializeIn (tree, "path_for_using_as_root")
  obj.serializeOut (tree, "path_for_using_as_root")
  ```
  This mode allows you to use one single <json::JsonTree> for several objects serialization.
  (If you don't know how to use <json::Parser> and <json::JsonTree>, check <Tutorials::File Manipulation>)

Here is an example of how to serialize two non related classes in the same file

---

#### json file, "file.json"
```json
{
  "first_path" : [
    10, 20
  ],
  "second_path" : {
    "key" : "A minor",
    "verse" : "I cry out for magic, I feel it dancing in the light"
  }
}
```
#### code
```c++
#include <iostream>
#include <string>
#include "serializable.hpp"

class A : public json::Serializable {
private:
  int numberA;
  int numberB;

  SERIAL_START
    numberA,
    numberB
  SERIAL_END
public:
  void changeContent () {
    numberA = 1;
    numberB = 2;
  }
};


class B : public json::Serializable {
private:
  std::string key;
  std::string verse;

  SERIAL_START
    "key", key,
    "verse", verse
  SERIAL_END
public:
  void changeContent () {
    key = "D minor";
    verse = "Uncovering things that wer sacred, manifest on this earth";
  }
};

int main (void) {
  A objA;
  B objB;

  json::Parser parser;
  json::JsonTree tree;

  parser.parseFile("pruebas/file.json", tree);  // reads file and generates the hierarchy

  objA.serializeIn (tree, "first_path");        // initialize objA using only the info at 'first_path'
  objB.serializeIn (tree, "second_path");       // initialize objB using only the info at 'first_path'

  // At this point both objects are initialized with their respective values

  std::cout << objA.toText() << std::endl;
  std::cout << objB.toText() << std::endl;

  objA.changeContent();
  objB.changeContent();

  // Now the values have changed, we must create the hierarchy again

  objA.serializeOut(tree, "first_path");
  objB.serializeOut(tree, "second_path");

  parser.saveFile("pruebas/file.json", tree);
  // alternatively json::Parser::saveFile("pruebas/file.json", tree);
}
```
#### output
```
[
  1, 2
]
{
  "key" : "D minor",
  "verse" : "Uncovering things that wer sacred, manifest on this earth"
}
```
#### modified json file, "file.json"
```
{
  "first_path" : [
    1, 2
  ],
  "second_path" : {
    "key" : "D minor",
    "verse" : "Uncovering things that were sacred, manifest on this earth"
  }
}
```

#<cldoc:Tutorials::Serialization::Using complex parameters, Vector-like>
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
  std::cout << obj.toText() << std::endl;
  obj.changeContent();
  obj.serializeOut("file.json", "content");
}
```
#### output
```
[
  [
    [
      1, 3, 4
    ],
    [
      6, 4, 1
    ],
    [
      4, 0, 0
    ]
  ]
]
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

#<cldoc:Tutorials::Serialization::Using complex parameters, Hash-like>
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
  std::cout << obj.toText() << std::endl;
  obj.changeContent();
  obj.serializeOut("file.json");
}
```
#### output
```
{
  "two-dimensional-matrix" : [
    [
      1, 3, 4
    ],
    [
      6, 4, 1
    ],
    [
      4, 0, 0
    ]
  ]
}
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

#<cldoc:Tutorials::Serialization::Using serializable classes as parameters>
Also mixing hash-like and vector-like serialization

The following example shows you how to use serializable classes as parameters
mixing vector-like and hash-like modes.

Class 'Hangar' has a vector that stores many instances of 'Rocket'.
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
  std::cout << obj.toText() << std::endl;
  obj.changeContent();
  obj.serializeOut("file.json");
}
```
#### output
```
{
  "personal" : {
    "engineers" : 12,
    "security" : 4,
    "enough-people" : false
  },
  "rockets" : [
    [
      "Saturn V"
    ],
    [
      "Soyuz"
    ],
    [
      "Proton"
    ],
    [
      "R7"
    ],
    [
      "Falcon 9"
    ]
  ],
  "temperature" : 15.60000038146972656
}
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
#<cldoc:Tutorials::Serialization::Inheriting serializable ability>
No multiple inheritance supported yet

There is a new macro for this case (see <Tutorials::Serialization::Serializing pointers of serializable classes>> to know why):

  `SERIAL_START_INHERITED (this_derived_class, base_class)`

To reminding it, think the order is the same than the c++ inheritance declaration
```c++
class derived_class : public base_class {
  SERIAL_START_INHERITED (derived_class, base_class)
  ...
  SERIAL_END
};

```
- Serialization occurs starting from the base class to derived
- You **cannot** mix vector-like and hash-like modes. The used in base class must be used for derived ones.

For this example, we have a base serializable class 'A' which has an integer as parameter and a derived class 'B'
which has a string parameter. As the 'A' parameters **and the serializable macros** are declared protected,
to serialize 'B' implies to serialize its 'A' parameters

#### json file, "file.json"
```
{
  "content" : [
    12, "Run"
  ]
}
```
where 12 is the integer parameter of 'A' and "Run" the string parameter of 'B'

#### code
```
#include <iostream>
#include <vector>
#include <string>
#include "serializable.hpp"

class A : public json::Serializable {
protected:                              // because we want parameters and macro functions to be accessible by B class
  int number;

  SERIAL_START
    number
  SERIAL_END
public:
  virtual void changeContent () { number = 90; }
};

class B : public A {
private:
  std::string word;

  SERIAL_START_INHERITED (B, A)          // new macro for inheritance cases
    word
  SERIAL_END
public:
  void changeContent () {
    number = 90;
    word = "to the hills";
  }
};

int main (void) {
  B obj;
  obj.serializeIn("file.json", "content");
  std::cout << obj.toText() << std::endl;
  obj.changeContent();
  obj.serializeOut("file.json", "content");
}
```
#### output
```
[
  12, "Run"
]
```
#### modified json file, "file.json"
```json
{
  "content" : [
    90, "to the hills"
  ]
}
```

#<cldoc:Tutorials::Serialization::Serializing pointers of basic types>
Non serializable class pointer case

You can use pointers of any type seen in previous examples excepting vectors:

```
  vector <x*> allowed
  vector <x>* not allowed
```

The serialization process checks types of the pointers and tries to do `new` for each one.

Note that pointers **cannot be used for arrays** of elements **yet** due the methods cannot know the lenght of that array,
so only the first element is serialized.

#### json file, "file.json"
```
{
  "content" : [
    10, "oh by the way, which one is pink?",
    false,
    [
      1, 2, 3, 4
    ]
  ]
}
```
#### code
```c++
#include <iostream>
#include <vector>
#include <string>
#include "serializable.hpp"

class B : public json::Serializable {
private:
  int* number;
  std::string* word;
  bool* boolean;
  A* element;
  vector<int*> vec1;

  SERIAL_START
    number,
    word,
    boolean,
    vec1,
  SERIAL_END
};

int main (void) {
  A obj;
  obj.serializeIn("file.json", "content");
  std::cout << obj.toText() << std::endl;
}
```
#### output
```
[
  10, "oh by the way, which one is pink?",
  false,
  [
    1, 2, 3, 4
  ]
]
```

#<cldoc:Tutorials::Serialization::Serializing pointers of serializable classes>
The polymorphic problem

## Explanation

Imagine we have this program:
```
class A : public json::Serializable;
class B : public A;
class C : public A;

class M : public json::Serializable {
  A* a;
  SERIAL_START;
    a                     // this could be an A, a B or a C
  SERIAL_END;
```

How do we know if we are serializing an A, B or C?
At execution time we can't ask for a list of derived classes for correctly trait this situation.

The solution adopted consist in explicitily specificate when inheritance occurs (and that is why the
SERIAL_START_INHERITED macro exist). This allow the serialization methods to manage the serializable inheritance
as seen in <Tutorials::Serialization::Inheriting serializable ability>, however, this isn't enough, and we
must use one last macro (promise, this is last) in order to tell the 'Seralizable' methods how
to create a new 'B' or 'C' once they know if is a B or a C what is required. (Thats because serialization methods
use templates and tries to do `new t` when the parameter is a pointer). The new macro is:

```
INHERITS (this_class)

```
And must be declared as first element in .cpp file of every class or inmediately after the class declaration.
For example, the previous program now would be
```
class A : public json::Serializable;

class B : public A;
INHERITS (A)              // tells serializable static method there is a B class

class C : public A;       // tells serializable static method there is a C class
INHERITS (C)

class M : public json::Serializable {
  A* a;
  SERIAL_START;
    a                     // this could be an A, a B or a C
  SERIAL_END;

```
The purpose of this is to create a 'dictionary' of derived pointer-used classes so when the serialization reach
the situation of having to manage an A*, but knows it really contains a B*, can truly create a B. And how
will know that? Well, json will say. Serializable pointers are stored as:
```
{
  "$classType" : "B",
  "$classContent" : [/{

  ]/}
}
```
So, they are **always** hash serialized, but its content could be vector or hash, so all we learned still works here,
the difference is it will not be aplied at the root of the serialized element but in it's '$classContent' path.

## Example

#### json file, "file.json"
```
{
  "content" : [
    {
      "$classType" : "Rover",
      "$classContent" : [
        "Opportunity",
        "Mars"
      ]
    },
    {
      "$classType" : "Rocket",
      "$classContent" : [
        "Zenit",
        0,
        2
      ]
    },
    {
      "$classType" : "SpaceShip",
      "$classContent" : [
        "Shenzou",
        1,
      ]
    }
  ]
}
```
#### code
```c++
#include <iostream>
#include <vector>
#include <string>
#include "serializable.hpp"

class AwesomeVehicle : public json::Serializable {
protected:
  string name;

  SERIAL_START
    name
  SERIAL_END
};

class Rocket : public AwesomeVehicle {
private:
  int boosters;
  int stages;

  SERIAL_START_INHERITED (Rocket, AwesomeVehicle)
    boosters,
    stages
  SERIAL_END
};

INHERITS (Rocket)                                     // Do not forget

class SpaceShip : public AwesomeVehicle {
private:
  int crew;

  SERIAL_START_INHERITED (SpaceShip, AwesomeVehicle)
    crew
  SERIAL_END
};

INHERITS (SpaceShip)                                 // Do not forget

class Rover : public AwesomeVehicle {
private:
  std::string celestial_body;

  SERIAL_START_INHERITED (Rover, AwesomeVehicle)
    celestial_body
  SERIAL_END
};

INHERITS (Rover)                                    // Do not forget

class ProudOfScience : public json::Serializable{
private:
  std::vector <AwesomeVehicle*> spaceStuff;

  SERIAL_START
    "content", spaceStuff
  SERIAL_END
};

int main (void) {
  ProudOfScience obj;
  obj.serializeIn("file.json");
  std::cout << obj.toText() << std::endl;
  //obj.serializeOut("file.json", "content");
}
```
#### ouput
```
{
  "content" : [
    {
      "$classType" : "Rover",
      "$classContent" : [
        "Opportunity",
        "Mars"
      ]
    },
    {
      "$classType" : "Rocket",
      "$classContent" : [
        "Zenit",
        0, 2
      ]
    },
    {
      "$classType" : "SpaceShip",
      "$classContent" : [
        "Shenzou",
        1
      ]
    }
  ]
}
```




