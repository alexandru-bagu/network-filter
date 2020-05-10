#include "pch.h"
#include "CNetworkFilterRule.h"

CNetworkFilterRule::CNetworkFilterRule()
{
	this->_syncRoot = new MUTEX();
}

MUTEX* CNetworkFilterRule::SyncRoot()
{
	return this->_syncRoot;
}

SOCKET_VECTOR_ITERATOR CNetworkFilterRule::Begin()
{
	return _sockets.begin();
}

SOCKET_VECTOR_ITERATOR CNetworkFilterRule::End()
{
	return _sockets.end();
}

BOOL CNetworkFilterRule::Match(CSocket* socket) {
	return _source.Match(socket->LocalEndpoint()) &&
		_target.Match(socket->RemoteEndpoint());
}

BOOL CNetworkFilterRule::Register(CSocket* socket)
{
	if (this->Match(socket) && !this->Filter(socket))
	{
		BEGIN_LOCK_PTR(SyncRoot());
		{
			SOCKET_VECTOR_ITERATOR it = std::find(Begin(), End(), socket);
			if (it == End()) {
				this->_sockets.push_back(socket);
				return TRUE;
			}
		}
		END_LOCK(SyncRoot());
	}
	return FALSE;
}

BOOL CNetworkFilterRule::Unregister(CSocket* socket)
{
	if (this->Match(socket))
	{
		BEGIN_LOCK_PTR(SyncRoot());
		{
			SOCKET_VECTOR_ITERATOR it = std::find(Begin(), End(), socket);
			if (it != End()) {
				this->_sockets.erase(it);
				return TRUE;
			}
		}
		END_LOCK(SyncRoot());
	}
	return FALSE;
}

BOOL CNetworkFilterRule::Parse(STRING_STREAM& stream)
{
	return FALSE;
}

BOOL CNetworkFilterRule::Filter(CSocket*)
{
	return TRUE;
}