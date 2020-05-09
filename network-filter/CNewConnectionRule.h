#pragma once
#include "CNetworkFilterRule.h"
#include "endpoint.h"

typedef std::map<STRING, CStatistics> ENDPOINT_MAP;
typedef ENDPOINT_MAP::iterator ENDPOINT_ITERATOR;

class CNewConnectionRule : public CNetworkFilterRule {
private:
	ENDPOINT _local, _remote;
	INT32 _amount;
	DURATION  _perAcceptDuration;
	ENDPOINT_MAP _endpoints;
public:
	CNewConnectionRule();
	BOOL Register(CSocket* socket) override;

	///Format: *:5816 *:* <amount>/<duration>
	BOOL Parse(STRING_STREAM& stream) override;

	BOOL Filter(CSocket*) override;
};