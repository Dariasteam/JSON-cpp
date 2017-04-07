#include "abstract_serializable.h"

using namespace json;

json::__abstract_serializable__::__abstract_serializable__() {}

std::map<std::string, std::function<json::__abstract_serializable__*()> > json::__abstract_serializable__::dictionary;
