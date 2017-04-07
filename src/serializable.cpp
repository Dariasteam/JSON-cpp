#include "serializable.h"

json::Serializable::Serializable() {}

bool json::Serializable::serializeIn (std::string file, std::string path) {
  json::Parser parser;
  json::JsonTree tree;

  if (parser.parseFile(file, tree, false) & JSON_PARSE_OUTPUT::OK)
    return tree.get(*this, path);
  else
    return false;
}

bool json::Serializable::serializeIn (json::JsonTree& tree, std::string path) {
  return tree.get(*this, path);
}

bool json::Serializable::serializeOut (std::string file, std::string path) {
  json::JsonTree tree;

  if (tree.set(*this, path))
    return json::Parser::saveFile(file, tree, false);
  else
    return false;
}

bool json::Serializable::serializeOut (json::JsonTree& tree, std::string path) {
  return tree.set(*this, path);
}

std::string json::Serializable::toTxt(bool uglify) {
  json::JsonTree tree;
  tree.set(*this, "a");
  return tree.toText(uglify, "a");
}
