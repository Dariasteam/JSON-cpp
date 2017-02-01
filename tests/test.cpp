#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include "../src/parser.hpp"
#include "../src/manager.hpp"

#include <string>
#include <vector>
#include <iostream>

using namespace std;
using namespace json;

Parser parser;
JsonTree tree;

TEST_CASE ("Open Files OK") {
  REQUIRE ((parser.parseFile("tests/input.json", tree) & JSON_PARSE_OUTPUT::OK));
}

TEST_CASE ("Open Files CANT_OPEN_FILE") {
  REQUIRE (parser.parseFile("nofile", tree) != JSON_PARSE_OUTPUT::OK);
  REQUIRE (parser.parseFile("nofile", tree) == JSON_PARSE_OUTPUT::CANT_OPEN_FILE);
}

TEST_CASE ("Open Files EMPTY") {
  REQUIRE (parser.parseFile("nofile", tree) != JSON_PARSE_OUTPUT::OK);
  REQUIRE (parser.parseFile("tests/empty.json", tree) == JSON_PARSE_OUTPUT::EMPTY_FILE);
}

TEST_CASE ("Open Files ERRRORS") {
  REQUIRE (parser.parseFile("nofile", tree) != JSON_PARSE_OUTPUT::OK);
  REQUIRE (parser.parseFile("tests/error.json", tree) == JSON_PARSE_OUTPUT::ERRORS);
}

TEST_CASE ("Open Files WARNINGS") {
  REQUIRE (parser.parseFile("tests/warning.json", tree) ==
                        (JSON_PARSE_OUTPUT::WARNINGS | JSON_PARSE_OUTPUT::OK));
}

TEST_CASE ("Open Files BOTH") {
  REQUIRE (parser.parseFile("nofile", tree) != JSON_PARSE_OUTPUT::OK);
  REQUIRE (parser.parseFile("tests/both.json", tree) ==
                          (JSON_PARSE_OUTPUT::WARNINGS | JSON_PARSE_OUTPUT::ERRORS));
}


TEST_CASE ("Detect warnings / errors", "[Can load error / warning vector]") {
  // OK
  parser.parseFile("tests/input.json", tree);
  CHECK (parser.getErrors().size() == 0);
  CHECK (parser.getWarnings().size() == 0);
  // WARNINGS
  parser.parseFile("tests/warning.json", tree);
  CHECK (parser.getWarnings().size() == 1);
  CHECK (parser.getErrors().size() == 0);
  // ERRORS
  parser.parseFile("tests/error.json", tree);
  CHECK (parser.getErrors().size() == 4);
  CHECK (parser.getWarnings().size() == 0);
  // BOTH
  parser.parseFile("tests/both.json", tree);
  CHECK (parser.getErrors().size() == 1);
  CHECK (parser.getWarnings().size() == 1);
  // CANT_OPEN_FILE
  parser.parseFile("nofile", tree);
  CHECK (parser.getErrors().size() == 0);
  CHECK (parser.getWarnings().size() == 0);
}

TEST_CASE ("Catch correct warnings / errors") {
  JsonLog jlog;
  // WARNING
  parser.parseFile("tests/warning.json", tree);
  jlog = { ".first.element_2.", JSON_PARSER_FLAG::EMPTY };
  CHECK ((parser.getWarnings()[0] == jlog));
  // MULTIPLE ERRORS
  parser.parseFile("tests/error.json", tree);
  jlog = { ".none.first.element", JSON_PARSER_FLAG::INVALID_KEY };
  CHECK ((parser.getErrors()[0] == jlog));
  jlog = { ".none.second.element_2", JSON_PARSER_FLAG::EXPECTED_MORE };
  CHECK ((parser.getErrors()[1] == jlog));
  jlog = { ".third.element.", JSON_PARSER_FLAG::NO_CLOSED };
  CHECK ((parser.getErrors()[2] == jlog));
  jlog = { ".third", JSON_PARSER_FLAG::NO_CLOSED };
  CHECK ((parser.getErrors()[3] == jlog));
}

TEST_CASE ("Updates JsonTree object succesfully") {
  parser.parseFile("tests/input.json", tree);
  CHECK (tree.getKeys("").size() == 3);
  CHECK (tree.getKeys(".").size() == 3);
  CHECK (tree.isMap(""));
  CHECK (tree.isMap("."));
}

