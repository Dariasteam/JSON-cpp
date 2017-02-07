#ifndef PARSER_H
#define PARSER_H

#include <regex>
#include <fstream>
#include <string>
#include <iostream>
#include <climits>
#include <exception>

#include "./object.hpp"
#include "manager.hpp"

namespace json {

// Binary flags used as output for <Parser::parseFile> and <Parser::saveFile> functions.
enum JSON_PARSE_OUTPUT {
  // `0x00001` the file has been opened and parsed successfuly
  OK          	  = 1 << 0,
  // `0x00010` the file can not be opened
  CANT_OPEN_FILE  = 1 << 1,
  // `0x00100` the parse operation has detected non critical extraneous situations in the format of the input file. Parsing can continue
  WARNINGS    	  = 1 << 2,
  // `0x01000` the parse operation has detected critical problems in the format of the input file. parsing cannot continue
  ERRORS     	  = 1 << 3,
  // `0x10000` the file is empty and can not be parsed
  EMPTY_FILE      = 1 << 4
};

//- do not move these elements, their numeric value is used for comparisons
// Flags used to tell the status of each element of a json tree
enum JSON_PARSER_FLAG {
  // the element has no comma at the end, should be the last element of the collection
  LAST_ELEMENT,
  // the element has comma at the end, should not be the last element of the collection
  REGULAR_ELEMENT,
  // [ERROR] no end brace / bracket after the last element
  NO_CLOSED,
  // [ERROR] end brace / bracket after comma (non last element)
  EXPECTED_MORE,
  // [ERROR] the key has extraneous characters
  INVALID_KEY,
  // [ERROR] a key was expected but not reached
  EXPECTED_KEY,
  // used to check type. Greater values are warnings, lower errors
  CONTROL_WARNING,
  // [WARNING] the collection is empty
  EMPTY
};

/* Contains information about errors
 *
 * Instances of this are created by <Parser::parseFile>
 * when a syntax error or interesting situation is detected
 * in a json file.
 *
 * They can be retrieved after with
 * <Parser::getWarnings> and <Parser::getErrors>
 */
struct JsonLog {
  //route of the problematic element
  std::string path;
  // contains a <JSON_PARSER_FLAG> indicating the problem
  int flag;
  friend bool operator== (const JsonLog& j1, const JsonLog& j2) {
    return (j1.path == j2.path) && (j1.flag == j2.flag);
  }
};

/* Translates between <json::JsonTree> and json files / strings
 *
 * ## Description
 * Provides functionalities for generating a <JsonTree> with the information retrieved from  a `.json` file  and vice versa.
 * If errors are detected in the file manipulation process, the methods involved returns a flag from <JSON_PARSE_OUTPUT>
 *
 * If errors or extraneous situations are detected in the json syntax, they are reflected as a <JsonLog> objects.
 * Those situations are clasified in two groups:
 *
 *  - Warnings: empty container (hash or array)
 *  - Errors:   everything else
 *
 * As described in <JSON_PARSER_FLAG>
 *
 * ## Usage example:
 * ```c++
 * include "parser.hpp"
 *
 * int main (void) {
 *
 *  json::Parser parser;
 *  json::JsonTree tree;
 *
 *  std::cout << "Parsing" << std::endl;
 *
 *  // Parsing
 *  std::cout << parser.parseFile("no_file.json", tree)                       << std::endl; // returns <JSON_PARSE_OUTPUT::CANT_OPEN_FILE>
 *  std::cout << parser.parseFile("file_empty.json", tree)                    << std::endl; // returns <JSON_PARSE_OUTPUT::EMPTY_FILE>
 *  std::cout << parser.parseFile("file_with_errors.json", tree)              << std::endl; // returns <JSON_PARSE_OUTPUT::ERRORS>
 *  std::cout << parser.parseFile("file_with_warnings.json", tree)            << std::endl; // returns <JSON_PARSE_OUTPUT::WARNINGS> + 1
 *  std::cout << parser.parseFile("file_with_warnings_and_errors.json", tree) << std::endl; // returns <JSON_PARSE_OUTPUT::WARNINGS> + <JSON_PARSE_OUTPUT::ERRORS>
 *  std::cout << parser.parseFile("file_ok.json", tree)                       << std::endl; // returns <JSON_PARSE_OUTPUT::OK>
 *
 *  std::cout << parser.parseFile("some_file.json", tree, true)               << std::endl; // shows information about parsing while doing it
 *
 *  // Error handling
 *  parser.getErrors();                                                                     // returns vector <JsonLog> of errors
 *  parser.getWarnings();                                                                   // returns vector <JsonLog> of warnings
 *
 *  std::cout << "Saving" << std::endl;
 *
 *  // Saving
 *  std::cout << parser.saveFile("/path/with/no/permissions/new_file", tree) << std::endl;  // returns <JSON_PARSE_OUTPUT::CANT_OPEN_FILE>
 *  std::cout << parser.saveFile("new_file.json", tree) << std::endl;                       // creates human readable file, returns <JSON_PARSE_OUTPUT::OK>
 *  std::cout << parser.saveFile("new_file_ugly.json", tree, true) << std::endl;            // creates non human readable file, returns <JSON_PARSE_OUTPUT::OK>
 *
 * }
 * ```
 * ### Output
 * ```
 * Parsing
 * 2  // 0x00010
 * 16 // 0x10000
 * 8  // 0x01000
 * 4  // 0x00100
 * 12 // 0x01100
 * 1  // 0x00001
 * Saving
 * 2  // 0x00010
 * 1  // 0x00001
 * ```
 * ### Generated files
 * #### new_file
 * ```json
 * {
 *  "element" : 12,
 *  "boolean" : true
 * }
 * ```
 * #### new_file_ugly
 * ```json
 * {"element":12,"boolean":true}
 * ```
 */
class Parser {
  private:

