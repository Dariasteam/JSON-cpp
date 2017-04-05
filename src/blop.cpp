#include "blop.h"

using namespace json;

json::BLOP::BLOP()
{

}

std::map<std::string, std::function<json::BLOP*()> > json::BLOP::dictionary;

