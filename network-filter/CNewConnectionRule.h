#pragma once
#include "CNetworkFilterRule.h"
#include "CStatistics.h"
#include <map>

typedef std::map<STRING, CStatistics> ENDPOINT_MAP;
typedef ENDPOINT_MAP::iterator ENDPOINT_ITERATOR;

class CNewConnectionRule : public CNetworkFilterRule {
private:
	ENDPOINT_MAP _endpoints;
public:
	BOOL Register(CSocket* socket) override;
};