TEST_CASE ("Detects wrog path format") {
  CHECK (tree.exist("first.element_1"));
  CHECK (tree.exist("first,element_1") == false);
  CHECK (tree.exist(".first.element_1") == true);
}

TEST_CASE ("Can access to the hierarchy and the types are correct", "") {
  CHECK (tree.isMap("first"));
    CHECK (tree.isNumber("first.element_1"));
    CHECK (tree.isString("first.element_2"));
    CHECK (tree.isBool  ("first.element_3"));
  CHECK (tree.isVector("second"));
    CHECK (tree.isNumber("second.0"));
    CHECK (tree.isNumber("second.[0]"));
    CHECK (tree.isString("second.1"));
    CHECK (tree.isString("second.[1]"));
    CHECK (tree.isBool  ("second.2"));
    CHECK (tree.isBool  ("second.[2]"));
  CHECK (tree.isVector("third"));
    CHECK (tree.isMap("third.0"));
    CHECK (tree.isMap("third.[0]"));
      CHECK (tree.isNumber("third.0.element_1"));
      CHECK (tree.isNumber("third.[0].element_1"));
      CHECK (tree.isString("third.0.element_2"));
      CHECK (tree.isString("third.[0].element_2"));
      CHECK (tree.isBool  ("third.0.element_3"));
      CHECK (tree.isBool  ("third.[0].element_3"));
    CHECK (tree.isVector("third.1"));
    CHECK (tree.isVector("third.[1]"));
      CHECK (tree.isNumber("third.1.0"));
      CHECK (tree.isNumber("third.1.[0]"));
      CHECK (tree.isNumber("third.[1].0"));
      CHECK (tree.isNumber("third.[1].[0]"));
}

TEST_CASE ("Can get the information of the tree, the informations is consistent") {
  // INITIALIZE
  int _int_;
  float _float_;
  double _double_;
  string _string_;
  bool _bool_;
  // VECTOR INT
  vector <int> _vector_i;
  vector <int> compare_i (3);
  compare_i[0] = 12;
  compare_i[1] = 13;
  compare_i[2] = 14;
  // VECTOR BOOL
  vector <bool> _vector_b;
  vector <bool> compare_b (3);
  compare_b[0] = true;
  compare_b[1] = false;
  compare_b[2] = true;
  // VECTOR STRING
  vector <string> _vector_s;
  vector <string> compare_s (3);
  compare_s[0] = string("first");
  compare_s[1] = string("second");
  compare_s[2] = string("third");
  // TEST
  CHECK ((tree.get(_int_,    "first.element_1") && (_int_    == 12)));
  CHECK ((tree.get(_float_,  "first.element_1") && (_float_  == 12)));
  CHECK ((tree.get(_double_, "first.element_1") && (_double_ == 12)));
  CHECK ((tree.get(_string_, "first.element_2") && (_string_ == "Word")));
  CHECK ((tree.get(_bool_,   "first.element_3") && (_bool_   == true)));
  // VECTORS
  CHECK (tree.getSizeAt("third") == 4);
  CHECK ((tree.get(_vector_i, "third.1") && (_vector_i == compare_i)));
  CHECK ((tree.get(_vector_b, "third.2") && (_vector_b == compare_b)));
  CHECK ((tree.get(_vector_s, "third.3") && (_vector_s == compare_s)));
  // WILL FAIL
  CHECK (tree.get(_bool_,    "first.element_1") == false);
  CHECK (tree.get(_string_,  "first.element_1") == false);
  CHECK (tree.get(_vector_i, "second") == false);
}

TEST_CASE ("Can add empty containers to the tree") {
  // ADD CONTAINER
  CHECK (tree.addMap     ("fourth"));
  CHECK (tree.addVector   ("fifth"));
  // CHECK IF EXIST
  CHECK (tree.isVector("fifth"));
  CHECK (tree.isMap   ("fourth"));
  // CAN'T BE REPLACED
  CHECK (tree.addVector("fourth") == false);
  CHECK (tree.addMap   ("fourth") == false);
  CHECK (tree.addVector("fifth") == true); // push back a vector inside the vector
  CHECK (tree.addMap   ("fifth") == true); // push_back a hash inside the vector
}

