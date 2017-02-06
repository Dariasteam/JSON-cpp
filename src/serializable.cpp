#include "./serializable.hpp"

std::map<std::string, std::function<json::Serializable*()> > json::Serializable::dictionary;
