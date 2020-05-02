#pragma once
#include <string>
#include <WTypesbase.h>
typedef std::string STRING;
#define TO_STRING(T) std::to_string(T)

STRING net_speed_format(DOUBLE bytes);