TEST_CASE ("Can't copy heterogeneus vectors") {
  vector <int> _vector_i;
  vector <int> compare_i (3);
  compare_i[0] = 12;
  compare_i[1] = 13;
  compare_i[2] = 14;
  CHECK ((tree.get(_vector_i, "second") && (_vector_i == compare_i)) == false);
}

TEST_CASE ("Can add finals to the tree (hash)") {
  // INITIALIZE
  int _int_;
  float _float_;
  double _double_;
  string _string_;
  bool _bool_;
  // ADD ELEMENTS TO A MAP
  CHECK (tree.add(double(12.3),   "fourth.element_1"));
  CHECK (tree.add(float(12.3),    "fourth.element_2"));
  CHECK (tree.add(int(12),        "fourth.element_3"));
  CHECK (tree.add(bool(true),     "fourth.element_4"));
  CHECK (tree.add("Word",         "fourth.element_5"));
  CHECK (tree.add(string("Word"), "fourth.element_6"));
  // CHECK IF EXIST
  CHECK ((tree.get(_double_, "fourth.element_1") && (_double_ == 12.3)));
  CHECK ((tree.get(_float_,  "fourth.element_2") && (_float_  == float(12.3))));
  CHECK ((tree.get(_int_,    "fourth.element_3") && (_int_    == 12)));
  CHECK ((tree.get(_bool_,   "fourth.element_4") && (_bool_   == true)));
  CHECK ((tree.get(_string_, "fourth.element_5") && (_string_ == "Word")));
  CHECK ((tree.get(_string_, "fourth.element_6") && (_string_ == "Word")));
  // CAN'T BE REPLACED
  CHECK (tree.add(12, "fourth.element_1") == false);
  CHECK (tree.add(12, "fourth.element_2") == false);
  CHECK (tree.add(12, "fourth.element_3") == false);
  CHECK (tree.add(12, "fourth.element_4") == false);
  CHECK (tree.add(12, "fourth.element_5") == false);
  CHECK (tree.add(12, "fourth.element_6") == false);
}

TEST_CASE ("Can add finals to the tree (vector)") {
  int _int_;
  float _float_;
  double _double_;
  string _string_;
  bool _bool_;
  // ADD ELEMENTS TO A VECTOR
  CHECK (tree.add(double(12.3),    "fifth"));
  CHECK (tree.add(float(12.3),     "fifth"));
  CHECK (tree.add(int(12),         "fifth"));
  CHECK (tree.add(bool(true),      "fifth"));
  CHECK (tree.add("Word1",         "fifth"));
  CHECK (tree.add(string("Word2"), "fifth"));
  // CHECK IF EXIST
  CHECK ((tree.get(_double_, "fifth.2") && (_double_ == 12.3)));
  CHECK ((tree.get(_float_,  "fifth.3") && (_float_ == float(12.3))));
  CHECK ((tree.get(_int_,    "fifth.4") && (_int_ == 12)));
  CHECK ((tree.get(_bool_,   "fifth.5") && (_bool_ == true)));
  CHECK ((tree.get(_string_, "fifth.6") && (_string_ == "Word1")));
  CHECK ((tree.get(_string_, "fifth.7") && (_string_ == "Word2")));
}

TEST_CASE ("Can add vectors to the tree") {
  vector <int> _vector_i;
  vector <int> compare_i (3);
  compare_i[0] = 12;
  compare_i[1] = 13;
  compare_i[2] = 14;
  // VECTOR BOOL
  vector <bool> _vector_b;
  vector <bool> compare_b (3);
  compare_b[0] = true;
  compare_b[1] = false;
  compare_b[2] = true;
  // VECTOR STRING
  vector <string> _vector_s;
  vector <string> compare_s (3);
  compare_s[0] = string("first");
  compare_s[1] = string("second");
  compare_s[2] = string("third");
  // ADD
  CHECK (tree.add(compare_i, "fifth"));
  CHECK (tree.add(compare_b, "fifth"));
  CHECK (tree.add(compare_s, "fifth"));
  // CHECK IF EXIST
  CHECK ((tree.get(_vector_i, "fifth.8") && (_vector_i == compare_i)));
  CHECK ((tree.get(_vector_b, "fifth.9") && (_vector_b == compare_b)));
  CHECK ((tree.get(_vector_s, "fifth.10") && (_vector_s == compare_s)));
}

