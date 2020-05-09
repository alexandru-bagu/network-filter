#pragma once
#include "CSocket.h"
#include "CNetworkFilterRule.h"
#include <vector>

const INT64 BYTE_UNIT = 1;
const INT64 KILOBYTE = 1024 * BYTE_UNIT;
const INT64 MEGABYTE = 1024 * KILOBYTE;
const INT64 GIGABYTE = 1024 * MEGABYTE;

typedef std::vector<CNetworkFilterRule*> RULE_VECTOR;
typedef RULE_VECTOR::iterator RULE_ITERATOR;

class CNetworkFilter {
private:
	RULE_VECTOR _rules;
public:
	CNetworkFilter();

	/// Parses provided configuration file
	VOID Parse(STRING);

	/// Returns true if the socket was registered (it was not pre-filtered)
	BOOL Register(CSocket* socket);

	/// Removed a socket from the rule pool
	VOID Unregister(CSocket* socket);

	/// Returns true if the socket was flagged
	BOOL Filter(CSocket* socket);
};