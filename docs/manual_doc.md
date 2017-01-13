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


