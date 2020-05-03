#include "pch.h"
#include "endpoint.h"

std::pair<STRING, STRING> endpoint_parse(STRING endpoint)
{
	size_t index = endpoint.find_last_of(':');
	if (index) {
		return std::make_pair(endpoint.substr(0, index), endpoint.substr(index + 1));
	}
	return std::make_pair(STRING("*"), STRING("*"));
}