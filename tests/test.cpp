#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include "../parser.hpp"
#include "../manager.hpp"

#include <string>
#include <vector>
#include <iostream>

using namespace std;
using namespace json;

Parser parser;
JsonTree tree;

TEST_CASE ("Open Files", "[Return correct flags]") {
  REQUIRE ((parser.parseFile("tests/input.json", tree) & JSON_PARSE_OUTPUT::OK));
  REQUIRE (parser.parseFile("nofile", tree) != JSON_PARSE_OUTPUT::OK);
  REQUIRE (parser.parseFile("nofile", tree) == JSON_PARSE_OUTPUT::CANT_OPEN_FILE);
  REQUIRE (parser.parseFile("tests/empty.json", tree) == JSON_PARSE_OUTPUT::EMPTY_FILE);
  REQUIRE (parser.parseFile("tests/error.json", tree) == JSON_PARSE_OUTPUT::ERRORS);
  REQUIRE (parser.parseFile("tests/warning.json", tree) ==
                        (JSON_PARSE_OUTPUT::WARNINGS | JSON_PARSE_OUTPUT::OK));
  REQUIRE (parser.parseFile("tests/both.json", tree) ==
                          (JSON_PARSE_OUTPUT::WARNINGS | JSON_PARSE_OUTPUT::ERRORS));

}

// tree = JsonTree ();

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

TEST_CASE ("Catch correct warnings / errors", "") {
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

TEST_CASE ("Updates JsonTree object succesfully", "") {
  parser.parseFile("tests/input.json", tree);
  CHECK (tree.getKeys("").size() == 3);
  CHECK (tree.getKeys(".").size() == 3);
  CHECK (tree.isMap(""));
  CHECK (tree.isMap("."));
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
  CHECK (tree.exist("third.2"));
  CHECK (tree.exist("third.3"));

  CHECK ((tree.get(_vector_i, "third.1") && (_vector_i == compare_i)));
  CHECK ((tree.get(_vector_b, "third.2") ));
  CHECK ((tree.get(_vector_s, "third.3") && (_vector_s == compare_s)));
  // WILL FAIL
  CHECK (tree.get(_bool_, "first.element_1") == false);
  CHECK (tree.get(_string_, "first.element_1") == false);
  CHECK (tree.get(_vector_i, "second") == false);
}

TEST_CASE ("Can add empty containers to the tree") {
  // ADD CONTAINER
  CHECK (tree.addVector("fourth"));
  CHECK (tree.addMap("fifth"));
  // CHECK IF EXIST
  CHECK (tree.isVector("fourth"));
  CHECK (tree.isMap("fifth"));
  // CAN'T BE REPLACED
  CHECK (tree.addVector("fourth") == false);
  CHECK (tree.addMap("fourth") == false);
}

TEST_CASE ("Can add finals to the tree") {
  // ADD ELEMENTS TO A MAP
  CHECK (tree.add(double(12.3), "fourth.element_1"));
  CHECK (tree.add(float(12.3), "fourth.element_2"));
  CHECK (tree.add(int(12), "fourth.element_3"));
  CHECK (tree.add(bool(true), "fourth.element_4"));
  CHECK (tree.add("Word", "fourth.element_5"));
  CHECK (tree.add(string("Word"), "fourth.element_6"));
}
