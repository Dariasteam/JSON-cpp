#include "./serializable.hpp"

map<string, function<json::Serializable*()> > json::Serializable::dictionary;
