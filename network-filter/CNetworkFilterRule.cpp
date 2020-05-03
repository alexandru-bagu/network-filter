#include "pch.h"
#include "CNetworkFilterRule.h"

CNetworkFilterRule::CNetworkFilterRule()
{

}

MUTEX& CNetworkFilterRule::SyncRoot()
{
	return _syncRoot;
}

SOCKET_ITERATOR CNetworkFilterRule::Begin()
{
	return _sockets.begin();
}

SOCKET_ITERATOR CNetworkFilterRule::End()
{
	return _sockets.end();
}

BOOL CNetworkFilterRule::Match(CSocket* socket) {
	return _source.Match(socket->LocalEndpoint()) &&
		_target.Match(socket->RemoteEndpoint());
}

BOOL CNetworkFilterRule::Register(CSocket* socket)
{
	if (this->Match(socket)) 
	{
		BEGIN_LOCK(SyncRoot());
		{
			SOCKET_ITERATOR it = std::find(Begin(), End(), socket);
			if (it == End()) {
				this->_sockets.push_back(socket);
				return true;
			}
			return false;
		}
		END_LOCK(SyncRoot());
	}
	return false;
}

BOOL CNetworkFilterRule::Remove(CSocket* socket)
{
	if (this->Match(socket)) 
	{
		BEGIN_LOCK(SyncRoot());
		{
			SOCKET_ITERATOR it = std::find(Begin(), End(), socket);
			if (it != End()) {
				this->_sockets.erase(it);
				return true;
			}
			return false;
		}
		END_LOCK(SyncRoot());
	}
	return false;
}

VOID CNetworkFilterRule::Tick() {

}