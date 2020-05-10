#pragma once
#include "CNetworkFilterRule.h"
#include "endpoint.h"

class CMaxSendRule : public CNetworkFilterRule {
private:
	INT64 _amountLimit;
public:
	CMaxSendRule();
	BOOL Parse(STRING_STREAM& stream) override;
	BOOL Filter(CSocket*) override;
};