#pragma once
#include "CNetworkFilterRule.h"
#include "endpoint.h"

class CMaxRecvRule : public CNetworkFilterRule {
private:
	INT64 _amountLimit;
public:
	CMaxRecvRule();
	BOOL Parse(STRING_STREAM& stream) override;
	BOOL Filter(CSocket*) override;
};