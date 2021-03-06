#pragma once
#include "CNetworkFilterRule.h"
#include "endpoint.h"

namespace ncr {
	typedef std::map<STRING, CStatistics> ENDPOINT_MAP;
	typedef ENDPOINT_MAP::iterator ENDPOINT_ITERATOR;
}

class CNewConnectionRule : public CNetworkFilterRule {
private:
	INT32 _amount;
	DURATION  _perAcceptDuration;
	ncr::ENDPOINT_MAP _endpoints;
public:
	CNewConnectionRule();
	BOOL Register(CSocket* socket) override;
	BOOL Parse(STRING_STREAM& stream) override;
	BOOL Filter(CSocket*) override;
};