TEST_CASE ("add methods create entire hash hierarchy if doesn't exist") {
  // ADD ELEMENTS TO A MAP
  CHECK (tree.add(double(12.3), "a.very.long.path.for.testing"));
  // CHECK IF EXIST
  CHECK (tree.isFloat          ("a.very.long.path.for.testing"));
  // CANT REPLACE
  CHECK (tree.add(double(12.3), "a.very.long.path.for.testing") == false);
}

TEST_CASE ("Can replace elements in the tree (hash)") {
  // INITIALIZE
  int _int_;
  float _float_;
  double _double_;
  string _string_;
  bool _bool_;
  // REPLACE NON EXISTING ELEMENTS
  CHECK (tree.replace (double(17.3), "non.existing.path") == false);
  // REPLACE EXISTING FINAL ELEMENTS
  CHECK (tree.replace (double(17.3),       "fourth.element_6"));
  CHECK (tree.replace (float(17.3),        "fourth.element_5"));
  CHECK (tree.replace (int(17),            "fourth.element_4"));
  CHECK (tree.replace (bool(false),        "fourth.element_3"));
  CHECK (tree.replace ("replaced",         "fourth.element_2"));
  CHECK (tree.replace (string("replaced"), "fourth.element_1"));
  // CHECK IF REPLACED
  CHECK ((tree.get(_double_, "fourth.element_6") && (_double_ == 17.3)));
  CHECK ((tree.get(_float_,  "fourth.element_5") && (_float_  == float(17.3))));
  CHECK ((tree.get(_int_,    "fourth.element_4") && (_int_    == 17)));
  CHECK ((tree.get(_bool_,   "fourth.element_3") && (_bool_   == false)));
  CHECK ((tree.get(_string_, "fourth.element_2") && (_string_ == "replaced")));
  CHECK ((tree.get(_string_, "fourth.element_1") && (_string_ == "replaced")));
  // REPLACE NON FINAL EXISTING ELEMENTS
  CHECK (tree.replace (double(17.3), "fifth"));
  CHECK (tree.replace (double(17.3), "fourth"));
  // CHECK IF REPLACED
  CHECK ((tree.get(_double_, "fifth") &&  (_double_ == 17.3)));
  CHECK ((tree.get(_double_, "fourth") && (_double_ == 17.3)));
}

TEST_CASE ("Can set finals in the tree (hash)") {
  // INITIALIZE
  int _int_;
  float _float_;
  double _double_;
  string _string_;
  bool _bool_;
  // CREATE NON EXISTING ELEMENTS
  CHECK (tree.set (double(12.3),   "sixth.element_1"));
  CHECK (tree.set (float(12.3),    "sixth.element_2"));
  CHECK (tree.set (int(12),        "sixth.element_3"));
  CHECK (tree.set (false,          "sixth.element_4"));
  CHECK (tree.set ("Word",         "sixth.element_5"));
  CHECK (tree.set (string("Word"), "sixth.element_6"));
  // CHECK IF EXIST
  CHECK ((tree.get(_double_, "sixth.element_1") && (_double_ == 12.3)));
  CHECK ((tree.get(_float_,  "sixth.element_2") && (_float_  == float(12.3))));
  CHECK ((tree.get(_int_,    "sixth.element_3") && (_int_    == 12)));
  CHECK ((tree.get(_bool_,   "sixth.element_4") && (_bool_   == false)));
  CHECK ((tree.get(_string_, "sixth.element_5") && (_string_ == "Word")));
  CHECK ((tree.get(_string_, "sixth.element_6") && (_string_ == "Word")));
  // REPLACE EXISTING ELEMENTS
  CHECK (tree.set (double(17.3),       "sixth.element_6"));
  CHECK (tree.set (float(17.3),        "sixth.element_5"));
  CHECK (tree.set (int(17),            "sixth.element_4"));
  CHECK (tree.set (true,              "sixth.element_3"));
  CHECK (tree.set ("replaced",         "sixth.element_2"));
  CHECK (tree.set (string("replaced"), "sixth.element_1"));
  // CHECK IF EXIST
  CHECK ((tree.get(_double_, "sixth.element_6") && (_double_ == 17.3)));
  CHECK ((tree.get(_float_,  "sixth.element_5") && (_float_  == float(17.3))));
  CHECK ((tree.get(_int_,    "sixth.element_4") && (_int_    == 17)));
  CHECK ((tree.get(_bool_,   "sixth.element_3") && (_bool_   == true)));
  CHECK ((tree.get(_string_, "sixth.element_2") && (_string_ == "replaced")));
  CHECK ((tree.get(_string_, "sixth.element_1") && (_string_ == "replaced")));
  // CAN'T SET IN VECTORS
  tree.addVector ("sixth.element_7");
  CHECK (tree.set (double(9), "sixth.element_7.0") == false);
}

