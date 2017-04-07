#include "./deprecated_serializable.hpp"

std::map<std::string, std::function<json::deprecated_serializable*()> > json::deprecated_serializable::dictionary;
