#include "auxserialization.h"


std::map<std::string, std::function<json::AuxSerialization*()> > json::AuxSerialization::dictionary;

json::AuxSerialization::AuxSerialization() {}
