#include "pch.h"
#include "CMaxConnectionsRule.h"

CMaxConnectionsRule::CMaxConnectionsRule() {
	this->_amount = 0;
	::CNetworkFilterRule();
}

BOOL CMaxConnectionsRule::Register(CSocket* socket) {
	if (this->Match(socket)) {
		auto ep = endpoint_parse(socket->RemoteEndpoint());
		BEGIN_LOCK_PTR(SyncRoot());
		{
			mcr::ENDPOINT_ITERATOR it = this->_endpoints.find(ep.first);
			if (it == this->_endpoints.end()) {
				INT32* count = new INT32;
				this->_endpoints.insert(std::make_pair(ep.first, count));
				it = this->_endpoints.find(ep.first);
			}
		}
		END_LOCK(SyncRoot());

		if (CNetworkFilterRule::Register(socket))
		{
			BEGIN_LOCK_PTR(SyncRoot());
			{
				mcr::ENDPOINT_ITERATOR it = this->_endpoints.find(ep.first);
				if (it != this->_endpoints.end()) {
					*it->second += 1;
				}
			}
			END_LOCK(SyncRoot());
		}
	}
	return FALSE;
}

BOOL CMaxConnectionsRule::Unregister(CSocket* socket) {
	BEGIN_LOCK_PTR(SyncRoot());
	{
		auto ep = endpoint_parse(socket->RemoteEndpoint());
		mcr::ENDPOINT_ITERATOR it = this->_endpoints.find(ep.first);
		if (it != this->_endpoints.end()) {
			*it->second -= 1;
			it = this->_endpoints.find(ep.first);
			if (*it->second == 0) {
				delete it->second;
				this->_endpoints.erase(it);
			}
		}
	}
	END_LOCK(SyncRoot());
	return CNetworkFilterRule::Unregister(socket);
}

BOOL CMaxConnectionsRule::Parse(STRING_STREAM& stream) {
	STRING str;
	stream >> str; this->_source.Parse(str);
	stream >> str; this->_target.Parse(str);
	stream >> this->_amount;
	return TRUE;
}

BOOL CMaxConnectionsRule::Filter(CSocket* socket) {
	BEGIN_LOCK_PTR(SyncRoot());
	{
		auto ep = endpoint_parse(socket->RemoteEndpoint());
		mcr::ENDPOINT_ITERATOR it = this->_endpoints.find(ep.first);
		if (it != this->_endpoints.end()) {
			if (*it->second >= this->_amount) {
				return TRUE;
			}
		}
	}
	END_LOCK(SyncRoot());
	return FALSE;
}