#include "pch.h"
#include "CNewConnectionRule.h"

CNewConnectionRule::CNewConnectionRule() {
	this->_amount = 0;
	this->_perAcceptDuration = SECONDS(1);
	::CNetworkFilterRule();
}

BOOL CNewConnectionRule::Register(CSocket* socket) {
	if (this->Match(socket)) {
		LOG->Debug(string_format("Begin CNewConnectionRule::Register called for %s", socket->RemoteEndpoint().c_str()));
		auto ep = endpoint_parse(socket->RemoteEndpoint());

		BEGIN_LOCK_PTR(SyncRoot());
		{
			ncr::ENDPOINT_ITERATOR it = this->_endpoints.find(ep.first);
			if (it == this->_endpoints.end()) {
				CStatistics stat(_perAcceptDuration * 2, 2, 0.5f);
				this->_endpoints.insert(std::make_pair(ep.first, stat));
				it = this->_endpoints.find(ep.first);
			}
		}
		END_LOCK(SyncRoot());

		if (CNetworkFilterRule::Register(socket)) {
			BEGIN_LOCK_PTR(SyncRoot());
			{
				ncr::ENDPOINT_ITERATOR it = this->_endpoints.find(ep.first);
				if (it != this->_endpoints.end()) {
					it->second.Add(1);
				}
			}
			END_LOCK(SyncRoot());
		}
		LOG->Debug(string_format("End CNewConnectionRule::Register called for %s", socket->RemoteEndpoint().c_str()));
	}
	return FALSE;
}

BOOL CNewConnectionRule::Parse(STRING_STREAM& stream) {
	STRING str;
	CHAR chr;
	stream >> str; this->_source.Parse(str);
	stream >> str; this->_target.Parse(str);
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
		ncr::ENDPOINT_ITERATOR it = this->_endpoints.find(ep.first);
		if (it != this->_endpoints.end()) {
			if (it->second.ComputeRecent() >= this->_amount) {
				return TRUE;
			}
		}
	}
	END_LOCK(SyncRoot());
	return FALSE;
}