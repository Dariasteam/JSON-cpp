#include "auxserialization.h"


json::AuxSerialization::AuxSerialization() {}

bool json::AuxSerialization::serializeIn (std::string file, std::string path) {
  json::Parser parser;
  json::JsonTree tree;

  if (parser.parseFile(file, tree, false) & JSON_PARSE_OUTPUT::OK)
    return tree.get(*this, path);
  else
    return false;
}

bool json::AuxSerialization::serializeIn (json::JsonTree& tree, std::string path) {
  return tree.get(*this, path);
}

bool json::AuxSerialization::serializeOut (std::string file, std::string path) {
  json::JsonTree tree;

  if (tree.set(*this, path))
    return json::Parser::saveFile(file, tree, false);
  else
    return false;
}

bool json::AuxSerialization::serializeOut (json::JsonTree& tree, std::string path) {
  return tree.set(*this, path);
}
