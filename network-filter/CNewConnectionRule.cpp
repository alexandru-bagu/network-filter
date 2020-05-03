#include "pch.h"
#include "CNewConnectionRule.h"
#include "endpoint.h"

BOOL CNewConnectionRule::Register(CSocket* socket) {
	BOOL result = CNetworkFilterRule::Register(socket);
	if (result) {
		BEGIN_LOCK(SyncRoot());
		{
			auto ep = endpoint_parse(socket->RemoteEndpoint());
			ENDPOINT_ITERATOR it = this->_endpoints.find(socket->RemoteEndpoint());
			if (it == this->_endpoints.end()) {
				//CStatistics stat(1, 1);
			}
		}
		END_LOCK(SyncRoot());
	}
	return TRUE;
}