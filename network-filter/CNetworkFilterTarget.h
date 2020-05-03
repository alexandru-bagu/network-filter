#pragma once
#include "pch.h"

class CNetworkFilterTarget {
private:
	STRING _address;
	STRING _port;
public:
	CNetworkFilterTarget();
	BOOL Parse(STRING endpoint);
	BOOL Match(STRING endpoint);
};