TEST_CASE ("Can set vectors in the tree (hash)") {
  vector <int> _vector_i;
  vector <int> compare_i (3);
  compare_i[0] = 12;
  compare_i[1] = 13;
  compare_i[2] = 14;
  // VECTOR BOOL
  vector <bool> _vector_b;
  vector <bool> compare_b (3);
  compare_b[0] = true;
  compare_b[1] = false;
  compare_b[2] = true;
  // VECTOR STRING
  vector <string> _vector_s;
  vector <string> compare_s (3);
  compare_s[0] = string("first");
  compare_s[1] = string("second");
  compare_s[2] = string("third");
  // ADD
  CHECK (tree.set(compare_i, "sixth.vector_1"));
  CHECK (tree.set(compare_b, "sixth.vector_2"));
  CHECK (tree.set(compare_s, "sixth.vector_3"));
  // CHECK IF EXIST
  CHECK ((tree.get(_vector_i, "sixth.vector_1") && (_vector_i == compare_i)));
  CHECK ((tree.get(_vector_b, "sixth.vector_2") && (_vector_b == compare_b)));
  CHECK ((tree.get(_vector_s, "sixth.vector_3") && (_vector_s == compare_s)));
}

TEST_CASE ("Can erase elements") {
  // NON EXISTING ELEMENTS
  CHECK (tree.erase ("non.existing.path") == false);
  // FINAL ELEMENTS
  CHECK (tree.erase ("sixth.element_1"));
  CHECK (tree.erase ("sixth.element_2"));
  CHECK (tree.erase ("sixth.element_3"));
  CHECK (tree.erase ("sixth.element_4"));
  CHECK (tree.erase ("sixth.element_5"));
  CHECK (tree.erase ("sixth.element_6"));
  // CHECK IF EXIST
  CHECK (tree.exist ("sixth.element_1") == false);
  CHECK (tree.exist ("sixth.element_2") == false);
  CHECK (tree.exist ("sixth.element_3") == false);
  CHECK (tree.exist ("sixth.element_4") == false);
  CHECK (tree.exist ("sixth.element_5") == false);
  CHECK (tree.exist ("sixth.element_6") == false);
}

TEST_CASE ("Can remove elements") {
  // NON EXISTING ELEMENTS
  CHECK (tree.remove ("non.existing.path") == true);
  // FINAL ELEMENTS
  CHECK (tree.remove ("fourth.element_1"));
  CHECK (tree.remove ("fourth.element_2"));
  CHECK (tree.remove ("fourth.element_3"));
  CHECK (tree.remove ("fourth.element_4"));
  CHECK (tree.remove ("fourth.element_5"));
  CHECK (tree.remove ("fourth.element_6"));
  // CHECK IF EXIST
  CHECK (tree.exist ("fourth.element_1") == false);
  CHECK (tree.exist ("fourth.element_2") == false);
  CHECK (tree.exist ("fourth.element_3") == false);
  CHECK (tree.exist ("fourth.element_4") == false);
  CHECK (tree.exist ("fourth.element_5") == false);
  CHECK (tree.exist ("fourth.element_6") == false);
}

TEST_CASE ("Can write files") {
  CHECK (Parser::saveFile ("tests/output.json", tree));
  CHECK (parser.saveFile ("tests/output.json", tree));
  ifstream file;
  file.open ("tests/output.json");
  CHECK (file.is_open());
  file.close();
}

TEST_CASE ("The written file is consistent") {
  REQUIRE ((parser.parseFile("tests/output.json", tree) & JSON_PARSE_OUTPUT::OK));
}
