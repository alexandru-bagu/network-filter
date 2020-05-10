#pragma once
#include "CNetworkFilterRule.h"
#include "endpoint.h"

namespace mcr {
	typedef std::map<STRING, INT32*> ENDPOINT_MAP;
	typedef ENDPOINT_MAP::iterator ENDPOINT_ITERATOR;
}

class CMaxConnectionsRule : public CNetworkFilterRule {
private:
	INT32 _amount;
	mcr::ENDPOINT_MAP _endpoints;
public:
	CMaxConnectionsRule();
	BOOL Register(CSocket* socket) override;
	BOOL Unregister(CSocket* socket) override;
	BOOL Parse(STRING_STREAM& stream) override;
	BOOL Filter(CSocket*) override;
};