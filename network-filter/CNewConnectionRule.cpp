#include "pch.h"
#include "CNewConnectionRule.h"

CNewConnectionRule::CNewConnectionRule() {
	this->_amount = 0;
	this->_perAcceptDuration = SECONDS(1);
	::CNetworkFilterRule();
}

BOOL CNewConnectionRule::Register(CSocket* socket) {
	if (this->Match(socket)) {
		BEGIN_LOCK_PTR(SyncRoot());
		{
			auto ep = endpoint_parse(socket->RemoteEndpoint());
			ENDPOINT_ITERATOR it = this->_endpoints.find(ep.first);
			if (it == this->_endpoints.end()) {
				CStatistics stat(_perAcceptDuration * 2, 2);
				this->_endpoints.insert(std::make_pair(ep.first, stat));
			}
			it->second.Add(1);

			return CNetworkFilterRule::Register(socket);
		}
		END_LOCK(SyncRoot());
	}
	return FALSE;
}

BOOL CNewConnectionRule::Parse(STRING_STREAM& stream) {
	STRING str;
	CHAR chr;
	stream >> str; this->_local = endpoint_parse(str);
	stream >> str; this->_remote = endpoint_parse(str);
	stream >> this->_amount;
	stream >> chr; // read '/'
	UINT64 time;
	CHAR unit;
	stream >> time >> unit;
	switch (unit) {
	case 's':
	case 'S':
	default:
		this->_perAcceptDuration = SECONDS(1);
		break;
	case 'm':
	case 'M':
		this->_perAcceptDuration = MINUTES(1);
		break;
	case 'h':
	case 'H':
		this->_perAcceptDuration = HOURS(1);
		break;
	}
	this->_perAcceptDuration *= time;
	return TRUE;
}

BOOL CNewConnectionRule::Filter(CSocket* socket) {
	BEGIN_LOCK_PTR(SyncRoot());
	{
		auto ep = endpoint_parse(socket->RemoteEndpoint());
		ENDPOINT_ITERATOR it = this->_endpoints.find(ep.first);
		if (it != this->_endpoints.end()) {
			it->second.Add(1);
		}
	}
	END_LOCK(SyncRoot());
}