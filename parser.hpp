#ifndef PARSER_H
#define PARSER_H

#include <regex>
#include <fstream>
#include <string>
#include <iostream>
#include <climits>

#include "./object.hpp"
#include "manager.hpp"

using namespace std;

namespace json {

// Hex flags used as output for <Parser::parseFile> and <Parser::saveFile> functions.
enum JSON_PARSE_OUTPUT {
  // the file has been opened and parsed successfuly
  OK          	  = 1 << 0,
  // the file can not be opened
  CANT_OPEN_FILE  = 1 << 1,
  // the parse operation has detected non critical extraneous situations in the format of the input file
  WARNINGS    	  = 1 << 2,
  // the parse operation has detected critical problems in the format of the input file
  ERRORS      	  = 1 << 3,
  // the file is empty and can not be parsed
  EMPTY_FILE      = 1 << 4
};


struct ObjectNameFlag {
  AbstractObject* element;
  string key;
  int flag;
};


struct JsonLog {
  string path;
  int flag;
  friend bool operator== (const JsonLog& j1, const JsonLog& j2) {
    return (j1.path == j2.path) && (j1.flag == j2.flag);
  }
};


/* Manage I/O of .json files
 *
 *
 *
 *
*/
class Parser {
  private:

  //- do not move these elements, their numeric value is used for comparisons
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
    // used to check type. Greaters values are warnings, lower errors
    CONTROL_WARNING,
    // [WARNING] the collection is empty
    EMPTY
  };

  const string reverseflag [7] = {"LAST_ELEMENT",
                                  "REGULAR_ELEMENT",
                                  "NO_CLOSED",
                                  "EXPECTED_MORE",
                                  "INVALID_KEY",
                                  "-",
                                  "EMPTY"
                                 };
  vector <JsonLog> errors;
  vector <JsonLog> warnings;
  ifstream file;

  bool verbose;

  static regex startBrace;
  static regex startBracket;
  static regex keyDef;
  static regex finalQuote;
  static regex finalNumberFloat;
  static regex finalNumberInt;
  static regex finalBoolean;
  static regex nextBrace;
  static regex nextBracket;

  bool openFile (string fileName);
  bool hasComma (string buffer);
  void evaluateFlag (int flag, string path, string finalElement);

  ObjectNameFlag parse (string& content, string path);
  ObjectNameFlag parseKeyDef (string& content, smatch& matcher, string path);
  ObjectNameFlag parseFinal (string& content, smatch& matcher, ObjectFinal* obj);
  ObjectNameFlag parseContainer (string& content, smatch& matcher, regex& endSymbol, ObjectContainer* obj, string path);

  inline bool hasErrors () { return errors.size() > 0; }
  inline bool hasWarnings () { return warnings.size() > 0; }
  inline ifstream& getFile () { return file; }
public:
  // Default constructor
  Parser ();

  /* Read a .json file
   * @fileName path of the input file to be parsed
   * @tree object wich will store the hierarchy specificated in the file
   * @verbs use or not verbose mode, by default =true
   *
   * Tries to parse the file and create the hierarchy of it.
   * The method check if the file exists and can be open before
   * starting the process.
   *
   * If there are syntax errors in the input file, they will be
   * catched and a <JsonLog> is generated for any of them. Those
   * structs can be accessed with <getErrors> and <getWarnings> functions
   *
   * @return <JSON_PARSE_OUTPUT> with the result of the operation
  */
  int parseFile (string fileName, JsonTree& tree, bool verbs = true);

  /* Create a .json file
   * @fileName input file to be parsed
   * @tree hierarchy that wil be stored in the file
   * @uglify store content as human readable or minimizing file size. By default =false (human readable, max filesize)
   *
   * Tries to create the file and writes the tree's content in it.
   * As <JsonTree> objects are incoherence free, the only errors
   * that could happen are those involved in the manipulation of the file,
   * so the method can be static.
   *
   * @return <JSON_PARSE_OUTPUT> with the result of the operation
  */
  static int saveFile (string fileName, JsonTree& tree, bool uglify = false);

  /* Get error vector
   *
   * @return errors detected wile parsing
  */
  inline const vector<JsonLog>& getErrors () { return errors; }

  /* Get warning vector
   *
   * @return warnings detected wile parsing
  */
  inline const vector<JsonLog>& getWarnings () { return warnings; }
};

}

#endif