  struct ObjectNameFlag {    
    AbstractObject* element;
    std::string key;
    int flag;
  };

  const std::string reverseflag [8] = {"LAST_ELEMENT",
                                       "REGULAR_ELEMENT",
                                       "NO_CLOSED",
                                       "EXPECTED_MORE",
                                       "INVALID_KEY",
                                       "EXPECTED KEY",
                                       "-",
                                       "EMPTY"
                                      };
  std::vector <JsonLog> errors;
  std::vector <JsonLog> warnings;
  std::ifstream file;

  std::string content;

  bool verbose;


  bool openFile (std::string fileName);
  bool hasComma ();
  void evaluateFlag (int flag, std::string path, std::string finalElement);

  ObjectNameFlag parseExpectingKeyDef (std::string& path);
  ObjectNameFlag parseExpectingElement (std::string& path);

  ObjectNameFlag parseVector (std::string path);
  ObjectNameFlag parseMap (std::string& path);

  ObjectNameFlag parseBool ();
  ObjectNameFlag parseQuote ();
  ObjectNameFlag parseNumber ();

  inline bool hasErrors () { return errors.size() > 0; }
  inline bool hasWarnings () { return warnings.size() > 0; }
  inline std::ifstream& getFile () { return file; }
public:
  // Default constructor
  Parser ();

  /* Reads a json file
   * @fileName path of the input file to be parsed
   * @tree object wich will store the hierarchy specificated in the file
   * @verbs use or not verbose mode, by default =true
   *
   * ## Description
   *
   * Tries to parse the file and to create the hierarchy of it in `tree`.
   * The method checks if the file exists, can be opened and is not empty before
   * starting the process.
   *
   * If there are syntax errors in the input file, they are
   * catched and a <JsonLog> is generated for each of them. Those
   * structs are stacked in inner vectors nad are accesible with the <getErrors> and <getWarnings> functions. The method returns a
   * binary flag informing possible problems. The cases are described at <JSON_PARSE_OUTPUT>
   *
   * If verbosity is activated, the method shows messages by cerr when errors or
   * warnings happen. **There are no message for errors in file handling**.
   * Errors / Warnings are described at <JSON_PARSER_FLAGS>
   *
   *   - Warnings: empty container (hash or array)
   *   - Errors:   everything else
   *
   * Those message use this syntax:
   *
   *   - Warning parsing JSON: <EMPTY> in path: $path_of_the_warning
   *   - Error parsing JSON: <NO_CLOSED|EXPECTED_MORE|INVALID_KEY> in path: $path_of_the_error
   *
   * `tree` object is only modified if there are no errors (warnings are allowed) and the method returns 1 or 5.
   *
   * ## Example
   * ```c++
   * json::JsonTree tree;                                                  // object to store the information of the file
   * json::Parser parser;
   *
   * parser.parseFile ("no_file", tree, false);                            // returns 2, JSON_PARSE_OUTPUT::CANT_OPEN_FILE, no output by cerr
   * parser.parseFile ("no_file", tree, true);                             // returns 2, JSON_PARSE_OUTPUT::CANT_OPEN_FILE, no output by cerr
   * parser.parseFile ("no_file", tree);                                   // returns 2, JSON_PARSE_OUTPUT::CANT_OPEN_FILE, no output by cerr
   *
   * parser.parseFile ("empty_file.json", tree, false);                    // returns 16, JSON_PARSE_OUTPUT::EMPTY_FILE, no output by cerr
   * parser.parseFile ("empty_file.json", tree, true);                     // returns 16, JSON_PARSE_OUTPUT::EMPTY_FILE, no output by cerr
   * parser.parseFile ("empty_file.json", tree);                           // returns 16, JSON_PARSE_OUTPUT::EMPTY_FILE, no output by cerr
   *
   * parser.parseFile ("file_with_errors.json", tree, false);              // returns 8, JSON_PARSE_OUTPUT::ERRRORS, no output by cerr
   * parser.parseFile ("file_with_errors.json", tree, true);               // returns 8, JSON_PARSE_OUTPUT::ERRRORS, output by cerr
   * parser.parseFile ("file_with_errors.json", tree);                     // returns 8, JSON_PARSE_OUTPUT::ERRRORS, output by cerr
   *
   * parser.parseFile ("file_with_warnings.json", tree, false);            // returns 5, JSON_PARSE_OUTPUT::WARNINGS + JSON_PARSE_OUTPUT::OK, no output by cerr
   * parser.parseFile ("file_with_warnings.json", tree, true);             // returns 5, JSON_PARSE_OUTPUT::WARNINGS + JSON_PARSE_OUTPUT::OK, output by cerr
   * parser.parseFile ("file_with_warnings.json", tree);                   // returns 5, JSON_PARSE_OUTPUT::WARNINGS + JSON_PARSE_OUTPUT::OK, output by cerr
   *
   * parser.parseFile ("file_with_warnings_and_errors.json", tree, false); // returns 12, JSON_PARSE_OUTPUT::WARNINGS + JSON_PARSE_OUTPUT::ERRORS, no output by cerr
   * parser.parseFile ("file_with_warnings_and_errors.json", tree, true);  // returns 12, JSON_PARSE_OUTPUT::WARNINGS + JSON_PARSE_OUTPUT::ERRORS, output by cerr
   * parser.parseFile ("file_with_warnings_and_errors.json", tree);        // returns 12, JSON_PARSE_OUTPUT::WARNINGS + JSON_PARSE_OUTPUT::ERRORS, output by cerr
   *
   * parser.parseFile ("well_formated_file.json", tree, false);            // returns 1, JSON_PARSE_OUTPUT::OK, no output by cerr
   * parser.parseFile ("well_formated_file.json", tree, true);             // returns 1, JSON_PARSE_OUTPUT::OK, output by cerr
   * parser.parseFile ("well_formated_file.json", tree);                   // returns 1, JSON_PARSE_OUTPUT::OK, output by cerr
   * ```
   * #### Fast checking everything went well
   *
   * ```c++
   * json::JsonTree tree;
   * json::Parser parser;
   *
   * if (parser.parseFile ("example.json", tree) & json::JSON_PARSE_OUTPUT::OK) {
   *  // if we are here tree contains the info of example.json file
   * }
   * ```
   *
   * @return combination of flags <JSON_PARSE_OUTPUT> with the result of the operation
  */
  int parseFile (const std::string fileName, JsonTree& tree, bool verbs = true);

  /* Overloaded function
   * @fileName path of the input file to be parsed
   * @tree object wich will store the hierarchy specificated in the file
   * @verbs use or not verbose mode, by default =true
   *
   * Same as previous but giving a string with the information
   *
   * @return combination of flags <JSON_PARSE_OUTPUT> with the result of the operation
   * */
  int parseString (const std::string content, JsonTree& tree, bool verbs = true);

  /* Creates a json file
   * @fileName input file to be parsed
   * @tree hierarchy that wil be stored in the file
   * @uglify store content as human readable or minimizing file size. By default =false (human readable, max filesize)
   *
   * ## Description
   *
   * Tries to create the file `fileName` and to write the tree's content in it.
   * As <JsonTree> objects are incoherence free, the only errors
   * that can happen are those involved in the manipulation of the file,
   * so the method can be static (no entries to check at <getErrors> / <getWarnings>)
   *
   * Note: This method **does not append information to other files or checks if they previously exist**. Only generates / overwrites the file
   * indicated by `fileName`
   *
   * Uglification is explained at <JsonTree::toText>
   *
   * ## Example
   *
   * ```c++
   * json::JsonTree tree;
   * // some code
   *
   * json::Parser::saveFile ("/path/with/no/permissions", tree, false);        // returns 16, JSON_PARSE_OUTPUT::CANT_OPEN_FILE
   * json::Parser::saveFile ("/path/with/no/permissions", tree, true) ;        // returns 16, JSON_PARSE_OUTPUT::CANT_OPEN_FILE
   *
   * json::Parser::saveFile ("example_1.json", tree, false);                   // returns 1, JSON_PARSE_OUTPUT::OK. The file is human readable
   * json::Parser::saveFile ("example_2.json", tree, true );                   // returns 1, JSON_PARSE_OUTPUT::OK. The file is uglified
   * ```
   * @return <JSON_PARSE_OUTPUT> with the result of the operation
  */
  static int saveFile (std::string fileName, JsonTree& tree, bool uglify = false);

  /* Gets error vector
   *
   * @return errors detected while parsing
  */
  inline const std::vector<JsonLog>& getErrors () { return errors; }

  /* Gets warning vector
   *
   * @return warnings detected while parsing
  */
  inline const std::vector<JsonLog>& getWarnings () { return warnings; }
};

}

#endif
